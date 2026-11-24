/* Lector de PCM sobre Media Foundation (sustituto de QuickTime en CAEMFDecoder). Ver MfPcmReader.h. */

#ifndef MFPCM_STANDALONE_TEST
#include "StdInc.h" // TODO: Remove
#endif

#include "MfPcmReader.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <propvarutil.h>

#include <algorithm>
#include <cstring>

#pragma comment(lib, "mfuuid.lib") // solo GUIDs (MF_MT_*, MFMediaType_Audio...): no anade dependencia de DLL

namespace {
using MFStartupFn                   = HRESULT(WINAPI*)(ULONG, DWORD);
using MFShutdownFn                  = HRESULT(WINAPI*)();
using MFCreateSourceReaderFromURLFn = HRESULT(WINAPI*)(LPCWSTR, IMFAttributes*, IMFSourceReader**);
using MFCreateMediaTypeFn           = HRESULT(WINAPI*)(IMFMediaType**);

HMODULE                       g_mfplat{};
HMODULE                       g_mfreadwrite{};
MFStartupFn                   g_MFStartup{};
MFShutdownFn                  g_MFShutdown{};
MFCreateSourceReaderFromURLFn g_MFCreateSourceReaderFromURL{};
MFCreateMediaTypeFn           g_MFCreateMediaType{};
LONG                          g_refCount{};

DWORD g_comTlsSlot = TLS_OUT_OF_INDEXES; // marca por hilo "ya entro en COM" (TlsAlloc: sin thread_local, que el ejecutable independiente con TLS propio no garantiza)

// Cada hilo que usa Media Foundation entra en el MTA (los objetos del lector son libres de hilo). Si el hilo ya estaba en otro modelo (RPC_E_CHANGED_MODE) se deja como esta.
void EnsureComOnThread() {
    if (g_comTlsSlot == TLS_OUT_OF_INDEXES) {
        g_comTlsSlot = TlsAlloc();
    }
    if (g_comTlsSlot != TLS_OUT_OF_INDEXES && TlsGetValue(g_comTlsSlot) == nullptr) {
        (void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        TlsSetValue(g_comTlsSlot, reinterpret_cast<LPVOID>(1));
    }
}

template <class T>
void SafeRelease(T*& p) {
    if (p) {
        p->Release();
        p = nullptr;
    }
}
} // namespace

bool CMfPcmReader::Startup() {
    if (g_refCount > 0) {
        ++g_refCount;
        return true;
    }
    EnsureComOnThread();
    g_mfplat      = LoadLibraryW(L"mfplat.dll");
    g_mfreadwrite = LoadLibraryW(L"mfreadwrite.dll");
    if (g_mfplat && g_mfreadwrite) {
        g_MFStartup                   = reinterpret_cast<MFStartupFn>(GetProcAddress(g_mfplat, "MFStartup"));
        g_MFShutdown                  = reinterpret_cast<MFShutdownFn>(GetProcAddress(g_mfplat, "MFShutdown"));
        g_MFCreateMediaType           = reinterpret_cast<MFCreateMediaTypeFn>(GetProcAddress(g_mfplat, "MFCreateMediaType"));
        g_MFCreateSourceReaderFromURL = reinterpret_cast<MFCreateSourceReaderFromURLFn>(GetProcAddress(g_mfreadwrite, "MFCreateSourceReaderFromURL"));
    }
    if (g_MFStartup && g_MFShutdown && g_MFCreateMediaType && g_MFCreateSourceReaderFromURL && SUCCEEDED(g_MFStartup(MF_VERSION, MFSTARTUP_FULL))) {
        g_refCount = 1;
        return true;
    }
    if (g_mfreadwrite) {
        FreeLibrary(g_mfreadwrite);
    }
    if (g_mfplat) {
        FreeLibrary(g_mfplat);
    }
    g_mfplat = g_mfreadwrite = nullptr;
    g_MFStartup                   = nullptr;
    g_MFShutdown                  = nullptr;
    g_MFCreateMediaType           = nullptr;
    g_MFCreateSourceReaderFromURL = nullptr;
    return false;
}

void CMfPcmReader::Shutdown() {
    if (g_refCount <= 0) {
        return;
    }
    if (--g_refCount == 0) {
        g_MFShutdown();
        // Las DLL se dejan cargadas: pueden quedar lectores del proceso liberandose en otro hilo.
    }
}

CMfPcmReader::~CMfPcmReader() {
    SafeRelease(m_reader);
}

bool CMfPcmReader::Open(const char* ansiPath, uint32_t sampleRate, uint16_t channels) {
    SafeRelease(m_reader);
    m_pending.clear();
    m_pendingPos     = 0;
    m_endOfStream    = false;
    m_durationFrames = 0;
    if (g_refCount <= 0 || !ansiPath || sampleRate == 0 || channels == 0) {
        return false;
    }
    EnsureComOnThread();

    wchar_t wide[MAX_PATH * 2];
    if (MultiByteToWideChar(CP_ACP, 0, ansiPath, -1, wide, static_cast<int>(std::size(wide))) == 0) {
        return false;
    }
    if (FAILED(g_MFCreateSourceReaderFromURL(wide, nullptr, &m_reader)) || !m_reader) {
        m_reader = nullptr;
        return false;
    }

    // Solo el primer flujo de audio.
    m_reader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS), FALSE);
    if (FAILED(m_reader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), TRUE))) {
        SafeRelease(m_reader);
        return false;
    }

    // Salida pedida: PCM entero de 16 bits (el lector inserta decodificador, remuestreador y mezclador de canales).
    IMFMediaType* type = nullptr;
    bool          ok   = false;
    if (SUCCEEDED(g_MFCreateMediaType(&type))) {
        const UINT32 blockAlign = channels * 2u;
        type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
        type->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
        type->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, channels);
        type->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
        type->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
        type->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, blockAlign);
        type->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, blockAlign * sampleRate);
        type->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
        ok = SUCCEEDED(m_reader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, type));
        type->Release();
    }
    if (!ok) {
        SafeRelease(m_reader);
        return false;
    }
    m_reader->SetStreamSelection(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), TRUE);

    // Duracion de la fuente (unidades de 100 ns) -> fotogramas a la frecuencia de salida.
    PROPVARIANT duration;
    PropVariantInit(&duration);
    if (SUCCEEDED(m_reader->GetPresentationAttribute(static_cast<DWORD>(MF_SOURCE_READER_MEDIASOURCE), MF_PD_DURATION, &duration)) && duration.vt == VT_UI8) {
        m_durationFrames = static_cast<int64_t>(duration.uhVal.QuadPart * sampleRate / 10000000ull);
    }
    PropVariantClear(&duration);

    m_sampleRate = sampleRate;
    m_channels   = channels;
    return true;
}

