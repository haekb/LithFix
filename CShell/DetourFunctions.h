#pragma once
#include "framework.h"

typedef void (*GetClientShellFunctionsFn)(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
typedef BOOL (WINAPI *SetWindowPosFn)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
/// Detour Functions Class
/// ---------------------
/// Contains functions to be used with the detours library
class DetourFunctions
{
public:
	DetourFunctions();
	~DetourFunctions();

	// Proxy Functions
	void GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
	BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);

	IClientShell* CreateClientShell(ILTClient* pClientDE);

	CreateClientShellFn* m_pCreateClientShell;
	GetClientShellFunctionsFn* m_pGetClientShellFunctions;
	SetWindowPosFn* m_pSetWindowPos;

	ILTClient* m_pLTClient = NULL;
	IClientShell* m_pClientShell = NULL;

protected:
	bool m_bDetourTransactionOngoing;
	bool m_bSetWindowPosOngoing;

	

#if 1

#endif


private:
};

// FIXME: Do I really need these bindings?
extern DetourFunctions* g_pDetourFunctions;

// We can't directly assign class functions to detours, so these basically pass through to our class functions.
// Annoying, so if you know a fix, PR plz!
inline void df_GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete) { g_pDetourFunctions->GetClientShellFunctions(pCreate, pDelete); };
inline BOOL WINAPI df_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int  X, int  Y, int  cx, int  cy, UINT uFlags) { return g_pDetourFunctions->SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags); };

// Version specific classes
inline IClientShell* df_CreateClientShell(ILTClient* pClientDE) { return g_pDetourFunctions->CreateClientShell(pClientDE); };