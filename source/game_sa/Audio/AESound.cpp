/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAESound
 * Reconstrucción: COMPLETA — 18/18 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 1/18 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include <reversiblebugfixes/Bugs.hpp>
#include "AESound.h"

#include "AEAudioEnvironment.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

/**
 * Registra el estado y cálculos de una voz lógica antes de asignarla al hardware.
 */
void CAESound::InjectHooks() {
    RH_ScopedClass(CAESound);
    RH_ScopedCategory("Audio");

    RH_ScopedInstall(operator=, 0x4EF680);
    RH_ScopedInstall(UnregisterWithPhysicalEntity, 0x4EF1A0);
    RH_ScopedInstall(StopSound, 0x4EF1C0);
    RH_ScopedInstall(SetFlags, 0x4EF2B0);
    RH_ScopedInstall(UpdatePlayTime, 0x4EF2E0);
    RH_ScopedOverloadedInstall(GetRelativePosition, "orginal", 0x4EF350, void(CAESound::*)(CVector*) const);
    RH_ScopedInstall(CalculateFrequency, 0x4EF390);
    RH_ScopedInstall(UpdateFrequency, 0x4EF3E0);
    RH_ScopedInstall(GetRelativePlaybackFrequencyWithDoppler, 0x4EF400);
    RH_ScopedInstall(GetSlowMoFrequencyScalingFactor, 0x4EF440);
    RH_ScopedInstall(NewVPSLEntry, 0x4EF7A0);
    RH_ScopedInstall(RegisterWithPhysicalEntity, 0x4EF820);
    RH_ScopedInstall(StopSoundAndForget, 0x4EF850);
    RH_ScopedInstall(SetPosition, 0x4EF880);
    RH_ScopedInstall(CalculateVolume, 0x4EFA10);
    RH_ScopedInstall(Initialise, 0x4EFE50);
    RH_ScopedInstall(UpdateParameters, 0x4EFF50);
    RH_ScopedInstall(SoundHasFinished, 0x4EFFD0);
}

CAESound::CAESound(
    eSoundBankSlot  bankSlot,
    eSoundID        sfxId,
    CAEAudioEntity* audioEntity,
    CVector         pos,
    float           volume,
    float           rollOff,
    float           speed,
    float           doppler,
    uint8           frameDelay,
    uint16          flags,
    float           speedVariance
) :
    m_BankSlot{ bankSlot },
    m_SoundID{ sfxId },
    m_AudioEntity{ audioEntity },
    m_Volume{ volume },
    m_RollOffFactor{ rollOff },
    m_Speed{ speed },
    m_Doppler{ doppler },
    m_FrameDelay{ frameDelay },
    m_Flags{ flags },
    m_SpeedVariance{ speedVariance }
{
    assert((!(flags & SOUND_REQUEST_UPDATES) || audioEntity) && "SOUND_REQUEST_UPDATES flag requires `audioEntity` to be set!");

    SetPosition(pos);
}

// Al destruir la voz elimina cualquier referencia segura a la entidad física asociada.
CAESound::~CAESound() {
    UnregisterWithPhysicalEntity();
}

// 0x4EF680
/**
 * Copia todo el estado lógico conservando correctamente el seguimiento de EntityRef.
 *
 * La entidad física se reasigna desde el puntero crudo para registrar esta instancia
 * como nuevo observador, en lugar de copiar ciegamente el estado interno de referencia.
 */
CAESound& CAESound::operator=(const CAESound& other) {
    if (this == &other) {
        return *this;
    }

    UnregisterWithPhysicalEntity();
    m_BankSlot = other.m_BankSlot;
    m_SoundID = other.m_SoundID;
    m_AudioEntity = other.m_AudioEntity;
    m_Event = other.m_Event;
    m_ClientVariable = other.m_ClientVariable;
    m_Volume = other.m_Volume;
    m_RollOffFactor = other.m_RollOffFactor;
    m_Speed = other.m_Speed;
    m_SpeedVariance = other.m_SpeedVariance;
    m_CurrPos = other.m_CurrPos;
    m_PrevPos = other.m_PrevPos;
    m_LastFrameUpdatedAt = other.m_LastFrameUpdatedAt;
    m_CurrTimeUpdateMs = other.m_CurrTimeUpdateMs;
    m_PrevTimeUpdateMs = other.m_PrevTimeUpdateMs;
    m_CurrCamDist = other.m_CurrCamDist;
    m_PrevCamDist = other.m_PrevCamDist;
    m_Doppler = other.m_Doppler;
    m_FrameDelay = other.m_FrameDelay;
    m_Flags = other.m_Flags;
    m_IsInUse = other.m_IsInUse;
    m_PlayTime = other.m_PlayTime;
    m_IsPhysicallyPlaying = other.m_IsPhysicallyPlaying;
    m_ListenerVolume = other.m_ListenerVolume;
    m_ListenerSpeed = other.m_ListenerSpeed;
    m_HasRequestedStopped = other.m_HasRequestedStopped;
    m_Headroom = other.m_Headroom;
    m_Length = other.m_Length;
    // The original preserves m_IsAudioHardwareAware and padding. Assign the
    // raw entity pointer so EntityRef registers this destination, not a copy
    // of the source's pointer without lifetime tracking.
    m_PhysicalEntity = other.m_PhysicalEntity.Get();
    return *this;
}

