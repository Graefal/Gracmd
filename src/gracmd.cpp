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

std::unordered_map<AMX*, s_Command*> Data;
std::unordered_map<std::string, AMX*> Command_List;

#ifdef _WIN32
	DWORD Command_Addr = Utils::Memory::FindPattern("\x83\xEC\x08\x53\x8B\x5C\x24\x14\x55\x8B\x6C\x24\x14\x56\x33\xF6\x57\x8B\xF9\x89\x74\x24\x10\x8B\x04\xB7\x85\xC0", "xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
#else
	DWORD Command_Addr = Utils::Memory::FindPattern("\x55\x89\xE5\x57\x56\x53\x83\xEC\x2C\x8B\x75\x08\xC7\x45\xE4\x00\x00\x00\x00\x8B\x7D\x10\x89\xF3\xEB\x14", "xxxxxxxxxxxxxxxxxxxxxxxxxx");
#endif // _WIN32

typedef int(fastcall* Command_t)(void *This, void *notUsed, cell playerid, char *cmdtext);
static int fastcall Command_0(void *This, void *notUsed, cell playerid, char *cmdtext)
{
	int temp0 = 1;
	char temp1[29];

	while (cmdtext[temp0] > ' ')
	{
		if (cmdtext[temp0] == '_')
		{
			temp1[temp0 - 1] = '_';
		}
		else 
		{
			temp1[temp0 - 1] = cmdtext[temp0] | 0x20;

		}
		temp0++;
	}
	
	auto temp2 = Command_List.find(temp1);
	if (temp2 != Command_List.end())
	{
		while (cmdtext[temp0] == ' ')
		{
			temp0++;
		}
		
		cell temp00;
		cell temp01;
		auto temp02 = Data.find(temp2->second);

		if (temp02->second->OnPlayerCommandReceived != -1)
		{
			amx_PushString(temp2->second, &temp00, nullptr, &cmdtext[1], 0, 0);
			amx_Push(temp2->second, playerid);
			amx_Exec(temp2->second, &temp01, temp02->second->OnPlayerCommandReceived);
			amx_Release(temp2->second, temp00);
			if (temp01 == 0)
			{
				return 1;
			}
		}
		
		amx_PushString(temp2->second, &temp00, nullptr, &cmdtext[temp0], 0, 0);
		amx_Push(temp2->second, playerid);
		amx_Exec(temp2->second, &temp01, temp02->second->Command.find(temp1)->second);
		amx_Release(temp2->second, temp00);

		if (temp02->second->OnPlayerCommandPerformed != -1)
		{
			amx_Push(temp2->second, temp01);
			amx_PushString(temp2->second, &temp00, nullptr, &cmdtext[1], 0, 0);
			amx_Push(temp2->second, playerid);
			amx_Exec(temp2->second, &temp01, temp02->second->OnPlayerCommandPerformed);
			amx_Release(temp2->second, temp00);
		}
	}
	return 1;
}

AMX_NATIVE_INFO natives[] =
{
	{0,0}
};

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return sampgdk::Supports() | SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX * amx)
{
	int temp0 = 0;
	amx_NumPublics(amx, &temp0);
	if (temp0)
	{
		s_Command *Command_s = new s_Command();
		Data.insert(std::make_pair(amx, Command_s));

		bool j = false, k = false;
		for (int i = 0; i < temp0; i++)
		{
			char temp00[32];
			amx_GetPublic(amx, i, temp00);
			if (j)
			{
				goto FIRST;
			}
			if (!strcmp(temp00, "OnPlayerCommandReceived"))
			{
				j = true;
				Command_s->OnPlayerCommandReceived = i;
				continue;
			}
		FIRST:
			if (k)
			{
				goto SECOND;
			}
			if (!strcmp(temp00, "OnPlayerCommandPerformed"))
			{
				k = true;
				Command_s->OnPlayerCommandPerformed = i;
				continue;
			}
		SECOND:
			if (!memcmp(temp00, "cmd_", 4))
			{
				memcpy(temp00, &temp00[4], 29);

				Command_s->Command.insert(std::make_pair(temp00, i));
				Command_List.insert(std::make_pair(temp00, amx));
			}
		}
	}

	return amx_Register(amx, natives, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	auto iter = Data.find(amx);
	if (iter != Data.end())
	{
		delete iter->second;
		Data.erase(iter->first);
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
			throw "Coudln't find OnPlayerCommandText pattern";
		}
		Command_Hook.Install((void *)(Command_t)(Command_Addr), (void *)Command_0);
	}
	catch (char* e)
	{
		sampgdk::logprintf("Gracmd: %s.", e);
	}

	return sampgdk::Load(ppData);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	sampgdk::Unload();
}