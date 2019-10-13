#pragma once
#include <iltclient.h>
typedef void (*GetClientShellFunctionsFn)(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
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


	//void(__stdcall* GetClientShellFunctions)(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete);
	


	CreateClientShellFn* m_pCreateClientShell;
	GetClientShellFunctionsFn* m_pGetClientShellFunctions;


protected:
	bool m_bDetourTransactionOngoing;

	// FIX
	ILTClient* g_pLTClient = NULL;

#if 1

#endif


private:
};

// FIXME: Do I really need these bindings?
extern DetourFunctions* g_pDetourFunctions;

inline void df_GetClientShellFunctions(CreateClientShellFn* pCreate, DeleteClientShellFn* pDelete) { g_pDetourFunctions->GetClientShellFunctions(pCreate, pDelete); };
//IClientShell* df_CreateClientShell(ILTClient* pClientDE) { return g_pDetourFunctions->CreateClientShell(pClientDE); };