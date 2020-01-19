#pragma once
#include "framework.h"
#include <sdl.h>
extern Config g_sConfig;

#ifdef LITH_SANITY
	// We don't have headers for Sanity, so we need to typedef this stuff
	typedef LTRESULT (SetRenderModeFn)(RMode* pMode);

#endif

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
	void SetInputState(LTBOOL bOn);
	LTRESULT FlipScreen(uint32 flags);

	LTBOOL IsConnected();

	LTRESULT SetRenderMode(RMode* pMode);
	


	// Original function pointers
	void     (*m_pRunConsoleString)(char* pString) = NULL;
	LTRESULT (*m_pFlipScreen)(uint32 flags) = NULL;
	void	 (*m_pGetAxisOffsets)(LTFLOAT* offsets) = NULL;

	LTBOOL	 (*m_pIsConnected)() = NULL;	// Are we on a server currently?

	LTRESULT (*m_pSetRenderMode)(RMode* pMode) = NULL;
	void	 (*m_pSetInputState)(LTBOOL bOn) = NULL;

	void SetMaxFramerate() { if (!m_bLockFramerate) return; m_lFrametime = (m_lTimerFrequency.QuadPart / g_sConfig.fMaxFramerate); }

	RMode m_RMode;

protected:
	// Actual variables
	bool m_bGetBaseMouse;
	int  m_iCurrentMouseX;
	int  m_iCurrentMouseY;
	int  m_iPreviousMouseX;
	int  m_iPreviousMouseY;
	bool m_bAllowMouse;

	float m_fMouseSensitivity;

	// AVP2 calls GetAxisOffsets twice in the same frame...
	bool m_bGetAxisOffsetCalledThisFrame;
	float m_fOffsets[3];

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

inline void pf_SetInputState(LTBOOL bOn) { g_pProxyFunctions->m_pSetInputState(bOn); };
inline LTRESULT pf_SetRenderMode(RMode* pMode) { return g_pProxyFunctions->SetRenderMode(pMode); };
