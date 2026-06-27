#pragma once

#include "API/DodgeFrameworkAPI.h"
#include "API/SmoothCamAPI.h"
#include "API/TrueHUDAPI.h"
#include "API/IDRC_API.h"
#include "API/OpenAnimationReplacerAPI-Functions.h"

struct APIs
{
	static inline SmoothCamAPI::IVSmoothCam3* SmoothCam = nullptr;
	static inline DODGEFRAMEWORK_API::IVDodgeFramework1* DodgeFramework = nullptr;
	static inline TRUEHUD_API::IVTrueHUD3* TrueHUD = nullptr;
	static inline IDRC_API::IVIDRC1* IDRC = nullptr;
	static inline OAR_API::Functions::IFunctionsInterface1* OAR_Functions = nullptr;

	static void RequestAPIs();
};
