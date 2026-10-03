/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoManager_c
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

#include "FxInfoManager.h"
#include "FxManager.h"

#include "EmissionInfo.h"
#include "MovementInfo.h"
#include "RenderInfo.h"

#include "FxInfoEmRate.h"
#include "FxInfoEmSize.h"
#include "FxInfoEmSpeed.h"
#include "FxInfoEmDir.h"
#include "FxInfoEmAngle.h"
#include "FxInfoEmLife.h"
#include "FxInfoEmPos.h"
#include "FxInfoEmWeather.h"
#include "FxInfoEmRotation.h"
#include "FxInfoNoise.h"
#include "FxInfoForce.h"
#include "FxInfoFriction.h"
#include "FxInfoAttractPt.h"
#include "FxInfoAttractLine.h"
#include "FxInfoGroundCollide.h"
#include "FxInfoWind.h"
#include "FxInfoJitter.h"
#include "FxInfoRotSpeed.h"
#include "FxInfoFloat.h"
#include "FxInfoUnderwater.h"
#include "FxInfoColour.h"
#include "FxInfoSize.h"
#include "FxInfoSpriteRect.h"
#include "FxInfoHeatHaze.h"
#include "FxInfoTrail.h"
#include "FxInfoFlat.h"
#include "FxInfoDir.h"
#include "FxInfoAnimTexture.h"
#include "FxInfoColourRange.h"
#include "FxInfoSelfLit.h"
#include "FxInfoColourBright.h"
#include "FxInfoSmoke.h"
#include "FxInfoRotate.h"
#include "FxInfoTexCoords.h"
#include "FxInfoRotateOffset.h"


/**
 * Registra la creación y procesamiento de los distintos bloques FxInfo.
 */
