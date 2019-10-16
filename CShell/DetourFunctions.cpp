#include "DetourFunctions.h"
#include "framework.h"
#include <detours.h>
#include <SDL.h>
#include "ProxyFunctions.h"


extern SDL_Window* g_hSDLWindow;
extern Config g_sConfig;

// Mimicing regions
#define PROXY

DetourFunctions::DetourFunctions()
{
	m_bDetourTransactionOngoing = false;
	m_pCreateClientShell = NULL;
	m_pGetClientShellFunctions = NULL;
	m_pSetWindowPos = NULL;
	m_bSetWindowPosOngoing = false;

	m_bWindowedMode = false;
	m_bFirstRun = true;
}

DetourFunctions::~DetourFunctions()
{
	// Detach!
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)m_pCreateClientShell, df_CreateClientShell);
	DetourDetach(&(PVOID&)m_pSetWindowPos, df_SetWindowPos);
	DetourTransactionCommit();
}

// 
// Detour Functions
//
#ifdef PROXY
void DetourFunctions::GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete)
{
	// Call the real GetClientShellFunctions
	((GetClientShellFunctionsFn)m_pGetClientShellFunctions)(pCreate, pDelete);

	g_pDetourFunctions->m_pCreateClientShell = (CreateClientShellFn*)*pCreate;

	// Replace some functions with our own!
	// df_* functions are redirect functions that point to our class function
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// Attach some functions!
	DetourAttach(&(PVOID&)m_pCreateClientShell, df_CreateClientShell);
#ifndef LITH_SANITY
	DetourAttach(&(PVOID&)m_pSetWindowPos, df_SetWindowPos);
#endif

	DetourTransactionCommit();
}

IClientShell* DetourFunctions::CreateClientShell(ILTClient* pClientDE)
{
	SDL_Log(">> Running Detoured CreateClientShell");

	m_pLTClient = pClientDE;

	// Shogo has no mouse input on menus
#ifdef LITH_SHOGO
	if (g_hSDLWindow) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
#endif




#ifdef LITH_SANITY

#if 1
	int* pIntLTClient = (int*)m_pLTClient;

	int offset = offsetof(ILTClient, SetRenderMode);
	int offset2 = offsetof(ILTClient, GetRenderModes);

	int* test = (int*)m_pLTClient->SetRenderMode;
	//0x024BED04+0x24C

	//0x255EAD8+50h

	//0x38A5064
	int* RenderModePtr = pIntLTClient;//+72;//0x22C;//0x24;

	//int* RenderModePtr = (int*)m_pLTClient->SetRenderMode;

	SDL_Log("-- Hooking SetRenderMode Engine: <%p> Detoured: <%p>", RenderModePtr, pf_SetRenderMode);


	RMode pMode;

	RMode* RPTR = &pMode;

	g_pProxyFunctions->m_pSetRenderMode = (SetRenderModeFn*)*RenderModePtr;

	// TODO: clean up
	int* t = (int*)RenderModePtr;
	int off = 0x50;

	int* ptr = (int*)((char*)t + off);


#if 0
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	int* t = (int*)RenderModePtr;
	int off = 0x50;

	int* ptr = (int*) ((char*)t + off);

	//int* ptr = ((int*)RenderModePtr + 0x50);  //(int*) (*pIntLTClient + 0x50);

	// Attach some functions!
	DetourAttach(&(PVOID&)ptr, pf_SetRenderMode);
	DetourTransactionCommit();
#endif

	// Put that function pointer into the pointer at ptr
	*ptr = (int)pf_SetRenderMode;

#else
	int offset = offsetof(ILTClient, SetRenderMode);
	SDL_Log("-- Hooking SetRenderMode Engine: <%p> Detoured: <%p>", m_pLTClient->SetRenderMode, pf_SetRenderMode);
	g_pProxyFunctions->m_pSetRenderMode = m_pLTClient->SetRenderMode;
	m_pLTClient->SetRenderMode = pf_SetRenderMode;
	SDL_Log("-- Hooked SetRenderMode");
#endif

	//RenderModePtr = (int*)pf_SetRenderMode;
	SDL_Log("-- Hooked SetRenderMode");

	//RenderModePtr


#else
	SDL_Log("-- Hooking GetAxisOffsets Engine: <%p> Detoured: <%p>", m_pLTClient->GetAxisOffsets, pf_GetAxisOffsets);
	g_pProxyFunctions->m_pGetAxisOffsets = m_pLTClient->GetAxisOffsets;
	m_pLTClient->GetAxisOffsets = pf_GetAxisOffsets;
	SDL_Log("-- Hooked GetAxisOffsets");

	SDL_Log("-- Hooking RunConsoleString Engine: <%p> Detoured: <%p>", m_pLTClient->RunConsoleString, pf_RunConsoleString);
	g_pProxyFunctions->m_pRunConsoleString = m_pLTClient->RunConsoleString;
	m_pLTClient->RunConsoleString = pf_RunConsoleString;
	SDL_Log("-- Hooked RunConsoleString");

	SDL_Log("-- Hooking FlipScreen Engine: <%p> Detoured: <%p>", m_pLTClient->FlipScreen, pf_FlipScreen);
	g_pProxyFunctions->m_pFlipScreen = m_pLTClient->FlipScreen;
	m_pLTClient->FlipScreen = pf_FlipScreen;
	SDL_Log("-- Hooked FlipScreen");

	SDL_Log("-- Hooking SetRenderMode Engine: <%p> Detoured: <%p>", m_pLTClient->SetRenderMode, pf_SetRenderMode);
	g_pProxyFunctions->m_pSetRenderMode = m_pLTClient->SetRenderMode;
	m_pLTClient->SetRenderMode = pf_SetRenderMode;
	SDL_Log("-- Hooked SetRenderMode");
#endif


	//int* t = (int*)0x77327009;
	//int* o = (int*)*t;



	IClientShell* pClientShell = ((CreateClientShellFn)m_pCreateClientShell)(pClientDE);

	// TODO: Cast to CGameClientShell!
	//m_pClientShell = (CGameClientShell)pClientShell;
	m_pClientShell = pClientShell;



	SDL_Log(">> Finished Detoured CreateClientShell");
	return pClientShell;
}

