#pragma once

#include "Vector.h"

class CPlaceable;

/**
 * Mapeo compacto AudioZone -> preset de reverb y profundidad de sala.
 */
struct sReverbEnvironment {
    uint32 m_nEnvironment;
    int32  m_nDepth;
};

/**
 * Utilidades puras del motor de audio espacial.
 *
 * Centraliza Doppler, atenuación por distancia/dirección, selección de reverb y
 * transformación de posiciones al espacio relativo a cámara.
 */
class CAEAudioEnvironment {
public:
    static void InjectHooks();

    static float GetDopplerRelativeFrequency(float prevDist, float curDist, uint32 prevTime, uint32 curTime, float dopplerScale);
    static float GetDistanceAttenuation(float dist);
    static float GetDirectionalMikeAttenuation(const CVector& soundDir);
    static void  GetReverbEnvironmentAndDepth(int8* reverbEnv, int32* depth);
    static CVector  GetPositionRelativeToCamera(const CVector& pos);
    // Native ABI writes to an explicit output buffer and does not return it in EAX.
    static void GetPositionRelativeToCamera_Vector(CVector* out, const CVector* pos);
    static void GetPositionRelativeToCamera_Placeable(CVector* out, CPlaceable* placeable);
    static CVector  GetPositionRelativeToCamera(CPlaceable* placeable);
};

static constexpr int32 NUM_AUDIO_ENVIRONMENTS = 68;
extern sReverbEnvironment (&gAudioZoneToReverbEnvironmentMap)[NUM_AUDIO_ENVIRONMENTS];