void FxInfoManager_c::InjectHooks() {
    RH_ScopedClass(FxInfoManager_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(AddFxInfo, 0x4A7B00);
    // RH_ScopedInstall(Load, 0x5C0B70);
    RH_ScopedInstall(ProcessEmissionInfo, 0x4A4960);
    RH_ScopedInstall(ProcessMovementInfo, 0x4A4A10);
    RH_ScopedInstall(ProcessRenderInfo, 0x4A4A80);
}

namespace {
// Como el original: cada FxInfo ocupa 0x18 bytes de la arena FX (g_fxMan, alineación 4) y, si la
// reserva devuelve nullptr, no se ejecuta el constructor.
template<typename T>
FxInfo_c* NewFxInfoInPool() {
    static_assert(sizeof(T) <= 0x18);
    void* mem = g_fxMan.GetMemPool().GetMem(0x18, 4);
    if (!mem) {
        return nullptr;
    }
    return new (mem) T;
}
} // namespace

// 0x4A7B00
// Instancia la subclase correspondiente al identificador de bloque del archivo FX.
FxInfo_c* FxInfoManager_c::AddFxInfo(int32 info) {
    switch (info) {
    case FX_INFO_EMRATE_DATA:        return NewFxInfoInPool<FxInfoEmRate_c>();
    case FX_INFO_EMSIZE_DATA:        return NewFxInfoInPool<FxInfoEmSize_c>();
    case FX_INFO_EMSPEED_DATA:       return NewFxInfoInPool<FxInfoEmSpeed_c>();
    case FX_INFO_EMDIR_DATA:         return NewFxInfoInPool<FxInfoEmDir_c>();
    case FX_INFO_EMANGLE_DATA:       return NewFxInfoInPool<FxInfoEmAngle_c>();
    case FX_INFO_EMLIFE_DATA:        return NewFxInfoInPool<FxInfoEmLife_c>();
    case FX_INFO_EMPOS_DATA:         return NewFxInfoInPool<FxInfoEmPos_c>();
    case FX_INFO_EMWEATHER_DATA:     return NewFxInfoInPool<FxInfoEmWeather_c>();
    case FX_INFO_EMROTATION_DATA:    return NewFxInfoInPool<FxInfoEmRotation_c>();
    case FX_INFO_NOISE_DATA:         return NewFxInfoInPool<FxInfoNoise_c>();
    case FX_INFO_FORCE_DATA:         return NewFxInfoInPool<FxInfoForce_c>();
    case FX_INFO_FRICTION_DATA:      return NewFxInfoInPool<FxInfoFriction_c>();
    case FX_INFO_ATTRACTPT_DATA:     return NewFxInfoInPool<FxInfoAttractPt_c>();
    case FX_INFO_ATTRACTLINE_DATA:   return NewFxInfoInPool<FxInfoAttractLine_c>();
    case FX_INFO_GROUNDCOLLIDE_DATA: return NewFxInfoInPool<FxInfoGroundCollide_c>();
    case FX_INFO_WIND_DATA:          return NewFxInfoInPool<FxInfoWind_c>();
    case FX_INFO_JITTER_DATA:        return NewFxInfoInPool<FxInfoJitter_c>();
    case FX_INFO_ROTSPEED_DATA:      return NewFxInfoInPool<FxInfoRotSpeed_c>();
    case FX_INFO_FLOAT_DATA:         return NewFxInfoInPool<FxInfoFloat_c>();
    case FX_INFO_UNDERWATER_DATA:    return NewFxInfoInPool<FxInfoUnderwater_c>();
    case FX_INFO_COLOUR_DATA:        return NewFxInfoInPool<FxInfoColour_c>();
    case FX_INFO_SIZE_DATA:          return NewFxInfoInPool<FxInfoSize_c>();
    case FX_INFO_SPRITERECT_DATA:    return NewFxInfoInPool<FxInfoSpriteRect_c>();
    case FX_INFO_HEATHAZE_DATA:      return NewFxInfoInPool<FxInfoHeatHaze_c>();
    case FX_INFO_TRAIL_DATA:         return NewFxInfoInPool<FxInfoTrail_c>();
    case FX_INFO_FLAT_DATA:          return NewFxInfoInPool<FxInfoFlat_c>();
    case FX_INFO_DIR_DATA:           return NewFxInfoInPool<FxInfoDir_c>();
    case FX_INFO_ANIMTEX_DATA:       return NewFxInfoInPool<FxInfoAnimTexture_c>();
    case FX_INFO_COLOURRANGE_DATA:   return NewFxInfoInPool<FxInfoColourRange_c>();
    case FX_INFO_SELFLIT_DATA:       return NewFxInfoInPool<FxInfoSelfLit_c>();
    case FX_INFO_COLOURBRIGHT_DATA:  return NewFxInfoInPool<FxInfoColourBright_c>();
    case FX_INFO_SMOKE_DATA:         return NewFxInfoInPool<FxInfoSmoke_c>();
    // NOTSA case FX_INFO_ROTATE_DATA:        return NewFxInfoInPool<FxInfoRotate_c>();
    // NOTSA case FX_INFO_TEXCOORDS_DATA:     return NewFxInfoInPool<FxInfoTexCoords_c>();
    // NOTSA case FX_INFO_ROTATEOFFSET_DATA:  return NewFxInfoInPool<FxInfoRotateOffset_c>();
    default:                         return nullptr;
    };
}

constexpr struct { int32 type; const char* name; } FXINFOMANAGER_C_LOAD_MAPPING[] = {
    { FX_INFO_EMRATE_DATA,           "FX_INFO_EMRATE_DATA:"        },
    { FX_INFO_EMSIZE_DATA,           "FX_INFO_EMSIZE_DATA:"        },
    { FX_INFO_EMSPEED_DATA,          "FX_INFO_EMSPEED_DATA:"       },
    { FX_INFO_EMDIR_DATA,            "FX_INFO_EMDIR_DATA:"         },
    { FX_INFO_EMANGLE_DATA,          "FX_INFO_EMANGLE_DATA:"       },
    { FX_INFO_EMLIFE_DATA,           "FX_INFO_EMLIFE_DATA:"        },
    { FX_INFO_EMPOS_DATA,            "FX_INFO_EMPOS_DATA:"         },
    { FX_INFO_EMWEATHER_DATA,        "FX_INFO_EMWEATHER_DATA:"     },
    { FX_INFO_EMROTATION_DATA,       "FX_INFO_EMROTATION_DATA:"    },
    { FX_INFO_NOISE_DATA,            "FX_INFO_NOISE_DATA:"         },
    { FX_INFO_FORCE_DATA,            "FX_INFO_FORCE_DATA:"         },
    { FX_INFO_FRICTION_DATA,         "FX_INFO_FRICTION_DATA:"      },
    { FX_INFO_ATTRACTPT_DATA,        "FX_INFO_ATTRACTPT_DATA:"     },
    { FX_INFO_ATTRACTLINE_DATA,      "FX_INFO_ATTRACTLINE_DATA:"   },
    { FX_INFO_GROUNDCOLLIDE_DATA,    "FX_INFO_GROUNDCOLLIDE_DATA:" },
    { FX_INFO_WIND_DATA,             "FX_INFO_WIND_DATA:"          },
    { FX_INFO_JITTER_DATA,           "FX_INFO_JITTER_DATA:"        },
    { FX_INFO_ROTSPEED_DATA,         "FX_INFO_ROTSPEED_DATA:"      },
    { FX_INFO_FLOAT_DATA,            "FX_INFO_FLOAT_DATA:"         },
    { FX_INFO_UNDERWATER_DATA,       "FX_INFO_UNDERWATER_DATA:"    },
    { FX_INFO_COLOUR_DATA,           "FX_INFO_COLOUR_DATA:"        },
    { FX_INFO_SIZE_DATA,             "FX_INFO_SIZE_DATA:"          },
    { FX_INFO_SPRITERECT_DATA,       "FX_INFO_SPRITERECT_DATA:"    },
    { FX_INFO_HEATHAZE_DATA,         "FX_INFO_HEATHAZE_DATA:"      },
    { FX_INFO_TRAIL_DATA,            "FX_INFO_TRAIL_DATA:"         },
    { FX_INFO_FLAT_DATA,             "FX_INFO_FLAT_DATA:"          },
    { FX_INFO_DIR_DATA,              "FX_INFO_DIR_DATA:"           },
    { FX_INFO_ANIMTEX_DATA,          "FX_INFO_ANIMTEX_DATA:"       },
    { FX_INFO_COLOURRANGE_DATA,      "FX_INFO_COLOURRANGE_DATA:"   },
    { FX_INFO_SELFLIT_DATA,          "FX_INFO_SELFLIT_DATA:"       },
    { FX_INFO_COLOURBRIGHT_DATA,     "FX_INFO_COLOURBRIGHT_DATA:"  },
    { FX_INFO_SMOKE_DATA,            "FX_INFO_SMOKE_DATA:"         },
    // NOTSA { FX_INFO_ROTATE_DATA,           "FX_INFO_ROTATE_DATA:"        },
    // NOTSA { FX_INFO_TEXCOORDS_DATA,        "FX_INFO_TEXCOORDS_DATA:"     },
    // NOTSA { FX_INFO_ROTATEOFFSET_DATA,     "FX_INFO_ROTATEOFFSET_DATA:"  },
};

// 0x5C0B70
/**
 * La implementación efectiva todavía se delega al ejecutable original mediante
 * plugin::CallMethod. El parser C++ posterior al return se conserva como reversing
 * incompleto/de referencia y no se ejecuta en esta ruta.
 */
void FxInfoManager_c::Load(FILESTREAM file, int32 version) {
    plugin::CallMethod<0x5C0B70, FxInfoManager_c*, FILESTREAM, int32>(this, file, version);
    return;

    ReadField<void>(file);
    m_nNumInfos = ReadField<int32>(file, "NUM_INFOS:");
    m_MovementOffset = -1;
    m_RenderOffset = -1;

    m_pInfos = g_fxMan.Allocate<FxInfo_c*>(m_nNumInfos);
    assert(m_pInfos);

    char line[256], field[128];
    for (auto& info : GetInfos()) {
        ReadLine(file, line, sizeof(line));
        VERIFY(sscanf(line, "%s", field) == 1);

        const auto GetType = [=]() -> int32 {
            for (auto& [type, name] : FXINFOMANAGER_C_LOAD_MAPPING) {
                if (strcmp(field, name) == 0) {
                    return type;
                }
            }
            NOTSA_UNREACHABLE("Unknown FX Info: %s", field); // NOTSA
        };
        auto infoType = GetType();

        bool bTimeModePrt = true;
        if ((infoType & 0xF000) > 0x1000 && version >= 0.7f) {
            bTimeModePrt = ReadField<bool>(file, "TIMEMODEPRT:");
        }

        info = AddFxInfo(infoType);
        assert(info); // printf("Failed to load info: %s\n", field)
        info->Load(file, version);
        ReadField<void>(file);

        info->m_bTimeModeParticle = bTimeModePrt;
    }

    if (m_RenderOffset == -1)
        m_RenderOffset = m_nNumInfos;

    if (m_MovementOffset == -1)
        m_MovementOffset = m_RenderOffset;
}

/* todo:
 *  info->m_nType
 *  replace raw iteration to *irange*
 *  ProcessRenderInfo: mult = prt->m_fCurrentLife / prt->m_fTotalLife;
 */

// 0x4A4960
// Aplica primero el estado base de emisión y luego los FxInfo del bloque de emisión.
void FxInfoManager_c::ProcessEmissionInfo(float currentTime, float mult, float totalTime, bool useConst, EmissionInfo_t* emission) {
    emission->Process(mult);

    for (auto i = 0; i < m_MovementOffset; i++) {
        auto& info = m_pInfos[i];
        if ((info->m_nType & 0x1000) != 0) {
            info->GetValue(currentTime, 0.0f, mult, totalTime, useConst, emission);
        }
    }
}

// a6 - always false
// 0x4A4A10
// Procesa únicamente los FxInfo clasificados en el rango de movimiento.
void FxInfoManager_c::ProcessMovementInfo(float currentTime, float mult, float totalTime, float length, bool useConst, MovementInfo_t* movementInfo) {
    movementInfo->Process();

    for (int i = m_MovementOffset; i < m_RenderOffset; i++) {
        auto& info = m_pInfos[i];
        if ((info->m_nType & 0x2000) != 0) {
            info->GetValue(currentTime, mult, totalTime, length, useConst, movementInfo);
        }
    }
}

// a6 - always false
// 0x4A4A80
// Procesa el tramo final de FxInfo que modifica color, tamaño, sprites y otros datos de render.
void FxInfoManager_c::ProcessRenderInfo(float currentTime, float mult, float totalTime, float length, bool useConst, RenderInfo_t* renderInfo) {
    renderInfo->Process();

    for (int i = m_RenderOffset; i < m_nNumInfos; i++) {
        auto& info = m_pInfos[i];
        if ((info->m_nType & 0xC000) != 0) { // 0xC000 == -0x4000u | m_nType >= 0x4000u
            info->GetValue(currentTime, mult, totalTime, length, useConst, renderInfo);
        }
    }
}