// Lee una muestra del lector y la anade a la cola de sobrantes. Devuelve false al final de la pista o ante un error.
bool CMfPcmReader::FillPending() {
    if (!m_reader || m_endOfStream) {
        return false;
    }
    for (;;) {
        DWORD         streamIndex = 0, flags = 0;
        LONGLONG      timestamp   = 0;
        IMFSample*    sample      = nullptr;
        const HRESULT hr          = m_reader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, &streamIndex, &flags, &timestamp, &sample);
        if (FAILED(hr)) {
            m_endOfStream = true;
            return false;
        }
        if (flags & (MF_SOURCE_READERF_ENDOFSTREAM | MF_SOURCE_READERF_ERROR)) {
            m_endOfStream = true;
        }
        bool added = false;
        if (sample) {
            IMFMediaBuffer* buffer = nullptr;
            if (SUCCEEDED(sample->ConvertToContiguousBuffer(&buffer)) && buffer) {
                BYTE*  data   = nullptr;
                DWORD  length = 0;
                if (SUCCEEDED(buffer->Lock(&data, nullptr, &length))) {
                    m_pending.insert(m_pending.end(), data, data + length);
                    added = length > 0;
                    buffer->Unlock();
                }
                buffer->Release();
            }
            sample->Release();
        }
        if (added) {
            return true;
        }
        if (m_endOfStream) {
            return false;
        }
        // Sin datos y sin fin (p. ej. cambio de tipo o hueco): se vuelve a leer.
    }
}

size_t CMfPcmReader::Read(void* dest, size_t size) {
    EnsureComOnThread(); // la lectura corre en el hilo de streaming, distinto del que abrio la pista
    auto*  out     = static_cast<uint8_t*>(dest);
    size_t written = 0;
    while (written < size) {
        if (m_pendingPos >= m_pending.size()) {
            m_pending.clear();
            m_pendingPos = 0;
            if (!FillPending()) {
                break;
            }
        }
        const size_t n = std::min(size - written, m_pending.size() - m_pendingPos);
        std::memcpy(out + written, m_pending.data() + m_pendingPos, n);
        m_pendingPos += n;
        written += n;
    }
    return written;
}

bool CMfPcmReader::Seek(int64_t frame) {
    if (!m_reader || m_sampleRate == 0) {
        return false;
    }
    EnsureComOnThread();
    if (frame < 0) {
        frame = 0;
    }
    PROPVARIANT position;
    PropVariantInit(&position);
    position.vt            = VT_I8;
    position.hVal.QuadPart = frame * 10000000ll / static_cast<int64_t>(m_sampleRate);
    const HRESULT hr       = m_reader->SetCurrentPosition(GUID_NULL, position);
    m_pending.clear();
    m_pendingPos  = 0;
    m_endOfStream = false;
    return SUCCEEDED(hr);
}
