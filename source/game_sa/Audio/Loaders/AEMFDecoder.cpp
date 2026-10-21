/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEMFDecoder
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/* Decoder QuickTime de user tracks, transcrito de gta-sa.exe 1.0 US. Como el original, todas las llamadas a
 * QuickTime van a las funciones de pegamento del EXE (0x818xxx-0x81Axxx) y a sus ayudantes 0x4E7990 (abrir la
 * película y su media de sonido), 0x4E7A80 (formato de la media) y 0x4E7B80 (callback de SoundConverterFillBuffer).
 * Sin QuickTime instalado InitLibrary falla y los MP3 pasan al decoder WMA, como en el juego original.
 */

#include "StdInc.h" // TODO: Remove

#include "AEMFDecoder.h"
#include "X87Math.h"

// 0x4E7990
// Como el original: NativePathNameToFSSpec (0x818970, resultado ignorado) y FSpGetFInfo-like (0x81DAA0) sobre el FSSpec local; OpenMovieFile (0x81B680, permiso 1),
// NewMovieFromFile (0x81AB80) y CloseMovieFile (0x81A9E0, SIEMPRE tras NewMovieFromFile, antes de mirar su error); luego la primera pista 'soun' (0x81BC60) y su media
// (0x81A0E0). Devuelve el OSErr de 16 bits (el llamador, 0x4E7CB0, solo mira AX): -2009 sin pista de sonido, -2008 sin media. Con error antes de la pista no escribe nada en
// las salidas; con pista escribe media y pelicula (la media se escribe aunque sea nula).
int16 __cdecl AEMFOpenSoundMedia(char* filename, void** outMovie, void** outMedia) {
    uint8 fsSpec[0x46];            // FSSpec: lo rellena 0x818970
    uint8 fileInfo[0x10];          // FInfo: lo rellena 0x81DAA0
    void* movie        = nullptr;  // como el original: puesto a 0 antes de 0x818970
    int32 resourceId   = 0;        // como el original: puesto a 0 antes de OpenMovieFile (NewMovieFromFile lo escribe como short)
    int16 refNum       = 0;        // OpenMovieFile lo escribe como short; el EXE lo pasa como dword (los 16 bits altos son basura de la pila: no observables)
    uint8 dataRefChanged;

    plugin::CallAndReturn<int32, 0x818970, char*, void*, int32>(filename, fsSpec, 0);
    if (const int32 err = plugin::CallAndReturn<int32, 0x81DAA0, void*, void*>(fsSpec, fileInfo); static_cast<int16>(err) != 0) {
        return static_cast<int16>(err);
    }
    if (const int32 err = plugin::CallAndReturn<int32, 0x81B680, void*, int16*, int32>(fsSpec, &refNum, 1); static_cast<int16>(err) != 0) {
        return static_cast<int16>(err);
    }

    const int32 movieErr = plugin::CallAndReturn<int32, 0x81AB80, void**, int32, int32*, char*, int32, uint8*>(&movie, refNum, &resourceId, nullptr, 1, &dataRefChanged);
    plugin::CallAndReturn<int32, 0x81A9E0, int32>(refNum);
    int16 result = static_cast<int16>(movieErr);
    if (result != 0) {
        return result;
    }

    void* const track = plugin::CallAndReturn<void*, 0x81BC60, void*, int32, uint32, int32>(movie, 1, 0x736F756E /* 'soun' */, 1);
    if (!track) {
        return static_cast<int16>(0xF827); // -2009
    }

    void* const media = plugin::CallAndReturn<void*, 0x81A0E0, void*>(track);
    *outMedia = media;
    if (!media) {
        result = static_cast<int16>(0xF828); // -2008
    }
    *outMovie = movie;
    return result;
}

