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
// TODO: Make my own headers for Sanity
#ifdef LITH_SANITY
#define RMode RmodeNolf
#endif

#include <iltclient.h>

#ifdef LITH_SANITY
#undef RMode
// Render modes are what are used to describe a video mode/video card.
typedef struct RMode_t
{
	char			m_bHardware;

	char			m_RenderDLL[200];		// What DLL this comes from.		
	char			m_InternalName[100];	// This is what the DLLs use to identify a card.
	char			m_Description[100];		// This is a 'friendly' string describing the card.

	unsigned long	m_Width, m_Height, m_BitDepth;
	struct RMode_t* m_pNext;
} RMode;
#endif

#endif

// GetVarValueFloat keeps returning -nan(ind)
struct FloatVar {
	char unknown[16];
	unsigned char* commandName;
	unsigned char* strValue; // ?? Looks like it
	float value;
};
