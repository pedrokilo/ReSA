/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEGlobalWeaponAudioEntity
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
#include "X87Math.h"

#include "AEAudioUtility.h"
#include "AEGlobalWeaponAudioEntity.h"
#include "AEAudioHardware.h"

auto& pWaterfall = StaticRef<CAESound*[3]>(0xB612F0);
auto& pFogHorn = StaticRef<CAESound*>(0xB612FC);

namespace {
// Como el original: v += fcos(a) * r con fcos a precisión interna completa (producto y suma redondeados por la FPU)
void AddCosMulX87(float& v, float a, float r) {
    v = TrigCompoundCosMulAdd(a, r, v);
}

// Como el original: v += fsin(a) * r
void AddSinMulX87(float& v, float a, float r) {
    v = TrigCompoundSinMulAdd(a, r, v);
}
} // namespace

// 0x5075B0
CAEGlobalWeaponAudioEntity::CAEGlobalWeaponAudioEntity() : CAEWeaponAudioEntity() {
    m_FrequencyVariation = 0; // Como el original: el constructor pone a cero el índice de variación
    m_Physical = new CPhysical();
}

// 0x506C50
CAEGlobalWeaponAudioEntity::~CAEGlobalWeaponAudioEntity() {
    delete m_Physical;
}

// 0x4DEF90
void CAEGlobalWeaponAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (curPlayPos != -1) {
        return;
    }

    if (sound == pFogHorn) {
        pFogHorn = nullptr;
    } else {
        for (auto& waterfall : pWaterfall) {
            if (sound == waterfall) {
                waterfall = nullptr;
                break; // Como el original: solo se anula la primera coincidencia
            }
        }
    }
}

// 0x4DFAA0
void CAEGlobalWeaponAudioEntity::AddAudioEvent(eAudioEvents audioId, eWeaponType weaponType, CPhysical* entity) {
    if (!entity) {
        return;
    }

    switch (audioId) {
    case AE_WEAPON_FIRE:
    case AE_WEAPON_FIRE_PLANE: return WeaponFire(weaponType, entity, audioId);
    case AE_WEAPON_RELOAD_A:
    case AE_WEAPON_RELOAD_B:   return WeaponReload(weaponType, entity, audioId);
    case AE_PROJECTILE_FIRE:   return ProjectileFire(weaponType, entity, audioId);
    default:                   return;
    }
}

