#include "DetourFunctions.h"
#include "framework.h"
#include <detours.h>
#include <SDL.h>
#include "ProxyFunctions.h"

extern SDL_Window* g_hSDLWindow;

// Mimicing regions
//#define HELPERS
#define PROXY

DetourFunctions::DetourFunctions()
{
	m_bDetourTransactionOngoing = false;
	m_pCreateClientShell = NULL;
	m_pGetClientShellFunctions = NULL;
	m_pSetWindowPos = NULL;
}

DetourFunctions::~DetourFunctions()
{
}

//
// Helper functions
// FIXME: Figure out template nonsense
//
#ifdef HELPERS
bool DetourFunctions::StartTransaction()
{
	if (!m_bDetourTransactionOngoing) {
		DetourRestoreAfterWith();
		DetourTransactionBegin();

		m_bDetourTransactionOngoing = true;
	}

	return true;
}

bool DetourFunctions::CommitTransaction()
{
	// Nothing to commit!
	if (!m_bDetourTransactionOngoing) {
		return false;
	}

	DetourTransactionCommit();

	m_bDetourTransactionOngoing = false;

	return true;
}

template <typename T>
bool DetourFunctions::AttachFunction(T* pRealPtr, T* pProxyPtr)
{
	if (!m_bDetourTransactionOngoing) {
		StartTransaction();
	}

	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)pRealPtr, pProxyPtr);

	return true;
}

bool DetourFunctions::DetachFunction(void* pRealPtr, void* pProxyPtr)
{
	return false;
}
#endif

// 
// Detour Functions
//
#ifdef PROXY
void DetourFunctions::GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete)
{
	// Call the real GetClientShellFunctions
	((GetClientShellFunctionsFn)m_pGetClientShellFunctions)(pCreate, pDelete);

	
	if (g_hSDLWindow) {
		RECT rect;
		GetWindowRect(GetFocus(), &rect);

		SDL_SetWindowPosition(g_hSDLWindow, rect.left, rect.top);
	}
	

	g_pDetourFunctions->m_pCreateClientShell = (CreateClientShellFn*)*pCreate;
#ifndef HELPERS
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// Attach some functions!
	DetourAttach(&(PVOID&)m_pCreateClientShell, df_CreateClientShell);
	DetourAttach(&(PVOID&)m_pSetWindowPos, df_SetWindowPos);

	DetourTransactionCommit();
#else
	StartTransaction();
	AttachFunction<CreateClientShellFn*>(m_pCreateClientShell, df_CreateClientShell);
	CommitTransaction();
#endif
}

IClientShell* DetourFunctions::CreateClientShell(ILTClient* pClientDE)
{
	m_pLTClient = pClientDE;

	m_pLTClient->GetAxisOffsets = pf_GetAxisOffsets;

	g_pProxyFunctions->m_pRunConsoleString = m_pLTClient->RunConsoleString;
	m_pLTClient->RunConsoleString = pf_RunConsoleString;

	return ((CreateClientShellFn)m_pCreateClientShell)(pClientDE);
}

BOOL DetourFunctions::SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{

	BOOL ret = ((SetWindowPosFn)m_pSetWindowPos)(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

	return ret;
}

#endif