void DetourFunctions::FirstRun()
{
	auto hMaxFps = m_pLTClient->GetConsoleVar("lf_max_fps");
	if (hMaxFps) {
		FloatVar* fMaxFps = (FloatVar*)hMaxFps;
		g_sConfig.fMaxFramerate = fMaxFps->value;
		g_pProxyFunctions->SetMaxFramerate();
	}
	else {
		g_sConfig.fMaxFramerate = 60.0f;
		g_pProxyFunctions->SetMaxFramerate();
	}

	auto hWindowFix = m_pLTClient->GetConsoleVar("lf_window_fix");
	if (hWindowFix) {
		FloatVar* fWindowFix = (FloatVar*)hWindowFix;
		g_sConfig.bWindowFix = fWindowFix->value == 0.0f ? false : true;
	}

}

// Override SetWindowPos so we can centre the window in windowed mode
BOOL DetourFunctions::SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (m_bSetWindowPosOngoing) {
		return TRUE;
	}

	m_bSetWindowPosOngoing = true;

	// HACK: Client is ready here to read in console variables,
	// so let's do it! 
	if (m_bFirstRun) {
		//FirstRun();
		m_bFirstRun = false;
	}

	// Minor hack, there's a function pointer mismatch with our headers and `GetVarValueFloat`
	// So I reversed engineered the structure. Ezpz.
	HCONSOLEVAR hVar = m_pLTClient->GetConsoleVar("Windowed");

	if (hVar) {
		FloatVar* fVal = (FloatVar*)hVar;
		m_bWindowedMode = (bool)fVal->value;
	}

	RECT rect;

	GetClientRect(GetDesktopWindow(), &rect);

	int mX = rect.right / 2;
	int mY = rect.bottom / 2;

	mX -= cx / 2;
	mY -= cy / 2;

	// Adjust for windowed borders.
	if (g_sConfig.bWindowFix && m_bWindowedMode) {
		cx -= 16;
		cy -= 39;
	}
	
	SDL_SetWindowFullscreen(g_hSDLWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

	SDL_SetWindowSize(g_hSDLWindow, cx, cy);

	BOOL ret = ((SetWindowPosFn)m_pSetWindowPos)(hWnd, hWndInsertAfter, mX, mY, cx, cy, uFlags);



	m_bSetWindowPosOngoing = false;

	return ret;
}

#endif