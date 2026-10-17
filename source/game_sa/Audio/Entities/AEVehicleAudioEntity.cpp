/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEVehicleAudioEntity
 * Reconstrucción: COMPLETA — 106/106 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 106/106 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AEVehicleAudioEntity.h"
#include "AEAudioHardware.h"
#include "AEAudioUtility.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"

#include <Enums/eSoundBankSlot.h>
#include <DamageManager.h> // tComponent
#include <reversiblebugfixes/Bugs.hpp>
#include "X87Math.h"

constexpr size_t HORN_NUM_PATTERNS = 8;
// Como el original (0x4D9E50): p < 1e-5 (0x858C14) o NaN (fcomp + test ah,1 / jne) devuelve -5.0 (0x858FC8); si no, fldlg2/fyl2x a precision completa en ST0.
// Los llamadores del EXE multiplican ST0 sin almacenarlo por 10.0f (0x85862C) o 20.0f (0x858BA4), asi que el producto se redondea una sola vez:
// -5 * 10 = -50 y -5 * 20 = -100 son exactos en float y el logaritmo va a los ayudantes x87 de X87Math (Log10Times10Cpp / AudioLog10Times20Cpp).
static float AudioLog10ScaledExe(float input, float factor) {
    assert(factor == 10.0f || factor == 20.0f);
    if (!(input >= std::bit_cast<float>(0x3727C5ACu))) {
        return -5.0f * factor;
    }
    return factor == 10.0f ? Log10Times10Cpp(input) : AudioLog10Times20Cpp(input);
}
constexpr size_t HORN_PATTERN_SIZE = 44;
// Como el original: el EXE indexa 0x862B9C + patron*44 - contador (4F6263 imul edx,edx,2Ch / 4F626C sub edx,ecx / 4F626E mov al,[edx+862B9Ch]),
// es decir la fila 'patron' empieza en 0x862B70 (la tabla anterior estaba desplazada una fila).
constexpr notsa::mdarray<bool, HORN_NUM_PATTERNS, HORN_PATTERN_SIZE> HornPattern = {{ // 0x862B70
    { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false },
    { false, false, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, true, true, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
    { false, false, false, false, false, false, false, false, false, false, true, true, false, false, false, false, true, true, false, false, false, false, true, true, true, false, false, false, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false },
    { false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, true, true, true, false, false, false, false, true, true, true, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false },
    { false, false, true, true, true, true, false, false, false, false, true, true, true, true, true, false, false, false, false, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, false, false }
}};

bool IsSurfaceAudioGrass(eSurfaceType surface) {
    return g_surfaceInfos.IsAudioGrass(surface) || g_surfaceInfos.IsAudioLongGrass(surface);
}

bool IsSurfaceWet(eSurfaceType surface) {
    return g_surfaceInfos.IsAudioGravel(surface) || g_surfaceInfos.IsAudioSand(surface) || g_surfaceInfos.IsAudioWater(surface);
}

void GracefullyStopSound(CAESound*& sound) {
    if (auto* const s = std::exchange(sound, nullptr)) {
        s->SetFlags(SOUND_REQUEST_UPDATES, false);
        s->StopSound();
    }
}

void StopAndForgetSound(CAESound*& sound) {
    if (auto* const s = std::exchange(sound, nullptr)) {
        s->StopSoundAndForget();
    }
}

std::optional<uint32> GetNumOfContactWheels(CAEVehicleAudioEntity::tVehicleParams& vp) {
    // Como el original (0x4F8B1D..0x4F8B3E): byte en +0x960 (automovil) / +0x804 (moto); lectura cruda del vehiculo
    auto* const raw = reinterpret_cast<uint8*>(vp.Vehicle);
    switch (vp.BaseVehicleType) {
    case VEHICLE_TYPE_AUTOMOBILE: return raw[0x960];
    case VEHICLE_TYPE_BIKE:       return raw[0x804];
    default:                      return std::nullopt;
    }
}

namespace {
// Como el original: las interpolaciones de audio en linea del EXE usan `min + (max - min) * t` (x87 a 24 bits: cada
// operacion se redondea a float, igual que la aritmetica float de SSE).
inline float ExeLerp(float lo, float hi, float t) {
    return lo + (hi - lo) * t;
}

// Como el original (0x4F5C60 / 0x4F60B0): interpolacion por tramos de 5 puntos escrita en linea. El ultimo x de las tablas es
// 1.0001, el bucle se detiene en el primer x mayor que el valor, `x > 1.0` (doble) y `i == 5` devuelven el ultimo y, `x < 0`
// e `i == 0` el primero; y = y0 + (y1 - y0) * ((x - x0) / (x1 - x0)).
inline float ExeInlinePiecewise5(float x, const float (&p)[5][2]) {
    int16 i = 0;
    do {
        if (p[i][0] > x) {
            break;
        }
        ++i;
    } while (i < 5);
    if (x > 1.0 || i == 5) {
        return p[4][1];
    }
    if (x < 0.0f || i == 0) {
        return p[0][1];
    }
    const float t = (x - p[i - 1][0]) / (p[i][0] - p[i - 1][0]);
    return (p[i][1] - p[i - 1][1]) * t + p[i - 1][1];
}

// Como el original (0x4F51F0 / 0x4F53D0): saturacion a [0, 1] con fcom/test ah,5: >= 1 o NaN -> 1.0; < 0 -> 0.0; si no, el valor.
constexpr float G2Clamp01Exe(float r) {
    return (r < 1.0f) ? ((0.0f <= r) ? r : 0.0f) : 1.0f;
}

// Como el original (0x4F5080): pedal [0, 1] -> palabra de 16 bits (`fmul 255.0` + _ftol2) que se guarda sin recodificar.
inline int16 PedalToRaw(float v) {
    const float c = (1.0f < v) ? 1.0f : (v < 0.0f ? 0.0f : v);
    return static_cast<int16>(static_cast<int32>(c * 255.0f));
}

// Como el original: CAESoundManager::PlaySound de la DLL deja m_ClientVariable = 0 (por defecto de los parametros);
// el constructor del EXE (0x4EFE50, [esi+10h] = -1.0f) lo deja en -1.0f.
inline CAESound* PlayVehicleSound(CAESoundManager::tSoundPlayParams p) {
    p.ClientVariable = -1.0f;
    return AESoundManager.PlaySound(p);
}
}

// 0x4F63E0
CAEVehicleAudioEntity::CAEVehicleAudioEntity() :
    CAEAudioEntity(),
    m_SkidSound() {
    m_IsInitialized = false;
}

// 0x6D0A10
CAEVehicleAudioEntity::~CAEVehicleAudioEntity() {
    if (m_IsInitialized) {
        Terminate();
    }
}

// 0x4F7670
void CAEVehicleAudioEntity::Initialise(CEntity* entity) {
    m_TimeLastServiced            = 0;
    m_Entity                      = entity;
    m_IsPlayerDriver              = false;
    m_IsPlayerPassenger           = false;
    m_IsPlayerDriverAboutToExit   = false;
    m_IsWreckedVehicle            = false;
    m_State                       = eAEState::CAR_OFF;
    m_AuGear                      = 0;
    m_CrzCount                    = 0;
    m_DummySlot                   = SND_BANK_SLOT_NONE;
    m_RainDropCounter             = 0;
    m_DoCountStalls               = 0;
    m_StalledCount                = 0;
    m_SwapStalledTime             = 0;
    m_IsSilentStalled             = false;
    m_TimeSplashLastTriggered     = 0;
    m_TimeBeforeAllowAccelerate   = 0;
    m_TimeBeforeAllowCruise       = 0;
    m_bNitroOnLastFrame           = false;
    m_IsHelicopterDisabled        = false;
    m_ACPlayPositionThisFrame     = -1;
    m_ACPlayPositionLastFrame     = -1;
    m_ACPlayPositionWhenStopped   = 0;
    m_ACPlayPercentWhenStopped    = 0;
    m_FramesAgoACLooped           = 0;

    // Como el original (0x4F7720 mov word [ecx-4],ax / 0x4F7724 mov [ecx],ebx): solo escribe la palabra baja del tipo y el puntero
    for (int16 i = 0; i < AE_SOUND_ENGINE_MAX; ++i) {
        *reinterpret_cast<int16*>(&m_EngineSounds[i].EngineSoundType) = i;
        m_EngineSounds[i].Sound                                       = nullptr;
    }

    m_HornVolume                  = -100.0f;
    m_CurrentDummyEngineVolume    = -100.0f;
    m_SurfaceSoundType            = -1;
    m_RoadNoiseSoundType          = -1;
    m_FlatTireSoundType           = -1;
    m_ReverseGearSoundType        = -1;
    m_CurrentDummyEngineFrequency = -1.0f;
    m_CurrentRotorFrequency       = -1.0f;
    m_CurrentNitroRatio           = -1.0f;
    m_SurfaceSound                = nullptr;
    m_RoadNoiseSound              = nullptr;
    m_FlatTireSound               = nullptr;
    m_ReverseGearSound            = nullptr;
    m_HornSound                   = nullptr;
    m_SirenSound                  = nullptr;
    m_FastSirenSound              = nullptr;
    m_MovingPartSmoothedSpeed     = 0.0f;
    m_FadeIn                      = 1.0f;
    m_FadeOut                     = 0.0f;
    // Como el original (0x4F77B4 movsx eax,word [edx+22h] / 0x4F77D6 rep movsd): copia de 9 dwords (relleno incluido) desde 0x860AF0 + (modelo - 400) * 0x24
    const tVehicleAudioSettings& modelSettings = ::GetVehicleAudioSettings(static_cast<eModelID>(static_cast<int16>(entity->GetModelIndex())));
    std::memcpy(&m_AuSettings, &modelSettings, sizeof(m_AuSettings));
    m_HasSiren                    = entity->AsVehicle()->UsesSiren();

    if (m_AuSettings.RadioType == eAERadioType::AE_RT_UNKNOWN) {
        m_AuSettings.RadioStation = eRadioID::RADIO_OFF;
    }

    m_EventVolume = GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);
    m_IsSingleGear = notsa::contains({ // TODO: Add handling flag for this
        MODEL_PIZZABOY,
        MODEL_CADDY,
        MODEL_FAGGIO,
        MODEL_BAGGAGE,
        MODEL_FORKLIFT,
        MODEL_VORTEX,
        MODEL_KART,
        MODEL_MOWER,
        MODEL_SWEEPER,
        MODEL_TUG,
    }, entity->GetModelId());

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR: {
        m_EventVolume -= s_Config.VolOffsetOnGround;
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        if (m_IsInitialized) {
            return;
        }

        if (!notsa::contains<eSoundBank>({ SND_BANK_UNK, SND_BANK_GENRL_TEMP_D }, m_AuSettings.DummyBank)) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_BIKE:
    case AE_BMX: {
        m_PlayerEngineBank = m_AuSettings.PlayerBank;

        if (m_AuSettings.IsBike()) {
            m_EventVolume = m_EventVolume - s_Config.VolOffsetOnGround;
        }

        m_DummyEngineBank = m_AuSettings.DummyBank; // Como el original (0x4F7954..0x4F7961): se asigna antes de comprobar m_IsInitialized
        if (m_IsInitialized) {
            return;
        }

        if (m_DummyEngineBank != SND_BANK_UNK) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_BOAT:
    case AE_TRAIN: {
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        if (m_IsInitialized) {
            return;
        }

        if (!notsa::contains<eSoundBank>({ SND_BANK_UNK, SND_BANK_GENRL_TEMP_D }, m_AuSettings.DummyBank)) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_SEAPLANE: {
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        m_PlayerEngineBank = m_AuSettings.PlayerBank;

        m_IsInitialized    = true;
        return;
    }
    case AE_AIRCRAFT_PLANE: {
        m_DummyEngineBank  = m_AuSettings.DummyBank;
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        if (m_IsInitialized) {
            return;
        }

        if (m_AuSettings.DummyBank != SND_BANK_UNK) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    case AE_SPECIAL: {
        m_PlayerEngineBank = m_AuSettings.PlayerBank;
        m_EventVolume      = m_EventVolume - s_Config.VolOffsetOnGround; // Como el original (0x4F7948 fsub [8CBD50]): la misma constante que BIKE
        m_DummyEngineBank  = m_AuSettings.DummyBank;                     // antes de comprobar m_IsInitialized (0x4F7961)
        if (m_IsInitialized) {
            return;
        }

        if (m_DummyEngineBank != SND_BANK_UNK) {
            m_DummySlot = RequestBankSlot(m_AuSettings.DummyBank);
        }

        m_IsInitialized = true;
        return;
    }
    }
}

// 0x5B99F0
void CAEVehicleAudioEntity::StaticInitialise() {
    rng::fill(s_DummyEngineSlots, tDummyEngineSlot{});

    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_APACHE_D, SND_BANK_SLOT_COP_HELI);
    AEAudioHardware.LoadSoundBank(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN);

    s_pVehicleAudioSettingsForRadio = nullptr;
    s_pPlayerAttachedForRadio       = nullptr;
    s_pPlayerDriver                 = nullptr;
    s_NextDummyEngineSlot           = 0;
    s_HelicoptorsDisabled           = false;
}

// 0x4FB8C0
void CAEVehicleAudioEntity::Terminate() {
    if (!m_IsInitialized) {
        return;
    }

    // Como el original: primero los sonidos del motor...
    for (auto& es : m_EngineSounds) {
        GracefullyStopSound(es.Sound);
    }

    // ...despues derrape y freno de tren (velocidad 1, volumen -100) y los sonidos sueltos
    PlaySkidSound(-1, 1.0f, -100.0f);
    PlayTrainBrakeSound(-1, 1.0f, -100.0f);

    if (m_SurfaceSound) { // 0x4FB925: el tipo solo se restablece cuando habia sonido
        GracefullyStopSound(m_SurfaceSound);
        m_SurfaceSoundType = -1;
    }
    if (m_RoadNoiseSound) {
        GracefullyStopSound(m_RoadNoiseSound);
        m_RoadNoiseSoundType = -1;
    }
    if (m_FlatTireSound) {
        GracefullyStopSound(m_FlatTireSound);
        m_FlatTireSoundType = -1;
    }
    if (m_ReverseGearSound) {
        GracefullyStopSound(m_ReverseGearSound);
        m_ReverseGearSoundType = -1;
    }
    StopAndForgetSound(m_HornSound);
    StopAndForgetSound(m_SirenSound);
    StopAndForgetSound(m_FastSirenSound);

    // 0x4FBA0C - Radio (los punteros estaticos se limpian mas abajo)
    if (m_IsPlayerDriver) {
        switch (m_AuSettings.RadioType) {
        case AE_RT_CIVILIAN:
        case AE_RT_UNKNOWN:
        case AE_RT_EMERGENCY:
            AudioEngine.StopRadio(&m_AuSettings, false);
        }
    }

    // 0x4FBA43 - El slot solo pierde una referencia si todavia contiene el banco de esta entidad
    if (const int16 slotId = static_cast<int16>(m_DummySlot.get_underlying()); slotId != -1) {
        const int16 idx = static_cast<int16>(slotId - SND_BANK_SLOT_DUMMY_FIRST);
        if (idx >= 0 && idx < 10 && s_DummyEngineSlots[idx].BankID.get_underlying() == m_DummyEngineBank.get_underlying()) {
            auto& ref = s_DummyEngineSlots[idx].RefCnt;
            ref = ref > 0 ? static_cast<int16>(ref - 1) : static_cast<int16>(0);
        }
        m_DummySlot = SND_BANK_SLOT_NONE;
    }

    m_DummyEngineBank  = SND_BANK_UNK;
    m_PlayerEngineBank = SND_BANK_UNK;
    m_Entity           = nullptr;
    m_State            = eAEState::CAR_OFF;

    // 0x4FBAC4: los punteros estaticos de la radio se limpian tanto para el conductor como para el pasajero
    if (m_IsPlayerDriver) {
        m_IsPlayerDriver                = false;
        s_pPlayerDriver                 = nullptr;
        s_pPlayerAttachedForRadio       = nullptr;
        s_pVehicleAudioSettingsForRadio = nullptr;
    } else if (m_IsPlayerPassenger) {
        m_IsPlayerPassenger             = false;
        s_pPlayerAttachedForRadio       = nullptr;
        s_pVehicleAudioSettingsForRadio = nullptr;
    }

    m_IsPlayerDriverAboutToExit = false;
    m_IsInitialized             = false;
}

// 0x4F4ED0 - May return nullptr
tVehicleAudioSettings* CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio() {
    return s_pVehicleAudioSettingsForRadio;
}

// 0x4F4EF0
void CAEVehicleAudioEntity::EnableHelicoptors() {
    s_HelicoptorsDisabled = false; // Como el original: 0x4F4EF0 mov byte [B6B994],0
}

// 0x4F4EE0
void CAEVehicleAudioEntity::DisableHelicoptors() {
    s_HelicoptorsDisabled = true; // Como el original: 0x4F4EE0 mov byte [B6B994],1
}

// 0x4F5C00
void CAEVehicleAudioEntity::EnableHelicoptor() {
    m_IsHelicopterDisabled = false;
}

// 0x4F5BF0
void CAEVehicleAudioEntity::DisableHelicoptor() {
    m_IsHelicopterDisabled = true;
}

#pragma region Dummy Bank Slot Management
// 0x4F4E60
eSoundBankSlot CAEVehicleAudioEntity::DemandBankSlot(eSoundBank bankId) {
    // Como el original (0x4F4E60)
    const auto s = RequestBankSlot(bankId);
    if (static_cast<int16>(s) != -1) { // 0x4F4E6E cmp ax,0FFFFh
        return s;
    }

    // 0x4F4E74 - primer slot i >= 1 con menos referencias que el slot 0 (si no hay, el 0)
    const int16 ref0 = s_DummyEngineSlots[0].RefCnt;
    int16       idx  = 0;
    for (int16 i = 1; i < 10; ++i) {
        if (s_DummyEngineSlots[i].RefCnt < ref0) {
            idx = i;
            break;
        }
    }
    s_DummyEngineSlots[idx].RefCnt = 0;

    return RequestBankSlot(bankId);
}

// 0x4F4D10
eSoundBankSlot CAEVehicleAudioEntity::RequestBankSlot(eSoundBank bankId) {
    // Como el original (0x4F4D10)
    int16 lastFree = -1;
    for (int16 i = 0; i < 10; ++i) { // 0x4F4D20
        auto& s = s_DummyEngineSlots[i];
        if (s.BankID.get_underlying() == static_cast<int16>(bankId)) { // cmp word [..],bp
            s.RefCnt++;
            return static_cast<eSoundBankSlot>(SND_BANK_SLOT_DUMMY_FIRST + i);
        }
        if (!(s.RefCnt > 0)) {
            lastFree = i;
        }
    }
    if (lastFree == -1) {
        return SND_BANK_SLOT_NONE; // No hay slot libre
    }

    // 0x4F4D5F - Busca un slot libre empezando por s_NextDummyEngineSlot (con idiv con signo)
    int16       chosen = lastFree;
    const int32 next   = s_NextDummyEngineSlot;
    for (int16 k = 0; k < 10; ++k) {
        const int32 idx = (static_cast<int32>(k) + next) % 10;
        auto&       s   = s_DummyEngineSlots[static_cast<int16>(idx)];
        if (s.RefCnt > 0) {
            continue;
        }
        s.RefCnt = static_cast<int16>(s.RefCnt + 1);
        s_NextDummyEngineSlot = static_cast<int16>((idx + 1) % 10);
        chosen                = static_cast<int16>(idx);
        break;
    }

    // 0x4F4DB3 - Inicializa el slot
    const auto slot = static_cast<eSoundBankSlot>(chosen + SND_BANK_SLOT_DUMMY_FIRST);
    AESoundManager.CancelSoundsInBankSlot(slot, true);
    AEAudioHardware.LoadSoundBank(bankId, slot);
    s_DummyEngineSlots[chosen].BankID = bankId;
    s_DummyEngineSlots[chosen].RefCnt = 1;

    return slot;
}

// 0x4F4DF0
void CAEVehicleAudioEntity::StoppedUsingBankSlot(eSoundBankSlot bankSlot) {
    // Como el original (0x4F4DF0)
    if (auto* const s = GetDummyEngineSlot(bankSlot)) {
        s->RefCnt = s->RefCnt > 0 ? static_cast<int16>(s->RefCnt - 1) : static_cast<int16>(0);
    }
}

// notsa
auto CAEVehicleAudioEntity::GetDummyEngineSlot(eSoundBankSlot bankSlot) -> tDummyEngineSlot* {
    const auto s = static_cast<int16>(bankSlot - SND_BANK_SLOT_DUMMY_FIRST); // Como el original (0x4F4DF4 add eax,-7 / test ax,ax)
    return s >= 0 && s < static_cast<int16>(s_DummyEngineSlots.size())
        ? &s_DummyEngineSlots[s]
        : nullptr;
}

// 0x4F4E30
bool CAEVehicleAudioEntity::DoesBankSlotContainThisBank(eSoundBankSlot bankSlot, eSoundBank bankId) {
    // Como el original (0x4F4E30): compara la palabra baja del banco
    auto* const s = GetDummyEngineSlot(bankSlot);
    return s && s->BankID.get_underlying() == static_cast<int16>(bankId);
}
#pragma endregion

#pragma region Helpers
// 0x4F5C40
bool CAEVehicleAudioEntity::CopHeli() const noexcept {
    // Como el original (0x4F5C47 cmp ax,1E7h / 0x4F5C4D cmp ax,1E8h)
    const auto modelIndex = m_Entity->GetModelIndex();
    return modelIndex == MODEL_MAVERICK || modelIndex == MODEL_VCNMAV;
}

// 0x4F5EB0
void CAEVehicleAudioEntity::UpdateGasPedalAudio(CVehicle* vehicle, int32 vehicleType) {
    // Como el original (0x4F5EB0): solo automoviles (tipo 0, +0x964) y motos (tipo 9, +0x808); el resto no hace nada
    const float gas = std::fabs(vehicle->m_GasPedal);
    float*      revs;
    if (vehicleType == 0) {
        revs = &vehicle->AsAutomobile()->m_GasPedalAudioRevs;
    } else if (vehicleType == 9) {
        revs = &vehicle->AsBike()->m_GasPedalAudioRevs;
    } else {
        return;
    }

    const float cur = *revs;
    if (gas > cur) { // 0x4F5EDF fcomp [ecx] / test ah,41h / jne
        const float up = s_Config.GasPedal.StepUp + cur; // 0x4F5EE8 fld [8CBC24] / fadd [ecx]
        *revs          = up < gas ? up : gas;
    } else {
        const float down = cur - s_Config.GasPedal.StepDown; // 0x4F5F08 fsub [8CBC28]
        *revs            = down > gas ? down : gas;
    }
}

// 0x4F5080
void CAEVehicleAudioEntity::GetAccelAndBrake(tVehicleParams& vp) const noexcept {
    // Como el original (0x4F5080): los pedales se guardan como palabras de 16 bits (sin recodificar)
    using Pedal = std::remove_reference_t<decltype(vp.ThisAccel)>;
    if (CReplay::Mode == MODE_PLAYBACK) {
        vp.ThisAccel = Pedal{ PedalToRaw(vp.Vehicle->m_GasPedal) };
        vp.ThisBrake = Pedal{ PedalToRaw(vp.Vehicle->m_BrakePedal) };
    } else if (
           s_pPlayerDriver
        && !m_IsPlayerDriverAboutToExit
        && s_pPlayerDriver->m_nPedState != PEDSTATE_ARRESTED
        && s_pPlayerDriver->IsAlive()
    ) {
        CPad* pad    = s_pPlayerDriver->AsPlayer()->GetPadFromPlayer();
        vp.ThisAccel = Pedal{ static_cast<int16>(pad->GetAccelerate()) };
        vp.ThisBrake = Pedal{ static_cast<int16>(pad->GetBrake()) };
    } else {
        vp.ThisAccel = Pedal{ static_cast<int16>(0) };
        vp.ThisBrake = Pedal{ static_cast<int16>(0) };
    }
}


// 0x4F5C10
tVehicleAudioSettings CAEVehicleAudioEntity::GetVehicleAudioSettings(int16 vehId) {
    // Como el original (0x4F5C10 movsx eax,word [esp+8] / sub eax,190h / rep movsd de 9 dwords): el argumento es el id de modelo
    // (400..), no el indice, y se copian los 0x24 bytes de la entrada (relleno incluido)
    const tVehicleAudioSettings& entry = ::GetVehicleAudioSettings(static_cast<eModelID>(vehId));
    tVehicleAudioSettings out;
    std::memcpy(&out, &entry, sizeof(out));
    return out;
}

// 0x4F4F00
eAEVehicleAudioType CAEVehicleAudioEntity::GetVehicleTypeForAudio() const noexcept {
    // Como el original (0x4F4F00 movsx eax,byte [ecx+9Ch] / cmp eax,26h / ja; tabla de bytes en 0x4F4F48): coche 0, moto 1, generico 2
    if (!m_IsInitialized || !m_Entity) {
        return eAEVehicleAudioType::GENERIC;
    }
    switch (m_AuSettings.VehicleAudioTypeForName) {
    case AE_VAT_OFFROAD:
    case AE_VAT_TWO_DOOR:
    case AE_VAT_SPORTS_CAR:
    case AE_VAT_STATION_WAGON:
    case AE_VAT_SEDAN:
    case AE_VAT_STRETCH:
    case AE_VAT_LOWRIDER:
    case AE_VAT_TAXI:
    case AE_VAT_CONVERTIBLE:
    case AE_VAT_HEARSE:
    case AE_VAT_COUPE:
    case AE_VAT_POLICE_CAR:
        return eAEVehicleAudioType::CAR;
    case AE_VAT_MOPED:
    case AE_VAT_BIKE:
    case AE_VAT_QUADBIKE:
        return eAEVehicleAudioType::BIKE;
    default:
        return eAEVehicleAudioType::GENERIC;
    }
}

// 0x4F6150
float CAEVehicleAudioEntity::GetFlyingMetalVolume(CPhysical* physical) const noexcept {
    // Como el original (0x4F6150)
    const auto& ts   = physical->m_vecTurnSpeed;
    const float mag2 = ts.x * ts.x + ts.y * ts.y + ts.z * ts.z;
    const float lim  = std::bit_cast<float>(0x3F100000u); // 0.5625 = 0.75^2 (0x862CE8)
    const float v    = (mag2 < lim ? mag2 : lim) * std::bit_cast<float>(0x3FE38E39u); // 1/0.5625 (0x862CE4)
    if (v < std::bit_cast<float>(0x2EDBE6FEu)) { // 0x862CE0 (no es el float mas cercano a 1e-10)
        return -100.0f;
    }
    return AudioLog10ScaledExe(v, 10.0f); // 0x4F61CC: ST0 conserva fyl2x hasta fmul, sin redondeo intermedio.
}
#pragma endregion

#pragma region Base Sound Functions
// 0x4FB6C0
void CAEVehicleAudioEntity::UpdateParameters(CAESound* sound, int16 curPlayPos) {
    if (!sound) {
        return;
    }

    if (sound->m_Event == AE_BONNET_FLUBBER_FLUBBER) {
        if (sound->m_PhysicalEntity) {
            sound->m_Volume = GetDefaultVolume(AE_BONNET_FLUBBER_FLUBBER) + GetFlyingMetalVolume(sound->m_PhysicalEntity->AsPhysical());
        }
        return;
    }

    if (sound->m_Event == AE_BOAT_HIT_WAVE) {
        if (curPlayPos == -1) {
            return;
        }

        sound->SetPosition(m_Entity->GetPosition());
        if (CWeather::UnderWaterness >= 0.5F) {
            sound->StopSound();
        }
    }

    if (curPlayPos == -1) {
        if (sound == m_SurfaceSound) {
            m_SurfaceSound = nullptr;
        } else if (sound == m_RoadNoiseSound) {
            m_RoadNoiseSound = nullptr;
        } else if (sound == m_FlatTireSound) {
            m_FlatTireSound = nullptr;
        } else if (sound == m_ReverseGearSound) {
            m_ReverseGearSound = nullptr;
        } else if (sound == m_HornSound) {
            m_HornSound = nullptr;
        } else if (sound == m_SirenSound) {
            m_SirenSound = nullptr;
        } else if (sound == m_FastSirenSound) {
            m_FastSirenSound = nullptr;
        } else if (const auto it = rng::find(m_EngineSounds, sound, &tEngineSound::Sound); it != m_EngineSounds.end()) {
            it->Sound = nullptr;
        }
    } else if (m_AuSettings.IsAircraftEngineSound() && GetEngineSound(AE_SOUND_AIRCRAFT_NEAR) == sound) { // 0x4FB7F1
        sound->SetPosition(GetAircraftNearPosition());
    } else {
        sound->SetPosition(m_Entity->GetPosition());
    }

    if (m_AuSettings.IsCarEngineSound() && sound == m_EngineSounds[AE_SOUND_PLAYER_AC].Sound) {
        m_ACPlayPositionLastFrame = std::exchange(m_ACPlayPositionThisFrame, curPlayPos);
    }
}

// 0x4F6420
void CAEVehicleAudioEntity::AddAudioEvent(eAudioEvents event, float p1) {
    // Como el original (0x4F6420)
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_BULLET_HITS, SND_BANK_SLOT_BULLET_HITS)) {
        return;
    }

    // Como el original (0x4F68D4..0x4F6907): -3 para motos, -6 para BMX
    const auto GetPedEventVolume = [this](eAudioEvents ae) {
        float vol = GetDefaultVolume(ae);
        switch (m_AuSettings.VehicleAudioType) {
        case AE_BIKE: vol -= 3.f; break;
        case AE_BMX:  vol -= 6.f; break;
        }
        return vol;
    };
    const float pedFreqVariance = std::bit_cast<float>(0x3D75C290u); // 0x4F699D: NO es el float mas cercano a 0.06

    switch (event) {
    case AE_CAR_BONNET_OPEN:
    case AE_CAR_BOOT_OPEN:
    case AE_CAR_FRONT_LEFT_DOOR_OPEN:
    case AE_CAR_FRONT_RIGHT_DOOR_OPEN:
    case AE_CAR_REAR_LEFT_DOOR_OPEN:
    case AE_CAR_REAR_RIGHT_DOOR_OPEN: { // 0x4F64B3
        const auto PlaySound = [&](eSoundID sfx) {
            PlayVehicleSound({
                .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
                .SoundID           = sfx,
                .AudioEntity       = this,
                .Pos               = GetVehicle()->GetPosition(),
                .Volume            = GetDefaultVolume(AE_CAR_BONNET_OPEN),
                .FrequencyVariance = 0.02f,
            });
        };
        using enum eAEVehicleDoorType;
        switch (m_AuSettings.DoorType) {
        case LIGHT: PlaySound(SND_GENRL_VEHICLE_GEN_OPLIGHT); break;
        case OLD:   PlaySound(SND_GENRL_VEHICLE_GEN_OPOLD);   break;
        case TRUCK: PlaySound(SND_GENRL_VEHICLE_GEN_OTRUCK);  break;
        case VAN:   PlaySound(SND_GENRL_VEHICLE_GEN_OPVAN);   break;
        default:    PlaySound(SND_GENRL_VEHICLE_GEN_OPNEW);   break; // UNSET, NEW y valores fuera de rango (0x4F64E2)
        }
        break;
    }
    case AE_CAR_BONNET_CLOSE:
    case AE_CAR_BOOT_CLOSE:
    case AE_CAR_FRONT_LEFT_DOOR_CLOSE:
    case AE_CAR_FRONT_RIGHT_DOOR_CLOSE:
    case AE_CAR_REAR_LEFT_DOOR_CLOSE:
    case AE_CAR_REAR_RIGHT_DOOR_CLOSE: { // 0x4F654D
        const auto PlaySound = [&](eSoundID sfx) {
            PlayVehicleSound({
                .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
                .SoundID           = sfx,
                .AudioEntity       = this,
                .Pos               = GetVehicle()->GetPosition(),
                .Volume            = GetDefaultVolume(AE_CAR_BONNET_CLOSE),
                .FrequencyVariance = 0.02f,
            });
        };
        using enum eAEVehicleDoorType;
        switch (m_AuSettings.DoorType) {
        case LIGHT: PlaySound(SND_GENRL_VEHICLE_GEN_CLLIGHT); break;
        case OLD:   PlaySound(SND_GENRL_VEHICLE_GEN_CLOLD);   break;
        case TRUCK: PlaySound(SND_GENRL_VEHICLE_GEN_CTRUCK);  break;
        case VAN:   PlaySound(SND_GENRL_VEHICLE_GEN_CLVAN);   break;
        default:    PlaySound(SND_GENRL_VEHICLE_GEN_CLNEW);   break; // UNSET, NEW y valores fuera de rango (0x4F657C)
        }
        break;
    }
    case AE_TYRE_BURST: { // 0x4F65AC
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_BULLET_HITS,
            .SoundID           = SND_GENRL_BULLET_HITS_BLOWOUT,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_TYRE_BURST),
        });
        break;
    }
    case AE_WINDSCREEN_SHATTER: { // 0x4F65FA
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_WINDSCREEN,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_WINDSCREEN_SHATTER),
        });
        break;
    }
    case AE_LIGHT_SMASH: { // 0x4F6644
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_HEADLIGHT,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_LIGHT_SMASH),
        });
        break;
    }
    case AE_BOAT_HIT_WAVE: { // 0x4F66B1
        const auto* const cfg = &s_Config.Boat.WaveHit;

        const float defVol = GetDefaultVolume(AE_BOAT_HIT_WAVE);
        if (CTimer::GetTimeInMS() < m_TimeSplashLastTriggered + cfg->WaitTime) {
            break;
        }
        if (p1 < cfg->VolMin) {
            break;
        }
        float vol = p1 / cfg->VolMax;
        if (vol < 1.0f && 0.0f > vol) { // 0x4F6718
            break;
        }
        if (!(vol < 1.0f)) { // 0x4F672D
            vol = 1.0f;
        } else if (!(vol > 0.00001f)) { // 0x4F6748 fcomp [858C14] / test ah,41h / jne
            break;
        }
        if (!(CWeather::UnderWaterness < 0.5f)) { // 0x4F6763 fcomp [858B8C] + test ah,5 / jp: >= 0.5 o NaN
            break;
        }
        PlayVehicleSound({
            .BankSlotID    = SND_BANK_SLOT_COLLISIONS,
            .SoundID     = SND_GENRL_COLLISIONS_SPLASH_END,
            .AudioEntity = this,
            .Pos         = GetVehicle()->GetPosition(),
            .Volume      = (AudioLog10ScaledExe(vol, 20.0f) + StaticRef<float>(0xB6B9C8)) + defVol,
        });
        m_TimeSplashLastTriggered = CTimer::GetTimeInMS(); // 0x4F67F1
        break;
    }
    case AE_TRAIN_CLACK: { // 0x4F6802
        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            break;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = m_DummySlot,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_TRAIN_D_TRACK_JOIN_3, SND_GENRL_TRAIN_D_TRACK_JOIN_5),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_TRAIN_CLACK),
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.1f,
        });
        break;
    }
    case AE_NITRO_IGNITION: { // 0x4F6D7F
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
            break;
        }
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_NITRO_IGNITION, this)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID           = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID            = SND_GENRL_WEAPONS_TRIGGER,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_NITRO_IGNITION),
            .RollOffFactor      = 2.f,
            .Speed              = 0.89f,
            .FrequencyVariance  = 0.03f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_NITRO_IGNITION
        });
        break;
    }
    case AE_START_ENGINE: { // 0x4F6E49
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_GENERIC_START,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = GetDefaultVolume(AE_START_ENGINE),
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.05f,
            .EventID           = AE_START_ENGINE
        });
        break;
    }
    case AE_SUSPENSION_HYDRAULIC: { // 0x4F70C8
        const float vol = GetDefaultVolume(AE_SUSPENSION_HYDRAULIC) + (-6.0f); // 0x8CC1B8
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SUSPENSION_HYDRAULIC, this)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_HYDRAULIC_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.05f,
            .EventID           = AE_SUSPENSION_HYDRAULIC
        });
        break;
    }
    case AE_SUSPENSION_BOUNCE: { // 0x4F6ECD
        const float vol = GetDefaultVolume(AE_SUSPENSION_BOUNCE) + (-3.0f); // 0x8CC1BC
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SUSPENSION_BOUNCE, this)) { // 0x4F6EF1
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_BOUNCE_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.05f,
            .EventID           = AE_SUSPENSION_BOUNCE
        });
        break;
    }
    case AE_SUSPENSION_TRIGGER: { // 0x4F6F6E
        const float vol = GetDefaultVolume(AE_SUSPENSION_TRIGGER) + StaticRef<float>(0xB6BAC4);
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SUSPENSION_TRIGGER, this)) { // 0x4F6F92
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_TRIGGER_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 2.f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = 0.05f,
            .PlayTime          = 0,
            .EventID           = AE_SUSPENSION_TRIGGER,
        });
        break;
    }
    case AE_SUSPENSION_ON: { // 0x4F701B
        const float vol = GetDefaultVolume(AE_SUSPENSION_ON) + StaticRef<float>(0xB6BAC0);
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SUSPENSION_ON, this)) { // 0x4F703F
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_TRIGGER_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 2.f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = 0.05f,
            .PlayTime          = 0,
            .EventID           = AE_SUSPENSION_ON,
        });
        break;
    }
    case AE_SUSPENSION_OFF: { // 0x4F7169
        const float vol = GetDefaultVolume(AE_SUSPENSION_OFF) + (-6.0f); // 0x8CC1B4
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_SUSPENSION_OFF, this)) { // 0x4F718D
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_VEHICLE_GEN,
            .SoundID           = SND_GENRL_VEHICLE_GEN_HYDRAULIC_SUSPENSION,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 2.f,
            .FrequencyVariance = 0.05f,
            .EventID           = AE_SUSPENSION_OFF,
        });
        break;
    }
    case AE_TRAILER_ATTACH: { // 0x4F72CA
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_ATTACH, this)) {
            break;
        }
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_DETACH, this)) {
            break;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID           = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID            = SND_GENRL_WEAPONS_SAWNOFF_B,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_TRAILER_ATTACH),
            .RollOffFactor      = 2.f,
            .Speed              = 0.56f,
            .FrequencyVariance  = 0.05f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_TRAILER_ATTACH,
        });
        break;
    }
    case AE_TRAILER_DETACH: { // 0x4F73AA
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_ATTACH, this)) {
            break;
        }
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_TRAILER_DETACH, this)) {
            break;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_WEAPONS, SND_BANK_SLOT_WEAPON_GEN)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID           = SND_BANK_SLOT_WEAPON_GEN,
            .SoundID            = SND_GENRL_WEAPONS_SAWNOFF_B,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_TRAILER_DETACH),
            .RollOffFactor      = 2.f,
            .Speed              = 0.5f,
            .FrequencyVariance  = 0.05f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_TRAILER_DETACH,
        });
        break;
    }
    case AE_BODY_HARVEST: { // 0x4F720A
        if (AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_BODY_HARVEST, this)) {
            break;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HARVESTER_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID           = SND_BANK_SLOT_PLAYER_ENGINE_P,
            .SoundID            = 4,
            .AudioEntity        = this,
            .Pos                = GetVehicle()->GetPosition(),
            .Volume             = GetDefaultVolume(AE_BODY_HARVEST),
            .RollOffFactor      = 2.f,
            .FrequencyVariance  = 0.05f,
            .RegisterWithEntity = GetVehicle(),
            .EventID            = AE_BODY_HARVEST,
        });
        break;
    }
    case AE_PED_CRUNCH: { // 0x4F68CA
        const float vol = GetPedEventVolume(AE_PED_CRUNCH);
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        const bool playing = AESoundManager.AreSoundsOfThisEventPlayingForThisEntity(AE_PED_CRUNCH, this) != 0;
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_COLCARPED,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 1.5f,
            .Speed             = playing ? 0.8f : 0.94f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = pedFreqVariance,
            .PlayTime          = static_cast<int16>(playing ? 40 : 0),
            .EventID           = AE_PED_CRUNCH
        });
        break;
    }
    case AE_PED_DRIVE_OVER: { // 0x4F6B35
        const float vol = GetPedEventVolume(AE_PED_DRIVE_OVER);
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_GORE_SPLAT1, SND_GENRL_COLLISIONS_GORE_SPLAT3),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 1.5f,
            .Flags             = SOUND_START_PERCENTAGE, // 0x4F6BF7
            .FrequencyVariance = pedFreqVariance,
            .PlayTime          = 0,
            .EventID           = AE_PED_KNOCK_DOWN // Como el original (0x4F6C03 mov [esp+20h],79h): el EXE usa el evento de KNOCK_DOWN
        });
        break;
    }
    case AE_PED_KNOCK_DOWN: { // 0x4F69C6
        const float vol = GetPedEventVolume(AE_PED_KNOCK_DOWN);
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_COLSOLIDWOOD,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol - 2.0f, // 0x4F6A30 fsub [858CA0]
            .RollOffFactor     = 1.5f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = pedFreqVariance,
            .PlayTime          = 0,
            .EventID           = AE_PED_KNOCK_DOWN
        });
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_COLCAR01, SND_GENRL_COLLISIONS_COLCAR12),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 1.5f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = pedFreqVariance,
            .PlayTime          = 55,
            .EventID           = AE_PED_KNOCK_DOWN
        });
        break;
    }
    case AE_PED_BOUNCE: { // 0x4F6C10
        const float vol = GetPedEventVolume(AE_PED_BOUNCE);
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            break;
        }
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = SND_GENRL_COLLISIONS_COLGRASS,
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol - 2.0f, // 0x4F6C7A fsub [858CA0]
            .RollOffFactor     = 1.5f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = pedFreqVariance,
            .PlayTime          = 0,
            .EventID           = AE_PED_BOUNCE
        });
        PlayVehicleSound({
            .BankSlotID          = SND_BANK_SLOT_COLLISIONS,
            .SoundID           = CAEAudioUtility::GetRandomNumberInRange<eSoundID>(SND_GENRL_COLLISIONS_COLCAR01, SND_GENRL_COLLISIONS_COLCAR12),
            .AudioEntity       = this,
            .Pos               = GetVehicle()->GetPosition(),
            .Volume            = vol,
            .RollOffFactor     = 1.5f,
            .Flags             = SOUND_START_PERCENTAGE,
            .FrequencyVariance = pedFreqVariance,
            .PlayTime          = 25,
            .EventID           = AE_PED_BOUNCE
        });
        break;
    }
    }
}

