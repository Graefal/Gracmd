/*
	N-Gaming Network | sa-core

	  Path:
		../command/command.h
	  Description:
		N/A
	  Licenses:
		N/A

	Copyright (c) 2016 N-Gaming Network (All rights reserved).
*/
#ifndef _COMMAND_H_
#define _COMMAND_H_
#include <sampgdk/sampgdk.h>
#ifndef _AMXE_H_
	#include "amxe/amxe.h"
#endif // !_AMXE_H_
namespace Command
{
	struct s_command
	{
		int OnPlayerCommandReceived = 0;
		int OnPlayerCommandPerformed = 0;
		std::unordered_map<std::string, int> command;
		// std unordered map key: cmdname id 
	};
}

namespace AMXe
{
	struct s_AMXe
	{
		struct Command::s_command command;
	};
}

namespace Command
{
	extern DWORD OnPlayerCommandText_Addr;

	void Initialize();
	void RegisterAllFromAmx(AMX* amx, AMXe::s_AMXe* amxData);
	cell Command_AddAltNamed_N(AMX *amx, cell *params);

	extern std::unordered_map<std::string, AMX*> list;
}

#endif // !_COMMAND_H_