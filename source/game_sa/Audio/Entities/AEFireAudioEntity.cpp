/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEFireAudioEntity
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEFireAudioEntity.h"
#include "AEAudioHardware.h"

auto& gfFireFrequencyVariations = StaticRef<std::array<float, 5>>(0x8AE598);

// 0x4DCF20
// Vincula la entidad al FxSystem_c cuyo movimiento/estado seguirá el sonido.
void CAEFireAudioEntity::Initialise(FxSystem_c* system) {
    m_FxSystem   = system;
    m_SoundLeft  = nullptr;
    m_SoundRight = nullptr;
}

// 0x5B9A90
void CAEFireAudioEntity::StaticInitialise() {
    // Como el original: carga el banco de explosiones (0x34) en el slot 4, no el de lluvia.
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS);
}

// 0x4DCF40
void CAEFireAudioEntity::Terminate() {
    AESoundManager.CancelSoundsOwnedByAudioEntity(this, 1);
    m_FxSystem = nullptr;
}

// 0x4DD3C0
void CAEFireAudioEntity::AddAudioEvent(eAudioEvents audioId, CVector& posn) {
    if (audioId == AE_FIRE_HYDRANT) {
        if (m_FxSystem && !m_SoundLeft && !m_SoundRight)
            PlayWaterSounds(AE_FIRE_HYDRANT, posn);
    } else if (audioId <= AE_FIRE_HYDRANT || audioId > AE_HELI_DUST) {
        if (m_FxSystem && !m_SoundLeft) {
            PlayFireSounds(audioId, posn);
        }
    }
}

// 0x4DD0D0
// Crea dos capas de fuego con distinto banco, pitch y envolvente de volumen.
void CAEFireAudioEntity::PlayFireSounds(eAudioEvents audioId, CVector& posn) {
    // Como el original: el índice es un byte con signo (movsx + idiv) y se usa con signo.
    m_snLastFireFrequencyIndex = (uint8)(((int32)(int8)m_snLastFireFrequencyIndex + 1) % 5);
    const int32 freqIdx = (int8)m_snLastFireFrequencyIndex;
    const float volume = GetDefaultVolume(audioId);

    m_SoundLeft = AESoundManager.PlaySound({
        .BankSlotID      = SND_BANK_SLOT_EXPLOSIONS,
        .SoundID         = 0,
        .AudioEntity     = this,
        .Pos             = posn,
        .Volume          = volume,
        .RollOffFactor   = 2.0f,
        .Speed           = gfFireFrequencyVariations.data()[freqIdx],
        .Flags           = SOUND_REQUEST_UPDATES,
        .ClientVariable  = -1.0f, // Como el original: el valor que deja CAESound::Initialise.
    });

    if (!AEAudioHardware.EnsureSoundBankIsLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }

    AESoundManager.PlaySound({
        .BankSlotID      = SND_BANK_SLOT_VEHICLE_GEN,
        .SoundID         = 26,
        .AudioEntity     = this,
        .Pos             = posn,
        .Volume          = volume - 17.0f,
        .RollOffFactor   = 2.0f,
        .Speed           = gfFireFrequencyVariations.data()[freqIdx] * 0.6f,
        .Doppler         = 1.0f,
        .FrameDelay      = 0,
        .Flags           = SOUND_REQUEST_UPDATES,
        .FrequencyVariance = 0.0f,
        .PlayTime        = 0,
        .EventID         = AE_FRONTEND_SELECT,
        .ClientVariable  = volume + 3.0f,
    });
}

