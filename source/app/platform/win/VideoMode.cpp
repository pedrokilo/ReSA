/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: VideoMode
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "VideoMode.h"
#include "WinPlatform.h"
#include "PostEffects.h"

// Como el original: el EXE usa rwsprintf en RwEngineInstance+0xF0 y rwfree en +0x138
static_assert(offsetof(RwGlobals, stringFuncs) == 0xF0);
static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwfree) == 0x138);

// todo: "2014/12/07 [nick7]: is it related to streaming memory (45M default?)?"
#define GAME_FREE_VIDEO_MEM_REQUIRED (45 * 1024 * 1024)

#ifdef NOTSA_VIDEO_MODE_LOGS
    #define VIDEO_MODE_LOG(...) NOTSA_LOG_DEBUG(__VA_ARGS__)
#else
    #define VIDEO_MODE_LOG(...)
#endif

// 0x745AF0
char** GetVideoModeList() {
    if (gVideoModes) {
        if (gCurrentGpu == RwEngineGetCurrentSubSystem()) {
            return gVideoModes;
        }
        FreeVideoModeList();
    }
    gCurrentGpu = RwEngineGetCurrentSubSystem();

    // Como el original: recuento con signo (un valor negativo no entra en el bucle)
    const RwInt32 numVidModes = RwEngineGetNumVideoModes();
    assert(numVidModes != -1 && "Failed to get Video Modes");

    gVideoModes = (char**)CMemoryMgr::Calloc(numVidModes, sizeof(char*));

    for (RwInt32 modeId = 0; modeId < numVidModes; modeId++) {
        RwVideoMode vmi;
        RwEngineGetVideoModeInfo(&vmi, modeId);

        gVideoModes[modeId] = nullptr;
        if ((vmi.flags & rwVIDEOMODEEXCLUSIVE) == 0) {
            VIDEO_MODE_LOG("Unavailable video mode id={:02d}: {} X {} X {} [reason: video mode not exclusive]", modeId, videoMode.width, videoMode.height, videoMode.depth);
            continue;
        }

        if (vmi.width < APP_MINIMAL_WIDTH || vmi.height < APP_MINIMAL_HEIGHT) {
            VIDEO_MODE_LOG("Unavailable video mode id={:02d}: {} X {} X {} [reason: size]", modeId, videoMode.width, videoMode.height, videoMode.depth);
            continue;
        }

        // Como el original: comparación exacta con 0.8, 0.75, 0.6 y 0.625 (sin tolerancia ni 16:9)
        float fRatio = float(vmi.height) / float(vmi.width);
        if (fRatio != 0.8f && fRatio != 0.75f && fRatio != 0.6f && fRatio != 0.625f) {
            VIDEO_MODE_LOG("Unavailable video mode id={:02d}: {} X {} X {} [reason: ratio {:0.2f}]", modeId, videoMode.width, videoMode.height, videoMode.depth, fRatio);
            continue;
        }

        if (vmi.width != APP_MINIMAL_WIDTH || vmi.height != APP_MINIMAL_HEIGHT) {
            // Como el original (0x745BE5): el EXE resta de la VRAM total (0xC8CF80), no de la disponible
            if (s_OSStatus.VRAM.Total - vmi.height * vmi.width * vmi.depth / 8 <= GAME_FREE_VIDEO_MEM_REQUIRED) {
                continue;
            }
        }

        gVideoModes[modeId] = (char*)CMemoryMgr::Calloc(100, sizeof(char));                                  // 100 chars
        // Como el original: rwsprintf (RwEngineInstance->stringFuncs.vecSprintf), sin el relleno de sprintf_s
        RWSRCGLOBAL(stringFuncs).vecSprintf(gVideoModes[modeId], "%lu X %lu X %lu", vmi.width, vmi.height, vmi.depth);

        VIDEO_MODE_LOG("Available video mode id={:02d}: {}", modeId, gVideoModes[modeId]);
    }

    return gVideoModes;
}

// 0x745A80
bool FreeVideoModeList() {
    auto numVidModes = RwEngineGetNumVideoModes();
    if (gVideoModes) {
        // Como el original: se libera con RwFree (RwEngineInstance->memoryFuncs.rwfree), no con CMemoryMgr::Free
        for (auto mode = 0; mode < numVidModes; mode++) {
            RWSRCGLOBAL(memoryFuncs).rwfree(gVideoModes[mode]);
        }
        RWSRCGLOBAL(memoryFuncs).rwfree(gVideoModes);
        gVideoModes = nullptr;
    }
    return true;
}

// 0x745C70
void SetVideoMode(int32 mode) {
    // Como el original: sin comprobar RwEngineGetNumVideoModes (el assert añadía una llamada)
    VIDEO_MODE_LOG("Changing Video Mode to {} ({})", mode, gVideoModes ? gVideoModes[mode] : "unknown");

    RwD3D9ChangeVideoMode(mode);
    CPostEffects::SetupBackBufferVertex();
    RsGlobal.maximumWidth  = RwRasterGetWidth(RwCameraGetRaster(Scene.m_pRwCamera));
    RsGlobal.maximumHeight = RwRasterGetHeight(RwCameraGetRaster(Scene.m_pRwCamera));
}

// 0x745CA0
bool IsVideoModeExclusive() { // AKA isCurrentModeFullscreen
    RwVideoMode vmi;
    RwEngineGetVideoModeInfo(&vmi, gCurrentVideoMode);
    return vmi.flags & rwVIDEOMODEEXCLUSIVE;
}

void VideoModeInjectHooks() {
    RH_ScopedNamespaceName("VideoMode");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(GetVideoModeList, 0x745AF0);
    RH_ScopedGlobalInstall(FreeVideoModeList, 0x745A80);
    RH_ScopedGlobalInstall(SetVideoMode, 0x745C70);
    RH_ScopedGlobalInstall(IsVideoModeExclusive, 0x745CA0);
}
