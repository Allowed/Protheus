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

#include <string>
#include "AnimatedEntity.h"
#include "Inventory.h" 

using namespace std;

namespace Pro{
	namespace GameObject{ 
		class Avatar :
			public AnimatedEntity,
			public Inventory
		{
		public:
			Avatar(const std::string& name);
			Avatar();
			~Avatar();


			// LUA Functions

			// returns the Metatable's name assosiated with this object
			static inline string lGetMetatable(){
				return "gameobject_avatar_metatable";
			}
		};
	}
}