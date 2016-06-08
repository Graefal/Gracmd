/*
	N-Gaming Network | sa-core

	  Path:
		../sa-core.h
	  Description:
		N/A
	  Licenses:
		N/A

	Copyright (c) 2016 N-Gaming Network (All rights reserved).
 */
#ifndef _SACORE_H_
#define _SACORE_H_

#ifdef _WIN32
	#include <Windows.h>
	#define WIN32_LEAN_AND_MEAN
	#include <Psapi.h>

	#define fastcall __fastcall
#else
	#define fastcall 
	typedef unsigned long DWORD;
	typedef int BOOL;
	typedef unsigned char BYTE;
	typedef unsigned short WORD;
	typedef float FLOAT;
#endif // _WIN32

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <subhook/subhook.h>
#include <sampgdk/sampgdk.h>

namespace Utils {
	class Memory {
	public:
		static DWORD FindPattern(char *pattern, char *mask);
	};
}

struct s_Command
{
	int OnPlayerCommandReceived = 0;
	int OnPlayerCommandPerformed = 0;
	std::unordered_map<std::string, int> Command;
};

#endif // !_SACORE_SA_CORE_H_
#pragma once
