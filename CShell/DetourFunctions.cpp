#include "DetourFunctions.h"
#include "framework.h"
#include <detours.h>
#include <SDL.h>
#include "ProxyFunctions.h"


extern SDL_Window* g_hSDLWindow;

// Mimicing regions
#define PROXY

DetourFunctions::DetourFunctions()
{
	m_bDetourTransactionOngoing = false;
	m_pCreateClientShell = NULL;
	m_pGetClientShellFunctions = NULL;
	m_pSetWindowPos = NULL;
	m_bSetWindowPosOngoing = false;
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
	DetourAttach(&(PVOID&)m_pSetWindowPos, df_SetWindowPos);

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

	SDL_Log("-- Hooking GetAxisOffsets Engine: <%p> Detoured: <%p>", m_pLTClient->GetAxisOffsets, pf_GetAxisOffsets);
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

	SDL_Log(">> Finished Detoured CreateClientShell");

	IClientShell* pClientShell = ((CreateClientShellFn)m_pCreateClientShell)(pClientDE);

	// TODO: Cast to CGameClientShell!
	//m_pClientShell = (CGameClientShell)pClientShell;
	m_pClientShell = pClientShell;

	return pClientShell;
}

// Override SetWindowPos so we can centre the window in windowed mode
BOOL DetourFunctions::SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (m_bSetWindowPosOngoing) {
		return TRUE;
	}

	m_bSetWindowPosOngoing = true;

	RECT rect;

	GetWindowRect(GetDesktopWindow(), &rect);

	int mX = rect.right / 2;
	int mY = rect.bottom / 2;

	mX -= cx / 2;
	mY -= cy / 2;
	
	SDL_SetWindowSize(g_hSDLWindow, cx, cy);

	BOOL ret = ((SetWindowPosFn)m_pSetWindowPos)(hWnd, hWndInsertAfter, mX, mY, cx, cy, uFlags);


	m_bSetWindowPosOngoing = false;

	return ret;
}

#endif