// 0x4E7A80
// Como el original: NewHandle(0) (0x818DB0) y GetMediaSampleDescription (0x81A300, indice 1) sobre la media; el OSErr sale de GetMoviesError (0x819860). Sin error: otro NewHandle(0),
// GetSoundDescriptionExtension 'wave' (0x81C600) y, si lo hay, copia de la extension a un puntero propio (GetHandleSize 0x8192B0, HLock 0x818F90, NewPtr 0x818E70 -> *outAtom,
// MemError 0x818D70 pasa a ser el resultado, BlockMoveData 0x819390, HUnlock 0x818FB0) y se rellena `info` desde la descripcion (releyendo el handle en cada campo). Al final se liberan
// el handle de la extension y el de la descripcion (0x819270). Devuelve el OSErr de 16 bits (el llamador solo mira AX).
// El EXE tiene un hueco de pila sin inicializar (`push ecx`) que usa como handle de la extension: si GetMoviesError falla, se pasa a DisposeHandle lo que hubiera en ECX
// al entrar; el unico llamador (0x4E7CB0) llega con ECX = la media, y eso reproduce esta funcion.
int16 __cdecl AEMFGetMediaSoundFormat(void* media, void** outDecompressionAtom, CAEMFDecoder::tQTSoundFormatInfo* info) {
    void* extension = media; // hueco de pila del EXE (ECX de entrada = la media en el unico llamador)

    void* const description = plugin::CallAndReturn<void*, 0x818DB0, int32>(0);
    plugin::CallAndReturn<int32, 0x81A300, void*, int32, void*>(media, 1, description);
    int32 err = plugin::CallAndReturn<int32, 0x819860>();
    if (static_cast<int16>(err) == 0) {
        extension = plugin::CallAndReturn<void*, 0x818DB0, int32>(0);
        if (const int32 waveErr = plugin::CallAndReturn<int32, 0x81C600, void*, void**, uint32>(description, &extension, 0x77617665 /* 'wave' */); static_cast<int16>(waveErr) == 0) {
            const int32 size = plugin::CallAndReturn<int32, 0x8192B0, void*>(extension);
            plugin::CallAndReturn<int32, 0x818F90, void*>(extension);
            void* const copy = plugin::CallAndReturn<void*, 0x818E70, int32>(size);
            *outDecompressionAtom = copy;
            err = plugin::CallAndReturn<int32, 0x818D70>();
            plugin::CallAndReturn<int32, 0x819390, void*, void*, int32>(*static_cast<void**>(extension), *outDecompressionAtom, size); // el EXE relee *outAtom tras MemError
            plugin::CallAndReturn<int32, 0x818FB0, void*>(extension);
        } else {
            err = 0;
        }

        // Como el original: cada campo vuelve a leer el handle de la descripcion.
        const auto desc = [&]() { return *static_cast<uint8* const*>(description); };
        info->format        = *reinterpret_cast<const uint32*>(desc() + 4);
        info->numChannels   = *reinterpret_cast<const int16*>(desc() + 0x18);
        info->sampleSize    = *reinterpret_cast<const int16*>(desc() + 0x1A);
        info->sampleRate    = *reinterpret_cast<const uint32*>(desc() + 0x20);
        info->compressionID = *reinterpret_cast<const int16*>(desc() + 0x1C);
    }

    if (extension) {
        plugin::Call<0x819270, void*>(extension);
    }
    if (description) {
        plugin::Call<0x819270, void*>(description);
    }
    return static_cast<int16>(err);
}

// 0x4e7770
// Como el original: sólo escribe el nombre, el flag, la película, la media y la pista; el resto del objeto queda
// sin escribir hasta Initialise().
CAEMFDecoder::CAEMFDecoder(char* filename, int32 trackID)
    : CAEStreamingDecoder(nullptr), m_pFilename(filename), m_bInitialized(false), m_movie(nullptr), m_media(nullptr), m_nTrackID(trackID) {}

// 0x4e77c0
// Como el original: borra el nombre (operator delete escalar, sin anular el puntero) y, si llegó a inicializarse,
// libera la película (0x819B20), el buffer de origen (0x819270) y el conversor (0x818450), en ese orden.
CAEMFDecoder::~CAEMFDecoder() {
    if (m_pFilename) {
        operator delete(m_pFilename);
    }

    if (m_bInitialized) {
        plugin::Call<0x819B20, void*>(m_movie);
        plugin::Call<0x819270, uint8**>(m_sourceBuffer);
        plugin::Call<0x818450, void*>(m_soundConverter);
    }
}

