/*
	N-Gaming Network | Gracmd

	  Path:
		../gracmd.cpp
	  Description:
		N/A
	  Licenses:
		N/A

	Copyright (c) 2016 N-Gaming Network (All rights reserved).
 */
#include "gracmd.h"

extern void *pAMXFunctions;

SubHook Command_Hook;

std::deque<AMX*> amx_List;
std::deque<s_Command*> command_List;

#ifdef _WIN32
	DWORD Command_Addr = Utils::Memory::FindPattern("\x83\xEC\x08\x53\x8B\x5C\x24\x14\x55\x8B\x6C\x24\x14\x56\x33\xF6\x57\x8B\xF9\x89\x74\x24\x10\x8B\x04\xB7\x85\xC0", "xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
#else
	DWORD Command_Addr = Utils::Memory::FindPattern("\x55\x89\xE5\x57\x56\x53\x83\xEC\x2C\x8B\x75\x08\xC7\x45\xE4\x00\x00\x00\x00\x8B\x7D\x10\x89\xF3\xEB\x14", "xxxxxxxxxxxxxxxxxxxxxxxxxx");
#endif // _WIN32

typedef int(fastcall* Command_t)(void *This, void *notUsed, cell playerid, const char *cmdtext);
static int fastcall Command_0(DWORD *This, void *notUsed, cell playerid, const char *cmdtext)
{
	int pos = 1;
	char cmdname[29];

	while (cmdtext[pos] > ' ')
	{
		if (pos > 28)
			return 1;

		cmdname[pos - 1] = tolower(cmdtext[pos]);
		pos++;
	}

	AMX *amx;
	cell amx_addr, retval, unused;
	s_Command *command;
	const char *cmdparam;

	while (cmdtext[pos] == ' ')
	{
		pos++;
	}
	cmdparam = &cmdtext[pos];

	for (size_t i = 0, j = amx_List.size(); i < j; i++)
	{
		amx = amx_List[i];
		command = command_List[i];

		if (command->OnPlayerCommandReceived != -1)
		{
			amx_PushString(amx, &amx_addr, nullptr, cmdtext, 0, 0);
			amx_Push(amx, playerid);
			amx_Exec(amx, &retval, command->OnPlayerCommandReceived);
			amx_Release(amx, amx_addr);
		}

		if (retval)
		{
			auto iter = command->Command.find(cmdname);
			if (iter != command->Command.end())
			{
				amx_PushString(amx, &amx_addr, nullptr, cmdparam, 0, 0);
				amx_Push(amx, playerid);
				amx_Exec(amx, &unused, iter->second);
				amx_Release(amx, amx_addr);
			}
		}

		if (command->OnPlayerCommandPerformed != -1)
		{
			amx_Push(amx, retval);
			amx_PushString(amx, &amx_addr, nullptr, cmdtext, 0, 0);
			amx_Push(amx, playerid);
			amx_Exec(amx, &unused, command->OnPlayerCommandPerformed);
			amx_Release(amx, amx_addr);
		}

		if (!retval)
		{
			break;
		}
	}
	return 1;
}

void amx_ScanCommand(AMX *amx, bool state)
{
	for (size_t i = 0, j = amx_List.size(); i < j; i++)
	{
		if (amx_List[i] == amx)
		{
			throw "Something went wrong";
		}
	}
	
	int number = 0;
	amx_NumPublics(amx, &number);
	if (number)
	{
		bool opcr = false, opcp = false;
		s_Command *s_command = new s_Command();
		
		if(state)
		{
			amx_List.push_front(amx);
			command_List.push_front(s_command);
		}
		else
		{
			amx_List.push_back(amx);
			command_List.push_back(s_command);
		}

		for (int i = 0; i < number; i++)
		{
			char funcname[32];
			amx_GetPublic(amx, i, funcname);
			if (opcr)
			{
				goto FIRST;
			}
			if (!strcmp(funcname, "OnPlayerCommandReceived"))
			{
				opcr = true;
				s_command->OnPlayerCommandReceived = i;

				continue;
			}
		FIRST:
			if (opcp)
			{
				goto SECOND;
			}
			if (!strcmp(funcname, "OnPlayerCommandPerformed"))
			{
				opcp = true;
				s_command->OnPlayerCommandPerformed = i;

				continue;
			}
		SECOND:
			if (!memcmp(funcname, "cmd_", 4))
			{
				memcpy(funcname, &funcname[4], 29);

				s_command->Command.insert(std::make_pair(funcname, i));
			}
		}
	}
}

cell AMX_NATIVE_CALL iCommand_push_back_n(AMX *amx, cell *params) 
{
	try
	{
		amx_ScanCommand(amx, false);
	}
	catch (const char* e)
	{
		sampgdk::logprintf("Gracmd: %s.", e);
	}
	return 1;
}

cell AMX_NATIVE_CALL iCommand_push_front_n(AMX *amx, cell *params)
{
	try
	{
		amx_ScanCommand(amx, true);
	}
	catch (const char* e)
	{
		sampgdk::logprintf("Gracmd: %s.", e);
	}
	return 1;
}

AMX_NATIVE_INFO natives[] =
{
	{"iCommand_push_back", iCommand_push_back_n},
	{"iCommand_push_front", iCommand_push_front_n},
	{0,0}
};

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return sampgdk::Supports() | SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

std::unordered_map<AMX*, std::string> test;
PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, natives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	for (size_t i = 0, j = amx_List.size(); i < j; i++)
	{
		if (amx_List[i] == amx)
		{
			for (auto it = command_List.begin(); it != command_List.end(); ++it)
			{
				if (*it == command_List[i])
				{
					delete command_List[i];
					command_List.erase(it);

					break;
				}
			}
			break;
		}
	}
	for (auto it = amx_List.begin(); it != amx_List.end(); ++it)
	{
		if (*it == amx)
		{
			amx_List.erase(it);

			break;
		}
	}
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

	try
	{
		if (!Command_Addr)
		{
			throw "Gracmd: Your SA-MP version is unsuported yet.";
		}
		Command_Hook.Install((void *)(Command_t)(Command_Addr), (void *)Command_0);
	}
	catch (const char* e)
	{
		sampgdk::logprintf("Gracmd: %s.", e);
	}

	return sampgdk::Load(ppData);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	amx_List.clear();
	for (auto it = command_List.begin(); it != command_List.end(); ++it)
	{
		delete *it;
	}
	command_List.clear();

	sampgdk::Unload();
}