#pragma once

#include "API/DodgeFrameworkAPI.h"
#include "API/SmoothCamAPI.h"
#include "API/TrueHUDAPI.h"

struct APIs
{
	static inline SmoothCamAPI::IVSmoothCam3* SmoothCam = nullptr;
	static inline DODGEFRAMEWORK_API::IVDodgeFramework1* DodgeFramework = nullptr;
	static inline TRUEHUD_API::IVTrueHUD3* TrueHUD = nullptr;

	static void RequestAPIs();
};
