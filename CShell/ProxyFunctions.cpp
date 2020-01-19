#include "ProxyFunctions.h"
#include "DetourFunctions.h"
#include <string>
#include <regex>
#include <SDL.h>
#include <vector>


extern SDL_Window* g_hSDLWindow;
extern DetourFunctions* g_pDetourFunctions;
extern Config g_sConfig;

// Random number!
static uint32 g_LithFixIdentifier = 869453262;
static uint32* g_pMissionNumber = NULL;
static uint32* g_pLevelNumber = NULL;

ProxyFunctions::ProxyFunctions()
{
	m_bGetBaseMouse = true;
	m_iCurrentMouseX = 0;
	m_iCurrentMouseY = 0;
	m_iPreviousMouseX = 0;
	m_iPreviousMouseY = 0;

	// This is the lowest "Scale" in the autoexec.cfg
	m_fMouseSensitivity = 0.001125f;

	m_bAllowMouse = true;

	m_lNextUpdate = 1L;

	m_bLockFramerate = true;

	// Get timer frequency
	if (!QueryPerformanceFrequency(&m_lTimerFrequency)) {
		SDL_Log("!! Device doesn't support high resolution timer! Can't lock framerate.");
		m_bLockFramerate = false;
	}

	m_lFrametime = (m_lTimerFrequency.QuadPart / g_sConfig.fMaxFramerate);

	m_bGetAxisOffsetCalledThisFrame = false;
	m_fOffsets[0] = m_fOffsets[1] = m_fOffsets[2] = 0;
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
	if (!m_bAllowMouse) {
		offsets[0] = 0;
		offsets[1] = 0;
		offsets[2] = 0;

		return;
	}

	// AVP2 seems to call GetAxisOffsets twice a frame
	// so use the cached results for the second call
	if (m_bGetAxisOffsetCalledThisFrame) {
		offsets[0] = m_fOffsets[0];
		offsets[1] = m_fOffsets[1];
		offsets[2] = m_fOffsets[2];

		
		return;
	}

	POINT lpPoint;
	int deltaX = 0, deltaY = 0;

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

	float nScale = m_fMouseSensitivity + (1.0f * m_fMouseSensitivity);

	// Nerf the sensitivity scale so it matches the OG games.
	nScale *= 0.10f;

	offsets[0] = (float)(m_iCurrentMouseX - m_iPreviousMouseX) * nScale;
	offsets[1] = (float)(m_iCurrentMouseY - m_iPreviousMouseY) * (nScale*2);
	offsets[2] = 0.0f;

	m_iPreviousMouseX = m_iCurrentMouseX;
	m_iPreviousMouseY = m_iCurrentMouseY;

	// Cache the results so it can be used again this frame
	m_fOffsets[0] = offsets[0];
	m_fOffsets[1] = offsets[1];
	m_fOffsets[2] = offsets[2];

	m_bGetAxisOffsetCalledThisFrame = true;
}

void ProxyFunctions::SetInputState(LTBOOL bOn)
{
	m_bAllowMouse = bOn;

	// Pass the state to the engine too!
	m_pSetInputState(bOn);
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

	// Okay we've hit the end of the frame, reset our inputs.
	m_bGetAxisOffsetCalledThisFrame = false;

	return m_pFlipScreen(flags);
}


LTBOOL ProxyFunctions::IsConnected()
{
	LTBOOL bIsConnected = m_pIsConnected();
	SDL_Log("Is Connected? %d", bIsConnected);
	return bIsConnected;
}

LTRESULT ProxyFunctions::SetRenderMode(RMode* pMode)
{
	SDL_Log("SetRenderMode!");
	auto test = true;

	

	return LTRESULT();
}
