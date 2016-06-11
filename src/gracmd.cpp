/*
	N-Gaming Network | Gracmd

	  Path:
		../sa-core.cpp
	  Description:
		N/A
	  Licenses:
		N/A

	Copyright (c) 2016 N-Gaming Network (All rights reserved).
 */
#include "gracmd.h"

extern void *pAMXFunctions;

SubHook Command_Hook;

std::vector<std::map<AMX*, s_Command*>> amx_Data;

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
	const char *cmdparam;

	while (cmdtext[pos] > ' ')
	{
		if (pos > 28)
			return 1;

		cmdname[pos - 1] = tolower(cmdtext[pos]);
		pos++;
	}
	while (cmdtext[pos] == ' ')
	{
		pos++;
	}
	cmdparam = &cmdtext[pos];

	cell amx_addr;
	cell retval;

	for (size_t i = 0, j = amx_Data.size(); i < j; i++)
	{
		for (auto &k : amx_Data[i]) 
		{
			if (k.second->OnPlayerCommandReceived != -1)
			{
				amx_PushString(k.first, &amx_addr, nullptr, cmdtext, 0, 0);
				amx_Push(k.first, playerid);
				amx_Exec(k.first, &retval, k.second->OnPlayerCommandReceived);
				amx_Release(k.first, amx_addr);
				if (!retval)
				{
					break;
				}
			}
			auto iter = k.second->Command.find(cmdname);
			if (iter != k.second->Command.end())
			{
				amx_PushString(k.first, &amx_addr, nullptr, cmdparam, 0, 0);
				amx_Push(k.first, playerid);
				amx_Exec(k.first, &retval, iter->second);
				amx_Release(k.first, amx_addr);
				retval = 1;
			}
			else
			{
				retval = 0;
			}
			if (k.second->OnPlayerCommandPerformed != -1)
			{
				amx_Push(k.first, retval);
				amx_PushString(k.first, &amx_addr, nullptr, cmdtext, 0, 0);
				amx_Push(k.first, playerid);
				amx_Exec(k.first, &retval, k.second->OnPlayerCommandPerformed);
				amx_Release(k.first, amx_addr);
			}
		}
	}
	return 1;
}

void amx_ScanCommand(AMX *amx, bool state)
{
	int number = 0;
	amx_NumPublics(amx, &number);
	if (number)
	{
		s_Command *s_command = new s_Command();

		std::map<AMX*, s_Command*> content;
		content.insert(std::make_pair(amx, s_command));

		if(!state)
		{
			amx_Data.insert(amx_Data.begin(), content);
		}
		else
		{
			amx_Data.push_back(content);
		}

		bool opcr = false, opcp = false;
		
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
		for (size_t i = 0; i < amx_Data.size(); i++)
		{
			auto iter = amx_Data[i].find(amx);
			if (iter != amx_Data[i].end())
			{
				throw "Something went wrong";
			}
		}
		amx_ScanCommand(amx, true);
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
		for (size_t i = 0; i < amx_Data.size(); i++)
		{
			auto iter = amx_Data[i].find(amx);
			if (iter != amx_Data[i].end())
			{
				throw "Something went wrong";
			}	
		}
		amx_ScanCommand(amx, false);
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

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	return amx_Register(amx, natives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	for (auto i = amx_Data.begin(); i < amx_Data.end(); i++)
	{
		auto iter = i->find(amx);
		if (iter != i->end())
		{
			delete iter->second;
			i->erase(iter->first);
			amx_Data.erase(i);
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
			throw "Gracmd: SA-MP version unsuported.";
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
	sampgdk::Unload();
}