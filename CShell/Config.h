#pragma once
// Holds config values
struct Config {

	// -1 for no limiter
	float fMaxFramerate;

	// Windowed mode
	bool bWindowed;

	// Determines if windowed mode should be borderless!
	bool bBorderlessWindow;


	// Feature flags -- these can't be changed during gameplay.
	bool bUseMouseLook;
	bool bUseFramerateLimiter;
	bool bUseSetWindowPos;

};

static void InitConfig(Config &config) {
	config = {
		60.0f,		// Framerate limiter
		false,		// Windowed mode
		false,		// Borderless windowed mode
		// Feature flags, always enabled by default
		true,		// 
		true,
		true
	};
};



