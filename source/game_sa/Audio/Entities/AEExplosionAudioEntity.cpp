/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEExplosionAudioEntity
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEExplosionAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioEnvironment.h"
#include "AEAudioUtility.h"

// Tabla del EXE (1.12, 1.0, 0.88); el índice es un byte con signo como en el original
static auto& gfExplosionFrequencyVariations = StaticRef<float[3]>(0x8AE58C);
constexpr int32 NUM_VARIATIONS = 3;

// 0x5074B0
CAEExplosionAudioEntity::CAEExplosionAudioEntity() {
    m_Speed = 0;
}

void CAEExplosionAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS);
}

// 0x4DCBE0
/**
 * Construye una explosión por capas con distintos roll-off y dos componentes frontend.
 *
 * m_Speed rota entre variaciones discretas para evitar que explosiones consecutivas
 * compartan exactamente el mismo pitch.
 */
void CAEExplosionAudioEntity::AddAudioEvent(eAudioEvents audioEvent, CVector& posn, float volume) {
    volume += GetDefaultVolume(AE_EXPLOSION);

    if (audioEvent != AE_EXPLOSION) {
        return;
    }

    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS)) {
        AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS);
        return;
    }

    { // 0x4DCC93
        m_Speed = (int8)(((int32)m_Speed + 1) % NUM_VARIATIONS);
        const auto PlayExplosionSound = [&](eSoundID soundID, float rollOff) {
            AESoundManager.PlaySound({
                .BankSlotID        = SND_BANK_SLOT_EXPLOSIONS,
                .SoundID           = soundID,
                .AudioEntity       = this,
                .Pos               = posn,
                .Volume            = volume,
                .RollOffFactor     = rollOff,
                .Speed             = gfExplosionFrequencyVariations[m_Speed],
                .Flags             = SOUND_REQUEST_UPDATES,
                .FrequencyVariance = 0.06f,
            });
        };
        PlayExplosionSound(4, 2.f);
        PlayExplosionSound(3, 4.f);
        PlayExplosionSound(2, 7.5f);
    }
    { // 0x4DCE79
        // Como el original: distancia = sqrt(z² + y² + x²) · (1/12), factor 0x85F0F8 (no exactamente 2^0.25)
        // y sin variación de frecuencia en las dos capas frontend.
        const auto rel      = CAEAudioEnvironment::GetPositionRelativeToCamera(posn);
        const float dist    = std::sqrt(rel.z * rel.z + rel.y * rel.y + rel.x * rel.x) * std::bit_cast<float>(0x3DAAAAABu);
        const float frontVolume = CAEAudioEnvironment::GetDistanceAttenuation(dist) + volume - 3.0f;
        const float variationFactor = std::bit_cast<float>(0x3F983809u);
        const auto PlayExplosionSound = [&](CVector frontEndPos, float speed) {
            AESoundManager.PlaySound({
                .BankSlotID        = SND_BANK_SLOT_EXPLOSIONS,
                .SoundID           = 1,
                .AudioEntity       = this,
                .Pos               = frontEndPos,
                .Volume            = frontVolume,
                .RollOffFactor     = 12.f,
                .Speed             = speed,
                .Flags             = SOUND_FORCED_FRONT | SOUND_ROLLED_OFF | SOUND_REQUEST_UPDATES | SOUND_FRONT_END,
                .FrequencyVariance = 0.0f,
            });
        };
        const auto base = gfExplosionFrequencyVariations[m_Speed];
        const auto variance = CAEAudioUtility::ResolveProbability(0.5f);
        PlayExplosionSound({ -1.0f, 0.0f, 0.0f }, variance ? base * variationFactor : base);
        PlayExplosionSound({ 1.0f, 0.0f, 0.0f }, variance ? base : base * variationFactor);
    }
}

// 0x4DCB90
// Reduce progresivamente cualquier volumen positivo una vez iniciada la reproducción.
void CAEExplosionAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos > 0) {
        if (sound->m_Volume > 0.0f) {
            sound->m_Volume = std::max(sound->m_Volume - 1.0f, 0.0f);
        }
    }
}

/**
 * Registra la entidad global encargada de eventos de explosión.
 */
void CAEExplosionAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEExplosionAudioEntity, 0x862E60, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(StaticInitialise, 0x5B9A60);
    RH_ScopedInstall(AddAudioEvent, 0x4DCBE0);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DCB90);
}
