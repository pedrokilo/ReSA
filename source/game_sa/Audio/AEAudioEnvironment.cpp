/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEAudioEnvironment
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "AEAudioEnvironment.h"

auto& gAudioZoneToReverbEnvironmentMap = StaticRef<sReverbEnvironment[NUM_AUDIO_ENVIRONMENTS]>(0x8AD670);

#include "data/SoundAttenuationTable.h"

/**
 * Registra las utilidades que convierten posición, distancia y entorno en parámetros de audio 3D.
 */
void CAEAudioEnvironment::InjectHooks() {
    RH_ScopedClass(CAEAudioEnvironment);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(GetDopplerRelativeFrequency, 0x4D7E40);
    RH_ScopedInstall(GetDistanceAttenuation, 0x4D7F20);
    RH_ScopedInstall(GetDirectionalMikeAttenuation, 0x4D7F60);
    RH_ScopedInstall(GetReverbEnvironmentAndDepth, 0x4D8010);
    RH_ScopedNamedInstall(GetPositionRelativeToCamera_Vector, "GetPositionRelativeToCamera-vec", 0x4D80B0);
    RH_ScopedNamedInstall(GetPositionRelativeToCamera_Placeable, "GetPositionRelativeToCamera-placeable", 0x4D8340);
}

// 0x4D7E40
/**
 * Calcula el factor Doppler a partir del cambio de distancia entre dos muestras.
 *
 * Cambios de cámara, intervalos inválidos o velocidades físicas extremas devuelven
 * un factor neutro para evitar saltos audibles.
 */
float CAEAudioEnvironment::GetDopplerRelativeFrequency(float prevDist, float curDist, uint32 prevTime, uint32 curTime, float dopplerScale) {
    if (TheCamera.Get_Just_Switched_Status()) {
        return 1.0F;
    }

    const auto deltaDist = curDist - prevDist;
    if (dopplerScale == 0.0F || deltaDist == 0.0F || curTime <= prevTime) {
        return 1.0F;
    }

    const auto doppler = deltaDist * 1000.0F / (float)(curTime - prevTime) * dopplerScale;
    if (std::fabs(doppler) >= 340.0F) {
        return 1.0F;
    }

    return 340.0F / (std::clamp(doppler, -35.0F, 35.0F) + 340.0F);
}

// 0x4D7F20
// Consulta la tabla precalculada de atenuación y silencia distancias fuera de rango.
float CAEAudioEnvironment::GetDistanceAttenuation(float dist) {
    assert(dist >= 0.f);
    return dist < ATTENUATION_TABLE_MAX_DIST
        ? gSoundDistAttenuationTable[(uint32)std::floor(dist / ATTENUATION_TABLE_RESOLUTION)]
        : -100.f;
}

// 0x4D7F60
float CAEAudioEnvironment::GetDirectionalMikeAttenuation(const CVector& soundDir) {
    // https://en.wikipedia.org/wiki/Cutoff_frequency
    static constexpr float CUT_OFF_FREQ = 0.70710678118F; // sqrt(0.5F);
    static constexpr float ATTENUATION_FACTOR = -6.0F;

    // BUG? Seems weird that it uses just single axis,
    // seems like it should be normalized dot product
    // with for example Camera direction,
    // to work the same way regardless of direction
    const auto freq = soundDir.Dot({ 0.f, 1.f, 0.f });

    if (CUT_OFF_FREQ == -1.0F) {
        return 0.0F;
    }

    if (freq >= CUT_OFF_FREQ) {
        return 0.f;
    }

    if (freq <= -CUT_OFF_FREQ) {
        return ATTENUATION_FACTOR;
    }

    return (1.0f - invLerp(-CUT_OFF_FREQ, CUT_OFF_FREQ, freq)) * ATTENUATION_FACTOR;
}

// 0x4D8010
/**
 * Selecciona preset y profundidad de reverb según underwaterness, AudioZones y área.
 */
void CAEAudioEnvironment::GetReverbEnvironmentAndDepth(int8* reverbEnv, int32* depth) {
    if (CWeather::UnderWaterness >= 0.5F) {
        *reverbEnv = 22; // todo: EAX_ENVIRONMENT_UNDERWATER
        *depth = -12;
        return;
    }

    int32 nZone;
    if (CAudioZones::m_NumActiveSpheres > 0)
        nZone = CAudioZones::m_aSpheres[CAudioZones::m_aActiveSpheres[0]].m_nAudioZone;
    else if (CAudioZones::m_NumActiveBoxes > 0)
        nZone = CAudioZones::m_aBoxes[CAudioZones::m_aActiveBoxes[0]].m_nAudioZone;
    else if (CGame::currArea)
        nZone = 14;
    else {
        *reverbEnv = 23; // todo: EAX_ENVIRONMENT_DRUGGED
        *depth = -100;
        return;
    }

    if (nZone != -1) {
        *reverbEnv = gAudioZoneToReverbEnvironmentMap[nZone].m_nEnvironment;
        *depth     = gAudioZoneToReverbEnvironmentMap[nZone].m_nDepth;
        return;
    }

    *reverbEnv = 23; // todo: EAX_ENVIRONMENT_DRUGGED
    *depth = -100;
}