// 0x4EFE50
void CAESound::Initialise(
    eSoundBankSlot  bankSlot,
    eSoundID        sfxId,
    CAEAudioEntity* audioEntity,
    CVector         pos,
    float           volume,
    float           rollOff,
    float           speed,
    float           doppler,
    uint8           frameDelay,
    uint16          flags,
    float           speedVariance,
    int16           playTime
) {
    *this = CAESound{
        bankSlot,
        sfxId,
        audioEntity,
        pos,
        volume,
        rollOff,
        speed,
        doppler,
        frameDelay,
        (eSoundEnvironment)(flags),
        speedVariance,
    };
    m_PlayTime = playTime;
}

// 0x4EF1A0
void CAESound::UnregisterWithPhysicalEntity() {
    m_PhysicalEntity = nullptr;
}

// 0x4EF1C0
void CAESound::StopSound() {
    m_HasRequestedStopped = true;
    UnregisterWithPhysicalEntity();
}

// 0x4EF2B0
void CAESound::SetFlags(uint16 envFlag, uint16 bEnabled) {
    if (bEnabled)
        m_Flags |= envFlag;
    else
        m_Flags &= ~envFlag;
}

// 0x4EF2E0
/**
 * Avanza el tiempo lógico cuando la voz aún no está reproduciéndose físicamente.
 *
 * Al alcanzar el final, termina la voz o la reubica en loopStartTime. El bugfix
 * existente evita módulo por cero cuando faltan datos de audio.
 */
void CAESound::UpdatePlayTime(int16 soundLength, int16 loopStartTime, int16 playProgress) {
    m_Length = soundLength;

    if (m_IsPhysicallyPlaying) {
        return;
    }

    if (m_HasRequestedStopped) {
        m_PlayTime = -1;
        return;
    }

    m_PlayTime += static_cast<int16>(static_cast<float>(playProgress) * m_ListenerSpeed);
    if (m_PlayTime < soundLength) {
        return;
    }

    if (loopStartTime == -1) {
        m_PlayTime = -1;
        return;
    }

    // Avoid division by 0
    // This seems to have been fixed the same way in Android
    // The cause is/can be missing audio files, but I'm lazy to fix it, so this is gonna be fine for now
    m_PlayTime = !notsa::bugfixes::AESound_UpdatePlayTime_DivisionByZero || soundLength > 0
        ? loopStartTime + (m_PlayTime % soundLength)
        : loopStartTime;
}

// NOTSA: Simplified calling convention
CVector CAESound::GetRelativePosition() const {
    CVector outVec;
    GetRelativePosition(&outVec);
    return outVec;
}

// 0x4EF350 - Matches the original calling convention, to be used by reversible hooks, use the version returning CVector instead in our code
void CAESound::GetRelativePosition(CVector* outVec) const {
    *outVec = IsFrontEnd()
        ? m_CurrPos
        : CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos);
}

// 0x4EF390
// Calcula una velocidad de escucha con variación aleatoria limitada por m_SpeedVariance.
void CAESound::CalculateFrequency() {
    m_ListenerSpeed = m_SpeedVariance <= 0.f || m_SpeedVariance >= m_Speed
        ? m_Speed
        : m_Speed + CAEAudioUtility::GetRandomNumberInRange(-m_SpeedVariance, m_SpeedVariance);
}

// 0x4EF3E0
void CAESound::UpdateFrequency() {
    if (m_SpeedVariance == 0.f) {
        m_ListenerSpeed = m_Speed;
    }
}