// 0x4F7580
void CAEVehicleAudioEntity::AddAudioEvent(eAudioEvents event, CPhysical* physical) {
    // Como el original (0x4F7580): usa m_tempSound de la entidad (no una CAESound de pila)
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
        return;
    }
    if (event != AE_BONNET_FLUBBER_FLUBBER) {
        return;
    }
    const float defVol = GetDefaultVolume(AE_BONNET_FLUBBER_FLUBBER);
    if (!physical) {
        return;
    }
    const float flying = GetFlyingMetalVolume(physical);
    m_tempSound.Initialise(
        SND_BANK_SLOT_VEHICLE_GEN,
        SND_GENRL_VEHICLE_GEN_HOOD_FLY,
        this,
        physical->GetPosition(),
        flying + defVol,
        1.0f,
        1.0f,
        1.0f,
        0,
        0,
        0.0f,
        0
    );
    m_tempSound.m_RollOffFactor = 0.85f;
    m_tempSound.m_Flags         = SOUND_LIFESPAN_TIED_TO_PHYSICAL_ENTITY | SOUND_REQUEST_UPDATES;
    m_tempSound.RegisterWithPhysicalEntity(physical);
    m_tempSound.m_Event = AE_BONNET_FLUBBER_FLUBBER;
    AESoundManager.RequestNewSound(&m_tempSound);
}

// 0x502280
void CAEVehicleAudioEntity::Service() {
    // Como el original (0x502280)
    if (!m_IsInitialized) {
        return;
    }

    const auto veh = m_Entity->AsVehicle();

    if (m_IsPlayerDriver && s_pPlayerDriver && !m_IsPlayerDriverAboutToExit) {
        if (s_pPlayerDriver->GetPedState() == PEDSTATE_ARRESTED || !s_pPlayerDriver->IsAlive()) {
            PlayerAboutToExitVehicleAsDriver();
        }
    }

    const auto isWrecked = m_Entity->GetStatus() == STATUS_WRECKED;
    if (isWrecked && !m_IsWreckedVehicle) {
        JustWreckedVehicle();
    }
    m_IsWreckedVehicle = isWrecked;

    const bool isPlayerDriver    = veh->m_pDriver && veh->m_pDriver->IsPlayer();
    const bool isPlayerVehicle   = CAEAudioUtility::FindVehicleOfPlayer() == m_Entity;
    const bool isPlayerPassenger = isPlayerVehicle && !isPlayerDriver;
    const bool wasPlayerDriver    = m_IsPlayerDriver;
    const bool wasPlayerPassenger = m_IsPlayerPassenger;

    if (isPlayerDriver) {
        if (!wasPlayerDriver) { // 0x5023DF
            JustGotInVehicleAsDriver();
        }
        if (!wasPlayerDriver && !wasPlayerPassenger) { // 0x502362
            TurnOnRadioForVehicle();
        }
    } else {
        if (wasPlayerDriver) { // 0x50233F
            JustGotOutOfVehicleAsDriver();
        }
        if (isPlayerPassenger) { // 0x502346
            if (!wasPlayerPassenger && !wasPlayerDriver) {
                TurnOnRadioForVehicle();
            }
        } else if (wasPlayerDriver || wasPlayerPassenger) { // 0x502385 (ni conductor ni vehiculo del jugador)
            TurnOffRadioForVehicle();
        }
    }

    m_IsPlayerDriver    = isPlayerDriver;
    m_IsPlayerPassenger = isPlayerPassenger;

    ProcessVehicle(m_Entity->AsPhysical());
}

void CAEVehicleAudioEntity::StaticService() {
    // NOP
}

#pragma endregion

#pragma region Generic Process Functions
// 0x5002C0
void CAEVehicleAudioEntity::ProcessVehicleSirenAlarmHorn(tVehicleParams& vp) {
    // Figure out sirens state
    bool isFastSirenOn = false,
        isSirenOn     = false;
    GetSirenState(isSirenOn, isFastSirenOn, vp);

    // If no siren, possibly use horn
    bool isHornOn = false;
    if (!isSirenOn) {
        if (!vp.Vehicle->m_nAlarmState || vp.Vehicle->m_nAlarmState == 0xFFFF || vp.Vehicle->GetStatus() == STATUS_WRECKED) { // 0x500303: cmp cx,0FFFFh (m_nAlarmState es uint16)
            GetHornState(&isHornOn, vp);
        } else {
            const auto time = CTimer::GetTimeInMS();
            if (time > vp.Vehicle->m_HornCounter) {
                vp.Vehicle->m_HornCounter = time + 750;
            }
            isHornOn = vp.Vehicle->m_HornCounter < time + 750 / 2;
        }
    }

    // Play sounds now
    PlayHornOrSiren(isHornOn, isSirenOn, isFastSirenOn, vp);
    m_IsSirenOn     = isSirenOn;
    m_IsFastSirenOn = isFastSirenOn;
    m_IsHornOn      = isHornOn;
}
#pragma endregion

#pragma region Player Car (AE_CAR)
// 0x4F4F70
bool CAEVehicleAudioEntity::IsAccInhibited(tVehicleParams& vp) const noexcept {
    // Como el original (0x4F4F81 call 4D88C0 / 0x4F4F8E cmp word [ecx+22h],0 jg / 0x4F4F9A cmp byte [ecx+39h] / 0x4F4FC0 inline)
    return !AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)
        || vp.ThisBrake > 0.f
        || vp.IsHandbrakeOn
        || IsAccInhibitedBackwards(vp);
}

// inlined?
// 0x4F4FC0
bool CAEVehicleAudioEntity::IsAccInhibitedBackwards(tVehicleParams& vp) const noexcept {
    return vp.WheelSpin > 5.0f
        || !vp.NumDriveWheelsOnGround;
}

// 0x4F4FF0
bool CAEVehicleAudioEntity::IsAccInhibitedForLowSpeed(tVehicleParams& vp) const noexcept {
    return !m_IsSingleGear && vp.Speed < s_Config.PlayerEngine.AC.InhibitForLowSpeedLimit;
}

// 0x4F5020
bool CAEVehicleAudioEntity::IsAccInhibitedForTime() const noexcept {
    // Como el original (0x4F5026 cmp eax,[B7CB84] / setb al): comparacion sin signo
    return m_TimeBeforeAllowAccelerate < CTimer::GetTimeInMS();
}

// 0x4F5030
void CAEVehicleAudioEntity::InhibitAccForTime(uint32 time) {
    m_TimeBeforeAllowAccelerate = time + CTimer::GetTimeInMS();
}

// 0x4F5060
void CAEVehicleAudioEntity::InhibitCrzForTime(uint32 time) {
    m_TimeBeforeAllowCruise = time + CTimer::GetTimeInMS();
}

// 0x4F5050
bool CAEVehicleAudioEntity::IsCrzInhibitedForTime() const noexcept {
    // Como el original (0x4F5056 cmp eax,[B7CB84] / setb al): comparacion sin signo
    return m_TimeBeforeAllowCruise < CTimer::GetTimeInMS();
}

#pragma endregion

#pragma region Dummies
// 0x4FCA10
void CAEVehicleAudioEntity::ProcessDummyStateTransition(eAEState newState, float ratio, tVehicleParams& vp) {
    // Como el original (0x4FCA10). El EXE llama siempre primero al volumen y despues a la frecuencia.
    const auto Min1 = [](float v) { return (v < 1.0f) ? v : 1.0f; }; // fcom 1.0 / jnp

    m_TimeLastServiced = CTimer::GetTimeInMS();

    const auto oldState = m_State;
    if (oldState == eAEState::DUMMY_ID) {
        if (newState == eAEState::DUMMY_ID) { // 0x4FCA3D
            const auto* const cfg = &s_Config.DummyCar.Transitions.FromIdleToIdle;

            m_State  = newState;
            m_FadeIn = Min1(cfg->FadeInStep + m_FadeIn);
            const float volI = GetVolumeForDummyIdle(ratio, m_FadeIn);
            const float frqI = GetFrequencyForDummyIdle(ratio, m_FadeIn);
            UpdateVehicleEngineSound(AE_SOUND_CAR_ID, frqI, volI);

            m_FadeOut = Min1(cfg->FadeOutStep + m_FadeOut);
            if (m_FadeOut >= 0.99 || std::isnan(m_FadeOut)) { // 0x4FCAE5 fcomp qword [00862CD8] (0.99 doble)
                StopGenericEngineSound(AE_SOUND_CAR_REV);
            } else {
                const float volR = GetVolumeForDummyRev(ratio, m_FadeOut);
                const float frqR = GetFrequencyForDummyRev(ratio, m_FadeOut);
                UpdateVehicleEngineSound(AE_SOUND_CAR_REV, frqR, volR);
            }
        } else if (newState == eAEState::DUMMY_CRZ) { // 0x4FCB42
            m_State   = newState;
            m_FadeOut = 0.f;
            const float volI = GetVolumeForDummyIdle(ratio, 0.f);
            const float frqI = GetFrequencyForDummyIdle(ratio, m_FadeOut);
            UpdateVehicleEngineSound(AE_SOUND_CAR_ID, frqI, volI);

            m_FadeIn = 0.f;
            const float volR = GetVolumeForDummyRev(ratio, 0.f);
            const float frqR = GetFrequencyForDummyRev(ratio, m_FadeIn);
            StartVehicleEngineSound(AE_SOUND_CAR_REV, frqR, volR);
        } else { // 0x4FCBDF
            m_State = eAEState::CAR_OFF;
            StopGenericEngineSound(AE_SOUND_CAR_ID);
            StopGenericEngineSound(AE_SOUND_CAR_REV);
        }
    } else if (oldState == eAEState::DUMMY_CRZ) {
        if (newState == eAEState::DUMMY_ID) { // 0x4FCC46
            m_State   = newState;
            m_FadeOut = 0.f;
            const float volR = GetVolumeForDummyRev(ratio, 0.f);
            const float frqR = GetFrequencyForDummyRev(ratio, m_FadeOut);
            UpdateVehicleEngineSound(AE_SOUND_CAR_REV, frqR, volR);

            m_FadeIn = 0.f;
            const float volI = GetVolumeForDummyIdle(ratio, 0.f);
            const float frqI = GetFrequencyForDummyIdle(ratio, m_FadeIn);
            StartVehicleEngineSound(AE_SOUND_CAR_ID, frqI, volI);
        } else if (newState == eAEState::DUMMY_CRZ) { // 0x4FCCE9
            const auto* const cfg = &s_Config.DummyCar.Transitions.FromCrzToCrz;

            m_State  = newState;
            m_FadeIn = Min1(cfg->FadeInStep + m_FadeIn);
            const float volR = GetVolumeForDummyRev(ratio, m_FadeIn);
            const float frqR = GetFrequencyForDummyRev(ratio, m_FadeIn);
            UpdateVehicleEngineSound(AE_SOUND_CAR_REV, frqR, volR);

            m_FadeOut = Min1(cfg->FadeOutStep + m_FadeOut);
            if (m_FadeOut >= 0.99 || std::isnan(m_FadeOut)) { // 0x4FCD76 (0.99 doble) -> 0x4FCDF4
                StopGenericEngineSound(AE_SOUND_CAR_ID);
            } else {
                if (m_FadeOut > 0.65 && m_FadeOut < 0.75) { // 0x4FCD89 (0.65 y 0.75 dobles)
                    vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                }
                const float volI = GetVolumeForDummyIdle(ratio, m_FadeOut);
                const float frqI = GetFrequencyForDummyIdle(ratio, m_FadeOut);
                UpdateVehicleEngineSound(AE_SOUND_CAR_ID, frqI, volI);
            }
        } else { // 0x4FCDFA
            m_State = eAEState::CAR_OFF;
            StopGenericEngineSound(AE_SOUND_CAR_REV);
            StopGenericEngineSound(AE_SOUND_CAR_ID);
        }
    } else if (oldState == eAEState::CAR_OFF || oldState == eAEState::NUM_STATES) { // 0x4FCE52
        if (newState == eAEState::DUMMY_ID) { // 0x4FCE6B
            if (oldState == eAEState::CAR_OFF) {
                AddAudioEvent(AE_START_ENGINE, 0.f);
            }
            m_State   = eAEState::DUMMY_ID;
            m_FadeOut = 1.f;
            m_FadeIn  = 1.f;
            const float volI = GetVolumeForDummyIdle(ratio, 1.f);
            const float frqI = GetFrequencyForDummyIdle(ratio, 1.f);
            StartVehicleEngineSound(AE_SOUND_CAR_ID, frqI, volI);
        } else if (newState == eAEState::DUMMY_CRZ) { // 0x4FCEC8
            if (oldState == eAEState::CAR_OFF) {
                AddAudioEvent(AE_START_ENGINE, 0.f);
            }
            m_State   = eAEState::DUMMY_CRZ;
            m_FadeOut = 1.f;
            m_FadeIn  = 1.f;
            const float volR = GetVolumeForDummyRev(ratio, 1.f);
            const float frqR = GetFrequencyForDummyRev(ratio, 1.f);
            StartVehicleEngineSound(AE_SOUND_CAR_REV, frqR, volR);
        } else { // 0x4FCF2B
            m_State = eAEState::CAR_OFF;
        }
    } else { // 0x4FCF2B
        m_State = eAEState::CAR_OFF;
    }
}

// @notsa
constexpr float CAEVehicleAudioEntity::GetDummyIdleRatioProgress(float ratio) {
    return G2Clamp01Exe(ratio / s_Config.DummyEngine.ID.Ratio); // Como el original (0x4F5212 fdiv / 0x4F5218 fcom): NaN -> 1.0
}

// 0x4F51F0
float CAEVehicleAudioEntity::GetVolumeForDummyIdle(float ratio, float fadeRatio) const noexcept {
    // Como el original (0x4F51F0)
    const auto* const cfg = &s_Config.DummyEngine;
    if (GetVehicle()->GetModelId() == MODEL_CADDY) {
        return cfg->ID.VolumeBase - 30.0f;
    }

    float volume = ExeLerp(cfg->ID.VolumeBase, cfg->ID.VolumeMax, GetDummyIdleRatioProgress(ratio));
    float term; // El EXE lo suma al final: (term + EngineVolumeOffset) + volume
    if (m_State == eAEState::DUMMY_CRZ) {
        constexpr float points[5][2] = { // 0x8CC0C4
            { 0.0000f, 1.000f},
            { 0.3000f, 1.000f},
            { 0.5000f, 1.000f},
            { 0.7000f, 0.707f},
            { 1.0001f, 0.000f}
        };
        term = AudioLog10ScaledExe(CAEAudioUtility::GetPiecewiseLinearT(fadeRatio, points), 20.0f);
    } else if (!(fadeRatio > 0.99f)) { // 0x862CD0 (float)
        term = AudioLog10ScaledExe(fadeRatio, 10.0f);
    } else {
        term = 0.0f;
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        volume -= cfg->VolumeUnderwaterOffset;
    }
    if (GetVehicle()->m_pVehicleBeingTowed) {
        volume = cfg->VolumeTrailerOffset + volume;
    }
    return (term + m_AuSettings.EngineVolumeOffset) + volume;
}

// 0x4F5310
float CAEVehicleAudioEntity::GetFrequencyForDummyIdle(float ratio, float fadeRatio) const noexcept {
    // Como el original (0x4F5310)
    if (GetVehicle()->GetModelIndex() == MODEL_CADDY) {
        return 1.0f;
    }

    const auto* const cfg  = &s_Config.DummyEngine;
    float             freq = ExeLerp(cfg->ID.FreqBase, cfg->ID.FreqMax, GetDummyIdleRatioProgress(ratio));
    float             factor;
    if (m_State == eAEState::DUMMY_CRZ) {
        constexpr float points[5][2] = { // 0x8CC0EC
            { 0.0000f, 1.00f },
            { 0.3000f, 1.00f },
            { 0.5000f, 0.85f },
            { 0.7000f, 0.85f },
            { 1.0001f, 0.85f },
        };
        factor = CAEAudioUtility::GetPiecewiseLinearT(fadeRatio, points);
    } else {
        factor = 1.0f;
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        freq = s_Config.FreqUnderwaterFactor * freq;
    }
    return factor * freq;
}

// @notsa
constexpr float CAEVehicleAudioEntity::GetDummyRevRatioProgress(float ratio) {
    return G2Clamp01Exe(invLerp(s_Config.DummyEngine.Rev.Ratio, 1.f, ratio)); // Como el original (0x4F53D5..0x4F53EB): (r - k) / (1 - k); NaN -> 1.0
}

// 0x4F53D0
float CAEVehicleAudioEntity::GetVolumeForDummyRev(float ratio, float fadeRatio) const {
    // Como el original (0x4F53D0)
    const auto* const cfg = &s_Config.DummyEngine;
    float volume = ExeLerp(cfg->Rev.VolumeBase, cfg->Rev.VolumeMax, GetDummyRevRatioProgress(ratio));
    float term; // (term + EngineVolumeOffset) + volume
    if (m_State != eAEState::DUMMY_CRZ) {
        term = AudioLog10ScaledExe(1.0f - fadeRatio, 10.0f);
    } else if (!(fadeRatio > 0.99)) { // 0x4F544C fcomp qword [862CD8] + test ah,41h / jne: <= 0.99 o NaN (DOUBLE)
        constexpr float points[][2] = { // 0x8CC114
            { 0.0000f, 0.000f},
            { 0.3000f, 0.000f},
            { 0.5000f, 0.707f},
            { 0.7000f, 1.000f},
            { 1.0001f, 1.000f},
        };
        term = AudioLog10ScaledExe(CAEAudioUtility::GetPiecewiseLinearT(fadeRatio, points), 20.0f);
    } else {
        term = 0.0f;
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        volume -= cfg->VolumeUnderwaterOffset;
    }
    if (GetVehicle()->m_pVehicleBeingTowed) {
        volume = cfg->VolumeTrailerOffset + volume;
    }
    return (term + m_AuSettings.EngineVolumeOffset) + volume;
}

// 0x4F54F0
float CAEVehicleAudioEntity::GetFrequencyForDummyRev(float ratio, float fadeRatio) const {
    // Como el original (0x4F54F0)
    const auto* const cfg  = &s_Config.DummyEngine;
    float             freq = ExeLerp(cfg->Rev.FreqBase, cfg->Rev.FreqMax, GetDummyRevRatioProgress(ratio));
    float             factor;
    if (m_State == eAEState::DUMMY_CRZ && !(fadeRatio > 0.99)) { // 0x4F556C fcomp qword [862CD8] + test ah,41h / je: <= 0.99 o NaN (DOUBLE)
        constexpr float points[5][2] = { // 0x8CC13C
            { 0.0000f, 1.0f },
            { 0.5000f, 1.0f },
            { 0.7000f, 1.2f },
            { 0.8500f, 1.0f },
            { 1.0001f, 1.0f },
        };
        factor = CAEAudioUtility::GetPiecewiseLinearT(fadeRatio, points);
    } else {
        factor = 1.0f;
    }
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        freq = s_Config.FreqUnderwaterFactor * freq;
    }
    return factor * freq;
}
#pragma endregion

#pragma region Engine Sounds Management

// 0x4F55C0
void CAEVehicleAudioEntity::CancelVehicleEngineSound(eVehicleEngineSoundType st) {
    // Como el original (0x4F55C0)
    if (st < 0 || st >= AE_SOUND_ENGINE_MAX) { // 0x4F55E7 test bp,bp / jl; cmp bp,0Ch / jge
        return;
    }

    auto&      sound       = m_EngineSounds[st].Sound;
    const auto soundLength = sound ? sound->GetSoundLength() : static_cast<int16>(-1); // 0x4F55E3 mov bx,[ecx+70h] (antes de detenerlo)

    GracefullyStopSound(sound);

    if (st == AE_SOUND_PLAYER_AC) {
        m_ACPlayPositionWhenStopped = m_ACPlayPositionThisFrame;
        m_TimeACStopped             = CTimer::GetTimeInMS();

        m_ACPlayPositionThisFrame   = -1;
        m_ACPlayPositionLastFrame   = -1;

        if (soundLength > 0) {
            float r = static_cast<float>(m_ACPlayPositionWhenStopped) / static_cast<float>(soundLength);
            r = G2Clamp01Exe(r); // 0x4F5664..0x4F569F
            m_ACPlayPercentWhenStopped = static_cast<int16>(static_cast<int32>(r * 100.0f));
        } else {
            m_ACPlayPercentWhenStopped = 0;
        }
    }
}

