#pragma once
#include "eRadioID.h"
#include <General.h>

class CVehicle;

/**
 * Colección de helpers compartidos por los subsistemas de audio.
 *
 * Incluye RNG con semántica específica, interpolación de curvas, conversiones entre
 * bytes/tiempo, resolución de bancos de script y reloj monotónico relativo.
 */
class CAEAudioUtility {
public:
    static void      StaticInitialise();

    /*!
    * @brief Variante entera que conserva la semántica inclusiva esperada por audio.
    * Internamente adapta el límite superior a la API de CGeneral.
    */
    template<std::integral T>
    static T GetRandomNumberInRange(T min, T max) {
        // 0x4D9C10: min + (int)(rand() / 32768.0f * (max - min + 1)), i.e. [min, max]
        return static_cast<T>(CGeneral::GetRandomIntInRange(static_cast<int32>(min), static_cast<int32>(max) + 1));
    }

    template<typename T>
        requires std::is_floating_point_v<T>
    static T GetRandomNumberInRange(T min, T max) {
        // 0x4D9C50: rand · (1/32767) · (max - min) + min
        return (T)((float)CGeneral::GetRandomNumber() * (1.0f / 32767.0f) * (float)(max - min) + (float)min);
    }

    static CVehicle* FindVehicleOfPlayer();
    static bool      ResolveProbability(float prob);
    static float     AudioLog10(float p);

    static uint32    ConvertFromBytesToMS(uint32 lengthInBytes, uint32 sampleRate, uint16 numChannels);
    static uint32    ConvertFromMSToBytes(uint32 a, uint32 frequency, uint16 frequencyMult);

    static bool      GetBankAndSoundFromScriptSlotAudioEvent(const eAudioEvents& scriptID, eSoundBankS32& outBankID, int32& outSoundID, int32 slot);
    static float     GetPiecewiseLinear(float x, int16 dataCount, const float (*data)[2]);
    template<int16 DataCnt>
    static float     GetPiecewiseLinearT(float x, const float (&points)[DataCnt][2]) { return GetPiecewiseLinear(x, DataCnt, points); }
    static uint64    GetCurrentTimeInMS();

    // NOTSA
    static eRadioID GetRandomRadioStation() {
        return static_cast<eRadioID>(GetRandomNumberInRange(1, RADIO_COUNT - 1));
    }

private:
    static inline auto& m_sfLogLookup = StaticRef<float[50][2]>(0xb61100);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
