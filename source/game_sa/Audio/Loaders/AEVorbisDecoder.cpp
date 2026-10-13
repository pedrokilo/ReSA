/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEVorbisDecoder
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove
#include "X87Math.h"

#include "AEVorbisDecoder.h"

ov_callbacks CAEVorbisDecoder::staticCallbacks = {
    &CAEVorbisDecoder::ReadCallback,
    &CAEVorbisDecoder::SeekCallback,
    &CAEVorbisDecoder::CloseCallback,
    &CAEVorbisDecoder::TellCallback
};

namespace {
// Como el original: segundos (double) * 1000.0 (0x85A318) en x87 a la precisión vigente, truncado con _ftol2
// (0x821B40, 64 bits; se usa la parte baja).
long SecondsToMs(double seconds) {
    return SecondsToMsCpp(seconds);
}
} // namespace

// 0x5026b0
CAEVorbisDecoder::CAEVorbisDecoder(CAEDataStream* dataStream, int32 userTrack) : CAEStreamingDecoder(dataStream) {
    m_sCallbacks        = staticCallbacks;
    m_pVorbisFileHandle = nullptr;
    m_bInitialized      = false;
    reinterpret_cast<uint8&>(m_bIsUserTrack) = (uint8)(userTrack); // Como el original: se guarda el byte bajo tal cual
    m_pDataStreamCopy   = dataStream;
    m_pVorbisInfo       = nullptr;
}

// 0x5023f0
CAEVorbisDecoder::~CAEVorbisDecoder() {
    if (m_pVorbisFileHandle) {
        ov_clear(m_pVorbisFileHandle);
        delete m_pVorbisFileHandle; // Como el original: el puntero no se pone a nullptr
    }
}

// 0x5024d0
// Conecta libvorbis a CAEDataStream mediante callbacks.
bool CAEVorbisDecoder::Initialise() {
    m_pVorbisFileHandle = new OggVorbis_File; // Como el original: 0x2D0 bytes sin inicializar
    // Como el original: la fuente de los callbacks es la copia del stream (+0x20).
    if (ov_open_callbacks(m_pDataStreamCopy, m_pVorbisFileHandle, nullptr, 0, m_sCallbacks) != 0)
        return false;

    m_pVorbisInfo = ov_info(m_pVorbisFileHandle, -1);

    // Como el original: no se comprueba el número de canales.
    m_bInitialized = true;

    // Como el original: las pistas de usuario de menos de 7 s se rechazan y el decoder queda sin inicializar.
    if (m_bIsUserTrack && GetStreamLengthMs() < 7000) { // llamada por la vtable
        m_bInitialized = false;
        return false;
    }

    return true;
}

// 0x502470
/**
 * Decodifica Ogg Vorbis a PCM de 16 bits con signo (little endian).
 * Como el original: no mira los canales (una fuente mono se entrega tal cual).
 */
size_t CAEVorbisDecoder::FillBuffer(void* dest, size_t size) {
    size_t readTotal = 0;
    char*  buffer    = static_cast<char*>(dest);

    int32 bitstream = -1; // Como el original: se inicializa a -1
    while (readTotal < size) {
        const long read = ov_read(m_pVorbisFileHandle, buffer + readTotal, (int)(size - readTotal), 0, sizeof(int16), 1, &bitstream);
        if (read <= 0)
            break;

        readTotal += static_cast<size_t>(read);
    }

    return readTotal;
}

// 0x502610
long CAEVorbisDecoder::GetStreamLengthMs() {
    if (!m_bInitialized)
        return -1;

    return SecondsToMs(ov_time_total(m_pVorbisFileHandle, -1));
}

// 0x502640
long CAEVorbisDecoder::GetStreamPlayTimeMs() {
    if (!m_bInitialized)
        return -1;

    return SecondsToMs(ov_time_tell(m_pVorbisFileHandle));
}

// 0x502670
// Como el original: pos (fild; +2^32 si el bit alto está puesto) * 0.001f (0x858CDC) en x87, no / 1000.
void CAEVorbisDecoder::SetCursor(unsigned long pos) {
    if (!m_bInitialized)
        return;

    const double seconds = UnsignedTimesFloatCpp(pos, 0.001f); // 0x858CDC
    ov_time_seek(m_pVorbisFileHandle, seconds);
}

// 0x502560
int32 CAEVorbisDecoder::GetSampleRate() {
    return m_bInitialized ? static_cast<int32>(m_pVorbisInfo->rate) : -1;
}

// 0x502460
int32 CAEVorbisDecoder::GetStreamID() {
    return m_pDataStreamCopy->m_nTrackId; // Como el original: usa la copia del stream (+0x20)
}

// 0x502580
// Adapta FillBuffer() de CAEDataStream a la convención fread esperada por libvorbis.
size_t CAEVorbisDecoder::ReadCallback(void* ptr, size_t size, size_t nmemb, void* opaque) {
    if (opaque == nullptr)
        return 0;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);
    return dataStream->FillBuffer(ptr, size * nmemb) / size;
}

// 0x5025d0
int32 CAEVorbisDecoder::CloseCallback(void* opaque) {
    if (opaque == nullptr)
        return -1;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);
    return dataStream->Close();
}

// 0x5025b0
int32 CAEVorbisDecoder::SeekCallback(void* opaque, ogg_int64_t offset, int32 whence) {
    if (opaque == nullptr)
        return -1;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);
    unsigned long  result = dataStream->Seek(static_cast<long>(offset), whence);

    return static_cast<int32>(result);
}

// 0x5025f0
long CAEVorbisDecoder::TellCallback(void* opaque) {
    if (opaque == nullptr)
        return -1;

    CAEDataStream* dataStream = reinterpret_cast<CAEDataStream*>(opaque);

    return static_cast<int32>(dataStream->GetCurrentPosition());
}

/**
 * Registra el decoder Ogg Vorbis y sus callbacks de IO.
 */
void CAEVorbisDecoder::InjectHooks() {
    // Como el original: vtable 0x862D70 con 8 entradas (Initialise, FillBuffer, GetStreamLengthMs,
    // GetStreamPlayTimeMs, SetCursor, GetSampleRate, destructor con borrado, GetStreamID).
    RH_ScopedVirtualClass(CAEVorbisDecoder, 0x862D70, 8);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x5026B0);
    RH_ScopedInstall(Destructor, 0x5023F0);
    RH_ScopedVMTInstall(Initialise, 0x5024D0);
    RH_ScopedVMTInstall(FillBuffer, 0x502470);
    RH_ScopedVMTInstall(GetStreamLengthMs, 0x502610);
    RH_ScopedVMTInstall(GetStreamPlayTimeMs, 0x502640);
    RH_ScopedVMTInstall(SetCursor, 0x502670);
    RH_ScopedVMTInstall(GetSampleRate, 0x502560);
    RH_ScopedVMTInstall(GetStreamID, 0x502460);
    RH_ScopedInstall(ReadCallback, 0x502580);
    RH_ScopedInstall(CloseCallback, 0x5025D0);
    RH_ScopedInstall(SeekCallback, 0x5025B0);
    RH_ScopedInstall(TellCallback, 0x5025F0);
}

CAEVorbisDecoder* CAEVorbisDecoder::Constructor(CAEDataStream* dataStream, int32 unknown) {
    this->CAEVorbisDecoder::CAEVorbisDecoder(dataStream, unknown);
    return this;
}

CAEVorbisDecoder* CAEVorbisDecoder::Destructor() {
    this->CAEVorbisDecoder::~CAEVorbisDecoder();
    return this;
}