// notsa
void CAEVehicleAudioEntity::CancelAllVehicleEngineSounds(std::optional<eVehicleEngineSoundType> except) {
    for (int32 i{}; i < (int32)(std::size(m_EngineSounds)); i++) {
        if (!except.has_value() || *except != i) { // Como el original (p. ej. 0x4F5AC9): se cancelan todos MENOS el excluido
            CancelVehicleEngineSound((eVehicleEngineSoundType)(i));
        }
    }
}

// 0x4F56D0
// NB: Our function returns whenever the sound was updated, original doesn't, (!) so this function IS NOT hooked!
bool CAEVehicleAudioEntity::UpdateVehicleEngineSound(eVehicleEngineSoundType st, float speed, float volume) {
    return UpdateVehicleEngineSoundRawVolume(st, speed, m_EventVolume + volume);
}

// notsa
bool CAEVehicleAudioEntity::UpdateVehicleEngineSoundRawVolume(eVehicleEngineSoundType st, float speed, float volume) {
    if (CAESound* const sound = m_EngineSounds[st].Sound) {
        sound->m_Speed  = speed;
        sound->m_Volume = volume;
        return true;
    }
    return false;
}

// 0x4F6320
void CAEVehicleAudioEntity::StopGenericEngineSound(eVehicleEngineSoundType st) {
    GracefullyStopSound(m_EngineSounds[st].Sound);
}

// 0x4F7A50
void CAEVehicleAudioEntity::RequestNewPlayerCarEngineSound(eVehicleEngineSoundType st, float speed, float volume) {
    // Como el original (0x4F7A50)
    if (m_DummySlot == -1) {
        return;
    }

    switch (st) {
    case AE_SOUND_CAR_REV:
    case AE_SOUND_CAR_ID: { // 0x4F7AAD
        if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            return;
        }
        break;
    }
    case AE_SOUND_PLAYER_CRZ:
    case AE_SOUND_PLAYER_AC:
    case AE_SOUND_PLAYER_OFF: // 0x4F7ABA
        if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            return;
        }
        break;
    default: // Fuera del dominio real (el EXE salta la comprobacion de banco y no crea sonido)
        return;
    }

    volume = volume + m_EventVolume; // 0x4F7B05 fadd [esi+0D8h]

    if (m_EngineSounds[st].Sound && st != AE_SOUND_PLAYER_OFF) { // 0x4F7B0F..0x4F7B1A: cancela el sonido anterior
        CancelVehicleEngineSound(st);
    }

    const auto DoPlayEngineSoundSound = [&](eSoundBankSlot slot, eSoundID sfx, float rollOffFactor, uint32 flags = 0, int16 playTime = 0) {
        m_EngineSounds[st].Sound = PlayVehicleSound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOffFactor,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES | flags,
            .PlayTime      = playTime
        });
    };

    switch (st) {
    case AE_SOUND_CAR_REV: { // 0x4F7B2F
        DoPlayEngineSoundSound(
            m_DummySlot,
            0,
            s_Config.RevRollOffFactor
        );
        break;
    }
    case AE_SOUND_CAR_ID: { // 0x4F7BA7
        DoPlayEngineSoundSound(
            m_DummySlot,
            1,
            s_Config.IdRollOffFactor
        );
        break;
    }
    case AE_SOUND_PLAYER_CRZ: { // 0x4F7C1F
        DoPlayEngineSoundSound(
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            1,
            s_Config.PlayerEngine.Crz.RollOffFactor
        );
        break;
    }
    case AE_SOUND_PLAYER_AC: { // 0x4F7C77
        const auto* const cfg = &s_Config.PlayerEngine.AC;

        m_FramesAgoACLooped = 0;

        // 0x4F7CFC - posicion de reproduccion (en %) segun el tiempo desde que se detuvo
        int16        playTime = 0;
        const uint32 now      = CTimer::GetTimeInMS();
        if (cfg->SoundLength + m_TimeACStopped > now) {
            const float deltaF = static_cast<float>(now - m_TimeACStopped);
            const float lenF   = static_cast<float>(cfg->SoundLength);
            float       m      = (deltaF >= lenF) ? lenF : deltaF; // 0x4F7D3C fcomp / jp
            if (m < 0.0) {
                m = 0.0f;
            }
            const float g = 1.0f - m / lenF;
            const float P = static_cast<float>(m_ACPlayPercentWhenStopped);
            float       v = P * g;
            if (v >= cfg->LoopMaxPlayTimePercentage) { // 0x4F7DAC fcomp st(1) (el 80 del EXE es un entero convertido)
                v = cfg->LoopMaxPlayTimePercentage;
            }
            if (v < 0.0f) { // 0x4F7DCE
                v = 0.0f;
            }
            playTime = static_cast<int16>(static_cast<int32>(v));
            if (v > 10.0f) { // 0x4F7E13 fcomp [0085862C] / jne
                m_FramesAgoACLooped = static_cast<int16>(cfg->LoopFrameCnt + 1);
            }
        }
        DoPlayEngineSoundSound(
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            0,
            cfg->RollOffFactor,
            SOUND_START_PERCENTAGE,
            playTime
        );
        break;
    }
    case AE_SOUND_PLAYER_OFF: { // 0x4F7E39
        DoPlayEngineSoundSound(
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            2,
            s_Config.PlayerEngine.Off.RollOffFactor
        );
        break;
    }
    default:
        break;
    }
}

// 0x4F7F20
void CAEVehicleAudioEntity::StartVehicleEngineSound(eVehicleEngineSoundType st, float speed, float volume) {
    volume += m_EventVolume;

    if (auto* const s = GetEngineSound(st)) {
        s->SetVolume(volume);
        s->SetSpeed(speed);
        return;
    }

    const auto PlayEngineSound = [&](eSoundID sfx, float rollOffFactor) {
        // Como el original (0x4F7FF6 call 4EFE50): el constructor del EXE deja m_ClientVariable = -1.0 (PlayVehicleSound), 0x4F802F guarda el resultado
        m_EngineSounds[st].Sound = PlayVehicleSound({
            .BankSlotID      = m_DummySlot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOffFactor,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES,
        });
    };
    const auto* const cfg = &s_Config;
    switch (st) {
    case AE_SOUND_CAR_ID:  PlayEngineSound(1, cfg->IdRollOffFactor);  break;
    case AE_SOUND_CAR_REV: PlayEngineSound(0, cfg->RevRollOffFactor); break;
    }
}

#pragma endregion

#pragma region Player Only
// 0x4FD0B0
void CAEVehicleAudioEntity::JustWreckedVehicle() {
    if (m_IsPlayerDriver) {
        TurnOffRadioForVehicle();
    }
    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR:
    case AE_BIKE:
    case AE_BMX:
    case AE_BOAT:
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_PLANE:
    case AE_AIRCRAFT_SEAPLANE:
    case AE_TRAIN:
    case AE_SPECIAL: {
        CancelAllVehicleEngineSounds();
    }
    }
    StopNonEngineSounds();
}

// 0x4FCF40
void CAEVehicleAudioEntity::JustGotOutOfVehicleAsDriver() {
    m_IsPlayerDriverAboutToExit = false;

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR:
    case AE_BIKE: { // 0x4FD004
        m_EventVolume = GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND) - s_Config.VolOffsetOnGround;
        m_FadeIn      = 1.0f;
        m_FadeOut     = 1.0f;
        if (m_State == eAEState::PLAYER_ID) {
            CancelAllVehicleEngineSounds(AE_SOUND_CAR_ID);
            m_State = eAEState::DUMMY_ID;
        } else {
            CancelAllVehicleEngineSounds();
            if (m_State != eAEState::CAR_OFF) {
                m_State = eAEState::NUM_STATES; // ???
            }
        }
        PlayReverseSound(-1);
        m_AuGear        = 0;
        break;
    }
    case AE_TRAIN: { // 0x4FCF78
        PlayTrainBrakeSound(-1);
        break;
    }
    case AE_SPECIAL: { // 0x4FCF98
        m_EventVolume = GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND) - s_Config.VolOffsetOnGround;
        break;
    }
    case AE_AIRCRAFT_PLANE:
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_SEAPLANE: { // 0x4FCFC8
        CancelAllVehicleEngineSounds();
        m_CurrentRotorFrequency    = -1.0f;
        m_CurrentDummyEngineVolume = -100.0f;
        break;
    }
    default: // Como el original (0x4FD097): BMX, barco y el resto no hacen nada
        break;
    }
    s_pPlayerDriver = nullptr; // Como el original (0x4FCF8F / 0x4FCFBF / 0x4FCFF5 / 0x4FD099 mov [B6B990],ebx): siempre al final
}

// 0x4F5700
void CAEVehicleAudioEntity::JustGotInVehicleAsDriver() {
    s_pPlayerDriver             = m_Entity->AsVehicle()->GetDriver();
    m_IsPlayerDriverAboutToExit = false;

    const auto MaybeCancelPlayerEngineSounds = [this] {
        if (AESoundManager.AreSoundsPlayingInBankSlot(SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            AESoundManager.CancelSoundsInBankSlot(SND_BANK_SLOT_PLAYER_ENGINE_P, false);
        }
    };
    const auto EnsurePlayerEngineSoundBankIsLoaded = [this] {
        AEAudioHardware.EnsureSoundBankIsLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P);
    };

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR:
    case AE_BIKE: { // 0x4F5A08
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        m_AuGear                  = 0;
        m_CrzCount                = 0.f;
        m_ACPlayPositionThisFrame = -1;
        m_ACPlayPositionLastFrame = -1;

        MaybeCancelPlayerEngineSounds();

        if (m_DummyEngineBank == SND_BANK_UNK || m_PlayerEngineBank == SND_BANK_UNK) {
            return;
        }

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        EnsurePlayerEngineSoundBankIsLoaded();

        if (m_State != eAEState::DUMMY_ID || m_DummySlot == SND_BANK_SLOT_NONE) {
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;
        } else {
            CancelAllVehicleEngineSounds(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        }

        break;
    }
    case AE_AIRCRAFT_PLANE:
    case AE_AIRCRAFT_HELICOPTER:
    case AE_AIRCRAFT_SEAPLANE: { // 0x4F5957
        CancelAllVehicleEngineSounds();

        if (m_PlayerEngineBank != SND_BANK_UNK) {
            if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
                MaybeCancelPlayerEngineSounds();
                AEAudioHardware.LoadSoundBank(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P);
            }
        }

        if (m_DummyEngineBank != SND_BANK_UNK && m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        m_CurrentRotorFrequency    = -1.0f;
        m_CurrentDummyEngineVolume = -100.0f;

        break;
    }
    case AE_BMX: { // 0x4F576C
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        MaybeCancelPlayerEngineSounds();
        CancelAllVehicleEngineSounds();

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        EnsurePlayerEngineSoundBankIsLoaded();

        break;
    }
    case AE_BOAT: { // 0x4F57FF
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }
        break;
    }
    case AE_TRAIN: { // 0x4F5864
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        MaybeCancelPlayerEngineSounds();

        if (m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        EnsurePlayerEngineSoundBankIsLoaded();

        break;
    }
    case AE_SPECIAL: { // 0x4F58FD
        m_EventVolume = CAEAudioEntity::GetDefaultVolume(AE_GENERAL_VEHICLE_SOUND);

        MaybeCancelPlayerEngineSounds();
        CancelAllVehicleEngineSounds();

        if (m_DummyEngineBank != SND_BANK_UNK && m_DummySlot == SND_BANK_SLOT_NONE) {
            m_DummySlot = DemandBankSlot(m_DummyEngineBank);
        }

        EnsurePlayerEngineSoundBankIsLoaded(); // Como el original (0x4F57C2): siempre, no solo cuando se pide el slot

        break;
    }
    default: // Como el original: los tipos ONE_GEAR y NO_VEHICLE no hacen nada (0x4F5B0C)
        break;
    }
}

// 0x4F5BA0
void CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver() {
    m_IsPlayerDriverAboutToExit = true;

    if (m_IsPlayerDriver) {
        TurnOffRadioForVehicle();
    }
}

// 0x4F5B20
void CAEVehicleAudioEntity::TurnOnRadioForVehicle() {
    s_pPlayerAttachedForRadio       = m_Entity->AsVehicle()->m_pDriver;
    s_pVehicleAudioSettingsForRadio = &m_AuSettings;
    switch (m_AuSettings.RadioType) {
    case AE_RT_CIVILIAN:
    case AE_RT_UNKNOWN:
    case AE_RT_EMERGENCY:
        AudioEngine.StartRadio(m_AuSettings);
    }
}

// 0x4F5B60
void CAEVehicleAudioEntity::TurnOffRadioForVehicle() {
    switch (m_AuSettings.RadioType) {
    case AE_RT_CIVILIAN:
    case AE_RT_UNKNOWN:
    case AE_RT_EMERGENCY:
        AudioEngine.StopRadio(&m_AuSettings, false);
    }
    s_pVehicleAudioSettingsForRadio = nullptr;
    s_pPlayerAttachedForRadio       = nullptr;
}

// 0x4F5E50
bool CAEVehicleAudioEntity::JustFinishedAccelerationLoop() {
    const auto* const cfg = &s_Config.PlayerEngine.AC;

    if (m_FramesAgoACLooped < cfg->LoopFrameCnt) {
        m_FramesAgoACLooped++;
        return false;
    }
    if (m_ACPlayPositionThisFrame < 0 || m_ACPlayPositionLastFrame < 0) {
        return false;
    }
    if (const auto* const sound = GetEngineSound(AE_SOUND_PLAYER_AC)) {
        if (m_ACPlayPositionThisFrame + cfg->LoopInterval > sound->m_Length) {
            m_FramesAgoACLooped = 0;
            return true;
        }
    }
    return false;
}
#pragma endregion

#pragma region Play*Sound Functions
// Code for `0x4F8360` and `0x4FA630`
void CAEVehicleAudioEntity::GenericPlaySurfaceSound(eSoundBankSlot slot, eSoundID newSoundType, float speed, float volume, float rollOff) noexcept {
    volume += m_EventVolume;

    if (m_SurfaceSoundType == newSoundType) {
        if (m_SurfaceSoundType != -1) {
            m_SkidSound.UpdateTwinLoopSound(m_Entity->GetPosition(), volume, speed);
        }
    } else {
        if (m_SkidSound.IsActive()) {
            m_SkidSound.StopSoundAndForget();
        }
        if ((m_SurfaceSoundType = newSoundType) != -1) {
            m_SkidSound.Initialise(
                slot,
                newSoundType,
                newSoundType + 1,
                this,
                200,
                1'000
            );
            m_SkidSound.PlayTwinLoopSound(
                m_Entity->GetPosition(),
                volume,
                speed,
                rollOff
            );
        }
    }
}

// 0x4F8360
void CAEVehicleAudioEntity::PlaySkidSound(eSoundID newSkidSoundType, float speed, float volume) {
    GenericPlaySurfaceSound(SND_BANK_SLOT_VEHICLE_GEN, newSkidSoundType, speed, volume, 2.5f);
}

// 0x4FA630
void CAEVehicleAudioEntity::PlayTrainBrakeSound(eSoundID soundType, float speed, float volume) {
    // Como el original (0x4FA630): sin banco de jugador no hace nada
    if (m_PlayerEngineBank == SND_BANK_UNK) {
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
        return;
    }
    GenericPlaySurfaceSound(SND_BANK_SLOT_PLAYER_ENGINE_P, soundType, speed, volume, 3.5f);
}

// @notsa
void PlayGenericSound(
    CAEVehicleAudioEntity* self,
    CAESound*&             s,
    eSoundBankSlot         slot,
    eSoundID&              currSfx,
    eSoundID               newSfx,
    float                  speed,
    float                  volume,
    float                  rollOff
) {
    if (currSfx != newSfx) { // Sound type changing...
        GracefullyStopSound(s);
        if ((currSfx = newSfx) != -1) {
            // Como el original (0x4F84D0 / 0x4F8650 / 0x4F87D0): Initialise deja m_ClientVariable = -1.0f (PlayVehicleSound), no el 0 de PlaySound
            s = PlayVehicleSound({
                .BankSlotID      = slot,
                .SoundID       = newSfx,
                .AudioEntity   = self,
                .Pos           = self->GetVehicle()->GetPosition(),
                .Volume        = volume,
                .RollOffFactor = rollOff,
                .Speed         = speed,
                .Flags         = SOUND_REQUEST_UPDATES,
            });
        }
    } else if (s && currSfx != -1) { // Same sound type, update existing...
        s->SetSpeed(speed);
        s->SetVolume(volume);
    }
}

// 0x4F84D0
void CAEVehicleAudioEntity::PlayRoadNoiseSound(eSoundID sfx, float speed, float volume) {
    PlayGenericSound(
        this,
        m_RoadNoiseSound,
        SND_BANK_SLOT_VEHICLE_GEN,
        m_RoadNoiseSoundType,
        sfx,
        speed,
        m_EventVolume + volume,
        s_Config.RoadNoiseSoundRollOffFactor
    );
}

// 0x4F8650
void CAEVehicleAudioEntity::PlayFlatTyreSound(eSoundID sfx, float speed, float volume) {
    PlayGenericSound(
        this,
        m_FlatTireSound,
        SND_BANK_SLOT_VEHICLE_GEN,
        m_FlatTireSoundType,
        sfx,
        speed,
        m_EventVolume + volume,
        s_Config.FlatTireSoundRollOffFactor
    );
}

// 0x4F87D0
void CAEVehicleAudioEntity::PlayReverseSound(eSoundID sfx, float speed, float volume) {
    PlayGenericSound(
        this,
        m_ReverseGearSound,
        SND_BANK_SLOT_VEHICLE_GEN,
        m_ReverseGearSoundType,
        sfx,
        speed,
        m_EventVolume + volume,
        s_Config.ReverseGearSoundRollOffFactor
    );
}
#pragma endregion

#pragma region Horn/Siren
// 0x4F62A0
void CAEVehicleAudioEntity::GetSirenState(bool& isSirenOn, bool& isFastSirenOn, tVehicleParams& vp) const noexcept {
    // Como el original (0x4F62A0): sin atajos de teclado de depuracion (definir NOTSA_VEHAUDIO_DEBUG_SIREN_KEYS para recuperarlos)
#ifdef NOTSA_VEHAUDIO_DEBUG_SIREN_KEYS
    if (CPad::GetPad()->IsCtrlPressed()) {
        if (CPad::GetPad()->IsStandardKeyJustDown('J')) {
            isSirenOn = true;
        }
        if (CPad::GetPad()->IsStandardKeyJustDown('K')) {
            isFastSirenOn = true;
        }
        return;
    }
#endif
    if (m_IsWreckedVehicle || !m_HasSiren || !vp.Vehicle->vehicleFlags.bSirenOrAlarm) {
        isSirenOn = false;
    } else if (isSirenOn = (vp.Vehicle->GetStatus() != STATUS_ABANDONED)) {
        isFastSirenOn = vp.Vehicle->m_HornCounter && vp.Vehicle->GetModelIndex() != MODEL_MRWHOOP;
    }
}

// 0x4F61E0
void CAEVehicleAudioEntity::GetHornState(bool* out, tVehicleParams& params) const noexcept {
    *out = GetHornState(params);
}

// Code for 0x4F61E0 (But return value directly instead of taking it as an arg...Ö
bool CAEVehicleAudioEntity::GetHornState(tVehicleParams& vp) const noexcept {
    // Como el original (0x4F61E0)
    if (m_IsWreckedVehicle) {
        return false;
    }
    auto* const veh = vp.Vehicle;
    if (veh->m_nAlarmState != 0 && veh->m_nAlarmState != 0xFFFF && veh->GetStatus() != STATUS_WRECKED) { // m_nAlarmState es de 16 bits sin signo: el EXE compara con 0FFFFh
        return false;
    }
    if (!veh->m_HornCounter) {
        return false;
    }
    if (veh->GetStatus() == STATUS_PLAYER) {
        return true;
    }
    if (veh->m_HornCounter > HORN_PATTERN_SIZE) {
        veh->m_HornCounter = HORN_PATTERN_SIZE;
    }
    if (vp.Vehicle->m_HornCounter == HORN_PATTERN_SIZE) {
        vp.Vehicle->m_HornPattern = (int8)(CAEAudioUtility::GetRandomNumberInRange<int32>(0, 7)); // 0x4F6246 call 4D9C10 (0, 7)
    }
    return HornPattern[(uint8)vp.Vehicle->m_HornPattern][HORN_PATTERN_SIZE - vp.Vehicle->m_HornCounter];
}

// 0x4F99D0
void CAEVehicleAudioEntity::PlayHornOrSiren(bool isHornOn, bool isSirenOn, bool isFastSirenOn, tVehicleParams& vp) {
    // Como el original (0x4F99D0): los sonidos se crean sobre m_tempSound de la entidad
    const float carHornVol = GetDefaultVolume(AE_CAR_HORN);
    const float bellVol    = GetDefaultVolume(AE_BIKE_BELL);
    const float sirenVol   = GetDefaultVolume(AE_SIREN);
    const auto  hornType   = static_cast<int8>(m_AuSettings.HornType);

    const auto PlayOnTempSound = [&](eSoundBankSlot slot, eSoundID sfx, float volume, float speed, float rollOff, float doppler) -> CAESound* {
        m_tempSound.Initialise(slot, sfx, this, m_Entity->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f, 0);
        m_tempSound.m_Speed         = speed;
        m_tempSound.m_Doppler       = doppler;
        m_tempSound.m_Flags         = SOUND_REQUEST_UPDATES;
        m_tempSound.m_RollOffFactor = rollOff;
        return AESoundManager.RequestNewSound(&m_tempSound);
    };

    //
    // Bocina / campana
    //
    if (hornType == 0) { // eAEVehicleHornType::BIKE (0x4F9A1E)
        if (isHornOn && !m_IsHornOn) {
            StopAndForgetSound(m_HornSound);
            m_HornVolume = bellVol;
            if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN) && !AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
                return;
            }
            m_HornSound = PlayOnTempSound(SND_BANK_SLOT_HORN_AND_SIREN, hornType, m_HornVolume, 1.0f, 2.5f, s_Config.Horn.Doppler);
        } else { // 0x4F9B08 - la campana se apaga restando 1.5 por fotograma (sin escalar por el paso de tiempo)
            float v = m_HornVolume - s_Config.Horn.BikeBellFadeOut;
            if (!(v > -100.0f)) {
                v = -100.0f;
            }
            m_HornVolume = v;
            if (m_HornSound) {
                if (v <= -100.0f) {
                    StopAndForgetSound(m_HornSound);
                } else {
                    m_HornSound->m_Volume = v;
                }
            }
        }
    } else if (!isHornOn) { // 0x4F9C36
        if (m_IsHornOn) {
            StopAndForgetSound(m_HornSound);
        }
    } else if (!m_IsHornOn) { // 0x4F9B7A
        if (hornType <= 0 || hornType > 9) {
            return;
        }
        m_HornVolume = carHornVol + m_AuSettings.EngineVolumeOffset;
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN) && !AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
            return;
        }
        if (m_HornSound) {
            return;
        }
        m_HornSound = PlayOnTempSound(SND_BANK_SLOT_HORN_AND_SIREN, hornType, m_HornVolume, m_AuSettings.HornPitch, 2.5f, s_Config.Horn.Doppler);
    }

    //
    // Sirena normal (0x4F9C57)
    //
    const bool wantNormalSiren = isSirenOn && !isFastSirenOn;
    const bool wasNormalSiren  = m_IsSirenOn && !m_IsFastSirenOn;
    if (wantNormalSiren) {
        if (!wasNormalSiren) {
            if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN)) {
                return;
            }
            if (m_SirenSound) {
                return;
            }
            if (vp.Vehicle->GetModelIndex() == MODEL_MRWHOOP) { // 0x4F9CBC
                if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_ICEVAN_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
                    return;
                }
                m_SirenSound = PlayOnTempSound(SND_BANK_SLOT_PLAYER_ENGINE_P, SND_GENRL_HORN_HORNBMW328, sirenVol, 1.0f, 3.0f, s_Config.Siren.Doppler);
            } else {
                m_SirenSound = PlayOnTempSound(SND_BANK_SLOT_HORN_AND_SIREN, SND_GENRL_HORN_SIREN_LONG, sirenVol, 1.0f, 3.0f, s_Config.Siren.Doppler);
            }
        }
    } else if (wasNormalSiren) { // 0x4F9D96
        StopAndForgetSound(m_SirenSound);
    }

    //
    // Sirena rapida (0x4F9DAF)
    //
    if (isFastSirenOn) {
        if (m_IsFastSirenOn) {
            return;
        }
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HORN, SND_BANK_SLOT_HORN_AND_SIREN)) {
            return;
        }
        if (m_FastSirenSound) {
            return;
        }
        m_FastSirenSound = PlayOnTempSound(SND_BANK_SLOT_HORN_AND_SIREN, SND_GENRL_HORN_SIREN_SHORT, sirenVol, 1.0f, 3.0f, s_Config.Siren.Doppler);
    } else if (m_IsFastSirenOn) { // 0x4F9E6B
        StopAndForgetSound(m_FastSirenSound);
    }
}

#pragma endregion

#pragma region Vehicle Skidding
// 0x4F8F10
void CAEVehicleAudioEntity::ProcessVehicleSkidding(tVehicleParams& vp) {
    // Como el original (0x4F8F10)
    float sumWheelSkid = 0.0f;
    float speed        = 1.0f;
    int32 sfx          = -1;
    float volBase      = 0.0f;

    auto* const raw = reinterpret_cast<uint8*>(vp.Vehicle);
    int32*      wheelStates;
    float*      wheelCounts;
    float       gasPedalAudioRevs;
    uint8       contact;
    uint8       numWheels;
    bool        rearNotSkidding = true;

    if (vp.BaseVehicleType == VEHICLE_TYPE_AUTOMOBILE) { // 0x4F8F87
        auto* const a     = vp.Vehicle->AsAutomobile();
        gasPedalAudioRevs = a->m_GasPedalAudioRevs;
        contact           = raw[0x960];
        wheelStates       = reinterpret_cast<int32*>(a->m_WheelStates.data());
        wheelCounts       = a->m_WheelCounts.data();
        numWheels         = 4;
        if (wheelStates[1] == WHEEL_STATE_SKIDDING || wheelStates[3] == WHEEL_STATE_SKIDDING) {
            rearNotSkidding = false;
        }
    } else if (vp.BaseVehicleType == VEHICLE_TYPE_BIKE) { // 0x4F8F58
        auto* const b     = vp.Vehicle->AsBike();
        gasPedalAudioRevs = b->m_GasPedalAudioRevs;
        contact           = raw[0x804];
        wheelStates       = reinterpret_cast<int32*>(b->m_WheelStates.data());
        wheelCounts       = b->m_WheelCounts.data();
        numWheels         = 2;
        if (wheelStates[1] == WHEEL_STATE_SKIDDING) {
            rearNotSkidding = false;
        }
    } else {
        return; // 0x4F92A9: el EXE no hace nada para otros tipos
    }

    if (contact > 0 && CTimer::GetTimeScale() != 0.0f) { // Como el original (0x4F8FC7 fld [B7CB64] + 0x4F8FD3 test ah,44h / jnp): CTimer::ms_fTimeScale (0xB7CB64), NO ms_fTimeStep (0xB7CB5C)
        for (uint8 i = 0; i < numWheels; ++i) {
            const int32 ws = wheelStates[i];
            if (ws == WHEEL_STATE_NORMAL) {
                continue;
            }
            if (wheelCounts[i] <= 0.0f) {
                continue;
            }
            const bool isFront = i == 0 || i == 2;
            if (rearNotSkidding && isFront && ws == WHEEL_STATE_SKIDDING) {
                continue;
            }

            float value;
            switch (vp.Transmission->m_nDriveType) {
            case '4':
                value = GetVehicleDriveWheelSkidValue(vp.Vehicle, static_cast<tWheelState>(ws), gasPedalAudioRevs, *vp.Transmission, vp.Speed);
                break;
            case 'F':
                value = isFront
                    ? GetVehicleDriveWheelSkidValue(vp.Vehicle, static_cast<tWheelState>(ws), gasPedalAudioRevs, *vp.Transmission, vp.Speed)
                    : GetVehicleNonDriveWheelSkidValue(vp.Vehicle, static_cast<tWheelState>(ws), *vp.Transmission, vp.Speed);
                break;
            case 'R':
                value = (i == 1 || i == 3)
                    ? GetVehicleDriveWheelSkidValue(vp.Vehicle, static_cast<tWheelState>(ws), gasPedalAudioRevs, *vp.Transmission, vp.Speed)
                    : GetVehicleNonDriveWheelSkidValue(vp.Vehicle, static_cast<tWheelState>(ws), *vp.Transmission, vp.Speed);
                break;
            default:
                continue; // 0x4F9048 jne 4F9099: ningun otro tipo de traccion suma nada
            }
            sumWheelSkid = sumWheelSkid + value;
        }
    }

    if (sumWheelSkid > 0.0f) { // 0x4F90C2 fcomp / jne
        if (m_AuSettings.VehicleAudioType == AE_BMX) { // 0x4F90D3
            const auto* const cfg = &s_Config.Skid.BMX;
            speed   = cfg->FrqWheelSkidFactor * sumWheelSkid + cfg->FrqBase;
            sfx     = SND_GENRL_VEHICLE_GEN_BIKE_SKID_A;
            volBase = cfg->VolBase;
        } else {
            const auto surface = vp.Vehicle->m_nContactSurface;
            if (IsSurfaceAudioGrass(surface)) { // 0x4F91DD
                const auto* const cfg = &s_Config.Skid.Generic.GrassSurface;
                speed   = cfg->FrqWheelSkidFactor * sumWheelSkid + cfg->FrqBase;
                sfx     = SND_GENRL_VEHICLE_GEN_GRASS_SKID_A;
                volBase = cfg->VolBase;
            } else if (IsSurfaceWet(surface)) { // 0x4F91BB
                const auto* const cfg = &s_Config.Skid.Generic.WetSurface;
                speed   = cfg->FrqWheelSkidFactor * sumWheelSkid + cfg->FrqBase;
                sfx     = SND_GENRL_VEHICLE_GEN_GRAVEL_SKID_A;
                volBase = cfg->VolBase;
            } else { // 0x4F9189
                const auto* const cfg = &s_Config.Skid.Generic.StdSurface;
                speed = cfg->FrqWheelSkidFactor * sumWheelSkid + cfg->FrqBase;
                sfx   = SND_GENRL_VEHICLE_GEN_TARSKIDTWIN1;
                if (m_AuSettings.IsBike()) { // la frecuencia (no el volumen) se multiplica por 1.2 (0x4F91AF)
                    speed = speed * cfg->FrqFactorForBikes;
                }
            }
            if (m_AuSettings.IsPlaneOrHeli()) { // 0x4F9201
                volBase = s_Config.Skid.VolOffsetForPlaneOrHeli + volBase;
            }
        }
    }

    // 0x4F9221
    const float skidPerWheel = sumWheelSkid / static_cast<float>(numWheels);
    float       volume       = -100.0f;
    if (skidPerWheel > 0.00001f) {
        volume = (s_Config.Skid.VolBase + volBase) + AudioLog10ScaledExe(skidPerWheel, 20.0f);
        if (volume < -100.0f) {
            sfx    = -1;
            volume = -100.0f;
        }
    } else {
        sfx    = -1;
        volume = -100.0f;
    }
    PlaySkidSound(static_cast<eSoundID>(sfx), speed, volume);
}

