#pragma once
#include "framework.h"

extern Config g_sConfig;

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
	LTBOOL IsConnected();

	// Original function pointers
	void     (*m_pRunConsoleString)(char* pString) = NULL;
	LTRESULT (*m_pFlipScreen)(uint32 flags) = NULL;
	void	 (*m_pGetAxisOffsets)(LTFLOAT* offsets) = NULL;
	LTBOOL	 (*m_pIsConnected)() = NULL;	// Are we on a server currently?

	void SetMaxFramerate() { if (!m_bLockFramerate) return; m_lFrametime = (m_lTimerFrequency.QuadPart / g_sConfig.fMaxFramerate); }

protected:
	// Actual variables
	bool m_bGetBaseMouse;
	int  m_iCurrentMouseX;
	int  m_iCurrentMouseY;
	int  m_iPreviousMouseX;
	int  m_iPreviousMouseY;

	float m_fMouseSensitivity;

#ifdef LITH_AVP2
	// AVP2 calls GetAxisOffsets twice in the same frame...
	bool m_bGetAxisOffsetCalledThisFrame;
	float m_fOffsets[3];
#endif

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
inline LTBOOL pf_IsConnected() { return g_pProxyFunctions->IsConnected(); };