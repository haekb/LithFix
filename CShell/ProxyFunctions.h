#pragma once
#include "framework.h"
#include <iltclient.h>



/// Proxy Functions Class
/// ---------------------
/// Contains functions that replace engine level functions from g_pLTClient
class ProxyFunctions
{
public:
	ProxyFunctions();
	~ProxyFunctions();

	// Proxy functions
	void RunConsoleString(char* pString);
	void GetAxisOffsets(LTFLOAT* offsets);

	// Original function pointers
	void (*m_pRunConsoleString)(char* pString) = NULL;

protected:
	// Actual variables
	bool m_bGetBaseMouse;
	int  m_iCurrentMouseX;
	int  m_iCurrentMouseY;
	int  m_iPreviousMouseX;
	int  m_iPreviousMouseY;


private:

};

// FIXME: Do I really need these bindings?
extern ProxyFunctions* g_pProxyFunctions;

inline void pf_RunConsoleString(char* pString) { g_pProxyFunctions->RunConsoleString(pString); };
inline void pf_GetAxisOffsets(LTFLOAT* offsets) { g_pProxyFunctions->GetAxisOffsets(offsets); };