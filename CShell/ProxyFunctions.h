#pragma once
#include "framework.h"

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
	LTRESULT FlipScreen(uint32 flags);

	// Original function pointers
	void (*m_pRunConsoleString)(char* pString) = NULL;
	LTRESULT (*m_pFlipScreen)(uint32 flags) = NULL;

protected:
	// Actual variables
	bool m_bGetBaseMouse;
	int  m_iCurrentMouseX;
	int  m_iCurrentMouseY;
	int  m_iPreviousMouseX;
	int  m_iPreviousMouseY;

	float m_fMouseSensitivity;

	// Framerate limiting stuff
	LONGLONG m_lNextUpdate;
	LONGLONG m_lFrametime;
	LARGE_INTEGER m_lTimerFrequency;
	bool m_bLockFramerate;
private:

};

// FIXME: Do I really need these bindings?
extern ProxyFunctions* g_pProxyFunctions;

inline void pf_RunConsoleString(char* pString) { g_pProxyFunctions->RunConsoleString(pString); };
inline void pf_GetAxisOffsets(LTFLOAT* offsets) { g_pProxyFunctions->GetAxisOffsets(offsets); };
inline LTRESULT pf_FlipScreen(uint32 flags) { return g_pProxyFunctions->FlipScreen(flags); };