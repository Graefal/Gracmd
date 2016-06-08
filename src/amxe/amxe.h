/*
	N-Gaming Network | sa-core

	  Path:
		../utils/amxe.h
	  Description:
		N/A
	  Licenses:
		N/A

	Copyright (c) 2016 N-Gaming Network (All rights reserved).
 */
#ifndef _AMXE_H_
#define _AMXE_H_
#include <sampgdk/sampgdk.h>
#include "command/command.h"

namespace AMXe
{
	void Register(AMX *amx);
	void Release(AMX *amx);
	extern std::unordered_map<AMX*, s_AMXe*> list;
}

#endif // !_AMXE_H_