// 0x4EF400
float CAESound::GetRelativePlaybackFrequencyWithDoppler() const {
    return IsFrontEnd()
        ? m_ListenerSpeed
        : m_ListenerSpeed * CAEAudioEnvironment::GetDopplerRelativeFrequency(m_PrevCamDist, m_CurrCamDist, m_PrevTimeUpdateMs, m_CurrTimeUpdateMs, m_Doppler);
}

// 0x4EF440
float CAESound::GetSlowMoFrequencyScalingFactor() const {
    return IsUnpausable() || !CTimer::GetIsSlowMotionActive() || CCamera::GetActiveCamera().m_nMode == eCamMode::MODE_CAMERA
        ? 1.f
        : fSlowMoFrequencyScalingFactor;
}

// 0x4EF7A0
// Reinicia flags de hardware y calcula headroom/frecuencia al entrar a la lista VPSL.
void CAESound::NewVPSLEntry() {
    m_IsPhysicallyPlaying  = false;
    m_HasRequestedStopped  = false;
    m_IsAudioHardwareAware = false;
    m_IsInUse              = true;
    m_Headroom             = AEAudioHardware.GetSoundHeadroom(m_SoundID, m_BankSlot);
    CalculateFrequency();
}

// 0x4EF820
void CAESound::RegisterWithPhysicalEntity(CEntity* entity) {
    CAESound::UnregisterWithPhysicalEntity();
    if (entity) {
        m_PhysicalEntity = entity;
    }
}

// 0x4EF850
void CAESound::StopSoundAndForget() {
    m_RequestUpdates = false;
    m_AudioEntity    = nullptr;
    StopSound();
}

// 0x4EF880
/**
 * Actualiza posición, distancia a cámara y timestamps preservando la muestra anterior.
 * Sólo desplaza el estado previo una vez por frame, requisito del cálculo Doppler.
 */
void CAESound::SetPosition(CVector pos) {
    const auto dist = DistanceBetweenPoints(pos, TheCamera.GetPosition());
    if (!m_LastFrameUpdatedAt) { // 0x4EF898
        m_PrevCamDist       = dist;
        m_PrevPos           = pos;
        m_PrevTimeUpdateMs  = CTimer::GetTimeInMS();
    } else if (m_LastFrameUpdatedAt != CTimer::GetFrameCounter()) {
        m_PrevPos           = m_CurrPos;
        m_PrevCamDist       = m_CurrCamDist;
        m_PrevTimeUpdateMs  = m_CurrTimeUpdateMs;
    }
    m_LastFrameUpdatedAt = CTimer::GetFrameCounter();
    m_CurrCamDist        = dist;
    m_CurrPos            = pos;
    m_CurrTimeUpdateMs   = CTimer::GetTimeInMS();
}

// 0x4EFA10
// Combina volumen base/headroom con atenuación direccional y distancia salvo frontend.
void CAESound::CalculateVolume() {
    m_ListenerVolume = m_Volume - m_Headroom;
    if (!IsFrontEnd()) {
        m_ListenerVolume += CAEAudioEnvironment::GetDirectionalMikeAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos))
                          + CAEAudioEnvironment::GetDistanceAttenuation(CAEAudioEnvironment::GetPositionRelativeToCamera(m_CurrPos).Magnitude() / m_RollOffFactor);
    }
}

// 0x4EFF50
/**
 * Sincroniza la voz con su entidad física y solicita al CAEAudioEntity propietario
 * cualquier actualización específica mientras SOUND_REQUEST_UPDATES permanezca activo.
 */
void CAESound::UpdateParameters(int16 curPlayPos) {
    if (IsLifespanTiedToPhysicalEntity()) {
        if (m_PhysicalEntity) {
            SetPosition(m_PhysicalEntity->GetPosition());
        } else {
            m_HasRequestedStopped = true;
        }
    }
    if (GetRequestUpdates()) {
        if (m_AudioEntity) { // NB: References are clearable, so must check!
            m_AudioEntity->UpdateParameters(this, curPlayPos);
            if (m_SpeedVariance == 0.0F) {
                m_ListenerSpeed = m_Speed;
            }
        }
    }
}

// 0x4EFFD0
void CAESound::SoundHasFinished() {
    UpdateParameters(-1);
    UnregisterWithPhysicalEntity();
    m_IsInUse             = false;
    m_IsPhysicallyPlaying = false;
    m_PlayTime            = 0;
}
