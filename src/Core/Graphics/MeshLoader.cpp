#include "MeshLoader.h"

using namespace Pro;


MODEL_FORMAT  MeshLoader::queryFormat(CBuffer* buffer) {

	if (buffer->isEmpty())
		return MODEL_FORMAT::UNDEFINED;

	//switch () { 
	//default:
	//	return MODEL_FORMAT::UNDEFINED;
	//}
	return MODEL_FORMAT::UNDEFINED;
}

inline unsigned process_linei(const string& format, BufferReader& in, BufferWriter& out, unsigned& face_count) {
	int face[12];
	// vertex/uv/normal 
	// TEST does atoi stop at the /
	auto ret = sscanf(in.read_raw(), format.data(),
		face + 0, face + 1, face + 2,
		face + 3, face + 4, face + 5,
		face + 6, face + 7, face + 8,
		face + 9, face + 10, face + 11);
	for (unsigned char x = 0; x < ret; ++x)
		*(face + x) -= 1;

	out.write_elements<int>(face, ret);
	++face_count;
	return ret;
}

inline void processOBJ(std::vector<MeshObject>& objects, CBuffer* file, BufferWriter& vertex_writer, BufferWriter& normal_writer, BufferWriter& tex_coord_writer) {
	BufferReader reader(file);

	float vertex[4];
	bool first_object = true;
	memset(vertex, 0, sizeof(float) * 4);

	MeshObject* object = &objects.back();

	while (reader.hasNext()) {
		auto line = reader.read_delim('\n', false);
		BufferReader liner(&line);

		// Check that the next line is creating an object if one hasn't been
		// skip the line if one isn't created  


		switch (line.data<char>()[0]) {
		case 'o': {
			liner.setPosition(2);
			// pointer will break outside of this function

			auto nameBuf = liner.read_delim('\n', false);

			if (first_object) {
				object->name = string(nameBuf.data<char>(), nameBuf.size() - 1);
				first_object = false;
			}
			else
				objects.push_back(std::move(MeshObject(string(nameBuf.data<char>(), nameBuf.size() - 1), 0, 0)));

			object = &objects.back();
			object->temp = new CBuffer(1000);
			object->tempWriter = new BufferWriter(object->temp);
		}
				  break;
		case 'f':

			if (line.count<char>(' ') == 3)  
				object->vertex_per_face = 3; 
			else 
				object->vertex_per_face = 4; 

			switch (object->vertex_per_face) {
			case 3:
				if (liner.contains<char>("//", 2) != -1) {
					// Vector//Normal  
					process_linei("f %i//%i %i//%i %i//%i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX_NORMAL;
					break;
				}
				switch (line.count<char>('/')) {
				case 0:
					// simple vertex 
					process_linei("f %i %i %i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX;
					break;
				case 3:
					// vertex/uv   
					process_linei("f %i/%i %i/%i %i/%i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX_UV;
					break;
				case 6:
					// vertex/uv/normal 
					// TEST does atoi stop at the /
					process_linei("f %i/%i/%i %i/%i/%i %i/%i/%i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX_UV_NORMAL;
					break;
				}
				break;

			case 4:
				if (liner.contains<char>("//", 2) != -1) {
					// Vector//Normal   
					process_linei("f %i//%i %i//%i %i//%i %i//%i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX_NORMAL;
					break;
				}
				switch (line.count<char>('/')) {
				case 0:
					// simple vertex 
					process_linei("f %i %i %i %i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX;
					break;
				case 4:
					// vertex/uv  
					// TEST does atoi stop at the /
					process_linei("f %i/%i %i/%i %i/%i %i/%i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX_UV;
					object->has_tex_coord = true;
					break;
				case 8:
					// vertex/uv/normal 
					// TEST does atoi stop at the /
					process_linei("f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", liner, *object->tempWriter._ptr, object->face_count);
					object->face_count++;
					object->face_format = FACE_FORMAT::VERTEX_UV_NORMAL;
					object->has_normals = true;
					object->has_tex_coord = true;
					break;
				}
				break;
			default:
				error.reportErrorNR("Unsupported face type in .obj\0");
				return; 
			}

			break;

		case 'v':
			switch (line.data<char>()[1]) {
			case 't':
				object->tex_coord_per_vertex =
					sscanf(liner.read_raw(), "vt %f %f %f %f",
					vertex + 0, vertex + 1, vertex + 2);
				  
				tex_coord_writer.write_elements<float>(vertex, object->tex_coord_per_vertex);
				break;
			case 'n':
				sscanf(liner.read_raw(), "vn %f %f %f",
					vertex + 0, vertex + 1, vertex + 2);
				normal_writer.write_elements<float>(vertex, 3); 

				break;
			case ' ':
				object->floats_per_vertex = sscanf(liner.read_raw(), "v %f %f %f %f",
					vertex + 0, vertex + 1, vertex + 2, vertex + 3);

				vertex_writer.write_elements<float>(vertex, object->floats_per_vertex);
				break;
			}
		}
	}
	// Set the last size of the object
	object->size =
		((object->face_count - object->start) * object->vertex_per_face);
}


smart_pointer<Mesh> MeshLoader::loadOBJ(CBuffer* buffer) {
	BufferReader reader(buffer);
	CBuffer verticies;
	BufferWriter vertex_writer(&verticies);
	CBuffer normals;
	BufferWriter normal_writer(&normals);
	CBuffer tex_coords;
	BufferWriter tex_coord_writer(&tex_coords);

	FACE_FORMAT face_format = FACE_FORMAT::UNDEFINED;

	// code below uses a pointer, 
	// kept pointer so I didn't have to edit
	std::vector<MeshObject> object;

	object.push_back(MeshObject("", 0, 0));

	processOBJ(object, buffer, vertex_writer, normal_writer, tex_coord_writer);

	// populate the size of the last object 

	// Pack verticies 
	CBuffer packed(4000);
	CBuffer elements(4000);
	BufferWriter element_writer(&elements);
	BufferWriter packed_writer(&packed);

	CBuffer search_face(4 * sizeof(float));
	BufferWriter search_face_writer(&search_face);
	// okay because indicies will read ahead of the writer 
	// Perfect for OpenCL

	unsigned current_element = 0;
	unsigned current_position = 0;

	struct Face {
		int vertex = -1;
		int normal = -1;
		int tex_coord = 1;

		inline bool operator==(const Face& face) {
			return vertex == face.vertex && normal == face.normal && tex_coord == face.tex_coord;
		}
	};

	std::vector<Face> faces;

	for (auto& obj : object) {
		BufferReader reader(obj.temp);

		// get the next faces
		// compair to see if they have already be defined
		// if true then use it's index in the element buffer
		// otherwise add the verticies into the buffer 
		// and insert the new index into the element

		for (unsigned face_id = 0; face_id < obj.face_count; ++face_id) {

			// Get Faces
			Face face;
			bool existing = false;

			face.vertex = reader.read<int>();
			switch (face_format) {
			case FACE_FORMAT::VERTEX_NORMAL:
				face.normal = reader.read<int>();
				break;
			case FACE_FORMAT::VERTEX_UV:
				face.tex_coord = reader.read<int>();
				break;
			case FACE_FORMAT::VERTEX_UV_NORMAL:
				face.tex_coord = reader.read<int>();
				face.normal = reader.read<int>();
				break;
			}

			// Check if face has already been defined
			for (unsigned x = 0; x < faces.size(); ++x)
				if (faces[x] == face) {
					existing = true;
					element_writer.write<unsigned>(x);
					break;
				}

			// If no duplicates, add into the verticies and element buffer
			if (existing == false) {
				packed_writer.write_elements<float>(verticies.data<float>() + face.vertex * obj.floats_per_vertex, obj.floats_per_vertex);
				switch (face_format) {
				case FACE_FORMAT::VERTEX_NORMAL:
					packed_writer.write_elements<float>(normals.data<float>() + face.normal * 3, 3);
					break;
				case FACE_FORMAT::VERTEX_UV:
					packed_writer.write_elements<float>(tex_coords.data<float>() + face.tex_coord * obj.tex_coord_per_vertex, obj.tex_coord_per_vertex);
					break;
				case FACE_FORMAT::VERTEX_UV_NORMAL:
					packed_writer.write_elements<float>(tex_coords.data<float>() + face.tex_coord  * obj.tex_coord_per_vertex, obj.tex_coord_per_vertex);
					packed_writer.write_elements<float>(normals.data<float>() + face.normal * 3, 3);
					break;
				}
				faces.push_back(face);
				element_writer.write<unsigned>(current_element++);
			}
		}
	}

	for (int x = 0; x < object.size(); x++) { 
		object[x].temp = nullptr;
		object[x].tempWriter = nullptr;
	}
	 
	// Create VBO in OpenGL

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, packed_writer.getPosition(), packed.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (glGetError() != GL_NO_ERROR) {
		error.reportError("Unable to load OBJ Model: Create Array Buffer");
		return nullptr;
	}

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_writer.getPosition(), elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (glGetError() != GL_NO_ERROR) {
		error.reportError("Unable to load OBJ Model: Create Element Array");
		return nullptr;
	}
	Mesh* m = new Mesh(vbo, ebo);
	for (unsigned x = 0; x < object.size(); ++x)
		m->attachObject(std::move(object[x]));

	return m;
}

void nsa_backdoor() {
	error.reportFatalNR("illuminati");
}

smart_pointer<Mesh> MeshLoader::loadModel(CBuffer* buffer) {
	smart_pointer<Mesh> model = nullptr;

	if (buffer->isEmpty()) {
		error.reportError("Empty buffer passed to MeshLoader did file load correctly?\0");
		return nullptr;
	}

	switch (queryFormat(buffer)) {
	case MODEL_FORMAT::OBJ:
		model = loadOBJ(buffer);
		break;
	default:
		error.reportError("Unable to load model: unknown format\0");
		return nullptr;
	}

	error.reportError("Unable to load model: Load fail\0");
	if (model == nullptr)
		return nullptr;

	return model;
}
smart_pointer<Mesh> MeshLoader::loadModel(CBuffer&& buffer) {
	return loadModel(&buffer);
}