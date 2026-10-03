/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEWeatherAudioEntity
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <common.h>

#include "AEWeatherAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"

enum class eWeatherEvent {
    THUNDER    = 1,
    CITY_NOISE = 3,
    UNK_4      = 4,
    UNK_5      = 5,
};


// Como el original: posiciones frontales de los sonidos del clima (0xBF67EF9E = -0.906f, 0x3ED89375 = 0.423f).
constexpr CVector WEATHER_POS_LEFT_UP{ -0.906f, 0.423f, 0.f };
constexpr CVector WEATHER_POS_LEFT_DOWN{ -0.906f, -0.423f, 0.f };
constexpr CVector WEATHER_POS_RIGHT_UP{ 0.906f, 0.423f, 0.f };
constexpr CVector WEATHER_POS_RIGHT_DOWN{ 0.906f, -0.423f, 0.f };

auto& m_snLastRainDropSoundID = StaticRef<int32>(0x8CC310); // Como el original: se lee como dword y se escribe como word
auto& m_sRainSoundL = StaticRef<CAETwinLoopSoundEntity>(0xB6BB18);  // dunno about names
auto& m_sRainSoundR = StaticRef<CAETwinLoopSoundEntity>(0xB6BBC0);

namespace {
// Como el original: volumen por defecto leido como entero con signo (movsx) para fild/fiadd.
int32 DefaultVolumeInt(eAudioEvents event) {
    return StaticRef<int8*>(0xBD00F8)[event];
}

// Como el original (0x5067B0): baja el volumen con el paso global del viento (0x8CC2AC) o detiene el sonido.
void FadeWeatherSoundOut(CAESound* sound) {
    const float cur = sound->m_Volume;
    if (cur > -50.f) {
        const float v = cur - StaticRef<float>(0x8CC2AC);
        sound->m_Volume = v > -50.f ? v : -50.f;
    } else {
        sound->StopSoundAndForget();
    }
}
} // namespace

// 0x72A620
CAEWeatherAudioEntity::CAEWeatherAudioEntity() : CAEAudioEntity() {
    m_nThunderFrequencyVariationCounter = 0;
}

// 0x5B9A70
void CAEWeatherAudioEntity::StaticInitialise() {
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER);
}

// 0x5052B0
void CAEWeatherAudioEntity::StaticReset() {
    m_sfRainVolume = -100.0f;

    if (m_sRainSoundL.IsActive()) {
        m_sRainSoundL.StopSoundAndForget();
    }

    if (m_sRainSoundR.IsActive()) {
        m_sRainSoundR.StopSoundAndForget();
    }
}

// 0x506800
void CAEWeatherAudioEntity::AddAudioEvent(eAudioEvents event) {
    CAESound sound;
    if (event != AE_THUNDER || CGame::currArea != AREA_CODE_NORMAL_WORLD || CCullZones::PlayerNoRain() || CCullZones::CamNoRain()) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS)) {
        AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_EXPLOSIONS, SND_BANK_SLOT_EXPLOSIONS);
        return;
    }
    // The original evaluates the logarithm on the x87 stack, without first
    // rounding the lightning-duration expression to float.
    const auto duration = double(CWeather::LightningDuration);
    const auto defaultVolume = GetDefaultVolume(AE_THUNDER);
    const float durationScale = 0.0375f, decibels = 20.f;
    const double base = 0.25;
    float volume;
    __asm {
        fld duration
        fmul durationScale
        fadd base
        fldlg2
        fxch st(1)
        fyl2x
        fmul decibels
        fadd defaultVolume
        fstp volume
    }
    m_nThunderFrequencyVariationCounter = (m_nThunderFrequencyVariationCounter + 1) % 3;
    const auto& frequencies = StaticRef<std::array<float, 3>>(0x8CC300);
    const float pitch = frequencies[m_nThunderFrequencyVariationCounter];
    constexpr CVector left{-0.906f, 0.423f, 0.f}, right{0.906f, 0.423f, 0.f};
    // 0x862E44 vale 0x3EB674D2 (0.35636002f): un ulp por encima de lo que da el literal 0.35636f.
    constexpr float leftSpeed = 0.35636002f;
    const auto Play = [&](eSoundID id, const CVector& position, float level, float speed, uint16 flags, int32 weatherEvent) {
        sound.Initialise(SND_BANK_SLOT_EXPLOSIONS, id, this, position, level, 1.f, speed, 1.f, 0, flags);
        sound.m_Event = weatherEvent;
        AESoundManager.RequestNewSound(&sound);
    };
    Play(4, left, -100.f, pitch * leftSpeed, SOUND_FRONT_END | SOUND_IS_CANCELLABLE | SOUND_REQUEST_UPDATES, 1);
    Play(4, right, -100.f, pitch * 0.4f, SOUND_FRONT_END | SOUND_IS_CANCELLABLE | SOUND_REQUEST_UPDATES, 1);
    Play(1, left, std::min(volume, 0.f), pitch * leftSpeed, SOUND_FRONT_END | SOUND_IS_CANCELLABLE | SOUND_REQUEST_UPDATES | SOUND_ROLLED_OFF, 2);
    Play(1, right, std::min(volume, 0.f), pitch * 0.4f, SOUND_FRONT_END | SOUND_IS_CANCELLABLE | SOUND_REQUEST_UPDATES | SOUND_ROLLED_OFF, 2);
}

