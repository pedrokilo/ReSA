#pragma once
#include "DatosExe/DataBssG2.h"
#include "DatosExe/DataBssE4.h"
#include "DatosExe/DataBssE2.h"
#include "DatosExe/DataInicializadaB.h"

#include "Platform.h"
#include <array>
#include <rwplcore.h>

//
// Sub Systems
//

//! Maximum number of subsystems we can deal with
//! 1 SubSystem / Display (And maybe / GPU ?)
constexpr auto MAX_SUBSYSTEMS = 16;

//! Subsystem infos (Populated using `RwEngineGetSubSystemInfo`)
static inline auto& GsubSysInfo = StaticRef<std::array<RwSubSystemInfo, MAX_SUBSYSTEMS>>(0xC8CFC0);

//! Number of subsystems
static inline auto& GnumSubSystems = DatosExe::Bss::bC920F0;

//! Currently selected subsystem
static inline auto& GcurSelSS = DatosExe::Bss::bC920F4;

//! Whenever there are multiple subsystems available
static inline auto& MultipleSubSystems = DatosExe::Bss::bC92118;

//
// Video Mode
//

//! Currently selected videomode
static inline auto& GcurSelVM = StaticRef<RwInt32>(0x8D6220); // VM = Video Mode

//! Whenever to use the default videomode (Instead of the user selecting it)
static inline auto& UseDefaultVM = DatosExe::Bss::bC920FC;

//! Unused shit
static inline auto& DefaultVM = StaticRef<RwBool>(0x8D2E34);

//! Whenever FrontEndMemnuManager videomode stuff was **NOT** yet set (See WinMain)
static inline auto& IsVMNotSelected = StaticRef<RwBool>(0x8D6218);

/*
* Dynamic array of video modes with format "width x height x depth"
*/
static inline auto& gVideoModes = DatosExe::Bss::bC920D0;
static inline auto& gCurrentGpu = DatosExe::Data::v8D6248;
static inline auto& gCurrentVideoMode = StaticRef<int32>(0x8D6220);

void VideoModeInjectHooks();

extern char** GetVideoModeList();
extern RwInt32 GetNumVideoModes(); // 0x745A70 (lote 196A, sin validar)
extern bool FreeVideoModeList();
extern void SetVideoMode(int32 mode);
extern bool IsVideoModeExclusive();
