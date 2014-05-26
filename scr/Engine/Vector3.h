/*************************************************************************
Protheus Source File.
Copyright (C), Protheus Studios, 2013-2014.
-------------------------------------------------------------------------

Description:

-------------------------------------------------------------------------
History:
- 17:05:2014 Waring J.

*************************************************************************/

#ifdef __SSE
#include <xmmintrin.h>
#endif

#pragma once
namespace Pro{
	namespace Math{

#ifdef _MSC_VER
	_declspec(align(16))
#endif

		struct Vector3{
			float x, y, z;

			Vector3();
			~Vector3();
		};
	} 
}
