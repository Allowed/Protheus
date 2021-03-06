/*************************************************************************
Protheus Source File.
Copyright (C), Protheus Studios, 2013-2014.
-------------------------------------------------------------------------

Description:

-------------------------------------------------------------------------
History:
- 20:05:2014: Waring J.
*************************************************************************/

#pragma once

#include "GUIEntity.h"
namespace Pro{
	namespace GUI{
		class GUILabel :
			public GUIEntity
		{
		public:
			GUILabel(const std::string& name);
			GUILabel(); 

			// returns the Metatable's name assosiated with this object
			/*constexpr*/ static const char* lGetMetatable(){
				return "gui_label_metatable";
			}

			template<typename T>
			static inline void lGetFunctions(std::vector<luaL_Reg>& fields){
				GUIEntity::lGetFunctions<T>(fields);
			}
		};
	}
}
