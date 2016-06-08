/*
	N-Gaming Network | sa-core
	
	  Path:
		../utils/amxe.cpp
	  Description:
  		N/A
  	  Licenses:
		N/A

	Copyright (c) 2016 N-Gaming Network (All rights reserved).
*/
#include <utils/utils.h>

#include "amxe/amxe.h"
#include "command/command.h"

std::unordered_map<AMX*, AMXe::s_AMXe*> AMXe::list;

void AMXe::Register(AMX *amx)
{
	s_AMXe *x = new s_AMXe();
	list.insert(std::pair<AMX*, s_AMXe*>(amx, x));
	Command::RegisterAllFromAmx(amx, x);
}

void AMXe::Release(AMX *amx)
{
	std::unordered_map<AMX*, s_AMXe*>::iterator i = list.find(amx);
	{
		delete i->second;
		list.erase(i->first);
	}
}