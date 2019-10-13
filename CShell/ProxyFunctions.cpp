#include "ProxyFunctions.h"
#include "DetourFunctions.h"
#include <string>
#include <regex>
#include <SDL.h>


extern SDL_Window* g_hSDLWindow;
extern DetourFunctions* g_pDetourFunctions;

ProxyFunctions::ProxyFunctions()
{
	m_bGetBaseMouse = true;
	m_iCurrentMouseX = 0;
	m_iCurrentMouseY = 0;
	m_iPreviousMouseX = 0;
	m_iPreviousMouseY = 0;

	m_fMouseSensitivity = 0.001125f;

	m_lNextUpdate = 1L;

	m_bLockFramerate = true;

	// Get timer frequency
	if (!QueryPerformanceFrequency(&m_lTimerFrequency)) {
		SDL_Log("!! Device doesn't support high resolution timer! Can't lock framerate.");
		m_bLockFramerate = false;
	}

	m_lFrametime = (m_lTimerFrequency.QuadPart / 60);

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
	
	if (cmd.find("Mouse") != std::string::npos && cmd.find("X-axis") != std::string::npos) {

		std::regex commandRegex("([\d\.]+)");
		std::smatch match;

		std::string result = "";
		std::string search = cmd;

		if (std::regex_search(search, match, commandRegex))
		{
			result = match[0].first._Ptr;

			//search = match.suffix().str();
		}

		if (!result.empty()) {
			SDL_Log("!! Found mouse sensitivity <%s>", result.c_str());
			m_fMouseSensitivity = atof(result.c_str());
		}
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
	//float nMouseSensitivity = 1.0f;
	
	float nScale = m_fMouseSensitivity + (1.0f * m_fMouseSensitivity);

	offsets[0] = (float)(m_iCurrentMouseX - m_iPreviousMouseX) * nScale;
	offsets[1] = (float)(m_iCurrentMouseY - m_iPreviousMouseY) * nScale;
	offsets[2] = 0;

	m_iPreviousMouseX = m_iCurrentMouseX;
	m_iPreviousMouseY = m_iCurrentMouseY;

#endif
}

LTRESULT ProxyFunctions::FlipScreen(uint32 flags)
{

	if (m_bLockFramerate) {
		// Limit our framerate so the game actually runs properly.
		LARGE_INTEGER NewTime;

		// Just burn the CPU, it's more reliable timing wise. 
		while (1) {
			QueryPerformanceCounter(&NewTime);
			unsigned long lTime = NewTime.QuadPart - m_lNextUpdate;
			if (lTime > m_lFrametime) {
				m_lNextUpdate = NewTime.QuadPart;
				break;
			}
		}
	}

	return m_pFlipScreen(flags);
}
