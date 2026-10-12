/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEAudioUtility
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <chrono>

#include "AEAudioUtility.h"
#include "X87Math.h"

auto& Frequency = StaticRef<LARGE_INTEGER>(0xB610F0);
auto& startTimeMs = StaticRef<uint64>(0xb610f8);

// NOTE: For me all values were 0... The below values should be the correct ones:
static constexpr std::array<eSoundBank, AE_SCRIPT_BANK_LAST - AE_SCRIPT_BANK_FIRST> gScriptBanksLookup = {
    SND_BANK_SCRIPT_VIDEO_POKER,
    SND_BANK_SCRIPT_SHOOTING_RANGE,
    SND_BANK_SCRIPT_POOL_MINIGAME,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_KEYPAD,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_GOGO,
    SND_BANK_SCRIPT_DUALITY,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_BLACK_PROJECT,
    SND_BANK_SCRIPT_BEE,
    SND_BANK_SCRIPT_BASKETBALL,
    SND_BANK_SCRIPT_MEAT_BUSINESS,
    SND_BANK_SCRIPT_ROULETTE,
    SND_BANK_SCRIPT_BANDIT,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_NULL,
    SND_BANK_SCRIPT_OGLOC,
    SND_BANK_SCRIPT_CARGO_PLANE,
    SND_BANK_SCRIPT_DA_NANG,
    SND_BANK_SCRIPT_GYM,
    SND_BANK_SCRIPT_OTB,
    SND_BANK_SCRIPT_STINGER,
    SND_BANK_SCRIPT_UNCLE_SAM,
    SND_BANK_SCRIPT_VERTICAL_BIRD,
    SND_BANK_SCRIPT_MECHANIC,
    SND_BANK_SCRIPT_CAT2_BANK,
    SND_BANK_SCRIPT_AIR_HORN,
    SND_BANK_SCRIPT_RESTAURANT,
    SND_BANK_SCRIPT_TEMPEST
};

/**
 * Registra helpers matemáticos, temporales y de traducción de eventos de audio.
 */
void CAEAudioUtility::InjectHooks() {
    RH_ScopedClass(CAEAudioUtility);
    RH_ScopedCategory("Audio");

    RH_ScopedOverloadedInstall(GetRandomNumberInRange<int32>, "int", 0x4d9c10, int32(*)(int32, int32));
    RH_ScopedOverloadedInstall(GetRandomNumberInRange<float>, "float", 0x4d9c50, float(*)(float, float));
    RH_ScopedInstall(ResolveProbability, 0x4d9c80);
    RH_ScopedInstall(GetPiecewiseLinear, 0x4d9d90);
    RH_ScopedInstall(AudioLog10, 0x4d9e50);
    RH_ScopedInstall(ConvertFromBytesToMS, 0x4d9ef0);
    RH_ScopedInstall(ConvertFromMSToBytes, 0x4d9f40);
    RH_ScopedInstall(GetBankAndSoundFromScriptSlotAudioEvent, 0x4D9CC0);
    RH_ScopedInstall(FindVehicleOfPlayer, 0x4D9E10);

    RH_ScopedInstall(GetCurrentTimeInMS, 0x4d9e80);
    RH_ScopedInstall(StaticInitialise, 0x5b97f0);
}

// 0x4d9c80
// Como el original: cierto si p >= 1 (o NaN); si no, rand · (1/32767) < p.
bool CAEAudioUtility::ResolveProbability(float p) {
    if (!(p < 1.0f)) {
        return true;
    }
    return (float)CGeneral::GetRandomNumber() * (1.0f / 32767.0f) < p;
}

// 0x4d9d90
/**
 * Evalúa una curva definida por pares X/Y, saturando extremos e interpolando
 * linealmente entre los dos puntos que encierran x.
 */
float CAEAudioUtility::GetPiecewiseLinear(float x, int16 dataCount, const float (*data)[2]) {
    assert(dataCount >= 1);
    if (x > data[dataCount - 1][0]) { // El original solo satura por arriba si x es estrictamente mayor
        return data[dataCount - 1][1];
    }
    if (x <= data[0][0]) {
        return data[0][1];
    }

    assert(dataCount >= 2);
    int32 i = 0;
    for (; i < dataCount; i++) {
        if (data[i][0] >= x)
            break;
    }
    // Perform linear interpolation
    float t = (x - data[i - 1][0]) / (data[i][0] - data[i - 1][0]);
    return t * (data[i][1] - data[i - 1][1]) + data[i - 1][1];
}

// 0x4d9e50
// Como el original: log10 con fldlg2/fyl2x (precisión interna del FPU) y un solo redondeo a float (X87Math.cpp).
float CAEAudioUtility::AudioLog10(float p) {
    if (p < 0.00001f) {
        return -5.0f;
    }
    return AudioLog10Cpp(p);
}

// 0x4d9e80
// Como el original: (contador / frecuencia) · 1000 en x87 (con la precisión del juego) menos el inicio.
uint64 CAEAudioUtility::GetCurrentTimeInMS() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    const LARGE_INTEGER* freq = &Frequency;
    const uint64 start = startTimeMs;
    const uint32 startHigh = (uint32)(start >> 32);
    int32 startParts[2] = { (int32)(uint32)start, (int32)(startHigh & 0x7FFFFFFF) };
    int32 signParts[2]  = { 0, (int32)(startHigh & 0x80000000) };
    static const float kThousand = 1000.0f;
    double value;
    __asm {
        fild  qword ptr counter
        mov   eax, freq
        fild  qword ptr [eax]
        fdivp st(1), st
        fmul  kThousand
        fild  qword ptr startParts
        fild  qword ptr signParts
        fchs
        faddp st(1), st
        fsubp st(1), st
        fstp  value
    }
    return (uint64)(int64)value; // _ftol2 (0x821B40): truncamiento a 64 bits
}