// 0x505A00
void CAEWeatherAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos <= 0) {
        return;
    }

    switch (sound->m_Event) {
    case 1: { // Trueno (0x505A85)
        // Como el original: todo en la pila x87 (duracion sin signo, log10 con fyl2x, pow del EXE 0x822130).
        const uint32 duration = CWeather::LightningDuration;
        const int32  defaultVolume = DefaultVolumeInt(AE_THUNDER);
        const float  scale = std::bit_cast<float>(0x3D19999Au), decibels = 20.f, twoPow32 = 4294967296.f;
        const double base = 0.25;
        float reqVolume;
        __asm {
            mov ecx, duration
            fild dword ptr duration
            test ecx, ecx
            jge thunder_dur_ok
            fadd twoPow32
        thunder_dur_ok:
            fmul scale
            fadd base
            fldlg2
            fxch st(1)
            fyl2x
            fmul decibels
            fiadd defaultVolume
            fstp reqVolume
        }
        const uint32 reqPlayPos = 600u - duration / 20u * 500u;
        const int32  playPos = curPlayPos;
        float volume;
        if ((uint32)playPos <= reqPlayPos) { // 0x505AED
            __asm {
                fild playPos
                mov eax, reqPlayPos
                fild reqPlayPos
                test eax, eax
                jge thunder_req_ok
                fadd twoPow32
            thunder_req_ok:
                fdivp st(1), st
                fldlg2
                fxch st(1)
                fyl2x
                fmul decibels
                fadd reqVolume
                fstp volume
            }
        } else if ((uint32)playPos <= reqPlayPos + 200u && reqVolume > 0.f) { // 0x505B3F
            const uint32 diff = (uint32)playPos - reqPlayPos;
            const double ten = 10.0;
            const float  toLinear = std::bit_cast<float>(0x3D4CCCCDu), one = 1.f, invSteps = std::bit_cast<float>(0x3BA3D70Au);
            __asm {
                fld ten
                fld reqVolume
                fmul toLinear
                mov eax, 0x822130
                call eax
                mov eax, diff
                fild diff
                test eax, eax
                jge thunder_diff_ok
                fadd twoPow32
            thunder_diff_ok:
                fld one
                fsub st, st(2)
                fmulp st(1), st
                fmul invSteps
                fadd st, st(1)
                fldlg2
                fxch st(1)
                fyl2x
                fstp st(1)
                fmul decibels
                fstp volume
            }
        } else { // 0x505B97
            volume = reqVolume < 0.f ? reqVolume : 0.f;
        }
        sound->m_Volume = volume;
        return;
    }
    case 3: { // Ruido de ciudad (0x505BEB)
        if (CGame::currArea != AREA_CODE_NORMAL_WORLD) {
            FadeWeatherSoundOut(sound);
            return;
        }
        const float defaultVolume = (float)DefaultVolumeInt(AE_CITY_NOISE);
        CVector pov = TheCamera.GetPosition();
        auto* const player = FindPlayerPed();
        if (player) {
            pov.z = player->GetPosition().z;
        }
        CVector right = TheCamera.GetRightVector();
        right.Normalise();
        const CVector right4{ right.x * 4.f, right.y * 4.f, right.z * 4.f };

        // Como el original: comparacion exacta componente a componente (0x4DC670).
        const CVector& curPos = sound->m_CurrPos;
        const bool isLeftUp = curPos.x == WEATHER_POS_LEFT_UP.x && curPos.y == WEATHER_POS_LEFT_UP.y && curPos.z == WEATHER_POS_LEFT_UP.z;
        CVector origin, target;
        if (isLeftUp) { // 0x505CC1: de pov + derecha a pov + derecha * 4
            origin = CVector{ right.x + pov.x, right.y + pov.y, right.z + pov.z };
            target = CVector{ right4.x + pov.x, right4.y + pov.y, right4.z + pov.z };
        } else { // 0x5060D8: de pov - derecha a pov - derecha * 4
            origin = CVector{ right.x * -1.f + pov.x, right.y * -1.f + pov.y, right.z * -1.f + pov.z };
            target = CVector{ right4.x * -1.f + pov.x, right4.y * -1.f + pov.y, right4.z * -1.f + pov.z };
        }
        auto& lastEntity = isLeftUp ? StaticRef<CEntity*>(0xB6BC74) : StaticRef<CEntity*>(0xB6BC6C);
        auto& residue    = isLeftUp ? StaticRef<float>(0xB6BC70) : StaticRef<float>(0xB6BC68);

        CEntity*  hitEntity = nullptr;
        CColPoint hitCP;
        CWorld::ProcessLineOfSight(origin, target, hitCP, hitEntity, true, true, false, true, true, false, false, false);

        float volumeOffset = 0.f, speed = 1.f;
        if (hitEntity && hitEntity != lastEntity && std::fabs(TheCamera.GetPosition().z - pov.z) < 6.f) { // 0x505D44
            CVector velocity{}, hitVelocity{};
            if (player && player->bInVehicle && player->m_pVehicle) {
                velocity = player->m_pVehicle->m_vecMoveSpeed;
            }
            if (hitEntity->GetIsTypePhysical()) {
                hitVelocity = hitEntity->AsPhysical()->m_vecMoveSpeed; // Como el original: velocidad, no posicion
            }
            CVector fwd = TheCamera.GetForwardVector();
            fwd.Normalise();
            // Como el original: productos escalares sumados z + y + x.
            const float dotVel = fwd.z * velocity.z + fwd.y * velocity.y + fwd.x * velocity.x;
            const float dotHit = fwd.z * hitVelocity.z + fwd.y * hitVelocity.y + fwd.x * hitVelocity.x;
            const CVector rel{ dotVel * fwd.x - dotHit * fwd.x, dotVel * fwd.y - dotHit * fwd.y, dotVel * fwd.z - dotHit * fwd.z };
            const float force = rel.Magnitude();
            if (force > 0.35f) { // 0x505E8D
                speed = (force - 0.35f) * std::bit_cast<float>(0x3F86BCA2u);
                const CVector delta{ right.x + pov.x - hitCP.m_vecPoint.x, right.y + pov.y - hitCP.m_vecPoint.y, right.z + pov.z - hitCP.m_vecPoint.z };
                const float dist = delta.Magnitude();
                const float invRange = std::bit_cast<float>(0x3E2AAAABu), one = 1.f, decibels = 20.f, baseVolume = 30.f;
                __asm {
                    fld dist
                    fmul invRange
                    fsubr one
                    fmul speed
                    fldlg2
                    fxch st(1)
                    fyl2x
                    fmul decibels
                    fadd baseVolume
                    fstp volumeOffset
                }
                speed = speed * 1.75f + 1.75f;
            }
        }

        // 0x505F2C
        const float targetVolume = defaultVolume + volumeOffset;
        const float cur = sound->m_Volume;
        if (cur < targetVolume) {
            const float v = cur + volumeOffset + 0.3f;
            sound->m_Volume = v < targetVolume ? v : targetVolume;
            residue = volumeOffset;
        } else if (cur > targetVolume) {
            if (residue > 0.f) { // 0x505FB5
                const float step = 1.3f < residue ? 1.3f : residue;
                const float down = cur - 0.3f;
                sound->m_Volume = defaultVolume - step < down - step ? down - step : defaultVolume - step;
                residue = residue - step;
            } else { // 0x5060A5
                const float v = cur - 0.3f;
                sound->m_Volume = v > defaultVolume ? v : defaultVolume;
            }
        }
        sound->m_Speed = speed;
        lastEntity = hitEntity; // Como el original: tambien si no hubo impacto
        return;
    }
    case 4:
    case 5: { // Viento (0x506527)
        static auto& sbWindOffset  = StaticRef<uint8>(0x8CC2C0);
        static auto& sfWindOffset  = StaticRef<float>(0xB6BAFC);
        static auto& sfWindFreq    = StaticRef<float>(0xB6BAF8);
        static auto& sfOldFreqLeft = StaticRef<float>(0x8CC2C4);
        using Points = const float (*)[2];

        const float height = TheCamera.GetPosition().z * std::bit_cast<float>(0x3B03126Fu);
        const float factor = height > 1.f ? 1.f : (height < 0.f ? 0.f : height);
        const float low  = CAEAudioUtility::GetPiecewiseLinear(factor, 3, reinterpret_cast<Points>(0x8CC2D0));
        const float high = CAEAudioUtility::GetPiecewiseLinear(factor, 3, reinterpret_cast<Points>(0x8CC2E8));
        const float wind = CWeather::WindClipped;
        const float windClamped = 1.f < wind ? 1.f : (0.f > wind ? 0.f : wind);
        const float windRatio = (high - low) * windClamped + low;
        if (CAEAudioUtility::ResolveProbability(StaticRef<float>(0x8CC2CC))) {
            sbWindOffset = (uint8)(sbWindOffset == 0 ? 1 : 0);
        }
        const float windOn = (float)sbWindOffset;
        sfWindOffset = StaticRef<float>(0x8CC2B0) * windOn * windRatio;
        sfWindFreq   = windOn * StaticRef<float>(0x8CC2B4) * windRatio;
        if (CGame::currArea != AREA_CODE_NORMAL_WORLD) {
            FadeWeatherSoundOut(sound);
            return;
        }
        const float targetVolume = StaticRef<float>(0x8CC2A4) + sfWindOffset;
        const float cur = sound->m_Volume;
        if (cur < targetVolume) {
            const float v = cur + StaticRef<float>(0x8CC2A8);
            sound->m_Volume = v < targetVolume ? v : targetVolume;
        } else if (cur > targetVolume) {
            const float v = cur - StaticRef<float>(0x8CC2AC);
            sound->m_Volume = v > targetVolume ? v : targetVolume;
        }
        if (sfOldFreqLeft < sfWindFreq) {
            const float v = StaticRef<float>(0x8CC2B8) + sfOldFreqLeft;
            sfOldFreqLeft = v < sfWindFreq ? v : sfWindFreq;
        } else if (sfOldFreqLeft > sfWindFreq) {
            const float v = sfOldFreqLeft - StaticRef<float>(0x8CC2BC);
            sfOldFreqLeft = v > 1.f ? v : 1.f;
        }
        sound->m_Speed = sfOldFreqLeft;
        return;
    }
    default: // Como el original: otros eventos no hacen nada
        return;
    }
}