CVector CAEAudioEnvironment::GetPositionRelativeToCamera(const CVector& pos) {
    CVector result;
    GetPositionRelativeToCamera_Vector(&result, &pos);
    return result;
}

// 0x4D80B0
/**
 * Transforma una posición world al sistema relativo utilizado por el mixer.
 *
 * Conserva deliberadamente el comportamiento x87 del ejecutable para mantener
 * equivalencia numérica en las componentes y en el offset especial de primera persona.
 */
void CAEAudioEnvironment::GetPositionRelativeToCamera_Vector(CVector* output, const CVector* pos) {
    if (!pos) {
        return; // The executable leaves the output buffer untouched.
    }
    const auto* center = &TheCamera.GetPosition();
    const auto* matrix = &TheCamera.m_mCameraMatrix;
    const auto mode = CCamera::GetActiveCamera().m_nMode;
    const bool firstPerson = mode == eCamMode::MODE_SNIPER || mode == eCamMode::MODE_ROCKETLAUNCHER || mode == eCamMode::MODE_1STPERSON;
    float cameraOffset = -2.f;
    if (!firstPerson) {
        cameraOffset = 0.f;
        if (auto* player = FindPlayerPed()) {
            const auto* playerPos = &player->GetPosition();
            const float half = .5f;
            uint8 deltaXExt[10], deltaYExt[10], deltaZValue[10];
            __asm {
                mov eax, playerPos
                mov edx, center
                fld dword ptr [eax]
                fsub dword ptr [edx]
                fstp tbyte ptr deltaXExt
                fld dword ptr [eax + 4]
                fsub dword ptr [edx + 4]
                fstp tbyte ptr deltaYExt
                fld dword ptr [eax + 8]
                fsub dword ptr [edx + 8]
                fstp tbyte ptr deltaZValue
                fld tbyte ptr deltaXExt
                fmul st, st(0)
                fld tbyte ptr deltaYExt
                fmul st, st(0)
                faddp st(1), st
                fld tbyte ptr deltaZValue
                fmul st, st(0)
                faddp st(1), st
                fsqrt
                fsub half
                fstp cameraOffset
            }
            cameraOffset = std::clamp(cameraOffset, 0.f, .5f);
        }
    }
    // Preserve the original x87 intermediates: Y remains extended, Z is
    // rounded to float, and the lateral dot product adds Y+Z before X.
    uint8 deltaXExt[10], deltaYExt[10];
    float deltaZValue, temp;
    __asm {
        mov eax, matrix
        mov edx, center
        mov ecx, pos
        fld dword ptr [eax + 10h]
        fmul cameraOffset
        fstp temp
        fld temp
        fadd dword ptr [edx]
        fstp temp
        fld dword ptr [ecx]
        fsub temp
        fstp tbyte ptr deltaXExt
        fld dword ptr [eax + 14h]
        fmul cameraOffset
        fstp temp
        fld temp
        fadd dword ptr [edx + 4]
        fsubr dword ptr [ecx + 4]
        fstp tbyte ptr deltaYExt
        fld dword ptr [eax + 18h]
        fmul cameraOffset
        fadd dword ptr [edx + 8]
        fsubr dword ptr [ecx + 8]
        fstp deltaZValue
        mov edx, output
        fld tbyte ptr deltaYExt
        fmul dword ptr [eax + 4]
        fld deltaZValue
        fmul dword ptr [eax + 8]
        faddp st(1), st
        fld tbyte ptr deltaXExt
        fmul dword ptr [eax]
        faddp st(1), st
        fchs
        fstp dword ptr [edx]
        fld tbyte ptr deltaXExt
        fmul dword ptr [eax + 10h]
        fld tbyte ptr deltaYExt
        fmul dword ptr [eax + 14h]
        faddp st(1), st
        fld deltaZValue
        fmul dword ptr [eax + 18h]
        faddp st(1), st
        fstp dword ptr [edx + 4]
        fld tbyte ptr deltaXExt
        fmul dword ptr [eax + 20h]
        fld tbyte ptr deltaYExt
        fmul dword ptr [eax + 24h]
        faddp st(1), st
        fld deltaZValue
        fmul dword ptr [eax + 28h]
        faddp st(1), st
        fstp dword ptr [edx + 8]
    }
}

CVector CAEAudioEnvironment::GetPositionRelativeToCamera(CPlaceable* placeable) {
    CVector result;
    GetPositionRelativeToCamera_Placeable(&result, placeable);
    return result;
}

// 0x4D8340
void CAEAudioEnvironment::GetPositionRelativeToCamera_Placeable(CVector* output, CPlaceable* placeable) {
    GetPositionRelativeToCamera_Vector(output, &placeable->GetPosition());
}
