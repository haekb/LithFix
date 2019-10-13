// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <detours.h>

#include <SDL.h>
#include <string>
#include <fstream>

SDL_Window* g_hSDLWindow = NULL;

#include "DetourFunctions.h"
#include "ProxyFunctions.h"

ProxyFunctions* g_pProxyFunctions = NULL;
DetourFunctions* g_pDetourFunctions = NULL;

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

	if (!g_pDetourFunctions) {
		__debugbreak();
	}

	g_pDetourFunctions->GetClientShellFunctions(pCreate, pDelete);

	//df_GetClientShellFunctions(pCreate, pDelete);
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

		// If we're not debug, clear the debug log
#ifdef NDEBUG
		g_SDLLogFile.open("Lithfix.log", std::ios::out | std::ios::trunc);
		g_SDLLogFile << "";
		g_SDLLogFile.close();
#endif

		SDL_LogSetOutputFunction(&SDLLog, NULL);

		// Raw input has some troubles, so let's use the NOLF2M approach!
		//SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1");


		SDL_Log("LITHFIX Beta 1");
		SDL_Log("Hello world, Let's get going!");

		//g_hSDLWindow = SDL_CreateWindow("Hidden", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, 0);
		g_hSDLWindow = SDL_CreateWindowFrom(GetFocus());

		g_pDetourFunctions = new DetourFunctions();
		g_pProxyFunctions = new ProxyFunctions();


		if (!g_hSDLWindow) {
			//ASSERT(1);
			SDL_Quit();
		}

		//SDL_SetWindowSize(g_hSDLWindow, 1280, 960);

		HMODULE hLib;
		hLib = LoadLibrary("./CShellReal.dll");

		if (!hLib) {
			return FALSE;
		}

		g_pDetourFunctions->m_pSetWindowPos = (SetWindowPosFn*)SetWindowPos;

		// Send the GetClientShellFunctions to our detour class.
		g_pDetourFunctions->m_pGetClientShellFunctions = (GetClientShellFunctionsFn*)GetProcAddress(hLib, "GetClientShellFunctions");

		//GetClientShellFunctionsPtr = GetProcAddress(hLib, "GetClientShellFunctions");
		GetClientShellVersion = GetProcAddress(hLib, "GetClientShellVersion");
		SetInstanceHandle = GetProcAddress(hLib, "SetInstanceHandle");


		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		/*
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		//DetourDetach(&(PVOID&)TrueSleep, TimedSleep);
		DetourTransactionCommit();
		*/
        break;
    }
    return TRUE;
}