// 0x4e7cb0
// Como el original: abre la película (0x4E7990), lee el formato de la media (0x4E7A80), abre el conversor
// (0x818430) hacia PCM estéreo de 16 bits a 44100 Hz, le pasa 'cvbr' y 'wave' (0x818510), empieza la conversión
// (0x818490) y reserva el Handle de origen de 0x10000 bytes (0x818DB0). Los OSErr se comparan a 16 bits.
bool CAEMFDecoder::Initialise() {
    void*              decompressionAtom = nullptr; // Como el original: puesto a 0 antes de 0x4E7990.
    tQTSoundFormatInfo info;                        // Como el original: sin inicializar; lo rellena 0x4E7A80.

    if (plugin::CallAndReturn<int16, 0x4E7990, char*, void**, void**>(m_pFilename, &m_movie, &m_media) != 0)
        return false;

    if (plugin::CallAndReturn<int16, 0x4E7A80, void*, void**, tQTSoundFormatInfo*>(m_media, &decompressionAtom, &info) != 0)
        return false;

    m_inputFormat.numChannels = static_cast<int16>(info.numChannels);
    m_inputFormat.format      = info.format;
    m_inputFormat.sampleSize  = info.sampleSize;
    m_inputFormat.flags       = 0;
    m_inputFormat.sampleRate  = info.sampleRate;
    m_inputFormat.sampleCount = 0;
    m_inputFormat.buffer      = nullptr;
    m_inputFormat.reserved    = 0;

    m_outputFormat.flags       = 0;
    m_outputFormat.format      = 0x4E4F4E45; // 'NONE'
    m_outputFormat.numChannels = 2;
    m_outputFormat.sampleSize  = 16;
    m_outputFormat.sampleRate  = 0xAC440000; // 44100.0 en 16.16
    m_outputFormat.sampleCount = 0;
    m_outputFormat.buffer      = nullptr;
    m_outputFormat.reserved    = 0;

    m_bVariableCompression = info.compressionID == -2;

    if (plugin::CallAndReturn<int16, 0x818430, tQTSoundComponentData*, tQTSoundComponentData*, void**>(&m_inputFormat, &m_outputFormat, &m_soundConverter) != 0)
        return false;

    plugin::CallAndReturn<int16, 0x818510, void*, uint32, void*>(m_soundConverter, 0x63766272, reinterpret_cast<void*>(1)); // 'cvbr'
    const int16 err = plugin::CallAndReturn<int16, 0x818510, void*, uint32, void*>(m_soundConverter, 0x77617665, decompressionAtom); // 'wave'
    if (decompressionAtom) {
        plugin::Call<0x819210, void*>(decompressionAtom);
    }

    // Como el original: también se acepta -231 (0xFF19, selector desconocido).
    if (err != -231 && err != 0)
        return false;

    if (plugin::CallAndReturn<int16, 0x818490, void*>(m_soundConverter) != 0)
        return false;

    m_sourceMedia      = m_media;
    m_nPlayTime        = 0;
    m_nLength          = plugin::CallAndReturn<long, 0x81A180, void*>(m_media);
    m_bSourceDataLeft  = true;
    m_sourceBufferSize = 0x10000;
    m_sourceBuffer     = plugin::CallAndReturn<uint8**, 0x818DB0, int32>(0x10000);

    if (plugin::CallAndReturn<int16, 0x818D70>() != 0)
        return false;

    plugin::Call<0x8191F0, uint8**>(m_sourceBuffer);
    plugin::Call<0x818F90, uint8**>(m_sourceBuffer);

    // Como el original: copia (rep movs) del formato de entrada al bloque del callback.
    std::memcpy(&m_fillData, &m_inputFormat, sizeof(m_fillData));
    m_fillData.flags    = 0x4000;
    m_fillData.buffer   = *m_sourceBuffer;
    m_fillRecordSize    = 0x44;
    m_fillExtendedFlags = m_bVariableCompression ? 0xB : 3;
    m_fillBufferSize    = 0;
    m_fillBufferProc    = reinterpret_cast<void*>(0x4E7B80);
    m_nSampleRate       = 44100;
    m_fTimeDivider      = std::bit_cast<float>(0x42306666u); // 44.1

    // Como el original: fild · (1000/44100, float 0x85F390) con un solo redondeo a float (x87 a 24 bits), truncado con _ftol2.
    static const float kMediaUnitsToMs = std::bit_cast<float>(0x3CB9C278u);
    m_nLengthMs = static_cast<long>(static_cast<int64>(IntTimesFloatCpp(m_nLength, kMediaUnitsToMs)));

    // Como el original: la duración se consulta por la vtable y las pistas de menos de 7 s se descartan.
    m_bInitialized = true;
    if (GetStreamLengthMs() < 7000) {
        m_bInitialized = false;
    }
    return m_bInitialized;
}

// 0x4e7860
// Como el original: sin inicializar devuelve -1; si no, SoundConverterFillBuffer (0x818570) con el callback
// 0x4E7B80 y el bloque +0x54, y devuelve los bytes escritos (salidas puestas a 0 antes de la llamada).
size_t CAEMFDecoder::FillBuffer(void* dest, size_t size) {
    uint32 bytesWritten = 0, outputFlags = 0, framesWritten = 0;

    if (!m_bInitialized)
        return (size_t)-1;

    plugin::CallAndReturn<int16, 0x818570, void*, void*, void*, void*, size_t, uint32*, uint32*, uint32*>(
        m_soundConverter, m_fillBufferProc, &m_fillData, dest, size, &bytesWritten, &framesWritten, &outputFlags
    );
    return bytesWritten;
}

