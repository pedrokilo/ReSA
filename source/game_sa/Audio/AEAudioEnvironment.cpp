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
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "AEAudioEnvironment.h"

auto& gAudioZoneToReverbEnvironmentMap = StaticRef<sReverbEnvironment[NUM_AUDIO_ENVIRONMENTS]>(0x8AD670);

// Variables globales del EXE que el original lee (no constantes)
static auto& gMikeCutOff          = StaticRef<float>(0x8AC258); // 0.707
static auto& gMikeCutOffLow       = StaticRef<float>(0x8AC25C); // -0.707
static auto& gMikeAttenuation     = StaticRef<float>(0x8AC260); // -6
static auto& gCameraOffsetMax     = StaticRef<float>(0x8AC264); // 0.5
static auto& gCameraOffsetSub     = StaticRef<float>(0x8AC268); // 0.5
static auto& gCameraOffset1stPers = StaticRef<float>(0x8AC26C); // 2
static auto& gExeAttenuationTable = StaticRef<std::array<float, 1280>>(0x8AC270);

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
// Consulta la tabla de atenuación del EXE (índice floor(dist · 10)) y silencia distancias fuera de rango.
float CAEAudioEnvironment::GetDistanceAttenuation(float dist) {
    if (!(dist < ATTENUATION_TABLE_MAX_DIST)) {
        return -100.f;
    }
    return gExeAttenuationTable.data()[(int32)(int64)std::floor((double)(dist * 10.0f))];
}

// 0x4D7F60
// Como el original: normaliza una copia y usa solo su componente Y ((z + x) · 0 + y),
// con el corte y la atenuación leídos de variables globales del EXE.
float CAEAudioEnvironment::GetDirectionalMikeAttenuation(const CVector& soundDir) {
    CVector dir = soundDir;
    dir.Normalise();
    const float freq = (dir.z + dir.x) * 0.0f + dir.y;

    if (gMikeCutOff == -1.0f || !(freq < gMikeCutOff)) {
        return 0.0f;
    }
    if (!(freq > gMikeCutOffLow) && !std::isnan(freq)) {
        return gMikeAttenuation;
    }
    return (gMikeCutOff - freq) / (gMikeCutOff - gMikeCutOffLow) * gMikeAttenuation;
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

    // Como el original: una zona -1 (o ninguna activa) cae al área interior (zona 14) antes de "sin reverb".
    int16 nZone = -1;
    if (CAudioZones::m_NumActiveSpheres > 0)
        nZone = CAudioZones::m_aSpheres[CAudioZones::m_aActiveSpheres[0]].m_nAudioZone;
    else if (CAudioZones::m_NumActiveBoxes > 0)
        nZone = CAudioZones::m_aBoxes[CAudioZones::m_aActiveBoxes[0]].m_nAudioZone;

    if (nZone == -1) {
        if (!CGame::currArea) {
            *reverbEnv = 23; // todo: EAX_ENVIRONMENT_DRUGGED
            *depth = -100;
            return;
        }
        nZone = 14;
    }
    *reverbEnv = gAudioZoneToReverbEnvironmentMap[nZone].m_nEnvironment;
    *depth     = gAudioZoneToReverbEnvironmentMap[nZone].m_nDepth;
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
    float cameraOffset = -gCameraOffset1stPers;
    if (!firstPerson) {
        const float half = gCameraOffsetSub;
        cameraOffset = half - half;
        if (auto* player = FindPlayerPed()) {
            const auto* playerPos = &player->GetPosition();
            const float dx = playerPos->x - center->x, dy = playerPos->y - center->y, dz = playerPos->z - center->z;
            cameraOffset = std::sqrt(dx * dx + dy * dy + dz * dz) - half;
        }
        // Recorte del original: 0 si min(v, máx) < 0; si no, min(v, máx) (NaN -> máx)
        const float maxOffset = gCameraOffsetMax;
        const float t = cameraOffset < maxOffset ? cameraOffset : maxOffset;
        cameraOffset = 0.f > t ? 0.f : t;
    }
    // Mismo orden de operaciones que el EXE (x87 a 24 bits: cada operación se redondea a float).
    const auto* m = reinterpret_cast<const float*>(matrix);
    const float dx = pos->x - (m[4] * cameraOffset + center->x);
    const float dy = pos->y - (m[5] * cameraOffset + center->y);
    const float dz = pos->z - (m[6] * cameraOffset + center->z);
    output->x = -((dy * m[1] + dz * m[2]) + dx * m[0]);
    output->y = (dx * m[4] + dy * m[5]) + dz * m[6];
    output->z = (dx * m[8] + dy * m[9]) + dz * m[10];
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
