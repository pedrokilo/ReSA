#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

struct IMFSourceReader;

/**
 * Lector de PCM sobre Media Foundation (IMFSourceReader); sustituye a QuickTime/QTML en CAEMFDecoder.
 *
 * Entrega siempre PCM entero de 16 bits, estereo, a la frecuencia pedida (el juego original pedia al SoundConverter de QuickTime
 * 'NONE' / 2 canales / 16 bits / 44100 Hz). Mapeo de llamadas QTML -> Media Foundation (detalle en docs/QuickTime/sustitucion-quicktime.md):
 *   InitializeQTML / EnterMovies          -> Startup()   (CoInitializeEx + MFStartup)
 *   ExitMovies / TerminateQTML            -> Shutdown()  (MFShutdown)
 *   NewMovieFromDataRef + media de sonido -> Open()      (MFCreateSourceReaderFromURL)
 *   SoundConverterOpen/SetInfo/Begin      -> Open()      (SetCurrentMediaType con PCM 16 bits estereo)
 *   GetMediaDuration                      -> DurationFrames()  (MF_PD_DURATION)
 *   SoundConverterFillBuffer              -> Read()      (ReadSample + cola de sobrantes)
 *   posicion de la media (SetCursor)      -> Seek()      (SetCurrentPosition)
 *   DisposeMovie / SoundConverterClose    -> destructor  (Release)
 * Las DLL (mfplat.dll, mfreadwrite.dll) se cargan con LoadLibrary: sin ellas (ediciones N de Windows) Startup() devuelve false y el
 * gestor de user tracks deja el tipo como no soportado, igual que el original sin QuickTime.
 */
class CMfPcmReader {
public:
    CMfPcmReader() = default;
    ~CMfPcmReader();
    CMfPcmReader(const CMfPcmReader&) = delete;
    CMfPcmReader& operator=(const CMfPcmReader&) = delete;

    static bool Startup();  //!< Cuenta de referencias: la primera llamada carga las DLL y llama a MFStartup.
    static void Shutdown(); //!< Contrapartida de Startup (MFShutdown con la ultima).

    //! Abre un archivo (ruta ANSI, como la que guarda sa-ufiles.dat) y fija la salida PCM de 16 bits con `channels` canales a `sampleRate` Hz.
    bool Open(const char* ansiPath, uint32_t sampleRate, uint16_t channels);

    //! Copia hasta `size` bytes de PCM a `dest`; devuelve los bytes escritos (0 = fin de la pista o error).
    size_t Read(void* dest, size_t size);

    //! Coloca la lectura en el fotograma (muestra por canal) indicado, contado a la frecuencia de salida.
    bool Seek(int64_t frame);

    //! Duracion total de la pista en fotogramas a la frecuencia de salida (0 si no se conoce).
    int64_t DurationFrames() const { return m_durationFrames; }
    uint32_t SampleRate() const { return m_sampleRate; }

private:
    bool FillPending();

    IMFSourceReader*     m_reader{};
    uint32_t             m_sampleRate{};
    uint16_t             m_channels{};
    int64_t              m_durationFrames{};
    bool                 m_endOfStream{};
    std::vector<uint8_t> m_pending;     //!< PCM ya decodificado y aun no entregado.
    size_t               m_pendingPos{};
};