// 0x4DF060
void CAEGlobalWeaponAudioEntity::ProjectileFire(eWeaponType weaponType, CPhysical* physical, eAudioEvents event) {
    const float volume = GetDefaultVolume(event);

    switch (weaponType) {
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS:
        break;
    default:
        return;
    }

    // Como el original: tabla del EXE (0x8AE5AC), índice con signo y módulo entero (idiv) entre 3
    static const auto& gfRocketFrequencyVariations = StaticRef<std::array<float, 4>>(0x8AE5AC);
    m_FrequencyVariation = static_cast<int8>((m_FrequencyVariation + 1) % 3);

    // Como el original: si el banco no está, lo pide (salvo con la melodía de carga) y sale
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
        if (!AudioEngine.IsLoadingTuneActive()) {
            AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN);
        }
        return;
    }
    // Como el original: usa m_tempSound, no una voz local
    m_tempSound.Initialise(
        SND_BANK_SLOT_WEAPON_GEN,
        SND_GENRL_WEAPONS_ROCKET_LAUNCH,
        this,
        physical->GetPosition(),
        volume - 8.f,
        3.f,
        gfRocketFrequencyVariations.data()[m_FrequencyVariation],
        1.f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
        0.02f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(physical);
    AESoundManager.RequestNewSound(&m_tempSound);

    // Como el original: segundo sonido del banco general de vehículos (0x8A) en el slot 19, sin mirar la melodía de carga
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN);
        return;
    }
    m_tempSound.Initialise(
        SND_BANK_SLOT_VEHICLE_GEN,
        SND_GENRL_WEAPONS_SNIPER_SHOT_L,
        this,
        physical->GetPosition(),
        volume,
        3.f,
        gfRocketFrequencyVariations.data()[m_FrequencyVariation] * 1.25f,
        1.f,
        0,
        SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY,
        0.02f,
        0
    );
    m_tempSound.RegisterWithPhysicalEntity(physical);
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x4DF210
void CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire() {
    constexpr CVector s_FogHornPositions[]{
        {-2683.f, 1445.f, 37.f}, // 0x4DF271
        {-2683.f, 1804.f, 69.f}, // 0x4DF29F
    };
    constexpr uint32 FOGHORN_FADE_OUT_DELAY = 5000; // 0x8AE5BC
    constexpr uint32 FOGHORN_STOP_DELAY     = 5000; // 0x8AE5C8
    constexpr uint32 FOGHORN_RESTART_DELAY  = 6500; // 0x8AE5C4

    constexpr CVector s_WaterfallPositions[]{
        {2075.3f, 1925.9f, 13.0f}, // 0x4DF2C9
        {2085.8f, 1905.5f, 11.3f}, // 0x4DF2FD
        {2091.5f, 1888.9f, 11.1f}, // 0x4DF31D
    };

    enum eState : int32 {
        INITIAL,
        GUNSHOTS,
        FOGHORN_0,
        STATE_3,
        FOGHORN_1,
        WATERFALL_LV, // Only in LV
    };
    static auto& s_State = StaticRef<eState>(0xB61324);

    static auto& s_GunShots   = StaticRef<int32>(0xB61318);
    static auto& s_WeaponType = StaticRef<eWeaponType>(0xB6131C);
    static auto& s_Delay      = StaticRef<uint32>(0xB61320);
    // Como el original: la guarda del static se comprueba en cada llamada
    auto& s_LastTime = ScopedStaticRef<uint32>(0xB61364, 0xB61368, 0x1, CTimer::GetTimeInMS());

    // Como el original: voz local inicializada a mano; solo se fijan los campos que escribe el EXE
    const auto PlayLocalSound = [this](eSoundBankSlot slot, eSoundID sfx, const CVector& pos, float volume, float rollOff, float speed) {
        CAESound sound;
        sound.Initialise(slot, sfx, this, pos, volume, 1.f, 1.f, 1.f, 0, 0, 0.f, 0);
        sound.m_RollOffFactor = rollOff;
        sound.m_Speed         = speed;
        sound.m_Flags         = SOUND_REQUEST_UPDATES;
        return AESoundManager.RequestNewSound(&sound);
    };

    switch (s_State) {
    case eState::INITIAL: {
        switch (CWeather::WeatherRegion) {
        case eWeatherRegion::WEATHER_REGION_LA: { // 0x4DF35A
            if (CTimer::GetFrameCounter() % 1024) { // TODO: Maybe use a timer or something?
                return;
            }

            // Como el original: coloca la entidad en la cámara y suma el desplazamiento en su sitio
            m_Physical->SetPosn(TheCamera.GetPosition());
            const float a = CGeneral::GetRandomNumberInRange(0.f, std::bit_cast<float>(0x40C90FDBu)); // 2PI del EXE
            const float r = CGeneral::GetRandomNumberInRange(40.f, 80.f);
            AddCosMulX87(m_Physical->GetPosition().x, a, r);
            AddSinMulX87(m_Physical->GetPosition().y, a, r);
            auto& z = m_Physical->GetPosition().z;
            z = z < 20.f ? z : 20.f; // Como el original: min (0x404330) devuelve el segundo si el primero no es menor

            s_State = eState::GUNSHOTS;
            PickAmbientGunFire(s_GunShots, s_WeaponType, s_Delay);
            break;
        }
        case WEATHER_REGION_LV: { // 0x4DF580
            // Como el original: distancia, banco y punteros, en este orden
            if (!(CVector::DistSqr(TheCamera.GetPosition(), s_WaterfallPositions[1]) < sq(100.f))) {
                return;
            }
            if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
                return;
            }
            if (!rng::all_of(pWaterfall, notsa::IsNull{})) {
                return;
            }
            constexpr auto SPEED_BASE = 1.f;
            constexpr auto SPEED_VAR  = 0.01f;
            pWaterfall[0] = PlayLocalSound(SND_BANK_SLOT_COLLISIONS, 3, s_WaterfallPositions[0], -3.f, 2.5f, SPEED_BASE);
            pWaterfall[1] = PlayLocalSound(SND_BANK_SLOT_COLLISIONS, 3, s_WaterfallPositions[1], -3.f, 2.5f, SPEED_VAR + SPEED_BASE);
            pWaterfall[2] = PlayLocalSound(SND_BANK_SLOT_COLLISIONS, 3, s_WaterfallPositions[2], -3.f, 2.5f, SPEED_BASE - SPEED_VAR);

            s_State = eState::WATERFALL_LV;
            break;
        }
        case WEATHER_REGION_DESERT: {
            break;
        }
        default: { // 0x4DF490
            if (!(CWeather::Foggyness_SF >= 0.5f)) { // Como el original: NaN también sale
                return;
            }
            if (CTimer::GetFrameCounter() % 1024) { // TODO: Maybe use a timer or something?
                return;
            }
            if (!(CVector::DistSqr(TheCamera.GetPosition(), s_FogHornPositions[0]) < sq(2000.f))) {
                return;
            }
            if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) { // Como el original
                return;
            }
            if (pFogHorn) {
                return;
            }

            pFogHorn   = PlayLocalSound(SND_BANK_SLOT_HORN_AND_SIREN, 13, s_FogHornPositions[0], 12.f, 60.f, 1.f);
            s_LastTime = CTimer::GetTimeInMS();
            s_State    = eState::FOGHORN_0;
            break;
        }
        }
        break;
    }
    case eState::GUNSHOTS: { // 0x4DF769
        if (s_LastTime + s_Delay > CTimer::GetTimeInMS()) { // Como el original: sigue cuando son iguales
            return;
        }

        // Como el original: dispara cuando el jugador está a más de 24 unidades (o la distancia es NaN)
        if (!((FindPlayerCoors() - m_Physical->GetPosition()).Magnitude() <= 24.f)) {
            m_Physical->SetType(ENTITY_TYPE_NOTINPOOLS);
            AudioEngine.ReportWeaponEvent(AE_WEAPON_FIRE, s_WeaponType, m_Physical);

            if (--s_GunShots) { // 0x4DF862
                if (s_WeaponType == WEAPON_PISTOL) {
                    s_Delay = CAEAudioUtility::GetRandomNumberInRange(500, 1200);
                } else if (s_WeaponType == WEAPON_AK47) {
                    s_Delay = CAEAudioUtility::GetRandomNumberInRange(90, 450);
                }
            } else if (CGeneral::GetRandomIntInRange(0, 100) >= 40) { // 0x4DF7E9
                s_State = eState::INITIAL;
            } else { // 0x4DF849
                PickAmbientGunFire(s_GunShots, s_WeaponType, s_Delay);
            }
        } else {
            s_State = eState::INITIAL;
        }
        s_LastTime = CTimer::GetTimeInMS();

        break;
    }
    case eState::FOGHORN_0:   // 0x4DF8A4
    case eState::FOGHORN_1: { // 0x4DF98A
        if (CTimer::GetTimeInMS() > s_LastTime + FOGHORN_STOP_DELAY) {
            if (pFogHorn) {
                pFogHorn->StopSound();
            }
            s_State = s_State == eState::FOGHORN_0
                ? eState::STATE_3   // FOGHORN_0 -> S3
                : eState::INITIAL;  // FOGHORN_1 -> INITIAL
        } else if (CTimer::GetTimeInMS() > s_LastTime + FOGHORN_FADE_OUT_DELAY) { // BUG: Since `FOGHORN_TTL` and `FOGHORN_FADE_OUT_DELAY` are both 5000 the `if` here will never be true.
            if (pFogHorn) {
                pFogHorn->m_Volume -= 3.3f;
            }
        }
        break;
    }
    case eState::STATE_3: { // 0x4DF8D1
        if (CTimer::GetTimeInMS() <= s_LastTime + FOGHORN_RESTART_DELAY) { // Como el original: espera 6,5 s
            return;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_FRONTEND_GAME, SND_BANK_SLOT_FRONTEND_GAME)) { // NB (Bug?): Why check this sound bank, when it's not even used below?
            return;
        }
        if (!pFogHorn) {
            pFogHorn   = PlayLocalSound(SND_BANK_SLOT_HORN_AND_SIREN, 13, s_FogHornPositions[1], 12.f, 60.f, 1.2f);
            s_LastTime = CTimer::GetTimeInMS();
            s_State    = eState::FOGHORN_1;
        }
        break;
    }
    case eState::WATERFALL_LV: { // 0x4DF9D8
        if (CWeather::WeatherRegion != eWeatherRegion::WEATHER_REGION_LV || CVector::DistSqr(TheCamera.GetPosition(), s_WaterfallPositions[1]) > sq(100.f)) {
            for (auto* s : pWaterfall) {
                if (s) {
                    s->StopSound(); // Como el original: no anula los punteros
                }
            }
            s_State = eState::INITIAL;
        }
        break;
    }
    default:
        break; // Como el original: estados fuera de rango no hacen nada
    }
}

