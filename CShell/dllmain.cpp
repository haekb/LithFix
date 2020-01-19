// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <detours.h>

#include <SDL.h>
#include <string>
#include <fstream>

SDL_Window* g_hSDLWindow = NULL;

Config g_sConfig;

#include "DetourFunctions.h"
#include "ProxyFunctions.h"

ProxyFunctions* g_pProxyFunctions = NULL;
DetourFunctions* g_pDetourFunctions = NULL;

// Use a distinct value so it's easier to find in memory!

static int g_iLithFixVersion = 133717002; 

#if 1
// SDL Logging
std::fstream g_SDLLogFile;

void SDLLog(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
	// Open up SDL Log File
	g_SDLLogFile.open("Lithfix.log", std::ios::out | std::ios::app);

	g_SDLLogFile << message << "\n";

	g_SDLLogFile.close();
}
#endif

//FARPROC GetClientShellFunctionsPtr;
// Exports Ptrs
FARPROC GetClientShellVersion;
FARPROC SetInstanceHandle;

// FIXME: This is needed for the exports.
void _GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete)
{
	g_pDetourFunctions->GetClientShellFunctions(pCreate, pDelete);
}

__declspec(naked) void _GetClientShellVersion() { 
	__asm { jmp[GetClientShellVersion] } 
}
__declspec(naked) void _SetInstanceHandle() { 
	__asm { jmp[SetInstanceHandle] } 
}

///
/// MAIN
///
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	// Ignore if detour
	if (DetourIsHelperProcess()) {
		return TRUE;
	}

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

		// Init Config -- See Config.h
		InitConfig(g_sConfig);

		// If we're not debug, clear the debug log
#ifdef NDEBUG
		g_SDLLogFile.open("Lithfix.log", std::ios::out | std::ios::trunc);
		g_SDLLogFile << "";
		g_SDLLogFile.close();
#endif
		// Setup the logging function
		SDL_LogSetOutputFunction(&SDLLog, NULL);

		SDL_Log("LITHFIX Beta 2");
		SDL_Log("Hello world, Let's get going!");

		TCHAR szExeFileName[MAX_PATH];
		GetModuleFileName(NULL, szExeFileName, MAX_PATH);

		SDL_Log("EXE NAME: <%s>", szExeFileName);
		SDL_Log("Special Version Value: <%d>", g_iLithFixVersion);

		// Grab the hWND and turn it into a SDL_Window
		g_hSDLWindow = SDL_CreateWindowFrom(GetFocus());

		// TODO: Throw an error?
		if (!g_hSDLWindow) {
			SDL_Quit();
		}

		g_pDetourFunctions = new DetourFunctions();
		g_pProxyFunctions = new ProxyFunctions();

		HMODULE hLib;

		// Load the real dll file. Should be in the game's root
		// TODO: Make configurable
		hLib = LoadLibrary("./CShellReal.dll");

		if (!hLib) {
			return FALSE;
		}

		// Send the SetWindowPos pointer to our detour class
		g_pDetourFunctions->m_pSetWindowPos = (SetWindowPosFn*)SetWindowPos;

		// Send the GetClientShellFunctions pointer to our detour class
		g_pDetourFunctions->m_pGetClientShellFunctions = (GetClientShellFunctionsFn*)GetProcAddress(hLib, "GetClientShellFunctions");
		GetClientShellVersion = GetProcAddress(hLib, "GetClientShellVersion");
		SetInstanceHandle = GetProcAddress(hLib, "SetInstanceHandle");

		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