// 0x4DD270
// Crea las dos capas usadas por el efecto de hidrante/agua.
void CAEFireAudioEntity::PlayWaterSounds(eAudioEvents audioId, CVector& posn) {
    m_SoundLeft = AESoundManager.PlaySound({
         .BankSlotID        = SND_BANK_SLOT_COLLISIONS,
         .SoundID           = 3,
         .AudioEntity       = this,
         .Pos               = posn,
         .Volume            = GetDefaultVolume(audioId),
         .RollOffFactor     = 2.0f,
         .Speed             = 0.75f,
         .Doppler           = 0.6f,
         .FrameDelay        = 0,
         .Flags             = SOUND_REQUEST_UPDATES,
         .FrequencyVariance = std::bit_cast<float>(0x3D75C290u), // Como el original: 0x3D75C290, no el float más cercano a 0,06
         .PlayTime          = 0,
         .EventID           = AE_FRONTEND_HIGHLIGHT,
         .ClientVariable    = -1.0f, // Como el original: el valor que deja CAESound::Initialise.
    });
    m_SoundRight = AESoundManager.PlaySound({
        .BankSlotID        = SND_BANK_SLOT_FRONTEND_GAME,
        .SoundID           = 0,
        .AudioEntity       = this,
        .Pos               = posn,
        .Volume            = GetDefaultVolume(audioId) + 20.0f,
        .RollOffFactor     = 2.0f,
        .Speed             = 1.78f,
        .Doppler           = 0.6f,
        .FrameDelay        = 0,
        .Flags             = SOUND_REQUEST_UPDATES,
        .FrequencyVariance = std::bit_cast<float>(0x3D75C290u), // Como el original: 0x3D75C290, no el float más cercano a 0,06
        .PlayTime          = 0,
        .EventID           = AE_FRONTEND_ERROR,
        .ClientVariable    = -1.0f, // Como el original: el valor que deja CAESound::Initialise.
    });
}

// 0x4DCF60
/**
 * Sigue la matriz del FxSystem_c y usa m_Event como estado interno para modular/finalizar
 * las capas conforme cambia el efecto visual.
 */
void CAEFireAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos <= 0) {
        if (curPlayPos == -1) {
            if (sound == m_SoundLeft) {
                m_SoundLeft = nullptr;
            } else if (sound == m_SoundRight) {
                m_SoundRight = nullptr;
            }
        }
        return;
    }

    if (m_FxSystem) {
        RwMatrix matrix;
        m_FxSystem->GetCompositeMatrix(&matrix);
        sound->SetPosition(matrix.pos);
    }

    switch (sound->m_Event) {
    case AE_FRONTEND_SELECT:
        // Como el original: NaN cuenta como "alcanzado" (test ah,5 / jp) y satura con NaN.
        if (!(sound->m_Volume < sound->m_ClientVariable)) {
            sound->m_Event = AE_FRONTEND_BACK;
        } else {
            const float volume = sound->m_Volume + 2.0f;
            sound->m_Volume = volume < sound->m_ClientVariable ? volume : sound->m_ClientVariable;
        }
        break;
    case AE_FRONTEND_BACK:
        if (!(sound->m_Volume > -30.0f)) // Como el original: NaN también detiene (test ah,41h).
            sound->StopSoundAndForget();
        else
            sound->m_Volume -= 0.75f;
        break;
    case AE_FRONTEND_ERROR:
        if (m_FxSystem && m_FxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED) {
            sound->m_Volume = -100.0f;
        }
        break;
    case AE_FRONTEND_HIGHLIGHT:
        if (m_FxSystem && m_FxSystem->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED) {
            if (sound->m_Volume > -100.0f) {
                sound->m_Volume -= 1.0f;
            }
        }
        break;
    }
}

/**
 * Registra la entidad que sonoriza sistemas FX de fuego e hidrantes.
 */
void CAEFireAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEFireAudioEntity, 0x85AA94, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Initialise, 0x4DCF20);
    RH_ScopedInstall(StaticInitialise, 0x5B9A90);
    RH_ScopedInstall(Terminate, 0x4DCF40);
    RH_ScopedInstall(AddAudioEvent, 0x4DD3C0);
    RH_ScopedInstall(PlayFireSounds, 0x4DD0D0);
    RH_ScopedInstall(PlayWaterSounds, 0x4DD270);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DCF60);
}
