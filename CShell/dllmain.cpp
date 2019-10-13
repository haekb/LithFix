// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include <detours.h>
#include <iltclient.h>
#include <SDL.h>
#include <string>
#include <fstream>

SDL_Window* g_hSDLWindow = NULL;

#include "DetourFunctions.h"
#include "ProxyFunctions.h"
ProxyFunctions* g_pProxyFunctions = NULL;
DetourFunctions* g_pDetourFunctions = NULL;
//extern ProxyFunctions* g_pProxyFunctions;

#if 1
// SDL Logging
std::fstream g_SDLLogFile;

void SDLLog(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
	// Open up SDL Log File
	g_SDLLogFile.open("Debug.log", std::ios::out | std::ios::app);

	g_SDLLogFile << message << "\n";

	g_SDLLogFile.close();
}
#endif

ILTClient* g_pLTClient = NULL;
static CreateClientShellFn* g_pCreatePtr = NULL;

#if 1
void (__stdcall *GetClientShellFunctions)(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
typedef void (*GetClientShellFunctionsCast)(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);


static IClientShell* (*CreateClientShellReal)(ILTClient* pClientDE) = NULL;
#else
typedef void (*GetClientShellFunctions)(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
#endif
FARPROC GetClientShellFunctionsPtr;
FARPROC GetClientShellVersion;
FARPROC SetInstanceHandle;

static int g_iCentreX = 0;
static int g_iCentreY = 0;

static bool g_bCalculatedCentre = false;

static void (* g_pRunConsoleString)(char* pString) = NULL;
LTRESULT(*g_pTrackDevice)(DeviceInput* pInputArray, uint32* pnInOut) = NULL;

#if 1
static IClientShell* CreateClientShellProxy(ILTClient* pClientDE)
{
	g_pLTClient = pClientDE;

	g_pLTClient->GetAxisOffsets = pf_GetAxisOffsets;

	g_pProxyFunctions->m_pRunConsoleString = g_pLTClient->RunConsoleString;
	g_pLTClient->RunConsoleString = pf_RunConsoleString;

	return ((CreateClientShellFn)g_pCreatePtr)(pClientDE);
}

void GetClientShellFunctionsProxy(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete)
{
	// Call the real GetClientShellFunctions
	((GetClientShellFunctionsCast)GetClientShellFunctionsPtr)(pCreate, pDelete);

	if (g_hSDLWindow) {
		RECT rect;
		GetWindowRect(GetFocus(), &rect);
		
		SDL_SetWindowPosition(g_hSDLWindow, rect.left, rect.top);
	}
	
	// Set the Create function pointer
	//CreateClientShell = (CreateClientShellCast)pCreate;

	g_pCreatePtr = (CreateClientShellFn*)*pCreate;
	if (1) {
//		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		auto err = DetourAttach(&(PVOID&)g_pCreatePtr, CreateClientShellProxy);

		DetourTransactionCommit();
	}
}



#else
__declspec(naked) void _GetClientShellFunctions()
{ 
	__asm { jmp[GetClientShellFunctions] }

	//((GetClientShellFunctions)GetClientShellFunctionsPtr)(pCreate, pDelete);

	test();
	//__asm { jmp[GetClientShellFunctions] }
}
#endif
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
		SDL_LogSetOutputFunction(&SDLLog, NULL);

		//g_hSDLWindow = SDL_CreateWindow("Hidden", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 960, 0);
		g_hSDLWindow = SDL_CreateWindowFrom(GetFocus());

		g_pDetourFunctions = new DetourFunctions();
		g_pProxyFunctions = new ProxyFunctions();

		if (!g_hSDLWindow) {
			ASSERT(1);
			SDL_Quit();
		}

		SDL_SetWindowSize(g_hSDLWindow, 1280, 960);

		HMODULE hLib;
		hLib = LoadLibrary(L"./CShellReal.dll");

		if (!hLib) {
			return FALSE;
		}

		g_pDetourFunctions->m_pGetClientShellFunctions = (GetClientShellFunctionsFn*)GetProcAddress(hLib, "GetClientShellFunctions");
#if 1
		{
			//g_pDetourFunctions->StartTransaction();
			//g_pDetourFunctions->AttachFunction(GetClientShellFunctions, GetClientShellFunctionsProxy);
			//g_pDetourFunctions->CommitTransaction();
			/*
			DetourRestoreAfterWith();

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			//DetourAttach(&(PVOID&)g_pCreatePtr, CreateClientShellProxy);
			DetourAttach(&(PVOID&)GetClientShellFunctions, GetClientShellFunctionsProxy);

			DetourTransactionCommit();
			*/
		}
#else
		GetClientShellFunctionsPtr = GetProcAddress(hLib, "GetClientShellFunctions");
#endif

		GetClientShellVersion = GetProcAddress(hLib, "GetClientShellVersion");
		SetInstanceHandle = GetProcAddress(hLib, "SetInstanceHandle");


		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		//DetourDetach(&(PVOID&)TrueSleep, TimedSleep);
		DetourTransactionCommit();
        break;
    }
    return TRUE;
}

