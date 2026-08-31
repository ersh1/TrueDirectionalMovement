#pragma once

#pragma warning(push)
#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>

//#include <xbyak/xbyak.h>
#include <SimpleIni.h>
#include "ModAPI.h"
#define SMOOTHCAM_API_COMMONLIB

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

using namespace std::literals;

namespace logger = SKSE::log;

namespace util
{
	using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

#define RELOCATION_OFFSET(SE, AE) REL::VariantOffset(SE, AE, 0).offset()
#define RELOCATION_OFFSET1799(SE, AE, AE1799) REL::VariantOffset(SE, REL::Module::get().version().compare(SKSE::RUNTIME_SSE_1_7_99) == std::strong_ordering::less ? AE : AE1799, 0).offset()
#define RELOCATION_OFFSET3(SE, AE, AE629, AE1799)                                                                \
	REL::VariantOffset(                                                                                          \
		SE,                                                                                                      \
		REL::Module::get().version().compare(SKSE::RUNTIME_SSE_1_7_99) != std::strong_ordering::less  ? AE1799 : \
		REL::Module::get().version().compare(SKSE::RUNTIME_SSE_1_6_629) != std::strong_ordering::less ? AE629 :  \
																										AE,      \
		0)                                                                                                       \
		.offset()
#include "Plugin.h"