// 0x4DEFE0
void CAEGlobalWeaponAudioEntity::PickAmbientGunFire(int32& gunShots, eWeaponType& weaponType, uint32& delay) {
    // Como el original: CAEAudioUtility::GetRandomNumberInRange(0, 1); cualquier otro valor no toca nada
    switch (CAEAudioUtility::GetRandomNumberInRange(0, 1)) {
    case 0:
        gunShots   = CAEAudioUtility::GetRandomNumberInRange(4, 14);
        weaponType = WEAPON_AK47;
        delay      = CAEAudioUtility::GetRandomNumberInRange(90, 350);
        break;
    case 1:
        gunShots   = CAEAudioUtility::GetRandomNumberInRange(2, 4);
        weaponType = WEAPON_PISTOL;
        delay      = CAEAudioUtility::GetRandomNumberInRange(500, 1'200);
        break;
    }
}

void CAEGlobalWeaponAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEGlobalWeaponAudioEntity, 0x862E5C, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x5075B0);
    RH_ScopedInstall(Destructor, 0x506C50);
    RH_ScopedVMTInstall(UpdateParameters, 0x4DEF90); // Como el original: entrada 0 de la vtable 0x862E5C (virtual)
    RH_ScopedInstall(AddAudioEvent, 0x4DFAA0);
    RH_ScopedInstall(ProjectileFire, 0x4DF060);
    RH_ScopedInstall(ServiceAmbientGunFire, 0x4DF210);
}

CAEGlobalWeaponAudioEntity* CAEGlobalWeaponAudioEntity::Constructor() {
    this->CAEGlobalWeaponAudioEntity::CAEGlobalWeaponAudioEntity();
    return this;
}
CAEGlobalWeaponAudioEntity* CAEGlobalWeaponAudioEntity::Destructor() { this->CAEGlobalWeaponAudioEntity::~CAEGlobalWeaponAudioEntity(); return this; }