// 0x4F5F30
float CAEVehicleAudioEntity::GetVehicleDriveWheelSkidValue(CVehicle* veh, tWheelState wheelState, float gasPedalAudioRevs, cTransmission& tr, float speed) const noexcept {
    // Como el original (0x4F5F30)
    const auto* const cfg = &s_Config.Skid.DriveWheelSkid;

    if (!cfg->Enabled) {
        return 0.0f;
    }

    switch (static_cast<int32>(wheelState)) {
    case WHEEL_STATE_SPINNING: { // 0x4F5FCC
        if (!(gasPedalAudioRevs > 0.4f)) {
            return 0.0f * cfg->SpinningFactor;
        }
        return (((gasPedalAudioRevs - 0.4f) * std::bit_cast<float>(0x3FD55555u)) * 0.75f) * cfg->SpinningFactor; // 0x85F0A8 = 1.6666666
    }
    case WHEEL_STATE_SKIDDING: { // 0x4F5F9E
        const float s = std::fabs(speed);
        return (((1.0f < s) ? 1.0f : s) * 0.75f) * cfg->SkiddingFactor;
    }
    case WHEEL_STATE_FIXED: { // 0x4F5F5B
        const float s = std::fabs(speed);
        if (!(s > 0.04f)) {
            return 0.0f * cfg->StationaryFactor;
        }
        const float m = s * 5.0f;
        return ((1.0f < m) ? 1.0f : m) * cfg->StationaryFactor;
    }
    default:
        return 0.0f;
    }
}

// 0x4F6000
float CAEVehicleAudioEntity::GetVehicleNonDriveWheelSkidValue(CVehicle* vehicle, tWheelState wheelState, cTransmission&, float velocity) const noexcept {
    // Como el original (0x4F6000)
    const auto* const cfg = &s_Config.Skid.NonDriveWheelSkid;

    if (!cfg->Enabled) {
        return 0.0f;
    }

    switch (static_cast<int32>(wheelState)) {
    case WHEEL_STATE_SKIDDING: { // 0x4F6069
        const float s = std::fabs(velocity);
        float       r = (((1.0f < s) ? 1.0f : s) * 0.75f) * cfg->SkiddingFactor;
        if (m_AuSettings.IsBMX()) {
            r = r * 0.2f;
        }
        return r;
    }
    case WHEEL_STATE_FIXED: { // 0x4F6026
        const float s = std::fabs(velocity);
        if (!(s > 0.04f)) {
            return 0.0f * cfg->StationaryFactor;
        }
        const float m = s * 5.0f;
        return ((1.0f < m) ? 1.0f : m) * cfg->StationaryFactor;
    }
    default:
        return 0.0f;
    }
}
#pragma endregion

// 0x4FAD40
void CAEVehicleAudioEntity::UpdateGenericVehicleSound(eVehicleEngineSoundType st, eSoundBankSlot bankSlot, eSoundBank bank, eSoundID sfx, float speed, float volume, float rollOff) {
    // Como el original (0x4FAD40): el sonido nuevo se crea sobre m_tempSound de la entidad
    volume = volume + m_EventVolume;

    if (auto* const s = m_EngineSounds[st].Sound) {
        s->SetVolume(volume);
        s->SetSpeed(speed);
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(bank, bankSlot)) {
        return;
    }
    m_tempSound.Initialise(bankSlot, sfx, this, m_Entity->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f, 0);
    m_tempSound.m_Speed         = speed;
    m_tempSound.m_RollOffFactor = rollOff;
    m_tempSound.m_Volume        = volume;
    m_tempSound.m_Flags         = SOUND_REQUEST_UPDATES;
    m_EngineSounds[st].Sound    = AESoundManager.RequestNewSound(&m_tempSound);
}

// @notsa
void CAEVehicleAudioEntity::UpdateOrRequestVehicleEngineSound(eVehicleEngineSoundType st, float freq, float volume) {
    if (GetEngineSound(st)) {
        UpdateVehicleEngineSound(st, freq, volume);
    } else {
        RequestNewPlayerCarEngineSound(st, freq, volume);
    }
}

// 0x4F8940
void CAEVehicleAudioEntity::ProcessVehicleFlatTyre(tVehicleParams& vp) {
    // Como el original (0x4F8940)
    const auto* const cfg = &s_Config.FlatTyre;

    bool spinning = false;
    switch (static_cast<int32>(m_AuSettings.VehicleAudioType)) {
    case AE_SPECIAL: { // 0x4F896B
        if (vp.Vehicle->GetModelIndex() != MODEL_CADDY) {
            break;
        }
        [[fallthrough]];
    }
    case AE_CAR: { // 0x4F89E8
        auto* const a = vp.Vehicle->AsAutomobile();
        for (uint8 i = 0; i < 4; ++i) {
            if (static_cast<int32>(a->GetDamageManager().GetWheelStatus(static_cast<eCarWheel>(i))) != WHEEL_STATE_SPINNING) {
                continue;
            }
            if (a->m_WheelCounts[i] > 0.0f) {
                spinning = true;
                break;
            }
        }
        break;
    }
    case AE_BIKE:
    case AE_BMX: { // 0x4F8A27
        auto* const raw = reinterpret_cast<uint8*>(vp.Vehicle);
        for (uint8 i = 0; i < 2; ++i) {
            if (raw[0x65C + i] != WHEEL_STATE_SPINNING) {
                continue;
            }
            if (*reinterpret_cast<float*>(raw + 0x730 + 4 * i) > 0.0f) {
                spinning = true;
                break;
            }
        }
        break;
    }
    default:
        break;
    }

    if (spinning) { // 0x4F8A5D
        const float vf0 = std::fabs(vp.Speed) / (vp.Transmission->m_MaxVelocity * 0.3f);
        const float vf  = (1.0f < vf0) ? 1.0f : vf0;
        const float speed = cfg->FrqGearVelocityFactor * vf + 1.0f;
        float volume = -100.0f;
        if (vf > 0.0) {
            volume = AudioLog10ScaledExe(vf, 20.0f) + cfg->VolOffset;
        }
        PlayFlatTyreSound(4, speed, volume);
    } else {
        PlayFlatTyreSound(-1, 1.0f, -100.0f);
    }
}

// 0x4F8B00
void CAEVehicleAudioEntity::ProcessVehicleRoadNoise(tVehicleParams& vp) {
    // Como el original (0x4F8B00)
    const auto* const cfg = &s_Config.RoadNoise;

    float  volume      = -100.0f;
    float  speedFactor = 1.0f;
    int32  sfx         = -1;
    const auto contact = GetNumOfContactWheels(vp).value_or(4);

    bool cancel;
    switch (vp.Vehicle->GetModelIndex()) {
    case MODEL_ARTICT1:
    case MODEL_ARTICT2:
    case MODEL_PETROTR:
    case MODEL_ARTICT3: cancel = contact == 0; break; // 0x4F8B70
    default:            cancel = !vp.Transmission || contact == 0; break; // 0x4F8B65
    }

    const float absSpeed = std::fabs(vp.Speed);
    if (!cancel && !(absSpeed > 0.0f)) { // 0x4F8B7D fcom / jne
        cancel = true;
    }

    if (!cancel) {
        float v = absSpeed + absSpeed; // fadd st,st
        v       = (1.0f < v) ? 1.0f : v;

        // Como el original: push superficie desplaza ESP; 4F8C30 escribe el factor de frecuencia ([esp+10h] antes del push).
        const CVector delta = m_Entity->GetPosition() - TheCamera.GetPosition();
        const float distance = std::sqrt((delta.z * delta.z + delta.x * delta.x) + delta.y * delta.y);
        const float ratio = distance / StaticRef<float>(0x8CBD10);
        speedFactor = ((ratio < 1.0f) ? 1.0f : ratio) * 0.5f + 0.75f;
        float volBase;
        const auto surface = vp.Vehicle->m_nContactSurface;
        if (IsSurfaceAudioGrass(surface)) { // 0x4F8CD5
            speedFactor = cfg->GrassSurface.FrqFactor * speedFactor;
            volBase     = cfg->GrassSurface.VolOffset + cfg->VolBase;
            sfx         = 21;
        } else if (IsSurfaceWet(surface)) { // 0x4F8CB0
            speedFactor = cfg->WetSurface.FrqFactor * speedFactor;
            volBase     = cfg->WetSurface.VolOffset + cfg->VolBase;
            sfx         = 22;
        } else { // 0x4F8CA5
            volBase = cfg->VolBase;
            sfx     = 21;
        }
        if (v > std::bit_cast<float>(0x38A7C5ACu)) { // 0x862CF4
            volume = AudioLog10ScaledExe(v, 20.0f) + volBase;
        }
    }

    PlayRoadNoiseSound(sfx, speedFactor, volume);
}

// 0x4F8DF0 - Called only if `AE_CAR` and the player is the driver
void CAEVehicleAudioEntity::ProcessReverseGear(tVehicleParams& vp) {
    auto* const a = vp.Vehicle->AsAutomobile();
    const auto* cfg = &s_Config.ReverseGear;

    if (a->vehicleFlags.bEngineOn && (a->m_GasPedal < 0.0f || a->m_nCurrentGear == 0)) { // Are we reversing?
        if (!a->m_NumDriveWheelsOnGround && a->m_NumDriveWheelsOnGroundLastFrame) {
            a->m_GasPedalAudioRevs *= 0.4f;
        }
        const auto revs = a->m_NumDriveWheelsOnGround
            ? vp.Speed / vp.Transmission->m_MaxReverseVelocity
            : a->m_GasPedalAudioRevs;
        PlayReverseSound(
            !(a->m_GasPedal < 0.0f) ? 20 : 19, // Como el original (0x4F8EA7 fcomp + test ah,5 + jnp): con NaN elige 20
            cfg->FrqBase + std::abs(revs) * cfg->FrqRevsFactor,
            std::abs(revs) > 0.0f
                ? cfg->VolBase + AudioLog10ScaledExe(std::abs(revs), 20.0f)
                : -100.0f
        );
    } else { // Cancel sound otherwise
        PlayReverseSound(-1, 1.0f, -100.0f);
    }
}

// 0x4F92C0
void CAEVehicleAudioEntity::ProcessRainOnVehicle(tVehicleParams& params) {
    // Como el original (0x4F92C0): usa m_tempSound de la entidad
    const auto* const cfg = &s_Config.RainOnVehicle;

    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_RAIN, SND_BANK_SLOT_WEATHER)) {
        return;
    }
    if (!(CAEWeatherAudioEntity::m_sfRainVolume > -100.0f)) {
        return;
    }
    const float volume = cfg->VolBase + CAEWeatherAudioEntity::m_sfRainVolume;
    if (++m_RainDropCounter < static_cast<int16>(cfg->FramesPerDrop)) {
        return;
    }
    m_RainDropCounter = 0;
    const auto sfx = static_cast<eSoundID>(CAEAudioUtility::GetRandomNumberInRange<int32>(12, 15));
    m_tempSound.Initialise(SND_BANK_SLOT_WEATHER, sfx, this, m_Entity->GetPosition(), volume, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f, 0);
    m_tempSound.m_SpeedVariance = 0.1f;
    m_tempSound.m_RollOffFactor = 1.0f;
    m_tempSound.m_Event         = AE_RAIN_COLLISION;
    AESoundManager.RequestNewSound(&m_tempSound);
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerGolfCart(tVehicleParams& params) {
    // NOP
}

// 0x4FAE20
void CAEVehicleAudioEntity::ProcessEngineDamage(tVehicleParams& vp) {
    // Como el original (0x4FAE20)
    const auto* const cfg = &s_Config.EngineDamage;

    // 0x4FAE2D - El EXE calcula aqui dos banderas (vapor / motor destrozado)
    const float health = vp.Vehicle->GetHealth();
    bool        steam  = false;
    bool        fucked = false;
    if (health < 390.0f) {
        steam  = true;
        fucked = true;
    } else if (health < 650.0f) {
        steam = true;
    }
    const bool drowning = GetVehicle()->vehicleFlags.bIsDrowning;
    if (drowning) {
        steam  = false;
        fucked = true;
    }
    if (m_AuSettings.VehicleAudioType != AE_CAR) {
        fucked = false;
    }

    // 0x4FAEB0 - Factor de velocidad
    float vf = 0.0f;
    if (vp.Transmission) {
        const float v = std::fabs(vp.Speed / vp.Transmission->m_MaxVelocity);
        vf            = (1.0f < v) ? 1.0f : v; // 0x4FAEC2..0x4FAFDA: v > 1 -> 1.0; si no se conserva v (fabs: nunca < 0; un NaN queda como NaN)
    }

    if (m_IsWreckedVehicle) {
        fucked = false;
    }

    // 0x4FAFE3 - Vapor
    if (!m_IsWreckedVehicle && steam) {
        const auto& s = cfg->SteamSound;
        UpdateGenericVehicleSound(
            AE_SOUND_STEAM,
            SND_BANK_SLOT_WEAPON_GEN,
            SND_BANK_GENRL_WEAPONS,
            SND_GENRL_WEAPONS_SPRAY_PAINT,
            (s.FrqMax - s.FrqMin) * vf + s.FrqMin,
            (s.VolMax - s.VolMin) * vf + s.VolMin,
            s.RollOff
        );
    } else {
        StopGenericEngineSound(AE_SOUND_STEAM);
    }

    // 0x4FAF28 - Motor destrozado
    if (fucked && (reinterpret_cast<uint8*>(vp.Vehicle)[0x428] & 0x10)) {
        const auto& s     = cfg->FuckedSound;
        float       vol   = (s.VolMax - s.VolMin) * vf + s.VolMin;
        float       speed = (s.FrqMax - s.FrqMin) * vf + s.FrqMin;
        if (drowning) {
            vol   = vol - s_Config.DummyEngine.VolumeUnderwaterOffset;
            speed = s_Config.FreqUnderwaterFactor * speed;
        }
        UpdateGenericVehicleSound(
            AE_SOUND_FUCKED,
            SND_BANK_SLOT_VEHICLE_GEN,
            SND_BANK_GENRL_VEHICLE_GEN,
            2,
            speed,
            vol,
            s.RollOff
        );
    } else {
        StopGenericEngineSound(AE_SOUND_FUCKED);
    }
}

// 0x4FB070
void CAEVehicleAudioEntity::ProcessNitro(tVehicleParams& vp) {
    // Como el original (0x4FB070)
    const auto* const cfg = &s_Config.Nitro;
    auto* const       raw = reinterpret_cast<uint8*>(vp.Vehicle);

    if ((*reinterpret_cast<uint32*>(raw + 0x38C) & 0x80000)  // handlingFlags.bNosInst
        && *reinterpret_cast<float*>(raw + 0x8A4) < 0.0f     // m_fTireTemperature
        && (raw[0x428] & 0x10)                               // bEngineOn
        && !(raw[0x42B] & 0x40)                              // !bIsDrowning
    ) {
        if (!m_bNitroOnLastFrame) {
            AddAudioEvent(AE_NITRO_IGNITION, 0.f);
        }

        // 0x4FB0D2 - ThisAccel (palabra de 16 bits) * (1/255) (0x859A3C)
        const int16 rawAccel = std::bit_cast<int16>(vp.ThisAccel); // palabra cruda (movsx eax,[edi+20h]); FixedFloat<int16> es solo el entero
        const float nitro    = static_cast<float>(rawAccel) * std::bit_cast<float>(0x3B808081u);

        if (m_CurrentNitroRatio < 0.0f) {
            m_CurrentNitroRatio = nitro;
        }
        const float cur = m_CurrentNitroRatio;
        float       r;
        if (!(nitro >= cur)) { // 0x4FB0FD fcom + test ah,1 / jne: n < cur o NaN -> 0x4FB12D
            const float lo = cur - cfg->StepDown;
            r              = (nitro > lo) ? nitro : lo;
        } else { // 0x4FB10A
            const float up = cfg->StepUp + cur;
            r              = (nitro < up) ? nitro : up;
        }
        m_CurrentNitroRatio = r;

        const auto* const s1 = &cfg->Sounds[0];
        const auto* const s2 = &cfg->Sounds[1];
        UpdateGenericVehicleSound(
            AE_SOUND_NITRO1,
            SND_BANK_SLOT_WEAPON_GEN,
            SND_BANK_GENRL_WEAPONS,
            SND_GENRL_WEAPONS_GAS_LOOP,
            (s1->FrqMax - s1->FrqMin) * r + s1->FrqMin,
            (s1->VolMax - s1->VolMin) * r + s1->VolMin,
            3.f
        );
        UpdateGenericVehicleSound(
            AE_SOUND_NITRO2,
            SND_BANK_SLOT_VEHICLE_GEN,
            SND_BANK_GENRL_VEHICLE_GEN,
            26,
            (s2->FrqMax - s2->FrqMin) * r + s2->FrqMin,
            (s2->VolMax - s2->VolMin) * r + s2->VolMin,
            3.f
        );

        m_bNitroOnLastFrame = true;
    } else { // 0x4FB1E5
        if (m_bNitroOnLastFrame) {
            StopGenericEngineSound(AE_SOUND_NITRO1);
            StopGenericEngineSound(AE_SOUND_NITRO2);
        }
        m_bNitroOnLastFrame = false; // 0x4FB23F: se escriben siempre
        m_CurrentNitroRatio = -1.f;
    }
}

// 0x4FB260
void CAEVehicleAudioEntity::ProcessMovingParts(tVehicleParams& vp) {
    // Como el original (0x4FB260)
    auto* const a = vp.Vehicle->AsAutomobile();
    const auto* const cfg = &s_Config.MovingParts;

    switch (vp.Vehicle->GetModelIndex()) {
    case MODEL_PACKER:
    case MODEL_DOZER:
    case MODEL_DUMPER:
    case MODEL_CEMENT: // El EXE tambien actualiza la velocidad suavizada para el CEMENT (0x20C), pero no emite sonido
    case MODEL_ANDROM:
    case MODEL_FORKLIFT:
        break;
    default:
        return;
    }

    // 0x4FB297
    const float delta = static_cast<float>(static_cast<int32>(a->m_wMiscComponentAngle) - static_cast<int32>(a->m_wMiscComponentAnglePrev)) / cfg->AngleDeltaFactor;
    const float absd  = std::fabs(delta);
    const float t     = (1.0f < absd) ? 1.0f : (0.0f <= absd ? absd : 0.0f);
    float       s     = m_MovingPartSmoothedSpeed;
    if (t > s) { // 0x4FB2F6
        const float y = cfg->StepUp + s;
        s             = (t < y) ? t : y;
    } else { // 0x4FB319
        const float x = s - cfg->StepDown;
        s             = (t > x) ? t : x;
    }
    m_MovingPartSmoothedSpeed = s;

    const auto it = cfg->PropsByModel.find(vp.Vehicle->GetModelId());
    if (it == cfg->PropsByModel.end()) {
        return; // 0x4FB5DC (CEMENT)
    }
    const auto* const props  = &it->second;
    const auto        slot   = props->Slot.value_or(m_DummySlot);
    const auto* const params = &props->SoundParamsByPartDir[!(delta > 0.f) ? 0 : 1]; // 0x4FB368 fcomp qword [859EF8] + test ah,41h / jne: <= 0 o NaN -> primer juego

    const auto volume = AudioLog10ScaledExe(m_MovingPartSmoothedSpeed, 20.0f) + params->Vol;
    if (!(volume > -100.0f)) { // 0x4FB575 fcomp [859014] + test ah,41h / jne: <= -100 o NaN detiene el sonido
        StopGenericEngineSound(AE_SOUND_MOVING_PARTS);
    } else {
        UpdateGenericVehicleSound(
            AE_SOUND_MOVING_PARTS,
            slot,
            props->Bank,
            props->Sfx,
            params->Frq,
            volume,
            1.5f
        );
    }
}

// 0x4F5D00
float CAEVehicleAudioEntity::GetVolForPlayerEngineSound(tVehicleParams& vp, eVehicleEngineSoundType st) const noexcept {
    // Como el original (0x4F5D00)
    const auto& cfg = s_Config.PlayerEngine;

    float volume;
    switch (st) {
    case AE_SOUND_CAR_REV: // 0x4F5D16
        volume = ExeLerp(cfg.Rev.VolMin, cfg.Rev.VolMax, vp.RealRevsRatio);
        break;
    case AE_SOUND_CAR_ID: // 0x4F5D2E - usa SpeedRatio (vp+0x2C)
        volume = vp.Vehicle->GetModelIndex() == MODEL_CADDY
            ? cfg.ID.VolMin - 30.f
            : ExeLerp(cfg.ID.VolMin, cfg.ID.VolMax, vp.SpeedRatio);
        break;
    case AE_SOUND_PLAYER_CRZ: // 0x4F5D5C
        volume = m_IsSingleGear
            ? cfg.SingleGearVolume
            : ((cfg.Crz.VolMax - cfg.Crz.VolMin) / static_cast<float>(cfg.CrzMaxCnt)) * m_CrzCount + cfg.Crz.VolMin;
        break;
    case AE_SOUND_PLAYER_AC: // 0x4F5D82
        volume = m_IsSingleGear
            ? cfg.SingleGearVolume
            : static_cast<float>(m_AuGear) * ((cfg.AC.VolMax - cfg.AC.VolMin) * 0.2f) + cfg.AC.VolMin;
        break;
    case AE_SOUND_PLAYER_OFF: // 0x4F5DB1 - devuelve directamente el valor (ya ajustado) de CAR_ID
        return GetVolForPlayerEngineSound(vp, AE_SOUND_CAR_ID);
    default:
        return 0.0f;
    }

    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        volume -= s_Config.DummyEngine.VolumeUnderwaterOffset;
    }
    volume += m_AuSettings.EngineVolumeOffset; // 0x4F5DDC fadd [ecx+0A0h] (antes de sumar el remolque)
    if (GetVehicle()->m_pVehicleBeingTowed) {
        volume += s_Config.DummyEngine.VolumeTrailerOffset;
    }
    if (m_bNitroOnLastFrame && m_CurrentNitroRatio <= 1.0f && m_CurrentNitroRatio >= 0.0f) {
        volume += cfg.VolNitroFactor * m_CurrentNitroRatio;
    }
    return volume;
}

// 0x4F5C60
float CAEVehicleAudioEntity::GetFreqForIdle(float ratio) const noexcept {
    // Como el original (0x4F5C60): interpolacion en linea sobre la tabla de 5 puntos 0x8CC164
    static constexpr float points[5][2] = {
        { 0.000f, 0.000f },
        { 0.075f, 0.700f },
        { 0.150f, 1.100f },
        { 0.250f, 1.250f },
        { 1.0001f, 1.700f },
    };
    return ExeInlinePiecewise5(ratio, points);
}

// 0x4F8070
float CAEVehicleAudioEntity::GetFreqForPlayerEngineSound(tVehicleParams& vp, eVehicleEngineSoundType st) const noexcept {
    // Como el original (0x4F8070)
    auto* const cfg = &s_Config.PlayerEngine;

    // 0x4F80A6 - factor por velocidad vertical
    float zf;
    if (!(vp.ZOverSpeed > 0.0f)) {
        const float w = (vp.ZOverSpeed <= cfg->FrqZMoveSpeedLimitMin) ? cfg->FrqZMoveSpeedLimitMin : vp.ZOverSpeed;
        zf            = (w / cfg->FrqZMoveSpeedLimitMin) * cfg->FrqZMoveSpeedFactor;
    } else {
        const float w = (vp.ZOverSpeed >= cfg->FrqZMoveSpeedLimitMax) ? cfg->FrqZMoveSpeedLimitMax : vp.ZOverSpeed;
        zf            = -((w / cfg->FrqZMoveSpeedLimitMax) * cfg->FrqZMoveSpeedFactor);
    }
    if (vp.Speed < std::bit_cast<float>(0xBA83126Fu)) { // 0x862CF0 = -0.001
        zf = -zf;
    }

    // 0x4F8111 - factor de marcha
    const float g  = vp.AbsSpeed / vp.Transmission->m_aGears[1].ChangeUpVelocity;
    const float gf = !(g > 1.0) ? g : 1.0f; // 0x4F8118 fcomp qword [85A310] + test ah,41h / jne: <= 1.0 o NaN conserva g
    const float A  = zf * gf;

    // 0x4F8131 - factor de moto (|sin(angulo de inclinacion)| * 0.12 con fsin de precision completa)
    float B = 0.0f;
    if (m_AuSettings.VehicleAudioType == AE_BIKE) {
        // 0x4F8145..0x4F8196: fsin sin redondear; fchs solo si sin < 0 (no fabs: -0 y NaN conservan el signo); fmul lean; fadd boost (bPlayerBoost);
        // fmul gf; un solo fstp a float (ningun redondeo intermedio).
        auto* const bikeBytes = reinterpret_cast<uint8*>(vp.Vehicle);
        const float angle     = *reinterpret_cast<float*>(bikeBytes + 0x648);
        B = VehAudioBikeLeanCpp(angle, cfg->FrqBikeLeanFactor, (bikeBytes[0x614] & 0x20) != 0, cfg->FrqPlayerBikeBoostOffset, gf);
    }

    float f;
    switch (st) {
    case AE_SOUND_CAR_REV: { // 0x4F81B2
        if (!(vp.WheelSpin > 5.0f)) {
            f = vp.WheelSpin * 0.2f * cfg->Rev.FrqWheelSpinFactor + cfg->Rev.FrqOffset;
        } else {
            f = (((vp.WheelSpin - 5.0f) * 0.2f) * (cfg->Rev.FrqMax - cfg->Rev.FrqMin) + cfg->Rev.FrqMin) + cfg->Rev.FrqOffset;
        }
        f = (f * (A + 1.0f)) * (B + 1.0f);
        break;
    }
    case AE_SOUND_CAR_ID: { // 0x4F81FF
        f = GetFreqForIdle(vp.SpeedRatio) * (cfg->ID.FrqMax - cfg->ID.FrqMin) + cfg->ID.FrqMin;
        f = (f * (A + 1.0f)) * (B + 1.0f);
        if (vp.Vehicle->GetModelIndex() == MODEL_CADDY) {
            f = 1.0f;
        }
        break;
    }
    case AE_SOUND_PLAYER_CRZ: { // 0x4F824A
        f = vp.WheelSpin * 0.2f * cfg->FrqWheelSpinFactor
          + ((cfg->Crz.FrqMax - cfg->Crz.FrqMin) * std::bit_cast<float>(0x3BDA740Eu)) * m_CrzCount; // 0xB6BA50 = rango * [862D6C] (1/150)
        f = m_IsSingleGear ? f + cfg->Crz.FrqSingleGear : f + cfg->Crz.FrqMin;
        f = (f * (A + 1.0f)) * (B + 1.0f);
        break;
    }
    case AE_SOUND_PLAYER_AC: { // 0x4F8281
        f = vp.WheelSpin * 0.2f * cfg->FrqWheelSpinFactor;
        if (m_IsSingleGear) {
            f = f + cfg->AC.FrqSingleGear;
        } else {
            f = (f + cfg->AC.FrqPerGearFactor[m_AuGear]) + cfg->AC.FrqMultiGearOffset;
        }
        f = (f * (A + 1.0f)) * (B + 1.0f);
        break;
    }
    case AE_SOUND_PLAYER_OFF: { // 0x4F82B8
        f = (cfg->Off.FrqMax - cfg->Off.FrqMin) * vp.SpeedRatio + cfg->Off.FrqMin;
        f = (f * (A + 1.0f)) * (B + 1.0f);
        break;
    }
    default:
        f = 0.0f; // El EXE devuelve aqui los bits del argumento; fuera del dominio real
        break;
    }

    // 0x4F82E5 - bajo el agua / nitro
    if (GetVehicle()->vehicleFlags.bIsDrowning) {
        f = f * s_Config.FreqUnderwaterFactor;
    }
    if (m_bNitroOnLastFrame && m_CurrentNitroRatio <= 1.0f && m_CurrentNitroRatio >= 0.0f) {
        f = f * (cfg->FrqNitroFactor * m_CurrentNitroRatio + 1.0f);
    }
    return f;
}

