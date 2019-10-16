#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include "Helpers.h"
#include "Config.h"

// Release Version Chart -- If I find it in the exe/dll, i'll throw it here.
// Sanity - 25.1

#if defined(LITH_SHOGO)
#include <de_client.h>

// Some stuff is named differently or not defined in the older lithtech games
// Just typedef them!
typedef ClientShellDE IClientShell;
typedef ClientDE ILTClient;

typedef float LTFLOAT;
// This is bad. 
typedef unsigned long uint32;
typedef DRESULT LTRESULT;
#else
#include <iltclient.h>
#endif

// GetVarValueFloat keeps returning -nan(ind)
struct FloatVar {
	char unknown[16];
	unsigned char* commandName;
	unsigned char* strValue; // ?? Looks like it
	float value;
};
