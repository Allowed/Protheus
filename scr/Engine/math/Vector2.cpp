
#include "Vector2.h"

using namespace Pro;
using namespace Math;


Vector2::Vector2(int x, int y) : Vector2(static_cast<float>(x), static_cast<float>(y)) {}
Vector2::Vector2(unsigned int x, unsigned int y) : Vector2(static_cast<float>(x), static_cast<float>(y)) {}

Vector2::Vector2(float _x, float _y){
	x = _x;
	y = _y;
}


Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::~Vector2(){}

Vector2 Vector2::operator=(SDL_Point& p){ return Vector2(p.x, p.y); }

SDL_Point Vector2::operator=(Vector2& p){
	SDL_Point o;
	o.x = static_cast<int>(p.x);
	o.y = static_cast<int>(p.y);
	return o;
}

bool Vector2::operator==(Vector2& p){
	if (x == p.x && y == p.y)
		return true;
	return false;
}

void Vector2::operator+=(Vector2& p){
	x += p.x;
	y += p.y;
}

bool Vector2::contains(float p){
	if ((x > p && y < p) || (x < p && y > p))
		return true;
	return false;
}

float Vector2::hypotenuse(){
	return sqrtf((x * x) + (y * y));
}

 
int Vector2::lContains(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1);
	lua_pushboolean(L, v->contains(lua_tonumber(L, 2)));
	return 1;
}
int Vector2::lHypotenuse(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1); 
	lua_pushnumber(L, v->hypotenuse());
	return 1;
}
int Vector2::lGetX(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1);
	lua_pushnumber(L, v->x);
	return 1;
}
int Vector2::lGetY(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1);
	lua_pushnumber(L, v->y);
	return 1;
}
int Vector2::lSetX(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1);
	v->x = lua_tonumber(L, 2);
	return 0;
}
int Vector2::lSetY(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1);
	v->y = lua_tonumber(L, 2);
	return 0;
}
int Vector2::lGetXY(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1);
	float dat[] = { v->x, v->y };
	Util::luaP_pusharray<float>(L, dat, 2); 
	return 1;
}
int Vector2::lSetXY(lua_State* L){
	Vector2* v = Util::luaP_touserdata<Vector2>(L, 1);
	v->x = lua_tonumber(L, 2);
	v->y = lua_tonumber(L, 3);
	return 0;
}