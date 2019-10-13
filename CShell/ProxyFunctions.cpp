#include "ProxyFunctions.h"
#include <string>
#include <SDL.h>

extern SDL_Window* g_hSDLWindow;

ProxyFunctions::ProxyFunctions()
{
	m_bGetBaseMouse = true;
	m_iCurrentMouseX = 0;
	m_iCurrentMouseY = 0;
	m_iPreviousMouseX = 0;
	m_iPreviousMouseY = 0;
}

ProxyFunctions::~ProxyFunctions()
{
	
}

void ProxyFunctions::RunConsoleString(char* pString)
{
	std::string cmd = pString;

	// Ignore CursorCenter
	if (cmd.find("CursorCenter 1") != std::string::npos) {

		if (g_hSDLWindow) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
		return;
	}
	else if (cmd.find("CursorCenter 0") != std::string::npos) {

		if (g_hSDLWindow) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		// Intentionally pass through
	}

	// Otherwise pass it along!
	m_pRunConsoleString(pString);
}

void ProxyFunctions::GetAxisOffsets(LTFLOAT* offsets)
{
	POINT lpPoint;
	int deltaX = 0, deltaY = 0;
#if 0

	if (!g_bCalculatedCentre)
	{
		RECT rect;
		GetWindowRect(GetFocus(), &rect);
		g_iCentreX = (rect.left + rect.right) / 2;
		g_iCentreY = (rect.top + rect.bottom) / 2;
		g_bCalculatedCentre = true;
	}

	POINT pt;

	GetCursorPos(&pt);

	if (g_bCenterCursor) {
		//SetCursorPos(g_iCentreX, g_iCentreY);
	}

	LAST = NOW;
	NOW = SDL_GetPerformanceCounter();

	deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

	currentFrameTime += deltaTime;

	float nMouseSensitivity = 1.0f;//GetConsoleFloat("MouseSensitivity", 1.0f);
	float nScale = 0.00125f + ((float)nMouseSensitivity * 0.001125f);


	offsets[0] = (float)((int)pt.x - g_iPreviousMouseX) * nScale;
	offsets[1] = -((float)((int)pt.y - g_iPreviousMouseY) * nScale);
	offsets[2] = 0;

	g_iPreviousMouseX = pt.x;
	g_iPreviousMouseY = pt.y;
#else
	SDL_PumpEvents();

	// Firstly, we need a point of reference.
	// This conditional is here, in case we need to reset the mouse.
	if (m_bGetBaseMouse)
	{
		SDL_GetMouseState(&m_iCurrentMouseX, &m_iCurrentMouseY);
		m_bGetBaseMouse = false;
	}

	SDL_GetRelativeMouseState(&deltaX, &deltaY);

	m_iCurrentMouseX += deltaX;
	m_iCurrentMouseY += deltaY;

	// TODO: Clean up, Code is from GameSettings.
	float nMouseSensitivity = 1.0f;//GetConsoleFloat("MouseSensitivity", 1.0f);
	float nScale = 0.00125f + ((float)nMouseSensitivity * 0.001125f);

	offsets[0] = (float)(m_iCurrentMouseX - m_iPreviousMouseX) * nScale;
	offsets[1] = (float)(m_iCurrentMouseY - m_iPreviousMouseY) * nScale;
	offsets[2] = 0;

	m_iPreviousMouseX = m_iCurrentMouseX;
	m_iPreviousMouseY = m_iCurrentMouseY;

#endif
}