// 0x4FBB10
void CAEVehicleAudioEntity::ProcessPlayerVehicleEngine(tVehicleParams& vp) {
    auto* const cfg = &s_Config.PlayerEngine;

    if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
        return;
    }

    GetAccelAndBrake(vp);

    vp.AbsSpeed      = std::fabsf(vp.Speed);
    vp.SpeedRatio    = std::clamp(vp.AbsSpeed / vp.Transmission->m_MaxVelocity, 0.f, 1.f);
    vp.RealGear      = vp.Vehicle->m_nCurrentGear;
    vp.IsHandbrakeOn = vp.Vehicle->vehicleFlags.bIsHandbrakeOn;

    // 0x4FBBC3..0x4FBC1C: el EXE limita la relacion a [0, 1] (fcomp con 1.0 y 0.0 dobles); un NaN se queda como NaN
    auto* const gear = &vp.Transmission->m_aGears.data()[vp.RealGear]; // Como el original (4FBBC6): indexa sin comprobacion de limite.
    {
        // Como el original (4FBBCD..4FBC1C): x87 conserva subnormales, incluso en la ranura 6 solapada con campos de transmision.
        // SSE con FTZ/DAZ cambiaria un divisor subnormal por cero y produciria NaN para velocidad cero. (abs - bajar) / (max - bajar) y la
        // limitacion a [0, 1] se hacen sobre ST0 sin redondear (VehAudioRevsRatioCpp); solo el fstp final redondea a float.
        vp.RealRevsRatio = vp.RealGear
            ? VehAudioRevsRatioCpp(vp.AbsSpeed, gear->MaxVelocity, gear->ChangeDownVelocity)
            : 0.f;
    }

    vp.WheelSpin     = vp.Vehicle->m_fWheelSpinForAudio;
    vp.NumGears      = vp.Transmission->m_nNumberOfGears;
    vp.ZOverSpeed    = vp.Speed != 0.f
           ? vp.Vehicle->GetMoveSpeed().z / vp.Speed
           : 0.f;

    if (vp.BaseVehicleType == VEHICLE_TYPE_BIKE) { // 0x4FBC88: el EXE decide por BaseVehicleType (0 = coche, 9 = moto)
        auto* const b = vp.Vehicle->AsBike();

        vp.NumDriveWheelsOnGround          = b->m_NumDriveWheelsOnGround;
        vp.NumDriveWheelsOnGroundLastFrame = b->m_NumDriveWheelsOnGroundLastFrame;
        vp.GasPedalAudioRevs               = &b->m_GasPedalAudioRevs;
        vp.WheelState                      = b->m_WheelStates.data();
        vp.PrevSpeed                       = b->m_PrevSpeed;
    } else if (vp.BaseVehicleType == VEHICLE_TYPE_AUTOMOBILE) { // 0x4FBC86
        auto* const a = vp.Vehicle->AsAutomobile();

        vp.NumDriveWheelsOnGround          = a->m_NumDriveWheelsOnGround;
        vp.NumDriveWheelsOnGroundLastFrame = a->m_NumDriveWheelsOnGroundLastFrame;
        vp.GasPedalAudioRevs               = &a->m_GasPedalAudioRevs;
        vp.WheelState                      = a->m_WheelStates.data();
        vp.PrevSpeed                       = a->m_PrevSpeed;
    } else {
        return; // 0x4FBC8B jne 0x4FC9E5: cualquier otro tipo sale sin hacer nada
    }

    if (m_IsSingleGear) { // 0x4FBCEA
        vp.RealGear = static_cast<uint8>(vp.RealGear ? 1 : 0);
        vp.NumGears = 1;
    }

    // El EXE calcula siempre primero el volumen (0x4F5D00) y despues la frecuencia (0x4F8070)
    const auto RequestEngineSound = [&](int16 st) {
        const float vol = GetVolForPlayerEngineSound(vp, st);
        const float frq = GetFreqForPlayerEngineSound(vp, st);
        RequestNewPlayerCarEngineSound(st, frq, vol);
    };

    const auto UpdateEngineSound = [&](int16 st) {
        const float vol = GetVolForPlayerEngineSound(vp, st);
        const float frq = GetFreqForPlayerEngineSound(vp, st);
        UpdateVehicleEngineSound(st, frq, vol);
    };

    // 0x4FC6F9: aqui, excepcion, el EXE calcula primero la frecuencia y despues el volumen
    const auto UpdateOrRequestEngineSound = [&](int16 st) {
        const float frq = GetFreqForPlayerEngineSound(vp, st);
        const float vol = GetVolForPlayerEngineSound(vp, st);
        if (GetEngineSound(st)) {
            UpdateVehicleEngineSound(st, frq, vol);
        } else {
            RequestNewPlayerCarEngineSound(st, frq, vol);
        }
    };

    const auto DoFullAcceleration = [&] {
        if (m_IsSingleGear) {
            m_AuGear = 1;
        } else { // 0x4FC3AD: min(max(RealGear,1), AuGear) y, si no es menor que NumGears-1 (con signo), NumGears-1
            const int32 lo  = std::min<int32>(std::max<int32>(vp.RealGear, 1), m_AuGear);
            const int32 lim = static_cast<int32>(vp.Transmission->m_nNumberOfGears) - 1;
            m_AuGear        = static_cast<uint8>(lo >= lim ? lim : lo);
        }
        RequestEngineSound(AE_SOUND_PLAYER_AC);
        m_State = eAEState::PLAYER_AC_FULL;
    };

    const bool isReversing = vp.Speed < -0.001f;

    switch (m_State) {
    case eAEState::CAR_OFF: { // 0x4FBD24
        if (!vp.Vehicle->vehicleFlags.bEngineOn) {
            CancelAllVehicleEngineSounds(AE_SOUND_STEAM);
            m_State = eAEState::CAR_OFF;

            break;
        }

        AddAudioEvent(AE_START_ENGINE, 0.f);

        if (!isReversing) { // 0x4FBD66 (inverted!)
            if (vp.ThisAccel <= 0) { // 0x4FBDD0
                RequestNewPlayerCarEngineSound(AE_SOUND_CAR_ID, 1.f, -100.f);
                m_State = eAEState::PLAYER_ID;

                break;
            }

            if (!IsAccInhibited(vp) && !IsAccInhibitedForLowSpeed(vp)) { // 0x4FBDE9
                if (IsAccInhibitedForLowSpeed(vp)) { // 0x4FBDF7 - NB: Condition always false due to the cond. above...
                    RequestEngineSound(AE_SOUND_CAR_ID);
                    m_State = eAEState::PLAYER_ID;
                } else if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FBE0F
                    RequestNewPlayerCarEngineSound(AE_SOUND_CAR_ID);
                    m_State = eAEState::PLAYER_ID;
                } else if (vp.RealGear < vp.NumGears && s_Config.PlayerEngine.CrzSpeedOffset + vp.PrevSpeed > vp.Speed && vp.ZOverSpeed > s_Config.PlayerEngine.ZMoveSpeedThreshold) { // 0x4FBE15
                    vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                    RequestEngineSound(AE_SOUND_PLAYER_CRZ);
                    m_State = eAEState::PLAYER_CRZ;
                } else {
                    CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                    RequestEngineSound(AE_SOUND_PLAYER_AC);
                    m_State = eAEState::PLAYER_AC_FULL;
                }

                break;
            }
        }

        if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) {  // 0x4FBB10
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        } else {
            CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_REV);
            m_State = eAEState::PLAYER_WHEEL_SPIN;
        }

        break;
    }
    case eAEState::PLAYER_AC_FULL: {
        if (!vp.Vehicle->vehicleFlags.bEngineOn) { // 0x4FC4DA
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        if (isReversing) { // 0x4FC502
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            if (vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC504
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            } else {
                RequestEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            }
            break;
        }

        if (vp.ThisAccel <= 0) { // 0x4FC521
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (IsAccInhibited(vp)) { // 0x4FC52D
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC543
                RequestEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            } else {
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            }
            break;
        }

        if (   IsAccInhibitedForLowSpeed(vp)
            || IsAccInhibitedForLowSpeed(vp)
            || vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold
        ) { // 0x4FC558 - NB: Same function (in the cond) called twice?
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (   vp.RealGear < vp.NumGears
            && s_Config.PlayerEngine.CrzSpeedOffset + vp.PrevSpeed > vp.Speed
            && (vp.ZOverSpeed > s_Config.PlayerEngine.ZMoveSpeedThreshold || CCullZones::DoExtraAirResistanceForPlayer())
            && !IsCrzInhibitedForTime()
        ) { // 0x4FC5E2
            vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
            CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
            RequestEngineSound(AE_SOUND_PLAYER_CRZ);
            InhibitAccForTime(2'000); // 0x4FC61F: mov [esi+0D0h],edx
            m_State = eAEState::PLAYER_CRZ;

            break;
        }

        if (JustFinishedAccelerationLoop()) { // 0x4FC631 - Try change gears
            const auto gear = m_AuGear;
            m_AuGear = static_cast<uint8>(std::min<int32>(s_Config.PlayerEngine.MaxAuGear, m_AuGear + 1)); // 0x4FC66B cmp eax,5 / jl
            if (gear + 2 <= vp.NumGears || vp.RealGear < vp.NumGears && !CCullZones::DoExtraAirResistanceForPlayer()) { // 0x4FC640 - Got more gears?
                vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                UpdateEngineSound(AE_SOUND_PLAYER_AC);
                m_State = eAEState::PLAYER_AC_FULL;
            } else { // 0x4FC67B - Last gear, switch to crz
                if (!m_IsSingleGear) {
                    vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                }
                CancelVehicleEngineSound(AE_SOUND_PLAYER_AC);
                RequestEngineSound(AE_SOUND_PLAYER_CRZ);
                m_State = eAEState::PLAYER_CRZ;
            }
        } else {
            UpdateOrRequestEngineSound(AE_SOUND_PLAYER_AC);
        }

        break;
    }
    case eAEState::PLAYER_WHEEL_SPIN: { // 0x4FC76A
        if (!vp.Vehicle->vehicleFlags.bEngineOn) { // 0x4FC76A
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        if (isReversing) { // 0x4FC795
            if (vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC7A2
                UpdateEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            } else {
                CancelVehicleEngineSound(AE_SOUND_CAR_REV);
                RequestEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            }

            break;
        }

        if (vp.ThisAccel <= 0) { // 0x4FC7DA
            CancelVehicleEngineSound(AE_SOUND_CAR_REV);
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (IsAccInhibited(vp) || IsAccInhibitedForLowSpeed(vp)) { // 0x4FC7F6
            if (vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC93B
                UpdateEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;

                break;
            }
        } else if (!IsAccInhibitedForLowSpeed(vp) && vp.ThisAccel > s_Config.PlayerEngine.AC.WheelSpinThreshold) { // 0x4FC828
            CancelVehicleEngineSound(AE_SOUND_CAR_REV);
            if (   vp.RealGear < vp.NumGears
                && s_Config.PlayerEngine.CrzSpeedOffset + vp.PrevSpeed > vp.Speed
                && vp.ZOverSpeed > s_Config.PlayerEngine.ZMoveSpeedThreshold
            ) { // 0x4FC841
                vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                RequestEngineSound(AE_SOUND_PLAYER_CRZ);
                m_State = eAEState::PLAYER_CRZ;
            } else {
                DoFullAcceleration(); // 0x4FC3A1
            }
            break;
        }

        // 0x4FC96F
        CancelVehicleEngineSound(AE_SOUND_CAR_REV);
        RequestEngineSound(AE_SOUND_PLAYER_OFF);
        RequestEngineSound(AE_SOUND_CAR_ID);
        m_State = eAEState::PLAYER_ID;

        break;
    }
    case eAEState::PLAYER_CRZ: { // 0x4FBEC8
        if (!vp.Vehicle->vehicleFlags.bEngineOn) {
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        const auto EndCrz = [&] {
            m_CrzCount = 0.f;
            CancelVehicleEngineSound(AE_SOUND_PLAYER_CRZ);
        };
        const auto CrzToIdle = [&] {
            EndCrz();
            RequestEngineSound(AE_SOUND_PLAYER_OFF);
            RequestEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        };
        const auto EndCrzWhenInhibited = [&] {
            if (vp.ThisAccel <= cfg->AC.WheelSpinThreshold) {
                CrzToIdle();
            } else {
                EndCrz();
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            }
        };

        if (isReversing) { // 0x4FBEF3
            EndCrzWhenInhibited();
            break;
        }

        if (vp.ThisAccel <= 0) {
            CrzToIdle();
            break;
        }

        if (IsAccInhibited(vp) || IsAccInhibitedForLowSpeed(vp)) { // 0x4FBF55
            EndCrzWhenInhibited();
            break;
        }

        if (!IsAccInhibitedForLowSpeed(vp) && vp.ThisAccel > cfg->AC.WheelSpinThreshold) {
            if (   vp.RealGear < vp.NumGears
                && s_Config.PlayerEngine.AC.SpeedOffset + vp.PrevSpeed < vp.Speed
                && !IsAccInhibitedForTime()
                && !CCullZones::DoExtraAirResistanceForPlayer()
            ) { // 0x4FBFC7 (Inverted!)
                EndCrz();
                DoFullAcceleration(); // 0x4FC8C2
                InhibitCrzForTime(2'000); // 0x4FC099: mov [esi+0D4h],edx (despues de pedir el sonido)
            } else { // 0x4FC0B7 - Still doing crz, update sound
                // Como el original (0x4FC0AB..0x4FC10B): sube si (PrevSpeed - 0.05) < Speed (fcomp + test ah,5 / jp; con NaN baja);
                // sube: (c+1 < max) ? c+1 : max (fcom + test ah,5 / jnp); baja: (c-0.25 > 0) ? c-0.25 : 0 (fcom + test ah,41h / je)
                if (vp.PrevSpeed - 0.05f < vp.Speed) {
                    const float up  = m_CrzCount + 1.f;
                    const float cap = static_cast<float>(static_cast<int16>(cfg->CrzMaxCnt)); // movsx eax,word [8CBC8C]
                    m_CrzCount      = (up < cap) ? up : cap;
                } else {
                    const float dn = m_CrzCount - 0.25f;
                    m_CrzCount     = (dn > 0.f) ? dn : 0.f;
                }
                UpdateEngineSound(AE_SOUND_PLAYER_CRZ);
                m_State = eAEState::PLAYER_CRZ;
            }
        } else {
            CrzToIdle();
        }
        break;
    }
    case eAEState::PLAYER_ID: {
        if (!vp.Vehicle->vehicleFlags.bEngineOn) {
            CancelAllVehicleEngineSounds();
            m_State = eAEState::CAR_OFF;

            break;
        }

        if (isReversing) { // 0x4FC211
            if (vp.ThisAccel <= s_Config.PlayerEngine.AC.WheelSpinThreshold) { // (Inverted!)
                UpdateEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            } else { // 0x4FC224
                CancelVehicleEngineSound(AE_SOUND_CAR_ID);
                CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            }
            break;
        }

        if (vp.ThisAccel <= 0) { // 0x4FC27D
            UpdateEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;

            break;
        }

        if (IsAccInhibited(vp) || IsAccInhibitedForLowSpeed(vp)) { // 0x4FC28B
            if (vp.ThisAccel > cfg->AC.WheelSpinThreshold) { // 0x4FC41E
                CancelVehicleEngineSound(AE_SOUND_CAR_ID);
                CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                RequestEngineSound(AE_SOUND_CAR_REV);
                m_State = eAEState::PLAYER_WHEEL_SPIN;
            } else { // 0x4FC413 jle 0x4FC471: frenado/marcha lenta sin acelerar mas del umbral -> actualiza el sonido de ralenti
                UpdateEngineSound(AE_SOUND_CAR_ID);
                m_State = eAEState::PLAYER_ID;
            }
        } else if (!IsAccInhibitedForLowSpeed(vp) && vp.ThisAccel > cfg->AC.WheelSpinThreshold) {
            // 0x4FC2C7: el EXE cancela primero PLAYER_OFF (+0x110) y despues CAR_ID (+0xF8)
            if (   vp.RealGear < vp.NumGears
                && cfg->CrzSpeedOffset + vp.PrevSpeed > vp.Speed
                && vp.ZOverSpeed > cfg->ZMoveSpeedThreshold
            ) {
                vp.Vehicle->vehicleFlags.bAudioChangingGear = true;
                CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                CancelVehicleEngineSound(AE_SOUND_CAR_ID);
                RequestEngineSound(AE_SOUND_PLAYER_CRZ);
                m_State = eAEState::PLAYER_CRZ;
            } else {
                CancelVehicleEngineSound(AE_SOUND_PLAYER_OFF);
                CancelVehicleEngineSound(AE_SOUND_CAR_ID);
                DoFullAcceleration();
            }
        } else {
            UpdateEngineSound(AE_SOUND_CAR_ID);
            m_State = eAEState::PLAYER_ID;
        }
        break;
    }
    default: // 0x4FC9E5: estados de dummy y marcha atras: el EXE sale sin hacer nada
        break;
    }
}

#pragma region RC Utility functions
// notsa
void CAEVehicleAudioEntity::UpdateDummyRCAcAndBrake(tVehicleParams& vp) {
    if (s_pPlayerDriver && m_IsPlayerDriver) {
        GetAccelAndBrake(vp);
    } else {
        vp.ThisAccel = 0.f;
        vp.ThisBrake = 0.f;
    }
}
#pragma endregion

#pragma region Aircraft
// notsa - Update rotor frequency smoothly
void CAEVehicleAudioEntity::UpdateRotorFreq(float freq, float stepUp, float stepDown) {
    // Como el original (p. ej. 0x4FA9C7..0x4FAA49): sin escalar por el paso de tiempo
    if (m_CurrentRotorFrequency < 0.f) {
        m_CurrentRotorFrequency = freq;
    }
    const float cur = m_CurrentRotorFrequency;
    if (freq < cur) {
        const float lo          = cur - stepDown;
        m_CurrentRotorFrequency = (freq <= lo) ? lo : freq;
    } else {
        const float up          = stepUp + cur;
        m_CurrentRotorFrequency = (freq < up) ? freq : up;
    }
}

// notsa
auto GetAircraftDistantSoundVolume(const CPhysical* v, float baseVol) {
    const auto distSq = CVector::DistSqr(TheCamera.GetPosition(), v->GetPosition());
    if (distSq <= sq(48.f)) {
        return -100.f;
    }
    auto vol = baseVol;
    if (distSq <= 128.f) {
        vol += invLerp(48.f, 128.f, std::sqrtf(distSq)); // NB: I think this should be substracted (The closer the plane is, the less volume the distant sound should have)
    }
    return AudioLog10ScaledExe(vol, 20.0f) + 8.f;
}

// notsa - See 0x4FE5E5
auto GetAircraftCameraPoVFactor(const CPhysical* p) {
    return ((p->GetPosition() - TheCamera.GetPosition()).Normalized().Dot(p->GetForward()) + 1.f) * 0.5f;
}

// notsa - Get acceleration factor for aircrafts
auto GetAircraftAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) {
    auto s = vp.ThisAccel > 0
        ? 0.1f
        : 0.f;
    if (vp.ThisBrake > 0) {
        s -= 0.05f;
    }
    return s;
}

// notsa
float GetDummyRCRotorSpeedFactor(float p) {
    // Como el original (0x4FA945..0x4FA987)
    const float q = (p > 1.0f) ? 1.0f : p;
    float       x;
    if (q >= 0.2f || std::isnan(q)) {
        x = (q - 0.2f) * 1.25f;
    } else {
        x = 0.0f;
    }
    return x > 0.2 ? x : static_cast<float>(0.2); // fcom qword [00862D08] (0.2 doble)
}


// 0x4F96A0
CVector CAEVehicleAudioEntity::GetAircraftNearPosition() const noexcept {
    if (m_AuSettings.IsHeli()) {
        CVector pos;
        m_Entity->AsVehicle()->GetComponentWorldPosition(s_Config.HeliAudioComponent, pos);
        return pos;
    }
    return m_Entity->GetPosition();
}

// 0x501C50
void CAEVehicleAudioEntity::ProcessAircraft(tVehicleParams& params) {
    // Como el original (0x501C50): primero el banco general (0x501C5F call 004D88C0) y despues, en cada tipo de aeronave, m_IsWreckedVehicle (0x501CB0 / 0x501CDF / 0x501D50)
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
        return;
    }
    if (m_IsWreckedVehicle && m_AuSettings.VehicleAudioType >= AE_AIRCRAFT_HELICOPTER && m_AuSettings.VehicleAudioType <= AE_AIRCRAFT_SEAPLANE) {
        return;
    }

    auto* vehicle = params.Vehicle;
    switch (m_AuSettings.VehicleAudioType) {
    case AE_AIRCRAFT_HELICOPTER: {
        if (s_HelicoptorsDisabled || m_IsHelicopterDisabled) {
            JustWreckedVehicle();
        } else if (m_IsPlayerDriver) {
            ProcessPlayerHeli(params);
        } else if (vehicle->GetStatus() == STATUS_PHYSICS) {
            ProcessAIHeli(params);
        } else {
            ProcessDummyHeli(params);
        }
        break;
    }
    case AE_AIRCRAFT_PLANE: {
        switch (vehicle->GetModelIndex()) {
        case MODEL_SHAMAL:
        case MODEL_HYDRA:
        case MODEL_AT400:
        case MODEL_ANDROM: { // Originally there was an `isPlane` variable. I just rearranged stuff, to look nicer
            if (m_IsPlayerDriver) {
                ProcessPlayerJet(params);
            } else {
                ProcessDummyJet(params);
            }
            break;
        }
        default: {
            if (m_IsPlayerDriver) {
                ProcessPlayerProp(params);
            } else if (vehicle->GetStatus() == STATUS_PHYSICS || static_cast<int8>(reinterpret_cast<const uint8*>(vehicle)[0x424]) >= 0) { // 0x501D2C: cmp byte [edx+424h]; jge
                ProcessAIProp(params);
            } else {
                ProcessDummyProp(params);
            }
        }
        }
        break;
    }
    case AE_AIRCRAFT_SEAPLANE: {
        if (m_IsPlayerDriver) {
            ProcessPlayerSeaPlane(params);
        } else {
            ProcessDummySeaPlane(params);
        }
        break;
    }
    }
}

// 0x4F93C0
void CAEVehicleAudioEntity::PlayAircraftSound(eAircraftSoundType st, eSoundBankSlot slot, eSoundID sfx, float volume, float speed) {
    // Como el original (0x4F93C0): el volumen se usa tal cual (el EXE no suma m_EventVolume ni al actualizar ni al crear)
    if (UpdateVehicleEngineSoundRawVolume(st, speed, volume)) {
        return;
    }
    const auto DoPlaySound = [&](float rollOff, float doppler = 1.0f, std::optional<CVector> pos = std::nullopt) {
        m_EngineSounds[st].Sound = PlayVehicleSound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = pos.value_or(m_Entity->GetPosition()),
            .Volume        = volume,
            .RollOffFactor = rollOff,
            .Speed         = speed,
            .Doppler       = doppler,
            .Flags         = SOUND_REQUEST_UPDATES
        });
    };
    switch (st) {
    case AE_SOUND_AIRCRAFT_DISTANT:     DoPlaySound(6.0f);                                 break;
    case AE_SOUND_AIRCRAFT_FRONT:
    case AE_SOUND_AIRCRAFT_REAR:        DoPlaySound(4.0f);                                 break;
    case AE_SOUND_AIRCRAFT_NEAR:        DoPlaySound(1.0f, 0.4f);                           break;
    case AE_SOUND_AIRCRAFT_THRUST:      DoPlaySound(4.5f);                                 break;
    case AE_SOUND_AIRCRAFT_JET_DISTANT: DoPlaySound(50.0f, 1.f, CVector{ 0.f, 1.f, 0.f }); break;
    default:                            break; // Fuera del dominio real
    }
}

#pragma region Propellers

// notsa
auto GetPropSpeedFactor(float propSpeed) {
    return CGeneral::GetPiecewiseLinear({ // 0x8CBE00
        { 0.00f, 0.0f },
        { 0.15f, 0.3f },
        { 1.00f, 1.0f }
    }, std::clamp(propSpeed, 0.f, 1.f));
}

// notsa
float CalculatePropSpeed(const CPlane* plane, float gas, float brake) {
    return plane->m_autoPilot.m_vehicleRecordingId != -1
        ? 0.7f + 0.3f * std::clamp(std::max(gas, brake), 0.f, 1.f)
        : plane->m_fPropSpeed / 0.34f;
}

// notsa
float CalculatePlanePropFreq(CAEVehicleAudioEntity::tVehicleParams& vp, float ac) {
    // Como el original (0x4FA8F9..0x4FA9BD): ((|r.z'| * 0.1 + (-f.z') * 0.15) + ac) + 1
    const auto plane = vp.Vehicle->AsPlane();
    const auto& fwd  = plane->GetForward();
    const auto& rgt  = plane->GetRight();
    const float b    = -(((fwd.y + fwd.x) * 0.0f) + fwd.z);
    float       a    = ((rgt.y + rgt.x) * 0.0f) + rgt.z;
    if (a < 0.0f) {
        a = -a;
    }
    return ((a * 0.1f + b * 0.15f) + ac) + 1.0f;
}

namespace {
// Secuencias en linea del EXE compartidas por ProcessPlayerProp (0x4FD290), ProcessDummyProp (0x4FD8F0) y ProcessAIProp (0x4FDFD0)
constexpr float kRotorStepExe = std::bit_cast<float>(0x3BAEC33Cu); // 0x862D04
constexpr float kPropTiltExe  = std::bit_cast<float>(0x3F2B851Fu); // 0x8CBD98 (0.67)

// fcomp con 1.0 y 0.0: >1 -> 1, <0 -> 0, un NaN se queda como NaN
inline float ExeClamp01(float f) {
    return f > 1.0f ? 1.0f : (f < 0.0f ? 0.0f : f);
}

// 0x4D9D90 con la tabla 0x8CBE00 (el ultimo x es 1.0001)
inline float ExePropSpeedFactor(float f) {
    static constexpr float pts[3][2] = { { 0.0f, 0.0f }, { 0.15f, 0.3f }, { 1.0001f, 1.0f } };
    return CAEAudioUtility::GetPiecewiseLinearT(f, pts);
}

// 0x4FD3E7..0x4FD433 (componente lateral R y vertical Q de la matriz del vehiculo)
inline void ExePropOrientation(const CEntity* e, float& R, float& Q) {
    const auto& f = e->GetForward();
    const auto& r = e->GetRight();
    Q = -(((f.y + f.x) * 0.0f) + f.z);
    R = ((r.y + r.x) * 0.0f) + r.z;
    if (R < 0.0f) {
        R = -R;
    }
}

// 0x4FD3A8..0x4FD48B: direccion normalizada camara -> entidad contra el eje Y de la matriz del vehiculo
inline float ExePropCamFactor(const CVector& entityPos, const CEntity* vehicle) {
    CVector d = entityPos - TheCamera.GetPosition();
    d.Normalise();
    const auto& f = vehicle->GetForward();
    const float t = (d.y * f.y + d.z * f.z) + d.x * f.x;
    return (t + 1.0f) * 0.5f;
}

// 0x4FD521..0x4FD5F7: volumen del sonido lejano segun la distancia a la camara
inline float ExePropDistantVolume(const CVector& entityPos, float pf) {
    const CVector d    = entityPos - TheCamera.GetPosition();
    const float   dist = std::sqrt((d.x * d.x + d.y * d.y) + d.z * d.z);
    if (dist < 128.0f) {
        if (dist < 48.0f) {
            return -100.0f;
        }
        const float t  = (dist - 48.0f) * std::bit_cast<float>(0x3C4CCCCDu); // 0x85A5C4
        const float lt = AudioLog10ScaledExe(t, 20.0f);
        const float lp = AudioLog10ScaledExe(pf, 20.0f);
        return (lp + lt) + 8.0f;
    }
    return AudioLog10ScaledExe(pf, 20.0f) + 8.0f;
}
}

// 0x4FDFD0 - Process AI propeller sound
void CAEVehicleAudioEntity::ProcessAIProp(tVehicleParams& vp) {
    // Como el original (0x4FDFD0)
    if (m_DummyEngineBank == SND_BANK_UNK) { // 0x4FE008
        return;
    }
    if (m_DummySlot == -1) { // 0x4FE00E
        m_DummySlot = RequestBankSlot(m_DummyEngineBank);
        if (m_DummySlot == -1) {
            return;
        }
    }
    if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) { // 0x4FE04A
        if (!DoesBankSlotContainThisBank(m_DummySlot, m_DummyEngineBank)) { // 0x4FE04E..0x4FE07B
            m_DummySlot = RequestBankSlot(m_DummyEngineBank);
            m_State     = eAEState::CAR_OFF;
        }
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x4FE0CA
        return;
    }

    auto* const raw   = reinterpret_cast<const uint8*>(vp.Vehicle);
    const bool  recOk = static_cast<int8>(raw[0x424]) >= 0;
    if (!(raw[0x428] & 0x10) && !recOk) { // 0x4FE0EC
        CancelAllVehicleEngineSounds();
        return;
    }

    // 0x4FE10A - Velocidad de la helice a partir de los pedales del vehiculo
    const int16 accelRaw = static_cast<int16>(static_cast<int32>(*reinterpret_cast<const float*>(raw + 0x49C) * 255.0f));
    const int16 brakeRaw = static_cast<int16>(static_cast<int32>(*reinterpret_cast<const float*>(raw + 0x4A0) * 255.0f));
    const int16 maxRaw   = accelRaw > brakeRaw ? accelRaw : brakeRaw;
    float       f        = ExeClamp01(static_cast<float>(static_cast<int32>(maxRaw)) * std::bit_cast<float>(0x3B808081u));
    f = recOk
        ? f * std::bit_cast<float>(0x3E99999Au) + std::bit_cast<float>(0x3F333333u)
        : *reinterpret_cast<const float*>(raw + 0x9C4) * std::bit_cast<float>(0x403C3C3Cu);
    const float pf = ExePropSpeedFactor(ExeClamp01(f));

    float R, Q;
    ExePropOrientation(vp.Vehicle, R, Q);
    const float L3 = AudioLog10ScaledExe(pf, 20.0f) + 8.0f; // 0x4FE23E

    // 0x4FE256 - Frecuencia del rotor
    const float F = (R * std::bit_cast<float>(0x3DCCCCCDu) + Q * std::bit_cast<float>(0x3E19999Au)) + 1.0f;
    UpdateRotorFreq(F, kRotorStepExe, kRotorStepExe);

    // 0x4FE2E8 - Sonido lejano
    const float S = ExePropDistantVolume(m_Entity->GetPosition(), pf);

    PlayAircraftSound(AE_SOUND_AIRCRAFT_FRONT, m_DummySlot, 1, L3, m_CurrentRotorFrequency);   // 0x4FE3CF
    PlayAircraftSound(AE_SOUND_AIRCRAFT_DISTANT, m_DummySlot, 0, S, 1.0f);                    // 0x4FE3EE
}

