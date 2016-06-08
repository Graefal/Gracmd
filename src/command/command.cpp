#include <utils/utils.h>

static SubHook OnPlayerCommandText_Hook;

std::unordered_map<std::string, AMX*> Command::list;

typedef int(fastcall* OnPlayerCommandText_t)(void *This, void *notUsed, cell playerid, const char *cmdtext);
static int fastcall OnPlayerCommandText_New(void *This, void *notUsed, cell playerid, const char *cmdtext)
{
	std::string temp0(cmdtext);
	std::transform(temp0.begin(), temp0.end(), temp0.begin(), ::tolower);
	
	std::size_t temp1 = temp0.find(' ');
	if (temp1 == std::string::npos)
	{
		temp1 = temp0.length();
	}
	std::string temp2(temp0.substr(1, temp1 - 1));
	std::unordered_map<std::string, AMX*>::iterator temp3 = Command::list.find(temp2);
	
	if (temp3 != Command::list.end())
	{
		std::string temp04(temp0.substr(temp1, temp0.length() - temp1));
		if (temp1 != temp0.length())
		{
			temp1++;
			for (char& i : temp04)
			{
				if (i != ' ')
				{
					break;
				}
				temp1++;
			}
			temp04.assign(temp0.substr(temp1, temp0.length() - temp1));
		}
		std::unordered_map<AMX*, AMXe::s_AMXe*>::iterator temp00 = AMXe::list.find(temp3->second);
		cell temp01;
		cell temp02;
		
		if (temp00->second->command.OnPlayerCommandReceived != -1)
		{
			amx_PushString(temp3->second, &temp02, nullptr, temp0.c_str(), 0, 0);
			amx_Push(temp3->second, playerid);
			amx_Exec(temp3->second, &temp01, temp00->second->command.OnPlayerCommandReceived);
			amx_Release(temp3->second, temp02);
			if (temp01 == 0)
			{
				return 1;
			}
		}

		amx_PushString(temp3->second, &temp02, nullptr, temp04.c_str(), 0, 0);
		amx_Push(temp3->second, playerid);
		amx_Exec(temp3->second, &temp01, temp00->second->command.command.find(temp2)->second);
		amx_Release(temp3->second, temp02);

		if (temp00->second->command.OnPlayerCommandPerformed != -1)
		{
			amx_Push(temp3->second, temp01);
			amx_PushString(temp3->second, &temp02, nullptr, temp0.c_str(), 0, 0);
			amx_Push(temp3->second, playerid);
			amx_Exec(temp3->second, &temp01, temp00->second->command.OnPlayerCommandPerformed);
			amx_Release(temp3->second, temp02);
		}
	}
	return 1;
}

#ifdef _WIN32
	DWORD Command::OnPlayerCommandText_Addr = Utils::Memory::FindPattern("\x83\xEC\x08\x53\x8B\x5C\x24\x14\x55\x8B\x6C\x24\x14\x56\x33\xF6\x57\x8B\xF9\x89\x74\x24\x10\x8B\x04\xB7\x85\xC0", "xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	/*
	Pattern Guide

	\x83\xEC\x08"          sub esp,0x8
	\x53"                  push ebx
	\x8B\x5C\x24\x14"      mov ebx,DWORD PTR [esp+0x14]
	\x55"                  push ebp
	\x8B\x6C\x24\x14"      mov ebp,DWORD PTR [esp+0x14]
	\x56"                  push esi
	\x33\xF6"              xor esi,esi
	\x57"                  push edi
	\x8B\xF9"              mov edi,ecx
	\x89\x74\x24\x10"      mov DWORD PTR [esp+0x10],esi
	\x8B\x04\xB7"          mov eax,DWORD PTR [edi+esi*4]
	\x85\xC0"              test eax,eax
	*/
#else
	DWORD Command::OnPlayerCommandText_Addr = Utils::Memory::FindPattern("\x55\x89\xE5\x57\x56\x53\x83\xEC\x2C\x8B\x75\x08\xC7\x45\xE4\x00\x00\x00\x00\x8B\x7D\x10\x89\xF3\xEB\x14", "xxxxxxxxxxxxxxxxxxxxxxxxxx");
#endif // _WIN32

void Command::Initialize() 
{
	try
	{
		if (!Command::OnPlayerCommandText_Addr)
		{
			throw "Coudln't find OnPlayerCommandText pattern";
		}
		OnPlayerCommandText_Hook.Install((void *)(OnPlayerCommandText_t)(Command::OnPlayerCommandText_Addr), (void *)OnPlayerCommandText_New);
	}
	catch(char* e)
	{
		sampgdk::logprintf("NCMD: %s.", e);
	}
}
								 
void Command::RegisterAllFromAmx(AMX* amx, AMXe::s_AMXe* list)
{
	int temp0 = 0;
	amx_NumPublics(amx, &temp0);
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
			list->command.OnPlayerCommandReceived = i;
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
			list->command.OnPlayerCommandPerformed = i;
			continue;
		}
	SECOND:
		if (!memcmp(temp00, "cmd_", 4))
		{
			memcpy(temp00, &temp00[4], 29);
			Command::list.insert(std::make_pair(temp00, amx));
			list->command.command.insert(std::make_pair(temp00, i));
		}
	}
}