// 0x4d9ef0
// Como el original: bytes enteros sin redondear entre (canales · frecuencia · 0.002), floor en double.
uint32 CAEAudioUtility::ConvertFromBytesToMS(uint32 lengthInBytes, uint32 sampleRate, uint16 numChannels) {
    const uint32 product = (uint32)numChannels * sampleRate;
    static const float k2pow32 = 4294967296.0f, kScale = 0.002f;
    double quotient;
    __asm {
        fild  dword ptr lengthInBytes
        cmp   lengthInBytes, 0
        jge   L_bytesPositive
        fadd  k2pow32
    L_bytesPositive:
        fild  dword ptr product
        cmp   product, 0
        jge   L_productPositive
        fadd  k2pow32
    L_productPositive:
        fmul  kScale
        fdivp st(1), st
        fstp  quotient
    }
    return (uint32)(int64)std::floor(quotient);
}

// 0x4d9f40
uint32 CAEAudioUtility::ConvertFromMSToBytes(uint32 a, uint32 frequency, uint16 frequencyMult) {
    const uint32 product = (uint32)frequencyMult * frequency;
    static const float k2pow32 = 4294967296.0f, kScale = 0.002f;
    double bytes;
    __asm {
        fild  dword ptr product
        cmp   product, 0
        jge   L_productPositive
        fadd  k2pow32
    L_productPositive:
        fmul  kScale
        fild  dword ptr a
        cmp   a, 0
        jge   L_aPositive
        fadd  k2pow32
    L_aPositive:
        fmulp st(1), st
        fstp  bytes
    }
    const auto value = (int32)(int64)std::floor(bytes);
    return (uint32)(value % (int32)(2 * frequencyMult) + value); // división con signo como el original
}

// 0x5B97F0
// Como el original: 10 / pow(1.2, n) en x87 y su log10 escrito siempre en [1][1] (error del original).
void CAEAudioUtility::StaticInitialise() {
    constexpr size_t LOOKUP_SIZE = sizeof(m_sfLogLookup) / sizeof(float) / 2;
    m_sfLogLookup[0][0] = 0.0f;
    m_sfLogLookup[0][1] = -100.0f;
    m_sfLogLookup[1][0] = 0.00001f;
    static const float kTen = 10.0f;
    for (size_t i = 2; i < LOOKUP_SIZE; i++) {
        const double denom = std::pow((double)1.2f, (double)(LOOKUP_SIZE - i - 1)); // base 0x86A438 = (double)1.2f
        float v, logv;
        __asm {
            fld   denom
            fdivr kTen
            fst   v
            fldlg2
            fxch  st(1)
            fyl2x
            fstp  logv
        }
        m_sfLogLookup[i][0] = v;
        m_sfLogLookup[1][1] = logv;
    }
    VERIFY(QueryPerformanceFrequency(&Frequency));
    startTimeMs = 0;
    startTimeMs = GetCurrentTimeInMS();
}

// 0x4D9CC0
/**
 * Traduce un eAudioEvents de script al banco y sound ID que consume el hardware.
 * Los slots personalizados utilizan reglas separadas del esquema de 200 sonidos por banco.
 */
bool CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(const eAudioEvents& scriptID, eSoundBankS32& outBankID, int32& outSoundID, int32 slot) {
    if (scriptID < AE_SCRIPT_BANK_FIRST) {
        return false;
    }
    if (scriptID < AE_SCRIPT_SLOT_FIRST) {
        // Tabla de bancos del propio EXE (0x8ABC70 indexada por el evento)
        outBankID  = (eSoundBank)StaticRef<int32[0x7D0]>(0x8ABC70)[scriptID];
        outSoundID = -1;
    } else if (scriptID == AE_SCRIPT_SLOT_USE_CUSTOM) {
        outBankID  = SND_BANK_SCRIPT_NULL;
        outSoundID = (slot >= 0 && slot < 4) ? 2 * (slot % 2) : 0;
    } else {
        // floor((evento - 2000) · 0.005) en float (como el original), no división entera
        const auto n = (int32)scriptID - AE_SCRIPT_SLOT_FIRST;
        outBankID  = (eSoundBank)((int32)std::floor((double)((float)n * 0.005f)) + SND_BANK_SCRIPT_FIRST);
        outSoundID = n % 200;
    }
    return true;
}

// 0x4D9E10
// Devuelve el vehículo conducido por el jugador o, como fallback, el vehículo al que está adherido.
CVehicle* CAEAudioUtility::FindVehicleOfPlayer() {
    auto* vehicle = FindPlayerVehicle(CWorld::PlayerInFocus, true);
    auto* player  = FindPlayerPed();
    if (!vehicle && player && player->m_pAttachedTo && player->m_pAttachedTo->GetIsTypeVehicle()) {
        return player->m_pAttachedTo->AsVehicle();
    }
    return vehicle;
}