// Based on `ProcessDummyProp`. Implementation for both `ProcessDummyProp` and `ProcessPlayerProp`
// Process plane propeller sounds
void CAEVehicleAudioEntity::ProcessDummyOrPlayerProp(tVehicleParams& vp, bool isDummy) {
    // Como el original: 0x4FD290 (jugador) y 0x4FD8F0 (dummy), que solo difieren en el prologo y en el bloque de calado
    auto* const plane = vp.Vehicle->AsPlane();
    auto* const raw   = reinterpret_cast<const uint8*>(vp.Vehicle);

    float pf;
    if (isDummy) {
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)
            || !AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) { // 0x4FD936..0x4FD957
            return;
        }
        const bool recOk = static_cast<int8>(raw[0x424]) >= 0;
        if (!(raw[0x428] & 0x10) && !recOk) { // 0x4FD98A
            CancelAllVehicleEngineSounds();
            return;
        }
        // 0x4FD9B4 - Los pedales se leen del vehiculo y se guardan como palabras de 16 bits
        const int16 accelRaw = static_cast<int16>(static_cast<int32>(*reinterpret_cast<const float*>(raw + 0x49C) * 255.0f));
        using G4Pedal        = std::remove_reference_t<decltype(vp.ThisAccel)>;
        vp.ThisAccel         = G4Pedal{ accelRaw }; // palabra de 16 bits tal cual (0x4FD9CF mov [edx+20h],bp)
        const int16 brakeRaw = static_cast<int16>(static_cast<int32>(*reinterpret_cast<const float*>(raw + 0x4A0) * 255.0f));
        vp.ThisBrake         = G4Pedal{ brakeRaw }; // 0x4FD9EE mov [ecx+22h],ax
        const int16 maxRaw   = accelRaw > brakeRaw ? accelRaw : brakeRaw;
        float       f        = ExeClamp01(static_cast<float>(static_cast<int32>(maxRaw)) * std::bit_cast<float>(0x3B808081u));
        f = recOk
            ? f * std::bit_cast<float>(0x3E99999Au) + std::bit_cast<float>(0x3F333333u)
            : *reinterpret_cast<const float*>(raw + 0x9C4) * std::bit_cast<float>(0x403C3C3Cu);
        pf = ExePropSpeedFactor(ExeClamp01(f));
    } else {
        if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)
            || !AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x4FD2DB..0x4FD2F7
            return;
        }
        if (!(raw[0x428] & 0x10)) { // 0x4FD31E
            CancelAllVehicleEngineSounds();
            return;
        }
        // 0x4FD344
        pf = ExePropSpeedFactor(ExeClamp01(*reinterpret_cast<const float*>(raw + 0x9C4) * std::bit_cast<float>(0x403C3C3Cu)));
        GetAccelAndBrake(vp); // 0x4FD3A3
    }

    // 0x4FD3A8 - Geometria
    float       R, Q;
    const float C = ExePropCamFactor(m_Entity->GetPosition(), vp.Vehicle);
    ExePropOrientation(vp.Vehicle, R, Q);

    float A = 0.0f; // aceleracion del avion
    if (vp.ThisAccel > 0) {
        A = std::bit_cast<float>(0x3DCCCCCDu);
    }
    if (vp.ThisBrake > 0) {
        A = A - std::bit_cast<float>(0x3D4CCCCDu);
    }

    // 0x4FD4AD - Volumenes
    const float W1 = (1.0f - kPropTiltExe * C) * pf;
    const float L1 = AudioLog10ScaledExe(W1, 20.0f) + 8.0f;
    const float W2 = (C * 0.5f + 0.5f) * pf;
    const float L2 = AudioLog10ScaledExe(W2, 20.0f) + 8.0f;
    const float L3 = AudioLog10ScaledExe(pf, 20.0f) + 8.0f;
    const float S  = ExePropDistantVolume(m_Entity->GetPosition(), pf);

    // 0x4FD5FB - Calado
    float stallVol, stallFreq;
    ProcessPropOrJetStall(plane, stallVol, stallFreq, !isDummy);

    // 0x4FD7B3 - Frecuencia del rotor
    const float F = (((R * std::bit_cast<float>(0x3DCCCCCDu) + Q * std::bit_cast<float>(0x3E19999Au)) + A) + 1.0f) * stallFreq;
    UpdateRotorFreq(F, kRotorStepExe, kRotorStepExe);
    const float freq = m_CurrentRotorFrequency;

    PlayAircraftSound(AE_SOUND_AIRCRAFT_FRONT, SND_BANK_SLOT_PLAYER_ENGINE_P, 0, stallVol + L1, freq);
    PlayAircraftSound(AE_SOUND_AIRCRAFT_REAR, SND_BANK_SLOT_PLAYER_ENGINE_P, 1, stallVol + L2, freq);
    PlayAircraftSound(AE_SOUND_AIRCRAFT_NEAR, SND_BANK_SLOT_VEHICLE_GEN, 17, stallVol + L3, 1.0f);
    PlayAircraftSound(AE_SOUND_AIRCRAFT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 16, stallVol + S, 1.0f);
}

// 0x4FD290
void CAEVehicleAudioEntity::ProcessPlayerProp(tVehicleParams& vp) {
    ProcessDummyOrPlayerProp(vp, false);
}

// 0x4FD8F0
void CAEVehicleAudioEntity::ProcessDummyProp(tVehicleParams& vp) {
    ProcessDummyOrPlayerProp(vp, true);
}

// notsa (Used in ProcessPlayerProp, ProcessDummyProp, ProcessPlayerJet)
void CAEVehicleAudioEntity::ProcessPropOrJetStall(const CPlane* plane, float& outVol, float& outFreq, bool isPlayerProp) {
    // Como el original (0x4FD5FB / 0x4FDD03 / 0x50173E): bloque en linea con la salud del avion
    const auto* const raw = reinterpret_cast<const uint8*>(plane);
    const float       hp  = plane->m_fHealth;

    if (m_DoCountStalls) {
        m_StalledCount = 15;
    }

    outFreq = 1.0f;
    outVol  = 0.0f;

    bool timers = true;
    if (isPlayerProp && m_StalledCount > 0 && raw[0x960] == 0) { // 0x4FD60F (solo ProcessPlayerProp)
        outFreq = std::bit_cast<float>(0x3F4CCCCDu);
        outVol  = -12.0f;
    } else if (hp < 250.0f) { // 0x4FD644
        outFreq = std::bit_cast<float>(0x3F4CCCCDu);
        outVol  = -18.0f;
    } else if (hp < 390.0f) {
        outFreq = std::bit_cast<float>(0x3F59999Au);
        outVol  = -12.0f;
    } else if (hp < 460.0f) {
        outFreq = std::bit_cast<float>(0x3F666666u);
        outVol  = -9.0f;
    } else if (hp < 650.0f) {
        outFreq = std::bit_cast<float>(0x3F733333u);
        outVol  = -6.0f;
    } else {
        timers = false; // 0x4FD7AB: salud suficiente, volumen 0
    }
    if (!timers) {
        outVol = 0.0f;
        return;
    }

    if (CTimer::GetTimeInMS() > m_SwapStalledTime) { // 0x4FD6E2 (jbe)
        uint32 minT, maxT;
        uint32 now;
        if (m_IsSilentStalled) {
            m_IsSilentStalled = false;
            const bool fast   = CAEAudioUtility::ResolveProbability(std::bit_cast<float>(0x3F59999Au)); // 0.85
            now               = CTimer::GetTimeInMS();
            minT              = fast ? 25 : 175;
            maxT              = fast ? 75 : 250;
        } else {
            m_IsSilentStalled = true;
            const bool fast   = CAEAudioUtility::ResolveProbability(std::bit_cast<float>(0x3F733333u)); // 0.95
            now               = CTimer::GetTimeInMS();
            minT              = fast ? 75 : 400;
            maxT              = fast ? 175 : 410;
        }
        m_SwapStalledTime = CAEAudioUtility::GetRandomNumberInRange(minT, maxT) + now;
    }

    // 0x4FD788
    m_StalledCount = static_cast<int16>(std::max<int32>(static_cast<int32>(m_StalledCount) - 1, 0));

    if (!m_IsSilentStalled) { // 0x4FD7A1: el EXE anula el volumen cuando el byte vale 0
        outVol = 0.0f;
    }
}

#pragma endregion

#pragma region Heli
// Secuencias en linea del EXE compartidas por ProcessPlayerHeli (0x4FE420), ProcessDummyHeli (0x4FE940) y ProcessAIHeli (0x4FEE20)
namespace {
// 0x4D9D90 con la tabla 0x8CBE40 (volumen) / 0x8CBE18 (frecuencia); el ultimo x es 1.0001
inline float ExeHeliVolFactor(float rs) {
    static constexpr float pts[5][2] = { { 0.00f, 0.00f }, { 0.15f, 0.00f }, { 0.38f, 0.40f }, { 0.63f, 0.70f }, { 1.0001f, 1.00f } };
    return CAEAudioUtility::GetPiecewiseLinearT(rs, pts);
}
inline float ExeHeliFreqFactor(float rs) {
    static constexpr float pts[5][2] = { { 0.00f, 0.00f }, { 0.15f, 0.00f }, { 0.38f, 0.60f }, { 0.63f, 1.00f }, { 1.0001f, 1.00f } };
    return CAEAudioUtility::GetPiecewiseLinearT(rs, pts);
}
// 0x4FE4B7..0x4FE4FC: velocidad relativa del rotor (+0x84C) * 4.5454545, limitada a [0, 1]
inline float ExeHeliRotorSpeed(const CVehicle* v) {
    return ExeClamp01(*reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(v) + 0x84C) * std::bit_cast<float>(0x4091745Du));
}
// 0x4FE5BA..0x4FE5CF: inclinacion hacia delante = (1 - ((up.y + up.x) * 0 + up.z)) * 0.5
inline float ExeHeliTilt(const CEntity* v) {
    const auto& u = v->GetUp();
    return (1.0f - (((u.y + u.x) * 0.0f) + u.z)) * 0.5f;
}
// 0x4FE6CE..0x4FE70A: con la salud por debajo de 390 la frecuencia se perturba
inline float ExeHeliDamageFreq(const CVehicle* v, float f0) {
    const float hp = *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(v) + 0x4C0);
    if (hp < 390.0f) {
        const float r = CAEAudioUtility::GetRandomNumberInRange(-std::bit_cast<float>(0x3DCCCCCDu), std::bit_cast<float>(0x3DCCCCCDu));
        return (r + std::bit_cast<float>(0x3F400000u)) * f0;
    }
    return f0;
}
}

auto GetHeliRotorSpeed(const CHeli* h) {
    return h->m_fHeliRotorSpeed / 0.22f;
}

// notsa
auto GetVolumeFactorForHeliRotor(float rotorSpeed) {
    return CGeneral::GetPiecewiseLinear({ // 0x8CBE40
        { 0.00f, 0.00f },
        { 0.15f, 0.00f },
        { 0.38f, 0.40f },
        { 0.63f, 0.70f },
        { 1.00f, 1.00f },
    }, rotorSpeed);
}

// notsa
auto GetHeliRotorFreqFactor(float rotorSpeed) {
    return CGeneral::GetPiecewiseLinear({ // 0x8CBE18
        { 0.00f, 0.00f },
        { 0.15f, 0.00f },
        { 0.38f, 0.60f },
        { 0.63f, 1.00f },
        { 1.00f, 1.00f },
    }, rotorSpeed);
}

// notsa
auto GetHeliForwardTilt(const CHeli* h) {
    return (1.f - h->GetUp().Dot(CVector::ZAxisVector())) * 0.5f;
}

// notsa
void CAEVehicleAudioEntity::UpdateHeliRotorFrequency(const CHeli* heli, float rotorSpeed, float acceleration, bool isDummy) {
    const auto* const cfg = &s_Config.Heli;

    auto rotorFreq = (GetHeliForwardTilt(heli) * 0.2f + 1.f) // Forward tilt
                   * GetHeliRotorFreqFactor(rotorSpeed); // Factor for rotor rotation speed
    if (!isDummy) {
        rotorFreq *= acceleration + 1.f;
    }
    if (heli->GetHealth() < 390.f) {
        const auto* const var = &cfg->RotorFreqVariance;
        rotorFreq *= CAEAudioUtility::GetRandomNumberInRange(var->Min, var->Max) + var->Offset;
    }
    const auto* const stepping = rotorSpeed <= 0.63f
        ? &cfg->RotorFreqStepLoSpeed
        : &cfg->RotorFreqStepHiSpeed;
    UpdateRotorFreq(rotorFreq, stepping->StepUp, stepping->StepDown);
}

// 0x4FEE20
// 0x4FEE20
void CAEVehicleAudioEntity::ProcessAIHeli(tVehicleParams& vp) {
    // Como el original (0x4FEE20)
    if (m_PlayerEngineBank == SND_BANK_GENRL_APACHE
        && !AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_APACHE_D, SND_BANK_SLOT_COP_HELI)) { // 0x4FEE4B
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x4FEE72
        return;
    }

    const float rs = ExeHeliRotorSpeed(vp.Vehicle);   // 0x4FEEAC
    const float vf = ExeHeliVolFactor(rs);            // 0x4FEEFC
    const float ff = ExeHeliFreqFactor(rs);           // 0x4FEF0D

    const float C    = ExePropCamFactor(m_Entity->GetPosition(), vp.Vehicle); // 0x4FEF19..0x4FEFD1
    const float tilt = ExeHeliTilt(vp.Vehicle);

    const float W1 = (1.0f - C) * vf;
    const float L1 = AudioLog10ScaledExe(W1, 20.0f) + 8.0f;
    const float W2 = (C * 0.5f + 0.5f) * vf;
    const float L2 = AudioLog10ScaledExe(W2, 20.0f) + 8.0f;
    const float L3 = AudioLog10ScaledExe(vf, 20.0f) + 2.0f;

    // 0x4FF050 - Frecuencia del rotor (solo con el paso 0x3BAEC33C)
    const float F0 = ExeHeliDamageFreq(vp.Vehicle, (tilt * std::bit_cast<float>(0x3E4CCCCDu) + 1.0f) * ff);
    UpdateRotorFreq(F0, kRotorStepExe, kRotorStepExe);
    const float freq = m_CurrentRotorFrequency;

    const float S = ExePropDistantVolume(m_Entity->GetPosition(), vf); // 0x4FF11D

    if (CWeather::UnderWaterness >= 0.5f) { // 0x4FF1EA: fcomp [858B8C] / test ah,1 / jne (un NaN cuenta como "menor")
        CancelAllVehicleEngineSounds(AE_SOUND_AIRCRAFT_DISTANT);
        const float v = AudioLog10ScaledExe(vf, 20.0f) + 8.0f;
        PlayAircraftSound(AE_SOUND_AIRCRAFT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 3, v, 1.0f);
    } else if (m_PlayerEngineBank == SND_BANK_GENRL_APACHE) { // 0x4FF23D
        const float x = AudioLog10ScaledExe(vf, 20.0f) + 8.0f;
        PlayAircraftSound(AE_SOUND_AIRCRAFT_FRONT, SND_BANK_SLOT_COP_HELI, 1, std::bit_cast<float>(0x40C00000u) + x, freq);
        PlayAircraftSound(AE_SOUND_AIRCRAFT_DISTANT, SND_BANK_SLOT_COP_HELI, 0, std::bit_cast<float>(0x40C00000u) + S, 1.0f);
    } else { // 0x4FF295
        PlayAircraftSound(AE_SOUND_AIRCRAFT_FRONT, SND_BANK_SLOT_VEHICLE_GEN, 5, L1, freq);
        PlayAircraftSound(AE_SOUND_AIRCRAFT_REAR, SND_BANK_SLOT_VEHICLE_GEN, 18, L2, freq);
        PlayAircraftSound(AE_SOUND_AIRCRAFT_NEAR, SND_BANK_SLOT_VEHICLE_GEN, 23, L3, 1.0f);
        PlayAircraftSound(AE_SOUND_AIRCRAFT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 3, S, 1.0f);
    }
}

// notsa
// Used for `ProcessPlayerHeli` and `ProcessDummyHeli`
// References code from `ProcessPlayerHeli`
void CAEVehicleAudioEntity::ProcessDummyOrPlayerHeli(tVehicleParams& vp, bool isDummy) {
    // Como el original: 0x4FE420 (jugador) y 0x4FE940 (dummy); el dummy no pide pedales ni aplica la aceleracion
    if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)
        || !AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x4FE475..0x4FE48F
        return;
    }

    const float rs = ExeHeliRotorSpeed(vp.Vehicle);   // 0x4FE4B7
    const float vf = ExeHeliVolFactor(rs);            // 0x4FE508
    const float ff = ExeHeliFreqFactor(rs);           // 0x4FE519
    if (!isDummy) {
        GetAccelAndBrake(vp); // 0x4FE528
    }

    const float C    = ExePropCamFactor(m_Entity->GetPosition(), vp.Vehicle); // 0x4FE53B..0x4FE5E5
    const float tilt = ExeHeliTilt(vp.Vehicle);

    float A = 0.0f;
    if (!isDummy) {
        if (vp.ThisAccel > 0) { // 0x4FE602
            A = std::bit_cast<float>(0x3DCCCCCDu);
        }
        if (vp.ThisBrake > 0) {
            A = A - std::bit_cast<float>(0x3D4CCCCDu);
        }
    }

    const float W1 = (1.0f - kPropTiltExe * C) * vf;
    const float L1 = AudioLog10ScaledExe(W1, 20.0f) + 8.0f;
    const float W2 = (C * 0.5f + 0.5f) * vf;
    const float L2 = AudioLog10ScaledExe(W2, 20.0f) + 8.0f;
    const float L3 = AudioLog10ScaledExe(vf, 20.0f) + 2.0f;

    // 0x4FE6AA - Frecuencia del rotor
    const float base = isDummy
        ? (tilt * std::bit_cast<float>(0x3E4CCCCDu) + 1.0f) * ff
        : ((tilt * std::bit_cast<float>(0x3E4CCCCDu) + 1.0f) * (A + 1.0f)) * ff;
    const float F0 = ExeHeliDamageFreq(vp.Vehicle, base);

    // 0x4FE70E - Suavizado: con el rotor por debajo de 0.63 el paso es 0.03 (doble), si no 0x3BAEC33C
    {
        if (m_CurrentRotorFrequency < 0.0f) {
            m_CurrentRotorFrequency = F0;
        }
        const float cur = m_CurrentRotorFrequency;
        const bool  slow = static_cast<double>(rs) < 0.63;
        float       F;
        if (F0 < cur) {
            const float lo = slow ? static_cast<float>(static_cast<double>(cur) - 0.03) : cur - kRotorStepExe;
            F = (F0 <= lo) ? lo : F0;
        } else {
            const float up = slow ? static_cast<float>(static_cast<double>(cur) + 0.03) : cur + kRotorStepExe;
            F = (F0 < up) ? F0 : up;
        }
        m_CurrentRotorFrequency = F;
    }
    const float freq = m_CurrentRotorFrequency;

    const float S = ExePropDistantVolume(m_Entity->GetPosition(), vf); // 0x4FE7E6

    PlayAircraftSound(AE_SOUND_AIRCRAFT_FRONT, SND_BANK_SLOT_PLAYER_ENGINE_P, 0, L1, freq);    // 0x4FE8C5
    PlayAircraftSound(AE_SOUND_AIRCRAFT_REAR, SND_BANK_SLOT_PLAYER_ENGINE_P, 1, L2, freq);     // 0x4FE8D8
    PlayAircraftSound(AE_SOUND_AIRCRAFT_NEAR, SND_BANK_SLOT_PLAYER_ENGINE_P, 3, L3, 1.0f);     // 0x4FE8EF
    PlayAircraftSound(AE_SOUND_AIRCRAFT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 3, S, 1.0f);       // 0x4FE906
}

// 0x4FE420
void CAEVehicleAudioEntity::ProcessPlayerHeli(tVehicleParams& vp) {
    ProcessDummyOrPlayerHeli(vp, false);
}

// 0x4FE940
void CAEVehicleAudioEntity::ProcessDummyHeli(tVehicleParams& vp) {
    //const auto* const heli = vp.Vehicle->AsHeli();
    //const auto* const cfg = &s_Config.Heli;
    //
    //if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
    //    return;
    //}
    //if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
    //    return;
    //}
    //
    //// 0x4FE9C6
    //const auto rotorSpeed = std::clamp(GetHeliRotorSpeed(heli), 0.f, 1.f);
    //
    //// 0x4FF067 - Calculate and update rotor frequency
    //UpdateHeliRotorFrequency(heli, rotorSpeed, true);
    ProcessDummyOrPlayerHeli(vp, true);
}
#pragma endregion

#pragma region Seaplane

// 0x4FF320
void CAEVehicleAudioEntity::ProcessPlayerSeaPlane(tVehicleParams& vp) {
    // Como el original (0x4FF320); el "hidroavion" lee su velocidad de motor de +0x5A4 (es un CBoat)
    auto* const raw = reinterpret_cast<const uint8*>(vp.Vehicle);

    if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)
        || !AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x4FF375..0x4FF38A
        return;
    }

    const float ps = *reinterpret_cast<const float*>(raw + 0x5A4) * std::bit_cast<float>(0x4091745Du); // 0x4FF3B7
    GetAccelAndBrake(vp); // 0x4FF3CA

    const float C = ExePropCamFactor(m_Entity->GetPosition(), vp.Vehicle); // 0x4FF3CF..0x4FF4AE
    float       R, Q;
    ExePropOrientation(vp.Vehicle, R, Q);

    // 0x4FF4B2: V08 = min(ps, 1) con suelo 0.2 (comparacion en doble), x = factor de velocidad del rotor
    const float q   = ps > 1.0f ? 1.0f : ps;
    const float x   = GetDummyRCRotorSpeedFactor(ps);
    const float pf2 = static_cast<double>(q) > 0.2 ? q : static_cast<float>(0.2);

    float A = 0.0f;
    if (vp.ThisAccel > 0) { // 0x4FF52A
        A = std::bit_cast<float>(0x3DCCCCCDu);
    }
    if (vp.ThisBrake > 0) {
        A = A - std::bit_cast<float>(0x3D4CCCCDu);
    }

    // 0x4FF552 - Volumenes
    const float W1 = (1.0f - C) * x;
    const float L1 = AudioLog10ScaledExe(W1, 20.0f) + 8.0f;
    const float W2 = (C * 0.5f + 0.5f) * x;
    const float L2 = AudioLog10ScaledExe(W2, 20.0f) + 8.0f;
    const float L3 = AudioLog10ScaledExe(x, 20.0f) + 8.0f;

    // 0x4FF5CD - Frecuencia del rotor
    const float F0 = (((R * std::bit_cast<float>(0x3DCCCCCDu) + Q * std::bit_cast<float>(0x3E19999Au)) + A) + 1.0f);
    UpdateRotorFreq(F0, kRotorStepExe, kRotorStepExe);
    const float freq = m_CurrentRotorFrequency;

    const float S = ExePropDistantVolume(m_Entity->GetPosition(), pf2); // 0x4FF663

    PlayAircraftSound(AE_SOUND_AIRCRAFT_FRONT, SND_BANK_SLOT_PLAYER_ENGINE_P, 0, L1, freq);   // 0x4FF74A
    PlayAircraftSound(AE_SOUND_AIRCRAFT_REAR, SND_BANK_SLOT_PLAYER_ENGINE_P, 1, L2, freq);    // 0x4FF75D
    PlayAircraftSound(AE_SOUND_AIRCRAFT_NEAR, SND_BANK_SLOT_VEHICLE_GEN, 17, L3, 1.0f);       // 0x4FF774
    PlayAircraftSound(AE_SOUND_AIRCRAFT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 16, S, 1.0f);     // 0x4FF78B
}

// 0x4FF7C0
void CAEVehicleAudioEntity::ProcessDummySeaPlane(tVehicleParams& vp) {
    // Como el original (0x4FF7C0): solo suena el sonido cercano
    auto* const raw = reinterpret_cast<const uint8*>(vp.Vehicle);

    if (!(raw[0x428] & 0x10)) { // 0x4FF7EE
        CancelAllVehicleEngineSounds();
        return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x4FF820
        return;
    }

    const float ps = *reinterpret_cast<const float*>(raw + 0x5A4) * std::bit_cast<float>(0x4091745Du); // 0x4FF830
    const float x  = GetDummyRCRotorSpeedFactor(ps);
    const float v  = AudioLog10ScaledExe(x, 20.0f) + 8.0f;                                      // 0x4FF87F
    PlayAircraftSound(AE_SOUND_AIRCRAFT_NEAR, SND_BANK_SLOT_VEHICLE_GEN, 17, v, 1.0f);                  // 0x4FF8A9
}
#pragma endregion

#pragma region Jet

// 0x4FF900
void CAEVehicleAudioEntity::ProcessGenericJet(bool bEngineOn, tVehicleParams& params, float fEngineSpeed, float fAccelRatio, float fBrakeRatio, float fStalledVolume, float fStalledFrequency) {
    if (!AEAudioHardware.IsSoundBankLoaded(static_cast<eSoundBank>(138), static_cast<eSoundBankSlot>(19))) return;
    float front, rear, thrust, distant;
    switch (params.Vehicle->GetModelId()) {
    case 519: front=StaticRef<float>(0x8CBE68); rear=StaticRef<float>(0x8CBE6C); thrust=StaticRef<float>(0xB6B9F4); distant=StaticRef<float>(0x8CBE78); break;
    case 520: front=StaticRef<float>(0x8CBE7C); rear=StaticRef<float>(0x8CBE80); thrust=StaticRef<float>(0xB6B9F8); distant=StaticRef<float>(0x8CBE8C); break;
    case 577: front=StaticRef<float>(0x8CBE90); rear=StaticRef<float>(0x8CBE94); thrust=StaticRef<float>(0x8CBE98); distant=StaticRef<float>(0x8CBEA4); break;
    case 592: front=StaticRef<float>(0x8CBEA8); rear=StaticRef<float>(0x8CBEAC); thrust=StaticRef<float>(0xB6B9FC); distant=StaticRef<float>(0x8CBEB8); break;
    default: CancelAllVehicleEngineSounds(); return;
    }
    if (!bEngineOn) { CancelAllVehicleEngineSounds(); return; }
    auto direction = m_Entity->GetPosition() - TheCamera.GetPosition();
    direction.Normalise();
    const auto& matrix = params.Vehicle->GetMatrix();
    const auto& forward = matrix.GetForward();
    const auto& right = matrix.GetRight();
    const auto facing = ((direction.y * forward.y + direction.z * forward.z) + direction.x * forward.x + 1.f) * .5f;
    const auto tilt = -((forward.y + forward.x) * 0.f + forward.z);
    auto bank = (right.y + right.x) * 0.f + right.z;
    if (bank < 0.f) { bank = -bank; } // 0x4FFB0E: fcomp 0.0 / test ah,5 / jp (con -0.0 no se invierte)
    float pitch = fAccelRatio > 0.f ? .1f : 0.f;
    if (fBrakeRatio > 0.f) pitch -= .05f;
    const auto frontVolume = AudioLog10ScaledExe((1.f - StaticRef<float>(0x8CBD98) * facing) * fEngineSpeed, 20.0f) + front;
    const auto rearVolume = AudioLog10ScaledExe((facing * .5f + .5f) * fEngineSpeed, 20.0f) + rear;
    auto& fadingThrust = StaticRef<float>(reinterpret_cast<uintptr_t>(this) + 0x230);
    fadingThrust = fAccelRatio > 0.f ? thrust : fadingThrust - 1.f;
    const auto thrustVolume = (AudioLog10ScaledExe(fEngineSpeed, 20.0f) + fadingThrust) + fStalledVolume;
    const auto target = (((bank * StaticRef<float>(0x858B1C) + tilt * StaticRef<float>(0x858FCC)) + pitch) + 1.f) * fStalledFrequency;
    auto& frequency = m_CurrentRotorFrequency;
    if (frequency < 0.f) frequency = target;
    frequency = target < frequency ? std::max(target, frequency - StaticRef<float>(0x862D04)) : std::min(target, frequency + StaticRef<float>(0x862D04));
    PlayAircraftSound(AE_SOUND_AIRCRAFT_FRONT, static_cast<eSoundBankSlot>(19), 10, frontVolume, frequency);
    PlayAircraftSound(AE_SOUND_AIRCRAFT_REAR, static_cast<eSoundBankSlot>(19), 11, rearVolume, frequency);
    PlayAircraftSound(AE_SOUND_AIRCRAFT_THRUST, static_cast<eSoundBankSlot>(19), 26, thrustVolume, 1.f);
    PlayAircraftSound(AE_SOUND_AIRCRAFT_JET_DISTANT, static_cast<eSoundBankSlot>(19), 14, distant, 1.f);
}

// 0x501960
void CAEVehicleAudioEntity::ProcessDummyJet(tVehicleParams& vp) {
    // Como el original (0x501960)
    auto* const raw = reinterpret_cast<const uint8*>(vp.Vehicle);

    const int16 accelRaw = static_cast<int16>(static_cast<int32>(*reinterpret_cast<const float*>(raw + 0x49C) * 255.0f)); // 0x50196D
    const bool  recOk    = static_cast<int8>(raw[0x424]) >= 0;
    const int16 brakeRaw = static_cast<int16>(static_cast<int32>(*reinterpret_cast<const float*>(raw + 0x4A0) * 255.0f)); // 0x50198C
    const int16 maxRaw   = accelRaw > brakeRaw ? accelRaw : brakeRaw;
    float       f        = ExeClamp01(static_cast<float>(static_cast<int32>(maxRaw)) * std::bit_cast<float>(0x3B808081u));
    f = recOk
        ? f * std::bit_cast<float>(0x3E99999Au) + std::bit_cast<float>(0x3F333333u)
        : *reinterpret_cast<const float*>(raw + 0x9C4) * std::bit_cast<float>(0x403C3C3Cu);

    // 0x501A0B - velocidad de motor: (min(f, 1) - 0.2) * 1.25 si f >= 0.2, si no 0 (sin suelo en el dummy)
    float y;
    if (f > 1.0f) {
        y = (1.0f - std::bit_cast<float>(0x3E4CCCCDu)) * std::bit_cast<float>(0x3FA00000u);
    } else if (f < std::bit_cast<float>(0x3E4CCCCDu)) {
        y = 0.0f;
    } else {
        y = (f - std::bit_cast<float>(0x3E4CCCCDu)) * std::bit_cast<float>(0x3FA00000u);
    }

    const bool engineOn = (raw[0x428] & 0x10) || recOk; // 0x501A30
    ProcessGenericJet(
        engineOn,
        vp,
        y,
        static_cast<float>(static_cast<int32>(accelRaw)) * std::bit_cast<float>(0x3B808081u),
        static_cast<float>(static_cast<int32>(brakeRaw)) * std::bit_cast<float>(0x3B808081u),
        0.0f,
        1.0f
    );
}

// 0x501650
void CAEVehicleAudioEntity::ProcessPlayerJet(tVehicleParams& vp) {
    // Como el original (0x501650)
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) { // 0x501662
        return;
    }

    auto* const plane = vp.Vehicle->AsPlane();
    auto* const raw   = reinterpret_cast<const uint8*>(vp.Vehicle);
    const bool  recOk = static_cast<int8>(raw[0x424]) >= 0;

    if (!recOk) { // 0x501683
        GetAccelAndBrake(vp);
    } else {
        using G4PedalJ = std::remove_reference_t<decltype(vp.ThisAccel)>;
        vp.ThisAccel   = G4PedalJ{ static_cast<int16>(0) }; // 0x501693 mov [edi+20h],ax
        vp.ThisBrake   = G4PedalJ{ static_cast<int16>(0) }; // 0x501697 mov [edi+22h],ax
    }

    // 0x50169B
    // Como el original (0x5016A1..0x5016AC: mov ax,[edi+20h]; mov cx,[edi+22h]; cmp ax,cx; jg; mov eax,ecx; movsx ecx,ax): maximo con signo de las dos palabras de 16 bits
    const int16 rawAccel = *reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(&vp) + 0x20);
    const int16 rawBrake = *reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(&vp) + 0x22);
    const int16 maxRaw   = rawAccel > rawBrake ? rawAccel : rawBrake;
    float       f      = ExeClamp01(static_cast<float>(static_cast<int32>(maxRaw)) * std::bit_cast<float>(0x3B808081u));
    f = recOk
        ? f * std::bit_cast<float>(0x3E99999Au) + std::bit_cast<float>(0x3F333333u)
        : *reinterpret_cast<const float*>(raw + 0x9C4) * std::bit_cast<float>(0x403C3C3Cu);
    const float y = GetDummyRCRotorSpeedFactor(f); // 0x501710..0x501746 (con suelo 0.2)

    // 0x50174B - Calado
    float stalledVolume, stalledFreq;
    ProcessPropOrJetStall(plane, stalledVolume, stalledFreq);

    // 0x5018F2
    ProcessGenericJet(
        (raw[0x428] >> 4) & 1,
        vp,
        y,
        static_cast<float>(static_cast<int32>(rawAccel)) * std::bit_cast<float>(0x3B808081u), // 0x50190B..0x50192B: movsx word [edi+20h/22h]; fild; fmul 1/255
        static_cast<float>(static_cast<int32>(rawBrake)) * std::bit_cast<float>(0x3B808081u),
        stalledVolume,
        stalledFreq
    );
}

#pragma endregion

