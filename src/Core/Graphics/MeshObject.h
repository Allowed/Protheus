#pragma once


#include <string> 
#include <memory>
#include <vector>
#include "Program.h"
#include <CBuffer.h>
#include <smart_pointer.h>
#include <BufferWriter.h>
#include "VertexArray.h"
#include "extern\glew.h"

namespace Pro {
	using namespace std;

	enum struct FACE_FORMAT {
		UNDEFINED,
		VERTEX,
		VERTEX_NORMAL,
		VERTEX_UV_NORMAL,
		VERTEX_UV
	};


	struct MeshObject {
		string name;

		union {
			GLuint start;
			const void* p_start;
		};
		GLuint size = 0; 
		unsigned char vertex_per_face = 0;
		unsigned char floats_per_vertex = 0;
		unsigned char tex_coord_per_vertex = 0; 
		unsigned face_count = 0; 

		bool has_tex_coord;
		bool has_normals;
		 
		FACE_FORMAT face_format = FACE_FORMAT::UNDEFINED;

		smart_pointer<CBuffer> temp = nullptr;
		smart_pointer<BufferWriter> tempWriter = nullptr;

		MeshObject();
		MeshObject(const string& name, unsigned start, unsigned size);
		MeshObject(MeshObject&&);
		MeshObject& operator=(MeshObject&&);
		MeshObject(const MeshObject&);
		MeshObject& operator=(const MeshObject&);
		~MeshObject();
		  
		bool     hasTexCoord() const;
		bool     hasNormals() const;
		GLuint   vertexSize() const;
		GLuint   normalSize() const;
		GLuint   texCoordSize() const;

		GLsizei  stride() const;
		GLuint   normalOffset() const;
		GLuint   vertexOffset() const;

		GLuint   texCoordOffset() const;
		GLenum   getMode() const;
	};
}