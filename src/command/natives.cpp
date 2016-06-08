#include <utils/utils.h>

cell AMX_NATIVE_CALL Command::Command_AddAltNamed_N(AMX *amx, cell *params)
{
	if (params[0] != (8)) 
	{ 
		sampgdk::logprintf("Command: Command_AddAltNamed Expecting 2 parameter(s), but found %d", params[0] / sizeof(cell)); 
		return 0; 
	}

/*
	auto iterAmx = _amx_map.find(amx);
	if (iterAmx == _amx_map.end())
		return logprintf("[Pawn.CMD] amx not found"), 0;

	auto iterPublic = iterAmx->second.cmd_map.end();
	int originalID{};

	cell *cptr_cmd{};
	int len{};
	std::string str_cmd;

	for (size_t i = 1; i <= params[0] / sizeof(cell); i++)
	{
		char cmd[32]{};

		amx_GetAddr(amx, params[i], &cptr_cmd);
		amx_StrLen(cptr_cmd, &len);

		if (len > 31)
			len = 31;

		amx_GetString(cmd, cptr_cmd, 0, len + 1);

		str_to_lower(str_cmd = cmd);

		if (i == 1)
		{
			iterPublic = iterAmx->second.cmd_map.find(str_cmd);
			if (iterPublic == iterAmx->second.cmd_map.end())
			{
				return logprintf("[Pawn.CMD] command '%s' not found", str_cmd.c_str()), 0;
			}

			originalID = iterPublic->second;
		}
		else
		{
			iterAmx->second.cmd_map[str_cmd] = originalID;

			logprintf("[Pawn.CMD] alias '%s' for '%s' has been registered", str_cmd.c_str(), iterPublic->first.c_str());
		}
	}
	*/
	return 1;
}