#pragma region Hovercraft (Vortex)
// 0x500F50
void CAEVehicleAudioEntity::ProcessDummyHovercraft(tVehicleParams& params) {
    auto* pad = CPad::GetPad(0);
    if (m_DummySlot == SND_BANK_SLOT_NONE) {
        m_DummySlot = RequestBankSlot(m_DummyEngineBank);
        if (m_DummySlot == SND_BANK_SLOT_NONE) return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
        if (DoesBankSlotContainThisBank(m_DummySlot, m_DummyEngineBank)) return;
        m_DummySlot = RequestBankSlot(m_DummyEngineBank); m_State = static_cast<eAEState>(0); return;
    }
    auto* bytes = reinterpret_cast<uint8*>(params.Vehicle);
    if (!(bytes[0x428] & 0x10) || m_IsWreckedVehicle) { CancelAllVehicleEngineSounds(); return; }
    const auto ratio = std::clamp(std::abs(*reinterpret_cast<float*>(bytes+0x988) * StaticRef<float>(0x862D68)), 0.f, 1.f);
    int16 accel = int16(std::bit_cast<uint32>(ratio)), brake = accel;
    if (!(bytes[0x36] & 0xF8)) {
        if (params.Speed < StaticRef<float>(0x858CDC)) { accel=pad->GetAccelerate(); accel=int16(accel-pad->GetBrake()); brake=0; }
        else { brake=pad->GetBrake(); accel=pad->GetAccelerate(); }
    }
    float offset = accel > 0 ? StaticRef<float>(0x858B1C) : ratio;
    if (brake > 0) offset = StaticRef<float>(0x85901C);
    const auto rev = std::clamp(*reinterpret_cast<float*>(bytes+0x9C4) * StaticRef<float>(0x862D18), 0.f, 1.f);
    const auto volume = StaticRef<float>(0xB6BA90) * rev + StaticRef<float>(0x8CBFAC);
    const auto target = ((StaticRef<float>(0xB6BA94) * rev + StaticRef<float>(0x8CBFBC) * ratio) + StaticRef<float>(0x8CBFB4)) + offset;
    if (m_CurrentRotorFrequency < 0.f) m_CurrentRotorFrequency = target;
    m_CurrentRotorFrequency = target < m_CurrentRotorFrequency ? std::max(target, m_CurrentRotorFrequency-StaticRef<float>(0x862D04)) : std::min(target, m_CurrentRotorFrequency+StaticRef<float>(0x862D04));
    UpdateGenericVehicleSound(1, m_DummySlot, m_DummyEngineBank, 0, m_CurrentRotorFrequency, volume, 3.5f);
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerHovercraft(tVehicleParams& params) {
    // NOP
}
#pragma endregion

#pragma region RC

// notsa
auto GetDummyRCAcceleration(CAEVehicleAudioEntity::tVehicleParams& vp) {
    return vp.ThisBrake > 0
        ? -0.05f
        : vp.ThisAccel > 0
            ? 0.1f
            : 0.f;
}

#pragma region RC Plane

// 0x4FA7C0
void CAEVehicleAudioEntity::ProcessDummyRCPlane(tVehicleParams& vp) {
    // Como el original (0x4FA7C0)
    const auto* const cfg   = &s_Config.DummyRCPlane;
    const auto* const plane = vp.Vehicle->AsPlane();

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }

    if (m_IsWreckedVehicle) { // 0x4FA877
        CancelAllVehicleEngineSounds();
        return;
    }

    // 0x4FA89B - Velocidad de la helice * (1 / 0.34) (0x862D18)
    const float propSpeed = plane->m_fPropSpeed * std::bit_cast<float>(0x403C3C3Cu);

    UpdateDummyRCAcAndBrake(vp);

    float accel = 0.0f;
    if (vp.ThisAccel > 0) {
        accel = 0.1f;
    }
    if (vp.ThisBrake > 0) {
        accel = -0.05f;
    }

    const float freq = CalculatePlanePropFreq(vp, accel);
    const float sf   = GetDummyRCRotorSpeedFactor(propSpeed);
    const float vol  = AudioLog10ScaledExe(sf, 20.0f) + cfg->RotorVolBase;

    UpdateRotorFreq(freq, cfg->RotorFreqStepUp, cfg->RotorFreqStepDown);

    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_FRONT,
        m_DummySlot,
        0,
        vol,
        m_CurrentRotorFrequency
    );
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerRCPlane(tVehicleParams& params) {
    // NOP
}
#pragma endregion

#pragma region RC Heli

// 0x4FAA80
void CAEVehicleAudioEntity::ProcessDummyRCHeli(tVehicleParams& vp) {
    // Como el original (0x4FAA80)
    const auto* const cfg = &s_Config.DummyRCHeli;
    const auto* const heli = vp.Vehicle->AsHeli();

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }

    if (m_IsWreckedVehicle) { // 0x4FAB37
        CancelAllVehicleEngineSounds();
        return;
    }

    // 0x4FAB5B - Velocidad del rotor * (1 / 0.22) (0x862D1C)
    const float rotor = heli->m_fHeliRotorSpeed * std::bit_cast<float>(0x4091745Du);

    UpdateDummyRCAcAndBrake(vp);

    float accel = 0.0f;
    if (vp.ThisAccel > 0) {
        accel = 0.1f;
    }
    if (vp.ThisBrake > 0) {
        accel = -0.05f;
    }

    // 0x4FABBF - Inclinacion
    const auto& up = heli->GetUp();
    float       tilt = ((1.0f - (((up.y + up.x) * 0.0f) + up.z)) * 0.5f) / cfg->TiltDownStep;
    if (tilt >= 1.0f || std::isnan(tilt)) {
        tilt = 1.0f;
    }

    const float sf  = GetDummyRCRotorSpeedFactor(rotor);
    const float vol = AudioLog10ScaledExe(sf, 20.0f) + cfg->VolBase;

    // 0x4FAC62 - sf * 0.19999999 (0x862D00) + tilt * 0.2 + aceleracion + 0.8
    const float freq = ((sf * cfg->FrqSpeedFactor + tilt * cfg->FrqTiltDownFactor) + accel) + cfg->FrqBase;
    UpdateRotorFreq(freq, cfg->RotorFreqStepUp, cfg->RotorFreqStepDown);

    PlayAircraftSound(
        AE_SOUND_AIRCRAFT_FRONT,
        m_DummySlot,
        0,
        vol,
        m_CurrentRotorFrequency
    );
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerRCHeli(tVehicleParams& params) {
    // NOP
}
#pragma endregion
#pragma endregion
#pragma endregion

#pragma region Train

// notsa
// Como el original (0x5008B2..0x5008E1 / 0x500AC8..0x500AEC): velocidad lineal con signo; sin carril es 0 y el signo se invierte
// cuando NO es sentido horario (test dl,40h / jne / fchs).
float GetTrainSpeed(const CTrain* train) {
    float trspeed = train->trainFlags.bNotOnARailRoad
        ? 0.f
        : train->m_fTrainSpeed;
    if (!train->trainFlags.bClockwiseDirection) {
        trspeed = -trspeed;
    }
    return trspeed;
}

// notsa
// Como el original (0x50090A..0x5009A4 / 0x500AFF..0x500B83): |v| saturado a 1; el EXE devuelve v (o -v si v < 0) sin tocar el signo del cero.
float G5TrainAbsClamp01(float trspeed) {
    const float a = (trspeed < 0.f) ? -trspeed : trspeed;
    return (1.0f < a) ? 1.0f : a;
}

// notsa
// Como el original (0x5008B2): sin carril el factor es 0
float GetTrainSpeedFactor(const CTrain* train, float trspeed) {
    return train->trainFlags.bNotOnARailRoad
        ? 0.f
        : G5TrainAbsClamp01(trspeed);
}

// notsa
// Como el original (0x5009D0..0x500A63): solo el tren del jugador lee el pad; con |v| < 0.001 acelerador - freno (primero el acelerador),
// si no: v > 0 -> acelerador, en otro caso -freno. El valor se pasa de entero a float (fild) y se escala con 1/255 (0x859A3C = 0x3B808081)
// multiplicando (no dividiendo).
float G5TrainDummyAcFactor(float trspeed) {
    const auto* const p = CPad::GetPad();
    const float a = (trspeed < 0.f) ? -trspeed : trspeed;
    int32 ac;
    if (a < 0.001f) { // 0x500A00
        const int32 accel = p->GetAccelerate();
        const int32 brake = p->GetBrake();
        ac = accel - brake;
    } else if (trspeed > 0.f) { // 0x500A2F
        ac = p->GetAccelerate();
    } else { // 0x500A41
        ac = -static_cast<int32>(p->GetBrake());
    }
    return static_cast<float>(ac) * std::bit_cast<float>(0x3B808081u);
}

// 0x500710
void CAEVehicleAudioEntity::ProcessDummyTrainEngine(tVehicleParams& vp) {
    const auto* const train = vp.Vehicle->AsTrain();
    const auto isTram = m_DummyEngineBank == SND_BANK_GENRL_TRAM_D;
    const auto* const cfg = isTram
        ? &s_Config.Train.Tram.EngineSound
        : &s_Config.Train.Generic.EngineSound;

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true, false)) {
        return;
    }

    if (!train->trainFlags.bIsFrontCarriage) {
        StopGenericEngineSound(AE_SOUND_TRAIN_ENGINE);
        return;
    }

    // Como el original (0x500710): CPad::GetPad(0) se llama al principio (0x500719); es puro
    float vol = cfg->VolBase; // 0x5007D4: el volumen parte de VolBase (tram 0x8CBF30 = 11.0; generico 0xB6BA00 = 0.0)

    // 0x500876 - Calculate frequency for tilt: z = (fy + fx) * 0 + fz; saturado con los DOBLES +-0.3 (0x862D58 / 0x862D60) y
    // multiplicado por la pendiente en x87 (fmul con operando double, redondeo a float al guardar)
    // (0x500868..0x5008BE: todo en ST0 sin redondear; solo el fstp final del producto redondea a float; NaN -> -0.3 como en el EXE)
    const auto& fwd = train->GetForward();
    const float tiltFrq = VehAudioTrainTiltCpp(fwd.x, fwd.y, fwd.z, cfg->FrqSlopeFactor);

    // 0x5008B2 - Calculate linear speed of train to use
    const auto trspeed = GetTrainSpeed(train);

    // 0x5008B2 - Calculate speed factor
    const auto sf = GetTrainSpeedFactor(train, trspeed);

    // 0x5009A9
    if (isTram) {
        vol = AudioLog10ScaledExe(sf, 20.0f) + vol;
    }

    // 0x5009D0
    if (train->GetStatus() == STATUS_PLAYER) {
        vol = (G5TrainDummyAcFactor(trspeed) * cfg->VolPlayerAcFactor) + vol;
    }

    // 0x500A98 - velocidad = (FrqMin + (FrqMax - FrqMin) * sf) * (1 + inclinacion)
    UpdateTrainSound(
        AE_SOUND_TRAIN_ENGINE,
        m_DummySlot,
        0,
        ExeLerp(cfg->FrqMin, cfg->FrqMax, sf) * (1.f + tiltFrq),
        vol
    );
}

// 0x500AB0
void CAEVehicleAudioEntity::ProcessPlayerTrainBrakes(tVehicleParams& vp) {
    // Como el original (0x500AB0)
    const auto* const train = vp.Vehicle->AsTrain();

    // 0x500AC8..0x500B83 - velocidad con signo (sin carril: 0, y se invierte si NO es sentido horario) y su factor |v| saturado a 1
    // (aqui el EXE no trata aparte el caso sin carril: el cero con signo da el mismo factor)
    const float trspeed = GetTrainSpeed(train);
    const float sf      = G5TrainAbsClamp01(trspeed);

    // 0x500B83 - solo el tren del jugador lee el pad. |v| < 0.001: ac = 0 (se llama al acelerador y luego al freno, resultados
    // descartados); v > 0: ac = freno (luego se llama al acelerador, descartado); v <= 0: freno (descartado) y ac = acelerador.
    // Con otro estado el EXE deja ac sin inicializar (fuera de dominio): aqui 0.
    float ac = 0.f;
    if (train->GetStatus() == STATUS_PLAYER) {
        const auto* const pad = CPad::GetPad(0);
        volatile int32 discarded = 0;
        const float a = (trspeed < 0.f) ? -trspeed : trspeed;
        if (a < 0.001f) { // 0x500BA9
            discarded = pad->GetAccelerate(); // 0x500BB3
            discarded = pad->GetBrake();      // 0x500BBA
            ac        = 0.f;
        } else if (trspeed > 0.f) { // 0x500BCE
            const int32 brake = pad->GetBrake();
            ac                = static_cast<float>(brake);
            discarded         = pad->GetAccelerate(); // 0x500BE4
        } else { // 0x500BEB
            discarded         = pad->GetBrake();
            const int32 accel = pad->GetAccelerate();
            ac                = static_cast<float>(accel);
        }
        (void)discarded;
    }

    // 0x500C0A
    if (train->trainFlags.bNotOnARailRoad || !train->trainFlags.bIsFrontCarriage) {
        PlayTrainBrakeSound(-1, 1.f, -100.f); // Stop sound
        return;
    }

    // 0x500C38 - frecuencia = (1.0 - 0.68) * sf + 0.68 (0xB6BA88 = 0x8CBF48 - 0x8CBF44; 0x8CBF44); no depende del tipo de tren
    const auto& brakeCfg = s_Config.Train.Generic; // bloque anonimo: VolBase 9.0 (0x8CBF3C), FrqMin 0.68 (0x8CBF44), FrqMax 1.0 (0x8CBF48)
    const float freq     = (brakeCfg.FrqMax - brakeCfg.FrqMin) * sf + brakeCfg.FrqMin;

    // 0x500C58 - tabla de 4 puntos 0x8CC1E8 (el ultimo x es 0x3F800347 = 1.0001)
    static constexpr float points[4][2] = {
        { 0.0f, 0.0f },
        { 0.001f, 0.0f },
        { 0.1f, 1.0f },
        { std::bit_cast<float>(0x3F800347u), 1.0f },
    };
    const float pw  = CAEAudioUtility::GetPiecewiseLinearT(sf, points);
    const float vol = pw * (ac * std::bit_cast<float>(0x3B808081u));

    // 0x500C70 - fcomp -100.0 (0x859014): con vol < -100 se detiene con la frecuencia calculada
    if (vol < -100.f) {
        PlayTrainBrakeSound(-1, freq, -100.f);
        return;
    }
    PlayTrainBrakeSound(0, freq, AudioLog10ScaledExe(vol, 20.0f) + brakeCfg.VolBase);
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerTrainEngine(tVehicleParams& params) {
    // NOP
}

// 0x4FA3F0
constexpr float TrainCarriagesNoise[]{ 1.08f, 0.96f, 1.04f, 0.92f, 1.f };
void CAEVehicleAudioEntity::ProcessTrainTrackSound(tVehicleParams& vp) {
    // Como el original (0x4FA3F0)
    const auto* const train = vp.Vehicle->AsTrain();

    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true, false)) {
        return;
    }

    const auto* const cfg = m_DummyEngineBank == SND_BANK_GENRL_TRAM_D
        ? &s_Config.Train.Tram.TrackSound
        : &s_Config.Train.Generic.TrackSound;
    const float range = cfg->FrqMax - cfg->FrqMin; // 0xB6BA84 / 0xB6BA78

    float volume = -100.0f;
    float sf;
    if (reinterpret_cast<const uint8*>(train)[0x5B9] & 1) { // trainFlags.bNotOnARailRoad (0x4FA4D3)
        sf = 0.0f;
    } else {
        const float absTrainSpeed = std::fabs(train->m_fTrainSpeed);
        if (1.0f < absTrainSpeed) { // 0x4FA57B
            sf     = 1.0f;
            volume = AudioLog10ScaledExe(sf, 20.0f) + cfg->VolBase;
        } else if (absTrainSpeed >= 0.00001f) { // 0x4FA5A0
            sf     = absTrainSpeed;
            volume = AudioLog10ScaledExe(sf, 20.0f) + cfg->VolBase;
        } else {
            sf = absTrainSpeed;
        }
    }

    // 0x4FA5E1 - carruajes enlazados
    int16 n = 0;
    for (auto* it = reinterpret_cast<const CTrain*>(train); ; ) {
        it = it->m_pPrevCarriage;
        ++n;
        if (!it) {
            break;
        }
    }

    // 0x4FA5EC - frecuencia con el ruido por carruaje (tabla 0x8CBF54)
    const float speed = (sf * range + cfg->FrqMin) * TrainCarriagesNoise[n % static_cast<int>(std::size(TrainCarriagesNoise))];
    UpdateTrainSound(AE_SOUND_TRAIN_TRACK, m_DummySlot, SND_GENRL_TRAIN_D_TRAIN_TRACK, speed, volume);
}

// 0x4FA1C0
void CAEVehicleAudioEntity::UpdateTrainSound(eTrainEngineSoundType st, eSoundBankSlot slot, eSoundID sfx, float speed, float volume) {
    // Como el original (0x4FA1C0): el sonido nuevo se crea sobre m_tempSound de la entidad
    if (m_DummySlot == SND_BANK_SLOT_NONE) {
        return;
    }

    volume = volume + m_EventVolume;

    if (UpdateVehicleEngineSoundRawVolume(st, speed, volume)) { // 0x4FA3C7 (no vuelve a sumar m_EventVolume)
        return;
    }

    const auto* const cfg = m_DummyEngineBank == SND_BANK_GENRL_TRAM_D
        ? &s_Config.Train.Tram
        : &s_Config.Train.Generic;
    float rollOff;
    switch (st) {
    case AE_SOUND_TRAIN_ENGINE: { // 0x4FA308
        if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            return;
        }
        rollOff = cfg->EngineSoundRollOff;
        break;
    }
    case AE_SOUND_TRAIN_TRACK: { // 0x4FA270
        if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            return;
        }
        rollOff = cfg->TrackSoundRollOff;
        break;
    }
    case AE_SOUND_TRAIN_DISTANT: { // 0x4FA20B
        rollOff = cfg->DistantSoundRollOff;
        break;
    }
    default:
        return;
    }

    m_tempSound.Initialise(slot, sfx, this, m_Entity->GetPosition(), 0.0f, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f, 0);
    m_tempSound.m_RollOffFactor = rollOff;
    m_tempSound.m_Volume        = volume;
    m_tempSound.m_Speed         = speed;
    m_tempSound.m_Flags         = SOUND_REQUEST_UPDATES;
    m_EngineSounds[st].Sound    = AESoundManager.RequestNewSound(&m_tempSound);
}
#pragma endregion

#pragma region Boat
// 0x5003A0
void CAEVehicleAudioEntity::ProcessBoatEngine(tVehicleParams& vp) {
    const auto* const boat = vp.Vehicle->AsBoat();
    const auto* const cfg  = &s_Config.Boat;


    if (!EnsureHasDummySlot()) {
        return;
    }
    // Como el original (0x5003D3..0x500445): sin banco cargado NO se toca m_State (a diferencia de ProcessDummyRCCar/GolfCart)
    if (!EnsureSoundBankIsLoaded(true, false)) {
        return;
    }

    // 0x500451 - Engine off/got wrecked?
    if (!boat->vehicleFlags.bEngineOn || m_IsWreckedVehicle) {
        StopGenericEngineSound(AE_SOUND_BOAT_ENGINE);
        StopGenericEngineSound(AE_SOUND_BOAT_DISTANT);
        return;
    }

    if (s_pPlayerDriver && m_IsPlayerDriver) {
        GetAccelAndBrake(vp);
    }

    // Engine speed factor (0x5004C1..0x500510): (v < 0) ? 0 : (1 < v) ? 1 : v con v = velocidad del motor * 3
    const float es = [&] {
        const float v = boat->m_EngineSpeed * 3.f;
        return (1.0f < v) ? 1.0f : (v < 0.0f ? 0.0f : v);
    }();

    // 0x5004C7 - Calculate speed
    // Como el original (0x500487..0x50053B): z = (arriba.y + arriba.x) * 0 + arriba.z; rodadura = min(1, 1 - z);
    // velocidad = rodadura * k + (factor * es + base) con el factor del bit 1 (bBoatEngineInWater) de 0x5AC
    float speed;
    {
        const auto& up   = boat->GetUp();
        const float z    = (up.y + up.x) * 0.0f + up.z;
        const float r    = 1.0f - z;
        const float roll = (r < 1.0f) ? r : 1.0f;
        const float factor = boat->m_nBoatFlags.bBoatEngineInWater ? cfg->FrqEngineOnWaterFactor : cfg->FrqEngineInAirFactor;
        speed = roll * cfg->FrqEngineRollFactor + (factor * es + cfg->FrqEngineBase);
        // 0x500551: si la frecuencia actual es negativa se toma la nueva; despues sube/baja a pasos de 0.02 (0x858B38)
        if (m_CurrentDummyEngineFrequency < 0.f) {
            m_CurrentDummyEngineFrequency = speed;
        }
        const float cur = m_CurrentDummyEngineFrequency;
        if (speed > cur) { // 0x500597
            const float lim = cur + 0.02f;
            speed = (speed < lim) ? speed : lim;
        } else {           // 0x5005B7
            const float lim = cur - 0.02f;
            speed = (speed > lim) ? speed : lim;
        }
    }

    // 0x5005E0 - Calculate volume
    // Como el original (0x500541..0x500621): volumen = es * 3 - 3; si el actual es negativo se toma el nuevo; pasos de 3.0 (0x858B3C)
    float volume = es * 3.0f - 3.0f;
    {
        if (m_CurrentDummyEngineVolume < 0.f) {
            m_CurrentDummyEngineVolume = volume;
        }
        const float cur = m_CurrentDummyEngineVolume;
        if (volume > cur) { // 0x5005D2
            const float lim = cur + 3.0f;
            volume = (volume < lim) ? volume : lim;
        } else {            // 0x500604
            const float lim = cur - 3.0f;
            volume = (volume > lim) ? volume : lim;
        }
    }
    m_CurrentDummyEngineFrequency = speed;
    m_CurrentDummyEngineVolume    = volume;

    // 0x500632 - Update sound
    if (CWeather::UnderWaterness < 0.5f) {
        UpdateBoatSound(AE_SOUND_BOAT_ENGINE, m_DummySlot, 0, m_CurrentDummyEngineFrequency, m_CurrentDummyEngineVolume);
        UpdateBoatSound(AE_SOUND_BOAT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 28, 1.f, -100.f);
    } else {
        UpdateBoatSound(AE_SOUND_BOAT_ENGINE, m_DummySlot, 0, 1.f, -100.f);
        UpdateBoatSound(AE_SOUND_BOAT_DISTANT, SND_BANK_SLOT_VEHICLE_GEN, 28, m_CurrentDummyEngineFrequency, m_CurrentDummyEngineVolume + 6.f);
    }
}

// 0x4FA0C0
void CAEVehicleAudioEntity::ProcessBoatMovingOverWater(tVehicleParams& vp) {
    // Como el original (0x4FA0C0)
    const auto* const cfg = &s_Config.Boat.MovingOverWaterSound;

    const float absSpeed = std::fabs(vp.Speed);
    const float sf       = ((absSpeed >= 0.75f || std::isnan(absSpeed)) ? 0.75f : absSpeed) * std::bit_cast<float>(0x3FAAAAABu); // 0x85F09C = 1/0.75

    float volume = -100.0f;
    if ((reinterpret_cast<uint8*>(vp.Vehicle)[0x5AC] & 1) && sf >= 0.00001f) { // bBoatInWater
        volume = AudioLog10ScaledExe(sf, 20.0f) + (m_AuSettings.IsSeaplane() ? cfg->VolBaseOfSeaplane : cfg->VolBase);
    }
    float speed = sf * std::bit_cast<float>(0x3E4CCCCCu) + 0.8f; // 0x862D00 NO es el float mas cercano a 0.2

    if (!(CWeather::UnderWaterness < 0.5f)) { // 0x4FA172: fcomp [858B8C] + test ah,1 + jne
        speed  = cfg->FrqUnderwaterFactor * speed;
        volume = cfg->VolUnderwaterOffset + volume;
    }

    UpdateBoatSound(AE_SOUND_BOAT_WATER_SKIM, SND_BANK_SLOT_COLLISIONS, 3, speed, volume);
}

// 0x4F9E90
void CAEVehicleAudioEntity::UpdateBoatSound(eBoatEngineSoundType st, eSoundBankSlot slot, eSoundID sfx, float speed, float volume) {
    // Como el original (0x4F9E90): el sonido nuevo se crea sobre m_tempSound de la entidad
    const auto* const cfg = &s_Config.Boat;

    if (m_AuSettings.VehicleAudioType != AE_AIRCRAFT_SEAPLANE && st != AE_SOUND_BOAT_WATER_SKIM && m_DummySlot == SND_BANK_SLOT_NONE) {
        return;
    }

    volume = volume + m_EventVolume;

    if (UpdateVehicleEngineSoundRawVolume(st, speed, volume)) { // 0x4FA097
        return;
    }

    float rollOff;
    switch (st) {
    case AE_SOUND_BOAT_ENGINE: { // 0x4F9FE5
        if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            return;
        }
        rollOff = cfg->EngineSoundRollOff;
        break;
    }
    case AE_SOUND_BOAT_DISTANT: { // 0x4F9F6A
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_VEHICLE_GEN, SND_BANK_SLOT_VEHICLE_GEN)) {
            return;
        }
        rollOff = cfg->DistantSoundRollOff;
        break;
    }
    case AE_SOUND_BOAT_WATER_SKIM: { // 0x4F9EF2
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            return;
        }
        rollOff = cfg->WaterSkimSoundRollOff;
        break;
    }
    default:
        return;
    }

    m_tempSound.Initialise(slot, sfx, this, m_Entity->GetPosition(), 0.0f, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f, 0);
    m_tempSound.m_Volume        = volume;
    m_tempSound.m_RollOffFactor = rollOff;
    m_tempSound.m_Speed         = speed;
    m_tempSound.m_Flags         = SOUND_REQUEST_UPDATES;
    m_EngineSounds[st].Sound    = AESoundManager.RequestNewSound(&m_tempSound);
}
#pragma endregion

#pragma region Bicycle
// 0x4F9710
void CAEVehicleAudioEntity::PlayBicycleSound(eBicycleSoundType st, eSoundBankSlot slot, eSoundID sfx, float volume, float speed) {
    // Como el original (0x4F9710): el volumen se usa tal cual (sin m_EventVolume)
    if (m_DummySlot == SND_BANK_SLOT_NONE) {
        return;
    }

    // 0x4F9759 - Comprobacion del banco de cada tipo (antes de actualizar el sonido existente)
    switch (st) {
    case AE_SOUND_BICYCLE_TYRE: // 0x4F978C
        if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
            return;
        }
        break;
    case AE_SOUND_BICYCLE_SPROCKET_1: // 0x4F976D
        if (!AEAudioHardware.IsSoundBankLoaded(m_PlayerEngineBank, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
            return;
        }
        break;
    case AE_SOUND_BICYCLE_CHAIN_CLANG: // 0x4F9767
        if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_COLLISIONS, SND_BANK_SLOT_COLLISIONS)) {
            return;
        }
        break;
    default:
        break;
    }

    if (st == AE_SOUND_BICYCLE_CHAIN_CLANG) { // 0x4F97A9
        if (GetEngineSound(AE_SOUND_BICYCLE_CHAIN_CLANG)) {
            return;
        }
        static constexpr struct { // 0x8CC1D8
            eSoundID SfxID;
            int16    PlayPercentage;
        } RandomArray[] = {
            {5, 38},
            {7, 25},
            {8, 25},
            {9, 50}
        };
        const auto* const choice = &RandomArray[static_cast<int16>(CAEAudioUtility::GetRandomNumberInRange<int32>(0, 3))];
        m_EngineSounds[st].Sound = PlayVehicleSound({
            .BankSlotID      = SND_BANK_SLOT_COLLISIONS,
            .SoundID       = choice->SfxID,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .Speed         = speed,
            .Flags         = SOUND_START_PERCENTAGE | SOUND_REQUEST_UPDATES,
            .PlayTime      = choice->PlayPercentage,
        });
        return;
    }

    if (UpdateVehicleEngineSoundRawVolume(st, speed, volume)) { // 0x4F9986
        return;
    }

    const auto DoPlaySound = [&](float rollOff) {
        m_EngineSounds[st].Sound = PlayVehicleSound({
            .BankSlotID      = slot,
            .SoundID       = sfx,
            .AudioEntity   = this,
            .Pos           = m_Entity->GetPosition(),
            .Volume        = volume,
            .RollOffFactor = rollOff,
            .Speed         = speed,
            .Flags         = SOUND_REQUEST_UPDATES,
        });
    };
    switch (st) {
    case AE_SOUND_BICYCLE_TYRE:       DoPlaySound(2.5f); break; // 0x4F98EF
    case AE_SOUND_BICYCLE_SPROCKET_1: DoPlaySound(1.5f); break; // 0x4F988B
    default:                          break;
    }
}

// 0x4F60B0
float CAEVehicleAudioEntity::GetBaseVolumeForBicycleTyre(float ratio) const noexcept {
    // Como el original (0x4F60B0): interpolacion en linea sobre la tabla de 5 puntos 0x8CC18C
    static constexpr float points[5][2] = {
        { 0.00f, 0.00f },
        { 0.10f, 0.30f },
        { 0.20f, 0.45f },
        { 0.50f, 0.85f },
        { 1.0001f, 1.00f },
    };
    return ExeInlinePiecewise5(ratio, points);
}

namespace {
void ProcessBicycleAudio(CAEVehicleAudioEntity& self, CAEVehicleAudioEntity::tVehicleParams& params, bool player) {
    const auto slot = self.m_DummySlot;
    const auto bank = self.m_DummyEngineBank;
    if (player && !AEAudioHardware.IsSoundBankLoaded(bank, slot)) return;
    auto* vehicle = reinterpret_cast<uint8*>(params.Vehicle);
    if (player) {
        if (*reinterpret_cast<float*>(vehicle+0x818) > 5.f && StaticRef<float>(0xB6BAD0) * *reinterpret_cast<float*>(vehicle+0x820) < 0.f && StaticRef<float>(0xB6BACC) == 0.f) StaticRef<float>(0xB6BACC)=6.f;
        StaticRef<float>(0xB6BACC) -= StaticRef<float>(0x858CB0);
        if (StaticRef<float>(0xB6BACC)<0.f) StaticRef<float>(0xB6BACC)=0.f;
    }
    // 0x4FFE75..0x4FFED9 / 0x5000EE..0x50014F: |v / vmax| limitado a 1 con NaN -> 1.0 (el EXE no usa clamp: fcomp + test ah,5 / jp), y un resultado <= 0 se guarda como +0
    const auto ratio = VehAudioBicycleRatioCpp(params.Speed, *reinterpret_cast<float*>(reinterpret_cast<uint8*>(params.Transmission)+0x58));
    const auto wheel = float(vehicle[0x804]);
    const auto volume = (AudioLog10ScaledExe(wheel * .25f * ratio, 20.0f) + self.m_EventVolume) + StaticRef<float>(0x8CBEBC);
    const auto frequencyBase = ((wheel * StaticRef<float>(0xB6BA6C)) * .125f + StaticRef<float>(0xB6BA6C) * ratio) + StaticRef<float>(0x8CBD5C);
    // 0x4FFF39..0x4FFF66 / 0x5001AF..0x5001DC: fsin sin redondear; fchs solo si sin < 0; fmul 0x8CBD60; fadd 1.0f; fmul frequencyBase; un solo fstp
    const float angle = *reinterpret_cast<float*>(vehicle+0x648);
    const float frequency = VehAudioBicycleFrequencyCpp(angle, StaticRef<float>(0x8CBD60), frequencyBase);
    const bool pedaling = vehicle[0x834] != 0;
    const auto tyre = pedaling ? self.GetBaseVolumeForBicycleTyre(ratio) : 0.f;
    const auto sprocketVolume = (AudioLog10ScaledExe(tyre, 20.0f) + self.m_EventVolume) + StaticRef<float>(0x8CBEC0);
    const auto sprocketFrequency = StaticRef<float>(0xB6BA70) * ratio + StaticRef<float>(0x8CBD64);
    self.PlayBicycleSound(CAEVehicleAudioEntity::AE_SOUND_BICYCLE_TYRE, slot, 0, volume, frequency);
    self.PlayBicycleSound(CAEVehicleAudioEntity::AE_SOUND_BICYCLE_SPROCKET_1, slot, 1, sprocketVolume, sprocketFrequency);
    auto& wasPedaling = StaticRef<uint8>(player ? 0xB6BAC9 : 0xB6BAC8);
    if (!pedaling && wasPedaling) self.PlayBicycleSound(CAEVehicleAudioEntity::AE_SOUND_BICYCLE_CHAIN_CLANG, slot, 1, volume, 1.f);
    wasPedaling=pedaling;
    if (player) StaticRef<float>(0xB6BAD0)=*reinterpret_cast<float*>(vehicle+0x820);
}
}

