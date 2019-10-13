#pragma once
#include "framework.h"
#include <iltclient.h>
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

	// Some helper functions
	bool StartTransaction();
	bool CommitTransaction();
	template <typename T>
	bool AttachFunction(T* pRealPtr, T* pProxyPtr);
	bool DetachFunction(void* pRealPtr, void* pProxyPtr);

	// Proxy Functions
	void GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
	IClientShell* CreateClientShell(ILTClient* pClientDE);
	BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);


	//void(__stdcall* GetClientShellFunctions)(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
	


	CreateClientShellFn* m_pCreateClientShell;
	GetClientShellFunctionsFn* m_pGetClientShellFunctions;
	SetWindowPosFn* m_pSetWindowPos;

	
protected:
	bool m_bDetourTransactionOngoing;
	bool m_bSetWindowPosOngoing;

	ILTClient* m_pLTClient = NULL;

#if 1

#endif


private:
};

// FIXME: Do I really need these bindings?
extern DetourFunctions* g_pDetourFunctions;

// We can't directly assign class functions to detours, so these basically pass through to our class functions.
// Annoying, so if you know a fix, PR plz!
inline void df_GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete) { g_pDetourFunctions->GetClientShellFunctions(pCreate, pDelete); };
inline IClientShell* df_CreateClientShell(ILTClient* pClientDE) { return g_pDetourFunctions->CreateClientShell(pClientDE); };
inline BOOL WINAPI df_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int  X, int  Y, int  cx, int  cy, UINT uFlags) { return g_pDetourFunctions->SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags); };