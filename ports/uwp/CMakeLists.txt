cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME WindowsStore)
set(CMAKE_SYSTEM_VERSION 10.0.19041.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(xash3d-uwp LANGUAGES C CXX)
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})

set(XASH_UWP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

function(xash_resolve_path out_var value)
	if(NOT value)
		set(${out_var} "" PARENT_SCOPE)
	elseif(IS_ABSOLUTE "${value}")
		set(${out_var} "${value}" PARENT_SCOPE)
	else()
		get_filename_component(resolved "${XASH_UWP_ROOT}/${value}" ABSOLUTE)
		set(${out_var} "${resolved}" PARENT_SCOPE)
	endif()
endfunction()

set(XASH_UWP_DEPS_ROOT "$ENV{XASH_UWP_DEPS_ROOT}" CACHE PATH "UWP dependency install root, for example a uwp-dep checkout.")

set(XASH_UWP_ENGINE_ROOT "$ENV{XASH_UWP_ENGINE_ROOT}" CACHE PATH "Installed x64 Xash engine output containing xash.dll and engine modules.")
set(XASH_UWP_HLSDK_ROOT "$ENV{XASH_UWP_HLSDK_ROOT}" CACHE PATH "hlsdk-portable build-x64 output root.")
set(XASH_UWP_HL_DLL "$ENV{XASH_UWP_HL_DLL}" CACHE FILEPATH "Optional explicit hlsdk-portable server DLL.")
set(XASH_UWP_CLIENT_DLL "$ENV{XASH_UWP_CLIENT_DLL}" CACHE FILEPATH "Optional explicit hlsdk-portable client DLL.")
set(XASH_UWP_DEFAULT_REF "ref_gl" CACHE STRING "Renderer passed to Host_Main on startup.")
set(XASH_UWP_PACKAGE_VALVE_DATA "" CACHE PATH "Optional valve data folder to package for internal testing.")
set(XASH_UWP_EXTRA_DLLS "" CACHE STRING "Additional package DLLs, separated with semicolons.")

xash_resolve_path(XASH_UWP_RESOLVED_DEPS_ROOT "${XASH_UWP_DEPS_ROOT}")
xash_resolve_path(XASH_UWP_RESOLVED_ENGINE_ROOT "${XASH_UWP_ENGINE_ROOT}")
xash_resolve_path(XASH_UWP_RESOLVED_HLSDK_ROOT "${XASH_UWP_HLSDK_ROOT}")
xash_resolve_path(XASH_UWP_RESOLVED_HL_DLL "${XASH_UWP_HL_DLL}")
xash_resolve_path(XASH_UWP_RESOLVED_CLIENT_DLL "${XASH_UWP_CLIENT_DLL}")
xash_resolve_path(XASH_UWP_RESOLVED_VALVE_DATA "${XASH_UWP_PACKAGE_VALVE_DATA}")

if(NOT XASH_UWP_RESOLVED_DEPS_ROOT)
	message(FATAL_ERROR "Set XASH_UWP_DEPS_ROOT so CMake can find UWP dependencies.")
endif()

set(XASH_UWP_DEPS_BIN_DIR "${XASH_UWP_RESOLVED_DEPS_ROOT}/bin")
set(XASH_UWP_DEPS_LIB_DIR "${XASH_UWP_RESOLVED_DEPS_ROOT}/lib")
set(XASH_UWP_DEPS_INCLUDE_DIR "${XASH_UWP_RESOLVED_DEPS_ROOT}/include")
if(EXISTS "${XASH_UWP_RESOLVED_DEPS_ROOT}/x64/bin")
	set(XASH_UWP_DEPS_BIN_DIR "${XASH_UWP_RESOLVED_DEPS_ROOT}/x64/bin")
	set(XASH_UWP_DEPS_LIB_DIR "${XASH_UWP_RESOLVED_DEPS_ROOT}/x64/lib")
	set(XASH_UWP_DEPS_INCLUDE_DIR "${XASH_UWP_RESOLVED_DEPS_ROOT}/x64/include")
endif()

set(XASH_UWP_SDL_INCLUDE "${XASH_UWP_DEPS_INCLUDE_DIR}")
set(XASH_UWP_SDL_INCLUDE_SDL2 "${XASH_UWP_DEPS_INCLUDE_DIR}/SDL2")
set(XASH_UWP_SDL_LIB_DIR "${XASH_UWP_DEPS_LIB_DIR}")
set(XASH_UWP_SDL_DLL "${XASH_UWP_DEPS_BIN_DIR}/SDL2.dll")
set(XASH_UWP_SDL_LIB "${XASH_UWP_SDL_LIB_DIR}/SDL2.lib")
set(XASH_UWP_OPENGL_DLL "${XASH_UWP_DEPS_BIN_DIR}/opengl32.dll")
set(XASH_UWP_GALLIUM_DLL "${XASH_UWP_DEPS_BIN_DIR}/libgallium_wgl.dll")
set(XASH_UWP_LIBUWP_DLL "${XASH_UWP_DEPS_BIN_DIR}/libuwp.dll")
set(XASH_UWP_DXIL_DLL "${XASH_UWP_DEPS_BIN_DIR}/dxil.dll")
set(XASH_UWP_ZLIB_DLL "${XASH_UWP_DEPS_BIN_DIR}/z-1.dll")

foreach(required_path
	"${XASH_UWP_SDL_INCLUDE_SDL2}"
	"${XASH_UWP_SDL_DLL}"
	"${XASH_UWP_SDL_LIB}"
	"${XASH_UWP_OPENGL_DLL}"
	"${XASH_UWP_GALLIUM_DLL}"
	"${XASH_UWP_LIBUWP_DLL}"
	"${XASH_UWP_DXIL_DLL}"
	"${XASH_UWP_ZLIB_DLL}"
)
	if(NOT EXISTS "${required_path}")
		message(FATAL_ERROR "Missing UWP dependency: ${required_path}")
	endif()
endforeach()

if(NOT XASH_UWP_RESOLVED_ENGINE_ROOT)
	message(FATAL_ERROR "Set XASH_UWP_ENGINE_ROOT to the x64 Xash install/output folder before configuring the UWP package.")
endif()

set(XASH_UWP_ENGINE_DLLS
	"${XASH_UWP_RESOLVED_ENGINE_ROOT}/xash.dll"
	"${XASH_UWP_RESOLVED_ENGINE_ROOT}/filesystem_stdio.dll"
	"${XASH_UWP_RESOLVED_ENGINE_ROOT}/ref_gl.dll"
	"${XASH_UWP_RESOLVED_ENGINE_ROOT}/ref_soft.dll"
	"${XASH_UWP_RESOLVED_ENGINE_ROOT}/menu.dll"
)

foreach(engine_dll ${XASH_UWP_ENGINE_DLLS})
	if(NOT EXISTS "${engine_dll}")
		message(FATAL_ERROR "Missing engine DLL: ${engine_dll}")
	endif()
endforeach()

if(NOT XASH_UWP_RESOLVED_HLSDK_ROOT)
	message(FATAL_ERROR "Set XASH_UWP_HLSDK_ROOT to hlsdk-portable/build-x64 before configuring the UWP package.")
endif()

if(NOT XASH_UWP_RESOLVED_HL_DLL)
	foreach(candidate
		"${XASH_UWP_RESOLVED_HLSDK_ROOT}/dlls/Release/hl_amd64.dll"
		"${XASH_UWP_RESOLVED_HLSDK_ROOT}/dlls/Release/hl.dll"
	)
		if(EXISTS "${candidate}")
			set(XASH_UWP_RESOLVED_HL_DLL "${candidate}")
			break()
		endif()
	endforeach()
endif()

if(NOT XASH_UWP_RESOLVED_CLIENT_DLL)
	foreach(candidate
		"${XASH_UWP_RESOLVED_HLSDK_ROOT}/cl_dll/Release/client_amd64.dll"
		"${XASH_UWP_RESOLVED_HLSDK_ROOT}/cl_dll/Release/client.dll"
	)
		if(EXISTS "${candidate}")
			set(XASH_UWP_RESOLVED_CLIENT_DLL "${candidate}")
			break()
		endif()
	endforeach()
endif()

foreach(game_dll "${XASH_UWP_RESOLVED_HL_DLL}" "${XASH_UWP_RESOLVED_CLIENT_DLL}")
	if(NOT EXISTS "${game_dll}")
		message(FATAL_ERROR "Missing hlsdk-portable x64 DLL: ${game_dll}")
	endif()
endforeach()

set(XASH_UWP_RUNTIME_DLLS
	"${XASH_UWP_SDL_DLL}"
	"${XASH_UWP_OPENGL_DLL}"
	"${XASH_UWP_GALLIUM_DLL}"
	"${XASH_UWP_LIBUWP_DLL}"
	"${XASH_UWP_DXIL_DLL}"
	"${XASH_UWP_ZLIB_DLL}"
	${XASH_UWP_EXTRA_DLLS}
)

foreach(runtime_dll ${XASH_UWP_RUNTIME_DLLS})
	if(NOT EXISTS "${runtime_dll}")
		message(FATAL_ERROR "Missing runtime DLL: ${runtime_dll}")
	endif()
endforeach()

add_executable(${PROJECT_NAME} WIN32
	main.cpp
	compat/xash_uwp_storage_root.c
	compat/xash_uwp_storage_root.h
	Package.appxmanifest
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	XASH_UWP_DEFAULT_REF="${XASH_UWP_DEFAULT_REF}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${XASH_UWP_SDL_INCLUDE}"
	"${XASH_UWP_SDL_INCLUDE_SDL2}"
)

target_link_directories(${PROJECT_NAME} PRIVATE
	"${XASH_UWP_SDL_LIB_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL2.lib
	WindowsApp.lib
)

set(PkgAssets
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/LargeTile.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/SmallTile.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/SplashScreen.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/Square44x44Logo.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/Square150x150Logo.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/StoreLogo.png"
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/WideTile.png"
)

set(UwpMenuAssets
	"${CMAKE_CURRENT_SOURCE_DIR}/Assets/uwp_cursor.tga"
)

set_source_files_properties(${PkgAssets} PROPERTIES
	VS_DEPLOYMENT_CONTENT TRUE
	VS_DEPLOYMENT_LOCATION "Assets"
)

set_source_files_properties(${UwpMenuAssets} PROPERTIES
	VS_COPY_TO_OUT_DIR Always
	VS_DEPLOYMENT_CONTENT TRUE
	VS_DEPLOYMENT_LOCATION "valve/gfx/shell"
)

set(BinDeps
	${XASH_UWP_RUNTIME_DLLS}
	${XASH_UWP_ENGINE_DLLS}
	"${CMAKE_CURRENT_SOURCE_DIR}/Package.appxmanifest"
)

set_source_files_properties(${BinDeps} PROPERTIES
	VS_COPY_TO_OUT_DIR Always
	VS_DEPLOYMENT_CONTENT TRUE
	VS_DEPLOYMENT_LOCATION "."
)

set_source_files_properties("${XASH_UWP_RESOLVED_HL_DLL}" PROPERTIES
	VS_COPY_TO_OUT_DIR Always
	VS_DEPLOYMENT_CONTENT TRUE
	VS_DEPLOYMENT_LOCATION "valve/dlls"
)

set_source_files_properties("${XASH_UWP_RESOLVED_CLIENT_DLL}" PROPERTIES
	VS_COPY_TO_OUT_DIR Always
	VS_DEPLOYMENT_CONTENT TRUE
	VS_DEPLOYMENT_LOCATION "valve/cl_dlls"
)

target_sources(${PROJECT_NAME} PRIVATE
	${PkgAssets}
	${UwpMenuAssets}
	${BinDeps}
	"${XASH_UWP_RESOLVED_HL_DLL}"
	"${XASH_UWP_RESOLVED_CLIENT_DLL}"
)

set(XASH_UWP_EXTRAS_PAK "${XASH_UWP_RESOLVED_ENGINE_ROOT}/valve/extras.pk3")
if(EXISTS "${XASH_UWP_EXTRAS_PAK}")
	set_source_files_properties("${XASH_UWP_EXTRAS_PAK}" PROPERTIES
		VS_COPY_TO_OUT_DIR Always
		VS_DEPLOYMENT_CONTENT TRUE
		VS_DEPLOYMENT_LOCATION "valve"
	)
	target_sources(${PROJECT_NAME} PRIVATE "${XASH_UWP_EXTRAS_PAK}")
endif()

if(XASH_UWP_RESOLVED_VALVE_DATA)
	if(NOT EXISTS "${XASH_UWP_RESOLVED_VALVE_DATA}")
		message(FATAL_ERROR "XASH_UWP_PACKAGE_VALVE_DATA does not exist: ${XASH_UWP_RESOLVED_VALVE_DATA}")
	endif()

	file(GLOB_RECURSE valve_files
		RELATIVE "${XASH_UWP_RESOLVED_VALVE_DATA}"
		"${XASH_UWP_RESOLVED_VALVE_DATA}/*"
	)

	foreach(valve_file ${valve_files})
		set(full_path "${XASH_UWP_RESOLVED_VALVE_DATA}/${valve_file}")
		get_filename_component(dest_dir "${valve_file}" DIRECTORY)
		set_source_files_properties("${full_path}" PROPERTIES
			VS_COPY_TO_OUT_DIR Always
			VS_DEPLOYMENT_CONTENT TRUE
			VS_DEPLOYMENT_LOCATION "valve/${dest_dir}"
		)
		target_sources(${PROJECT_NAME} PRIVATE "${full_path}")
	endforeach()
endif()
