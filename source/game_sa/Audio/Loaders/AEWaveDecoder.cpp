/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEWaveDecoder
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove

#include "AEWaveDecoder.h"

// 0x503250
CAEWaveDecoder::CAEWaveDecoder(CAEDataStream* dataStream) : CAEStreamingDecoder(dataStream), m_bInitialized(false), _dataStreamCopy(dataStream) {
    // NOP
}

namespace {
// Como el original: ambos enteros se cargan con fild (sin signo: +2^32 si el bit alto está puesto), se dividen y se
// multiplican por 1000 en x87 a la precisión vigente y el resultado se trunca (_ftol2, 0x821B40).
long BytesToMs(uint32 bytes, uint32 bytesPerSecond) {
    static const float k2pow32 = 4294967296.0f, k1000 = 1000.0f;
    double ms;
    __asm {
        fild  dword ptr bytes
        cmp   bytes, 0
        jge   L_bytesPositive
        fadd  k2pow32
    L_bytesPositive:
        fild  dword ptr bytesPerSecond
        cmp   bytesPerSecond, 0
        jge   L_bpsPositive
        fadd  k2pow32
    L_bpsPositive:
        fdivp st(1), st
        fmul  k1000
        fstp  ms
    }
    return (long)(int64)ms;
}
} // namespace

// 0x5032b0
// Como el original: lee la cabecera completa de 44 bytes (RIFF + fmt + cabecera del chunk data, sin buscar el
// chunk) y sólo acepta PCM estéreo de 16 bits que dure al menos 7 segundos.
bool CAEWaveDecoder::Initialise() {
    m_bInitialized = false;
    if (_dataStreamCopy->FillBuffer(fileHeaderArray, sizeof(fileHeaderArray)) == sizeof(fileHeaderArray) && bitsPerSample == 16 && channelCount == 2) {
        m_bInitialized = true;
        if (GetStreamLengthMs() < 7000) { // Como el original: llamada por la vtable
            m_bInitialized = false;
        }
    }
    return m_bInitialized;
}

// 0x5032a0
// Como el original: copia directa desde el stream (sin conversión mono).
size_t CAEWaveDecoder::FillBuffer(void* dest, size_t size) {
    return _dataStreamCopy->FillBuffer(dest, size);
}

// 0x503310
// Como el original: bytes del chunk data / (frecuencia · 4), sin usar blockAlign.
long CAEWaveDecoder::GetStreamLengthMs() {
    if (!m_bInitialized) {
        return -1;
    }
    return BytesToMs(dataChunkSize, sampleRate * 4);
}

// 0x503360
// Como el original: posición del stream menos los 44 bytes de cabecera.
long CAEWaveDecoder::GetStreamPlayTimeMs() {
    if (!m_bInitialized) {
        return -1;
    }
    return BytesToMs(_dataStreamCopy->GetCurrentPosition() - 44, sampleRate * 4);
}

// 0x5033c0
// Como el original: pos · 0.001 · (frecuencia · 4) + 44 en x87, truncado, y redondeado a par con el resto con signo.
void CAEWaveDecoder::SetCursor(unsigned long pos) {
    if (!m_bInitialized) {
        return;
    }
    static const float k2pow32 = 4294967296.0f, kMsToSec = 0.001f, kHeader = 44.0f;
    const uint32 bytesPerSecond = sampleRate * 4;
    double bytes;
    __asm {
        fild  dword ptr pos
        cmp   pos, 0
        jge   L_posPositive
        fadd  k2pow32
    L_posPositive:
        fmul  kMsToSec
        fild  dword ptr bytesPerSecond
        cmp   bytesPerSecond, 0
        jge   L_bpsPositive
        fadd  k2pow32
    L_bpsPositive:
        fmulp st(1), st
        fadd  kHeader
        fstp  bytes
    }
    const auto offset = (int32)(int64)bytes;
    _dataStreamCopy->Seek(offset % 2 + offset, SEEK_SET);
}

// 0x503300
int32 CAEWaveDecoder::GetSampleRate() {
    return m_bInitialized ? static_cast<int32>(sampleRate) : -1;
}

// 0x503280
// Como el original: usa la copia del stream de +0xC.
int32 CAEWaveDecoder::GetStreamID() {
    return _dataStreamCopy->m_nTrackId;
}

CAEWaveDecoder* CAEWaveDecoder::Constructor(CAEDataStream* dataStream) {
    this->CAEWaveDecoder::CAEWaveDecoder(dataStream);
    return this;
}

void CAEWaveDecoder::Destructor() {
    this->CAEWaveDecoder::~CAEWaveDecoder();
}

/**
 * Registra el decoder PCM RIFF/WAVE.
 */
void CAEWaveDecoder::InjectHooks() {
    // Como el original: vtable 0x862DE8 con 8 entradas (Initialise, FillBuffer, GetStreamLengthMs,
    // GetStreamPlayTimeMs, SetCursor, GetSampleRate, destructor con borrado, GetStreamID).
    RH_ScopedVirtualClass(CAEWaveDecoder, 0x862DE8, 8);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x503250);
    RH_ScopedInstall(Destructor, 0x503290);
    RH_ScopedVMTInstall(Initialise, 0x5032B0);
    RH_ScopedVMTInstall(FillBuffer, 0x5032A0);
    RH_ScopedVMTInstall(GetStreamLengthMs, 0x503310);
    RH_ScopedVMTInstall(GetStreamPlayTimeMs, 0x503360);
    RH_ScopedVMTInstall(SetCursor, 0x5033C0);
    RH_ScopedVMTInstall(GetSampleRate, 0x503300);
    RH_ScopedVMTInstall(GetStreamID, 0x503280);
}
