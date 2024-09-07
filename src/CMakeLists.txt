set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

set(SOURCE_DIR "${ROOT_DIR}/src")
set(SOURCE_FILES
	"${SOURCE_DIR}/DirectionalMovementHandler.cpp"
	"${SOURCE_DIR}/DirectionalMovementHandler.h"
	"${SOURCE_DIR}/Events.cpp"
	"${SOURCE_DIR}/Events.h"
	"${SOURCE_DIR}/Hooks.cpp"
	"${SOURCE_DIR}/Hooks.h"
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/ModAPI.cpp"
	"${SOURCE_DIR}/ModAPI.h"
	"${SOURCE_DIR}/Offsets.h"
	"${SOURCE_DIR}/Papyrus.cpp"
	"${SOURCE_DIR}/Papyrus.h"
	"${SOURCE_DIR}/PCH.h"
	"${SOURCE_DIR}/Settings.cpp"
	"${SOURCE_DIR}/Settings.h"
	"${SOURCE_DIR}/TrueDirectionalMovementAPI.h"
	"${SOURCE_DIR}/Raycast.cpp"
	"${SOURCE_DIR}/Raycast.h"
	"${SOURCE_DIR}/Utils.cpp"
	"${SOURCE_DIR}/Utils.h"
	"${SOURCE_DIR}/API/APIManager.cpp"
	"${SOURCE_DIR}/API/APIManager.h"
	"${SOURCE_DIR}/API/DodgeFrameworkAPI.h"
	"${SOURCE_DIR}/API/SmoothCamAPI.h"
	"${SOURCE_DIR}/API/TrueHUDAPI.h"
	"${SOURCE_DIR}/Widgets/TargetLockReticle.cpp"
	"${SOURCE_DIR}/Widgets/TargetLockReticle.h"
)

source_group(TREE "${ROOT_DIR}" FILES ${SOURCE_FILES})

set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h")
configure_file(
	"${ROOT_DIR}/cmake/Plugin.h.in"
	"${VERSION_HEADER}"
	@ONLY
)

source_group("src" FILES "${VERSION_HEADER}")

configure_file(
	"${ROOT_DIR}/cmake/version.rc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	@ONLY
)

add_library(
	"${PROJECT_NAME}"
	SHARED
	${SOURCE_FILES}
	"${VERSION_HEADER}"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	"${ROOT_DIR}/.clang-format"
	"${ROOT_DIR}/.editorconfig"
)

target_compile_features(
	"${PROJECT_NAME}"
	PRIVATE
		cxx_std_20
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:/ZI>"
			"$<$<CONFIG:RELEASE>:/Zi;/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		"${PROJECT_NAME}"
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}/src"
		"${SOURCE_DIR}"
)

# dependency macros
macro(find_dependency_path DEPENDENCY FILE)
	# searches extern for dependencies and if not checks the environment variable
	if(NOT ${DEPENDENCY} STREQUAL "")
		# Check extern
		message(
			STATUS
			"Searching for ${DEPENDENCY} using file ${FILE}"
		)
		find_path(PATH
			${FILE}
			PATHS
			"../extern/${DEPENDENCY}"
			"extern/${DEPENDENCY}"
			"../external/${DEPENDENCY}"
			"external/${DEPENDENCY}")

		set("${DEPENDENCY}Path" "${PATH}")
		message("${PATH}")
		message("${DEPENDENCY}Path")
		message("${${DEPENDENCY}Path}")
		if("${${DEPENDENCY}Path}" STREQUAL "PATH-NOTFOUND")
			# Check path
			message(
				STATUS
				"Getting environment variable for ${DEPENDENCY}Path: $ENV{${DEPENDENCY}Path}"
			)
			set("${DEPENDENCY}Path" "$ENV{${DEPENDENCY}Path}")
		endif()

		if (NOT "${${DEPENDENCY}Path}" STREQUAL "")
			message(
				STATUS
				"Found ${DEPENDENCY} in ${${DEPENDENCY}Path}; adding"
			)
			add_subdirectory("${${DEPENDENCY}Path}" ${DEPENDENCY})
		endif()
	endif()
endmacro()

# dependencies
find_dependency_path(CommonLibSSE include/REL/Relocation.h)

if(("${CommonLibSSEPath}" STREQUAL "CommonLibSSEPath-NOTFOUND") OR "${CommonLibSSEPath}" STREQUAL "")
	# fallback to CommonLibSSEPath_NG from environment
	message(
		STATUS
		"Found CommonLibSSE from CommonLibSSEPath_NG environment variable"
	)
	add_subdirectory("$ENV{CommonLibSSEPath_NG}" CommonLibSSE EXCLUDE_FROM_ALL)
endif()

find_package(tomlplusplus REQUIRED CONFIG)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		tomlplusplus::tomlplusplus
		CommonLibSSE::CommonLibSSE
)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
		"${SOURCE_DIR}/PCH.h"
)

install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
	COMPONENT "main"
)

install(
	FILES
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION "/"
	COMPONENT "pdbs"
)

if("${COPY_OUTPUT}")
	add_custom_command(
		TARGET "${PROJECT_NAME}"
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${CompiledPluginsPath}/SKSE/Plugins/"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${CompiledPluginsPath}/SKSE/Plugins/"
		VERBATIM
	)
endif()