// 0x5052F0
void CAEWeatherAudioEntity::Service() {
    // Como el original: un unico CAESound en la pila, Initialise + m_Event + RequestNewSound.
    CAESound sound;
    const auto RequestWeatherSound = [&](eSoundID sfx, const CVector& pos, uint16 flags, int16 playTime, int32 event) {
        sound.Initialise(SND_BANK_SLOT_FRONTEND_GAME, sfx, this, pos, -50.f, 1.f, 1.f, 1.f, 0, flags, 0.f, playTime);
        sound.m_Event = event;
        AESoundManager.RequestNewSound(&sound);
    };

    if (CGame::currArea == AREA_CODE_NORMAL_WORLD) { // 0x50538C
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(3, this)) { // 0x50539E
            if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
                RequestWeatherSound(0, WEATHER_POS_LEFT_UP, SOUND_FORCED_FRONT | SOUND_REQUEST_UPDATES | SOUND_FRONT_END, 0, 3);
                RequestWeatherSound(0, WEATHER_POS_RIGHT_UP, SOUND_FORCED_FRONT | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES | SOUND_FRONT_END, 50, 3);
            } else {
                AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME);
            }
        }
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(4, this)) { // 0x505482
            if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
                RequestWeatherSound(29, WEATHER_POS_LEFT_UP, SOUND_FORCED_FRONT | SOUND_REQUEST_UPDATES | SOUND_FRONT_END, 0, 4);
            } else {
                AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME);
            }
        }
        if (!AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(5, this)) { // 0x505509
            if (AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) {
                RequestWeatherSound(30, WEATHER_POS_RIGHT_UP, SOUND_FORCED_FRONT | SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES | SOUND_FRONT_END, 50, 5);
            } else {
                AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME);
            }
        }
    }

    // 0x505592
    auto& volume = m_sfRainVolume;
    float target;
    if (CWeather::Rain <= 0.f || CCullZones::PlayerNoRain() || CCullZones::CamNoRain() || CGame::currArea != AREA_CODE_NORMAL_WORLD) {
        target = -100.f;
    } else { // 0x5055C4
        if (!(volume > -50.f)) {
            volume = -50.f;
        }
        // Como el original: log10 con fyl2x (sin la saturacion de AudioLog10) y la suma en la pila x87.
        float wet = CWeather::Rain - 0.2f;
        if (!(wet > 0.f)) {
            wet = 0.f;
        }
        const int32 defaultVolume = DefaultVolumeInt(AE_RAIN);
        const float invRange = 1.25f, decibels = 20.f;
        __asm {
            fld wet
            fmul invRange
            fldlg2
            fxch st(1)
            fyl2x
            fild defaultVolume
            fxch st(1)
            fmul decibels
            faddp st(1), st
            fstp target
        }
    }

    // 0x505634
    if (target < volume) {
        const float v = volume - 0.5f;
        volume = v > target ? v : target;
    } else if (target > volume) {
        const float v = volume + 0.5f;
        volume = v < target ? v : target;
    }
    if (target == -100.f && volume <= -50.f) { // 0x505662
        volume = -100.f;
        if (m_sRainSoundL.IsActive()) {
            m_sRainSoundL.StopSoundAndForget();
        }
        if (m_sRainSoundR.IsActive()) {
            m_sRainSoundR.StopSoundAndForget();
        }
        return;
    }

    const auto PlayRainDrop = [&](const CVector& posUp, const CVector& posDown) { // 0x50579B
        int32 soundId = m_snLastRainDropSoundID + 1;
        if ((int16)soundId > 11) {
            soundId = 2;
        }
        // Como el original: el aleatorio del volumen se pide antes que la probabilidad de la posicion.
        const float rnd = CAEAudioUtility::GetRandomNumberInRange(-6.f, 6.f);
        const float dropVolume = rnd + (volume - 15.f);
        const CVector& pos = CAEAudioUtility::ResolveProbability(0.5f) ? posUp : posDown;
        sound.Initialise(SND_BANK_SLOT_WEATHER, (eSoundID)soundId, this, pos, dropVolume, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        sound.m_Flags = SOUND_IS_CANCELLABLE | SOUND_FRONT_END;
        AESoundManager.RequestNewSound(&sound);
        StaticRef<int16>(0x8CC310) = (int16)soundId;
    };

    // 0x5056EE
    if (!m_sRainSoundL.IsActive()) {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER)) {
            AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER);
            return; // Como el original: no atiende el canal derecho este frame
        }
        m_sRainSoundL.Initialise(SND_BANK_SLOT_WEATHER, 1, 0, this, 65, 350, 0, 0);
        m_sRainSoundL.PlayTwinLoopSound(WEATHER_POS_LEFT_UP, volume, 1.f, 1.f, 1.f, (eSoundEnvironment)(SOUND_START_PERCENTAGE | SOUND_IS_CANCELLABLE | SOUND_FRONT_END));
    } else {
        m_sRainSoundL.UpdateTwinLoopSound(WEATHER_POS_LEFT_UP, volume, 1.f);
        if (m_sRainSoundL.DoSoundsSwitchThisFrame()) {
            PlayRainDrop(WEATHER_POS_LEFT_UP, WEATHER_POS_LEFT_DOWN);
        }
    }

    // 0x50584B
    if (!m_sRainSoundR.IsActive()) {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER)) {
            AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER);
            return;
        }
        m_sRainSoundR.Initialise(SND_BANK_SLOT_WEATHER, 0, 1, this, 65, 350, 50, 50);
        m_sRainSoundR.PlayTwinLoopSound(WEATHER_POS_RIGHT_UP, volume, 1.f, 1.f, 1.f, (eSoundEnvironment)(SOUND_START_PERCENTAGE | SOUND_IS_CANCELLABLE | SOUND_FRONT_END));
    } else {
        m_sRainSoundR.UpdateTwinLoopSound(WEATHER_POS_RIGHT_UP, volume, 1.f);
        if (m_sRainSoundR.DoSoundsSwitchThisFrame()) {
            PlayRainDrop(WEATHER_POS_RIGHT_UP, WEATHER_POS_RIGHT_DOWN);
        }
    }
}

void CAEWeatherAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEWeatherAudioEntity, 0x872A74, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(StaticInitialise, 0x5B9A70);
    RH_ScopedInstall(StaticReset, 0x5052B0);
    RH_ScopedInstall(AddAudioEvent, 0x506800);
    RH_ScopedVMTInstall(UpdateParameters, 0x505A00);
    RH_ScopedInstall(Service, 0x5052F0);
}