// 0x4FFDC0
void CAEVehicleAudioEntity::ProcessDummyBicycle(tVehicleParams& params) {
    if (m_DummySlot == SND_BANK_SLOT_NONE) {
        m_DummySlot=RequestBankSlot(m_DummyEngineBank);
        if (m_DummySlot == SND_BANK_SLOT_NONE) return;
    }
    if (!AEAudioHardware.IsSoundBankLoaded(m_DummyEngineBank, m_DummySlot)) {
        if (DoesBankSlotContainThisBank(m_DummySlot,m_DummyEngineBank)) return;
        m_DummySlot=RequestBankSlot(m_DummyEngineBank); m_State=static_cast<eAEState>(0); return;
    }
    ProcessBicycleAudio(*this,params,false);
}

// 0x500040
void CAEVehicleAudioEntity::ProcessPlayerBicycle(tVehicleParams& params) {
    ProcessBicycleAudio(*this,params,true);
}
#pragma endregion

#pragma region RC Car
// 0x500DC0
void CAEVehicleAudioEntity::ProcessDummyRCCar(tVehicleParams& vp) {
    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }
    if (m_IsWreckedVehicle) {
        CancelAllVehicleEngineSounds();
        return;
    }
    // Como el original (0x500E8D..0x500F3B): sf = |velocidad / velocidad maxima| saturado a [0, 1] (fabs); tablas de 4 puntos con
    // CAEAudioUtility::GetPiecewiseLinear (0x4D9D90); la tabla 0x8CC0A4 (dB) es el VOLUMEN y la 0x8CBF8C la VELOCIDAD.
    const float x  = std::fabs(vp.Speed / vp.Transmission->m_MaxVelocity);
    const float sf = (1.0f < x) ? 1.0f : (x < 0.0f ? 0.0f : x);
    // Como el original (500EED/500EFE): consulta las tablas vivas de .data, no una copia congelada de sus valores iniciales.
    const float volume = CAEAudioUtility::GetPiecewiseLinear(sf, 4, reinterpret_cast<const float (*)[2]>(0x8CC0A4));
    const float speed  = CAEAudioUtility::GetPiecewiseLinear(sf, 4, reinterpret_cast<const float (*)[2]>(0x8CBF8C));
    UpdateGenericVehicleSound(
        AE_SOUND_CAR_ID,
        m_DummySlot,
        m_DummyEngineBank,
        0,
        speed,
        volume,
        1.f
    );
}

// Android
void CAEVehicleAudioEntity::ProcessPlayerRCCar(tVehicleParams& params) {
    // NOP
}
#pragma endregion

#pragma region Special Vehicle (Golf Cart, Combine, etc)
// 0x501AB0
void CAEVehicleAudioEntity::ProcessSpecialVehicle(tVehicleParams& params) {
    switch (params.Vehicle->GetModelId()) {
    case MODEL_ARTICT1:
    case MODEL_ARTICT2:
    case MODEL_PETROTR:
    case MODEL_ARTICT3: {
        ProcessVehicleRoadNoise(params);
        ProcessRainOnVehicle(params);
        break;
    }
    case MODEL_RCBANDIT:
    case MODEL_RCTIGER: {
        ProcessDummyRCCar(params);
        break;
    }
    case MODEL_CADDY: {
        ProcessVehicleRoadNoise(params);
        ProcessVehicleSkidding(params);
        ProcessVehicleFlatTyre(params);
        ProcessVehicleSirenAlarmHorn(params);
        ProcessDummyGolfCart(params);
        ProcessRainOnVehicle(params);
        if (params.Vehicle->GetStatus() != STATUS_SIMPLE) {
            ProcessEngineDamage(params);
        }
        break;
    }
    case MODEL_RCBARON: {
        ProcessDummyRCPlane(params);
        break;
    }
    case MODEL_RCRAIDER:
    case MODEL_RCGOBLIN: {
        ProcessDummyRCHeli(params);
        break;
    }
    case MODEL_VORTEX: {
        ProcessDummyHovercraft(params);
        ProcessRainOnVehicle(params);
        ProcessVehicleSirenAlarmHorn(params);
        break;
    }
    }
}

// 0x500CE0
void CAEVehicleAudioEntity::ProcessPlayerCombine(tVehicleParams& vp) {
    const auto* const cfg     = &s_Config.Combine;
    const auto* const combine = vp.Vehicle;
    if (!AEAudioHardware.IsSoundBankLoaded(SND_BANK_GENRL_HARVESTER_P, SND_BANK_SLOT_PLAYER_ENGINE_P)) {
        return;
    }
    if (combine->vehicleFlags.bEngineOn) {
        UpdateGenericVehicleSound(
            AE_SOUND_MOVING_PARTS,
            SND_BANK_SLOT_PLAYER_ENGINE_P,
            SND_BANK_GENRL_HARVESTER_P,
            SND_GENRL_HARVESTER_P_ZZHARVEST_LOOP,
            1.f,
            AudioLog10ScaledExe(std::clamp(vp.Speed / cfg->VolSpeedFactor, 0.f, 1.f), 20.0f) + cfg->VolBase,
            2.f
        );
    } else {
        CancelVehicleEngineSound(AE_SOUND_MOVING_PARTS);
    }
}

// 0x501270
void CAEVehicleAudioEntity::ProcessDummyGolfCart(tVehicleParams& vp) {
    auto* const cfg = &s_Config.DummyEngine.GolfCart;

    if (!EnsureHasDummySlot()) {
        return;
    }
    if (!EnsureSoundBankIsLoaded(true)) {
        return;
    }
    if (!vp.Vehicle->vehicleFlags.bEngineOn || m_IsWreckedVehicle) {
        CancelAllVehicleEngineSounds();
        return;
    }
    // Como el original (0x50131B..0x501461): t = |velocidad / maxima| saturado a [0, 1] (0 sin transmision o si la entidad de
    // audio esta abandonada: estado leido de m_Entity); se reproduce si NO (t < 0.05) con el DOBLE 0x859068.
    float t = 0.f;
    if (vp.Transmission) {
        const float x = std::fabs(vp.Speed / vp.Transmission->m_MaxVelocity);
        t = (1.0f < x) ? 1.0f : (x < 0.0f ? 0.0f : x);
    }
    if (m_Entity->GetStatus() == STATUS_ABANDONED) {
        t = 0.f;
    }
    if (!(static_cast<double>(t) < 0.05)) {
        static constexpr float volumePoints[4][2] = { // 0x8CBFD8 (el ultimo x es 0x3F800347 = 1.0001)
            { 0.00f, 0.00f },
            { 0.05f, 0.00f },
            { 0.10f, 0.50f },
            { std::bit_cast<float>(0x3F800347u), 1.00f },
        };
        const float volume = CAEAudioUtility::GetPiecewiseLinearT(t, volumePoints);
        UpdateGenericVehicleSound(
            AE_DUMMY_ID,
            m_DummySlot,
            m_DummyEngineBank,
            0,
            ExeLerp(cfg->FrqMin, cfg->FrqMax, t), // 0xB6BA9C * t + 0x8CBFD0
            AudioLog10ScaledExe(volume, 20.0f) + cfg->VolOffset,
            2.f
        );
    } else {
        StopGenericEngineSound(AE_DUMMY_ID);
    }
}
#pragma endregion

// 0x501480
void CAEVehicleAudioEntity::ProcessDummyVehicleEngine(tVehicleParams& vp) {
    if (!EnsureHasDummySlot() || !EnsureSoundBankIsLoaded(true)) {
        return;
    }
    if (vp.Vehicle->vehicleFlags.bEngineOn) {
        const auto ratio = m_Entity->GetStatus() != STATUS_ABANDONED && vp.Transmission // 0x501578 mov edx,[esi+4] / mov al,[edx+36h]: estado de m_Entity
            ? std::clamp(std::abs(vp.Speed / vp.Transmission->m_MaxVelocity), 0.f, 1.f)
            : 0.f;
        bool isIdling;
        switch (m_State) {
        case eAEState::CAR_OFF:
        case eAEState::DUMMY_ID:
        case eAEState::NUM_STATES: isIdling = ratio < s_Config.DummyEngine.ID.Ratio;          break;
        // case eAEState::NUM_STATES: NOTSA_UNREACHABLE(); // This one was originally handled as part of the above, but it doesn't make any sense.... - TODO: Commenting this out for now as the game work as expected, but we really should check this out sometime - Pirulax
        case eAEState::DUMMY_CRZ:  isIdling = ratio < s_Config.DummyEngine.Rev.Ratio;         break;
        default:                   ProcessDummyStateTransition(eAEState::CAR_OFF, ratio, vp); return;
        }
        ProcessDummyStateTransition(isIdling ? eAEState::DUMMY_ID : eAEState::DUMMY_CRZ, ratio, vp);
    } else {
        ProcessDummyStateTransition(eAEState::CAR_OFF, 0.f, vp);
    }
}

bool CAEVehicleAudioEntity::EnsureHasDummySlot() noexcept {
    if (m_DummySlot == -1) {
        m_DummySlot = RequestBankSlot(m_DummyEngineBank);
    }
    return m_DummySlot != -1;
}

// notsa
bool CAEVehicleAudioEntity::EnsureSoundBankIsLoaded(bool isDummy, bool turnOffIfNotLoaded) {
    const auto bank = isDummy ? m_DummyEngineBank : m_PlayerEngineBank;
    const auto slot = isDummy ? m_DummySlot.get() : SND_BANK_SLOT_PLAYER_ENGINE_P;
    if (AEAudioHardware.IsSoundBankLoaded(bank, slot)) {
        return true;
    }
    if (!DoesBankSlotContainThisBank(slot, bank)) {
        m_DummySlot = RequestBankSlot(bank);
        if (turnOffIfNotLoaded) {
            m_State = eAEState::CAR_OFF;
        }
    }
    return false;
}

// notsa
void CAEVehicleAudioEntity::StopNonEngineSounds() noexcept {
    // Como el original (0x4FD13F): el tipo de sonido solo se restablece cuando habia un sonido
    PlaySkidSound(-1, 1.0f, -100.0f);

    if (m_SurfaceSound) {
        GracefullyStopSound(m_SurfaceSound);
        m_SurfaceSoundType = -1;
    }
    if (m_RoadNoiseSound) {
        GracefullyStopSound(m_RoadNoiseSound);
        m_RoadNoiseSoundType = -1;
    }
    if (m_FlatTireSound) {
        GracefullyStopSound(m_FlatTireSound);
        m_FlatTireSoundType = -1;
    }
    if (m_ReverseGearSound) {
        GracefullyStopSound(m_ReverseGearSound);
        m_ReverseGearSoundType = -1;
    }

    // Como el original (0x4FD1FC..0x4FD260): bocina y sirenas solo se detienen con SetFlags(4, 0) + StopSound (0x4EF2B0 / 0x4EF1C0),
    // sin tocar m_AudioEntity (no usa StopSoundAndForget 0x4EF850)
    GracefullyStopSound(m_HornSound);
    GracefullyStopSound(m_SirenSound);
    GracefullyStopSound(m_FastSirenSound);
}

// 0x501E10
void CAEVehicleAudioEntity::ProcessVehicle(CPhysical* physical) {
    auto* const vehicle        = physical->AsVehicle();
    const auto  isStatusSimple = vehicle->GetStatus() == STATUS_SIMPLE;

    tVehicleParams vp{};
    vp.Vehicle               = vehicle;
    vp.ModelIndexMinusOffset = physical->GetModelIndex() - MODEL_VEHICLE_FIRST;
    vp.BaseVehicleType       = vehicle->m_nVehicleType;
    vp.SpecificVehicleType   = vehicle->m_nVehicleSubType;
    vp.Transmission          = vehicle->m_pHandlingData
        ? &vehicle->m_pHandlingData->m_transmissionData
        : nullptr;
    // Como el original (0x501EA0..0x501EC1): modo simple = m_speed * 0.02f (fmul [858B38]); si no, (z*fz + y*fy) + x*fx en ese orden
    if (isStatusSimple) {
        vp.Speed = vehicle->m_autoPilot.m_speed * std::bit_cast<float>(0x3CA3D70Au);
    } else {
        const CVector& fwd = physical->m_matrix->GetForward();
        const float    pz  = physical->m_vecMoveSpeed.z * fwd.z;
        const float    py  = physical->m_vecMoveSpeed.y * fwd.y;
        const float    zy  = pz + py;
        const float    px  = physical->m_vecMoveSpeed.x * fwd.x;
        vp.Speed           = zy + px;
    }

    switch (m_AuSettings.VehicleAudioType) {
    case AE_CAR: {
        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);

        if (m_IsWreckedVehicle) {
            break;
        }

        ProcessVehicleRoadNoise(vp);

        if (m_IsPlayerDriver) {
            ProcessReverseGear(vp);
        }

        if (!isStatusSimple) {
            ProcessVehicleSkidding(vp);
            ProcessVehicleFlatTyre(vp);
        }

        ProcessVehicleSirenAlarmHorn(vp);

        if (m_IsPlayerDriver) {
            ProcessPlayerVehicleEngine(vp);
        } else {
            ProcessDummyVehicleEngine(vp);
        }

        if (!isStatusSimple) {
            ProcessEngineDamage(vp);
        }

        ProcessRainOnVehicle(vp);

        if (vp.Vehicle->IsAutomobile()) {
            ProcessNitro(vp);
        }

        ProcessMovingParts(vp);

        if (vp.Vehicle->GetModelIndex() == MODEL_COMBINE) {
            ProcessPlayerCombine(vp);
        }

        vehicle->AsAutomobile()->m_PrevSpeed = vp.Speed;
        break;
    }
    case AE_BIKE: {
        if (m_IsWreckedVehicle) {
            break;
        }

        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessVehicleRoadNoise(vp);

        if (!isStatusSimple) {
            ProcessVehicleSkidding(vp);
        }

        ProcessVehicleSirenAlarmHorn(vp);

        if (m_IsPlayerDriver) {
            ProcessPlayerVehicleEngine(vp);
        } else {
            ProcessDummyVehicleEngine(vp);
        }

        if (!isStatusSimple) {
            ProcessEngineDamage(vp);
            ProcessVehicleFlatTyre(vp);
        }

        ProcessRainOnVehicle(vp);

        vehicle->AsBike()->m_PrevSpeed = vp.Speed;
        break;
    }
    case AE_BMX: {
        if (m_IsWreckedVehicle) {
            break;
        }

        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessVehicleSkidding(vp);
        ProcessVehicleSirenAlarmHorn(vp);

        if (m_IsPlayerDriver) {
            ProcessPlayerBicycle(vp);
        } else {
            ProcessDummyBicycle(vp);
        }

        vehicle->AsBike()->m_PrevSpeed = vp.Speed;
        break;
    }
    case AE_BOAT: {
        ProcessBoatEngine(vp);
        ProcessRainOnVehicle(vp);
        ProcessBoatMovingOverWater(vp);
        break;
    }
    case AE_AIRCRAFT_HELICOPTER: {
        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessAircraft(vp);
        ProcessRainOnVehicle(vp);
        break;
    }
    case AE_AIRCRAFT_PLANE: {
        UpdateGasPedalAudio(vehicle, vehicle->m_nVehicleType);
        ProcessAircraft(vp);
        ProcessRainOnVehicle(vp);
        ProcessMovingParts(vp);
        break;
    }
    case AE_AIRCRAFT_SEAPLANE: {
        ProcessAircraft(vp);
        ProcessRainOnVehicle(vp);
        ProcessBoatMovingOverWater(vp);
        break;
    }
    case AE_ONE_GEAR:
        break;
    case AE_TRAIN: {
        ProcessTrainTrackSound(vp);
        ProcessDummyTrainEngine(vp);
        if (m_IsPlayerDriver) {
            ProcessPlayerTrainBrakes(vp);
        }
        break;
    }
    case AE_SPECIAL: {
        ProcessSpecialVehicle(vp);
        break;
    }
    case AE_NO_VEHICLE:
        break;
    default: // Como el original (0x501ED1 cmp ecx,9 / ja 0x502240): cualquier otro valor (incluido negativo) sale sin hacer nada
        break;
    }
}

// Envolturas __fastcall de los cuatro colaboradores cuyos llamadores del EXE reutilizan un registro tras la llamada. Reciben ECX/EDX de entrada como
// parametros, llaman al metodo C++ y terminan en PlayerPedResidueFloatCpp (X87Math: `fld dword ptr [esp+4]; ret 4`, no toca ECX/EDX/EAX), de modo que
// el EXE ve los mismos ECX/EDX (y el resultado en ST0) sin ensamblador. Sin comprobaciones de ejecucion (RTC) ni cookie /GS para que no cambien
// ECX/EDX tras la ultima llamada (contrib/audit_register_residue.py vigila el objeto AEVehicleAudioEntity).
#pragma runtime_checks("", off)

// Como el original (0x4F5C60): solo escribe EAX, ECX y ST0 y deja EDX intacto; su llamador del EXE (0x4F8205, dentro de GetFreqForPlayerEngineSound)
// sigue usando EDX tras la llamada (0x4F8210 mov edx,[edx+10h]). ECX sale con el indice del bucle (xor ecx,ecx; primer i de 0..4 con
// tabla[i] > ratio, o 5; NaN no cumple la comparacion). ret 4.
static float __declspec(safebuffers) __fastcall G2GetFreqForIdle_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, float ratio) {
    const float result = self->GetFreqForIdle(ratio);
    static constexpr float xs[5] = { 0.000f, 0.075f, 0.150f, 0.250f, 1.0001f }; // 0x8CC164 + 8 * i
    uint32 index = 0;
    while (index < 5 && !(xs[index] > ratio)) {
        ++index;
    }
    return PlayerPedResidueFloatCpp(index, entryEdx, result);
}

// Como el original (0x4F5F30 / 0x4F6000): solo escriben EAX y ST0; ECX (this) y EDX quedan intactos y el unico llamador del EXE (0x4F905F push edx)
// reutiliza EDX. ret 0x14 / ret 0x10.
static float __declspec(safebuffers) __fastcall G3DriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, float revs, cTransmission* tr, float speed) {
    const float result = self->GetVehicleDriveWheelSkidValue(veh, ws, revs, *tr, speed);
    return PlayerPedResidueFloatCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(self)), entryEdx, result);
}
static float __declspec(safebuffers) __fastcall G3NonDriveSkid_KeepEdx(const CAEVehicleAudioEntity* self, uint32 entryEdx, CVehicle* veh, tWheelState ws, cTransmission* tr, float speed) {
    const float result = self->GetVehicleNonDriveWheelSkidValue(veh, ws, *tr, speed);
    return PlayerPedResidueFloatCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(self)), entryEdx, result);
}

// Como el original (4F5D00..4F5E2F): conserva ECX, que 4FC9CD reutiliza despues de 4FC9C1, y sale con EDX = [[this+4]+0x4C8] (0x4F5DD6 mov edx,[eax+4C8h],
// comun a todas las ramas). ret 8.
static float __declspec(safebuffers) __fastcall VehAudioPlayerVolume_KeepEcx(const CAEVehicleAudioEntity* self, uint32 /*entryEdx*/, CAEVehicleAudioEntity::tVehicleParams* vp, CAEVehicleAudioEntity::eVehicleEngineSoundType st) {
    const float result = self->GetVolForPlayerEngineSound(*vp, st);
    const uint32 exitEdx = *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(self->m_Entity) + 0x4C8);
    return PlayerPedResidueFloatCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(self)), exitEdx, result);
}

#pragma runtime_checks("", restore)

void CAEVehicleAudioEntity::InjectHooks() {
    RH_ScopedVirtualClass(CAEVehicleAudioEntity, 0x862CEC, 1);
    RH_ScopedCategory("Audio/Entities");

    RH_ScopedInstall(Constructor, 0x4F63E0);
    RH_ScopedInstall(Destructor, 0x6D0A10);

    // Static
    RH_ScopedInstall(StaticGetPlayerVehicleAudioSettingsForRadio, 0x4F4ED0);
    RH_ScopedInstall(StaticService, 0x4F4EC0);
    RH_ScopedInstall(StaticInitialise, 0x5B99F0);

    RH_ScopedVMTInstall(UpdateParameters, 0x4FB6C0);
    RH_ScopedInstall(Initialise, 0x4F7670);
    RH_ScopedInstall(Terminate, 0x4FB8C0);

    RH_ScopedInstall(GetFlyingMetalVolume, 0x4F6150);
    RH_ScopedInstall(GetVehicleAudioSettings, 0x4F5C10);

    // Heli
    RH_ScopedInstall(EnableHelicoptors, 0x4F4EF0);
    RH_ScopedInstall(DisableHelicoptors, 0x4F4EE0);
    RH_ScopedInstall(EnableHelicoptor, 0x4F5C00);
    RH_ScopedInstall(DisableHelicoptor, 0x4F5BF0);
    RH_ScopedInstall(CopHeli, 0x4F5C40);

    RH_ScopedInstall(GetVehicleTypeForAudio, 0x4F4F00);

    RH_ScopedInstall(GetVolumeForDummyIdle, 0x4F51F0);
    RH_ScopedInstall(GetFrequencyForDummyIdle, 0x4F5310);

    RH_ScopedInstall(GetVolumeForDummyRev, 0x4F53D0);
    RH_ScopedInstall(GetFrequencyForDummyRev, 0x4F54F0);

    RH_ScopedInstall(JustGotInVehicleAsDriver, 0x4F5700);
    RH_ScopedInstall(TurnOnRadioForVehicle, 0x4F5B20);
    RH_ScopedInstall(TurnOffRadioForVehicle, 0x4F5B60);
    RH_ScopedInstall(PlayerAboutToExitVehicleAsDriver, 0x4F5BA0);
    RH_ScopedNamedGlobalInstall(G2GetFreqForIdle_KeepEdx, "GetFreqForIdle", 0x4F5C60);
    RH_ScopedInstall(GetAccelAndBrake, 0x4F5080);
    RH_ScopedInstall(UpdateGasPedalAudio, 0x4F5EB0);

    RH_ScopedInstall(JustGotOutOfVehicleAsDriver, 0x4FCF40);
    RH_ScopedInstall(JustWreckedVehicle, 0x4FD0B0);

    // Bank slots
    RH_ScopedInstall(RequestBankSlot, 0x4F4D10);
    RH_ScopedInstall(DemandBankSlot, 0x4F4E60);
    RH_ScopedInstall(DoesBankSlotContainThisBank, 0x4F4E30);
    RH_ScopedInstall(StoppedUsingBankSlot, 0x4F4DF0);

    // Acceleration
    RH_ScopedInstall(JustFinishedAccelerationLoop, 0x4F5E50);
    RH_ScopedInstall(IsAccInhibited, 0x4F4F70);
    RH_ScopedInstall(IsAccInhibitedBackwards, 0x4F4FC0);
    // Como el original: 0x4FC29B y 0x4FC2A9 lo llaman dos veces sin recargar ECX; 0x4F4FF0 conserva this y hace ret 4.
    RH_ScopedInstall(IsAccInhibitedForLowSpeed, 0x4F4FF0, { .stackArguments = 1 });
    RH_ScopedInstall(IsAccInhibitedForTime, 0x4F5020);
    RH_ScopedInstall(InhibitAccForTime, 0x4F5030);

    // Crz
    RH_ScopedInstall(InhibitCrzForTime, 0x4F5060);
    RH_ScopedInstall(IsCrzInhibitedForTime, 0x4F5050);

    // Skidding
    RH_ScopedNamedGlobalInstall(G3DriveSkid_KeepEdx, "GetVehicleDriveWheelSkidValue", 0x4F5F30);
    RH_ScopedNamedGlobalInstall(G3NonDriveSkid_KeepEdx, "GetVehicleNonDriveWheelSkidValue", 0x4F6000);
    RH_ScopedInstall(PlaySkidSound, 0x4F8360);
    RH_ScopedInstall(ProcessVehicleSkidding, 0x4F8F10);

    // Horn
    RH_ScopedOverloadedInstall(GetHornState, "", 0x4F61E0, void(CAEVehicleAudioEntity::*)(bool*, tVehicleParams&) const); // Done
    RH_ScopedInstall(GetSirenState, 0x4F62A0);
    RH_ScopedInstall(PlayHornOrSiren, 0x4F99D0);
    RH_ScopedInstall(ProcessVehicleSirenAlarmHorn, 0x5002C0);

    RH_ScopedInstall(UpdateGenericVehicleSound, 0x4FAD40);

    // Engine Sound
    RH_ScopedInstall(StartVehicleEngineSound, 0x4F7F20);
    RH_ScopedInstall(CancelVehicleEngineSound, 0x4F55C0);
    RH_ScopedInstall(RequestNewPlayerCarEngineSound, 0x4F7A50);
    RH_ScopedInstall(GetFreqForPlayerEngineSound, 0x4F8070);
    RH_ScopedNamedGlobalInstall(VehAudioPlayerVolume_KeepEcx, "GetVolForPlayerEngineSound", 0x4F5D00);
    //RH_ScopedInstall(UpdateVehicleEngineSound, 0x4F56D0); // Don't hook because original function doesn't return a bool, ours does
    RH_ScopedInstall(StopGenericEngineSound, 0x4F6320);

    // Car + bike
    RH_ScopedInstall(PlayFlatTyreSound, 0x4F8650);
    RH_ScopedInstall(PlayReverseSound, 0x4F87D0);
    RH_ScopedInstall(PlayRoadNoiseSound, 0x4F84D0);

    // Boat
    RH_ScopedInstall(ProcessBoatMovingOverWater, 0x4FA0C0);
    RH_ScopedInstall(ProcessBoatEngine, 0x5003A0);
    RH_ScopedInstall(UpdateBoatSound, 0x4F9E90);

    // Train
    RH_ScopedInstall(UpdateTrainSound, 0x4FA1C0);
    RH_ScopedInstall(ProcessTrainTrackSound, 0x4FA3F0);
    RH_ScopedInstall(ProcessDummyTrainEngine, 0x500710);
    RH_ScopedInstall(ProcessPlayerTrainBrakes, 0x500AB0);
    RH_ScopedInstall(PlayTrainBrakeSound, 0x4FA630);

    // Aircrafts
    RH_ScopedInstall(PlayAircraftSound, 0x4F93C0);
    RH_ScopedInstall(GetAircraftNearPosition, 0x4F96A0);
    RH_ScopedInstall(ProcessAircraft, 0x501C50);

    // Other..
    RH_ScopedInstall(ProcessVehicleFlatTyre, 0x4F8940);
    RH_ScopedInstall(ProcessVehicleRoadNoise, 0x4F8B00);
    RH_ScopedInstall(ProcessReverseGear, 0x4F8DF0);
    RH_ScopedInstall(ProcessRainOnVehicle, 0x4F92C0);
    RH_ScopedInstall(ProcessDummyRCPlane, 0x4FA7C0);
    RH_ScopedInstall(ProcessDummyRCHeli, 0x4FAA80);
    RH_ScopedInstall(ProcessEngineDamage, 0x4FAE20);
    RH_ScopedInstall(ProcessNitro, 0x4FB070); 
    RH_ScopedInstall(ProcessMovingParts, 0x4FB260);
    RH_ScopedInstall(ProcessDummyStateTransition, 0x4FCA10);

    RH_ScopedInstall(ProcessPlayerProp, 0x4FD290);
    RH_ScopedInstall(ProcessDummyProp, 0x4FD8F0);
    RH_ScopedInstall(ProcessAIProp, 0x4FDFD0);

    RH_ScopedInstall(ProcessPlayerHeli, 0x4FE420);
    RH_ScopedInstall(ProcessAIHeli, 0x4FEE20);
    RH_ScopedInstall(ProcessDummyHeli, 0x4FE940);

    RH_ScopedInstall(ProcessPlayerSeaPlane, 0x4FF320);
    RH_ScopedInstall(ProcessDummySeaPlane, 0x4FF7C0);

    // Jet
    RH_ScopedInstall(ProcessPlayerJet, 0x501650);
    RH_ScopedInstall(ProcessDummyJet, 0x501960);
    RH_ScopedInstall(ProcessGenericJet, 0x4FF900);

    // Bicycle
    RH_ScopedInstall(PlayBicycleSound, 0x4F9710);
    RH_ScopedInstall(GetBaseVolumeForBicycleTyre, 0x4F60B0);
    RH_ScopedInstall(ProcessDummyBicycle, 0x4FFDC0);
    RH_ScopedInstall(ProcessPlayerBicycle, 0x500040);

    RH_ScopedInstall(ProcessPlayerCombine, 0x500CE0);
    RH_ScopedInstall(ProcessDummyRCCar, 0x500DC0);
    RH_ScopedInstall(ProcessDummyHovercraft, 0x500F50);
    RH_ScopedInstall(ProcessDummyGolfCart, 0x501270);
    RH_ScopedInstall(ProcessDummyVehicleEngine, 0x501480);
    RH_ScopedInstall(ProcessSpecialVehicle, 0x501AB0);

    // Conmutacion individual y combinada probada en reversing/HookUnlockAudio, con audio y vehiculos reales.
    RH_ScopedInstall(ProcessPlayerVehicleEngine, 0x4FBB10);
    RH_ScopedInstall(ProcessVehicle, 0x501E10);
    RH_ScopedInstall(Service, 0x502280);

    RH_ScopedOverloadedInstall(AddAudioEvent, "0", 0x4F6420, void(CAEVehicleAudioEntity::*)(eAudioEvents, float));
    RH_ScopedOverloadedInstall(AddAudioEvent, "1", 0x4F7580, void(CAEVehicleAudioEntity::*)(eAudioEvents, CPhysical*));
}