// 0x4e7920
long CAEMFDecoder::GetStreamLengthMs() {
    if (m_bInitialized)
        return m_nLengthMs;

    return -1;
}

// 0x4e7940
// Como el original: tiempo de reproducción (fild, con signo) / divisor en x87 a la precisión vigente, truncado
// con _ftol2 (0x821B40).
long CAEMFDecoder::GetStreamPlayTimeMs() {
    if (!m_bInitialized)
        return -1;

    return MfDecoderPlayTimeMsCpp(m_nPlayTime, m_fTimeDivider);
}

// 0x4e78e0
// Como el original: sólo guarda pos (sin signo: +2^32 si el bit alto está puesto) · divisor, truncado con _ftol2.
void CAEMFDecoder::SetCursor(unsigned long pos) {
    if (!m_bInitialized)
        return;

    m_nPlayTime = MfDecoderCursorToPlayTimeCpp(pos, m_fTimeDivider);
}

// 0x4e78c0
int32 CAEMFDecoder::GetSampleRate() {
    if (m_bInitialized)
        return m_nSampleRate;

    return -1;
}

// 0x4e77b0
int32 CAEMFDecoder::GetStreamID() {
    return m_nTrackID;
}

// 0x4e7c70
// Como el original: si QuickTime no estaba iniciado llama a InitializeQTML(0) (0x8195C0) y EnterMovies (0x819820);
// con éxito (o si ya lo estaba) pone el flag 0xB61D70 a 1. Devuelve si el último OSErr (16 bits) es 0.
bool CAEMFDecoder::InitLibrary() {
    int16 err = 0;
    if (!quickTimeInitialized) {
        err = plugin::CallAndReturn<int16, 0x8195C0, int32>(0);
        if (err == 0) {
            err = plugin::CallAndReturn<int16, 0x819820>();
        }
    }
    if (err == 0) {
        quickTimeInitialized = true;
    }
    return err == 0;
}

// 0x4e7850
// Como el original: ExitMovies (0x819840) y TerminateQTML (0x819770), sin mirar el flag.
void CAEMFDecoder::Shutdown() {
    plugin::Call<0x819840>();
    plugin::Call<0x819770>();
}

/**
 * Registra el decoder QuickTime.
 */
void CAEMFDecoder::InjectHooks() {
    // Como el original: vtable 0x85F370 con 8 entradas (Initialise, FillBuffer, GetStreamLengthMs,
    // GetStreamPlayTimeMs, SetCursor, GetSampleRate, destructor con borrado, GetStreamID). Las virtuales se
    // instalan en su hueco: un hook plano saltaría al thunk virtual de la DLL y volvería al EXE (bucle).
    RH_ScopedVirtualClass(CAEMFDecoder, 0x85F370, 8);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4e7770);
    RH_ScopedInstall(Destructor, 0x4e77c0);
    RH_ScopedInstall(InitLibrary, 0x4e7c70);
    RH_ScopedVMTInstall(Initialise, 0x4e7cb0);
    RH_ScopedVMTInstall(FillBuffer, 0x4e7860);
    RH_ScopedVMTInstall(GetStreamLengthMs, 0x4e7920);
    RH_ScopedVMTInstall(GetStreamPlayTimeMs, 0x4e7940);
    RH_ScopedVMTInstall(SetCursor, 0x4e78e0);
    RH_ScopedVMTInstall(GetSampleRate, 0x4e78c0);
    RH_ScopedVMTInstall(GetStreamID, 0x4e77b0);
    RH_ScopedNamedGlobalInstall(AEMFOpenSoundMedia, "OpenSoundMedia", 0x4E7990);
    RH_ScopedNamedGlobalInstall(AEMFGetMediaSoundFormat, "GetMediaSoundFormat", 0x4E7A80);
}

CAEMFDecoder* CAEMFDecoder::Constructor(char* filename, int32 trackID) {
    this->CAEMFDecoder::CAEMFDecoder(filename, trackID);
    return this;
}

CAEMFDecoder* CAEMFDecoder::Destructor() {
    this->CAEMFDecoder::~CAEMFDecoder();
    return this;
}
