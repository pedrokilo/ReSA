/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEWMADecoder
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove

#include "AEWMADecoder.h"

#include "MemoryMgr.h"

// WMA functions
HRESULT(__stdcall*& CAEWMADecoder::WMCreateSyncReader)(IUnknown*, DWORD, IWMSyncReader**) = *(HRESULT(__stdcall**)(IUnknown*, DWORD, IWMSyncReader**))0xb6bad8;

// 0x502720
CAEWMADecoder::CAEWMADecoder(CAEDataStream* dataStream) : CAEStreamingDecoder(dataStream) {
    m_bInitialized = false;
    m_nTempBufferUsed = 0;
}

// 0x502760
CAEWMADecoder::~CAEWMADecoder() {
    // Como el original: cierra y libera el lector sin poner m_bInitialized a falso ni llamar a CoUninitialize.
    if (m_bInitialized) {
        syncReader->Close();
        syncReader->Release();
    }
}

// 0x502c60
/**
 * Inicializa COM/Windows Media, selecciona el primer stream de audio y valida
 * salida PCM estéreo de 16 bits antes de aceptar el decoder.
 */
bool CAEWMADecoder::Initialise() {
    // Como el original: el resultado se guarda en m_bInitialized en todas las salidas.
    bool                 result         = false;
    IWMProfile*          profile        = nullptr;
    WMT_STREAM_SELECTION activateStream = WMT_ON;
    DWORD                dummyBufLen    = 4;
    const auto Finish = [&] {
        m_bInitialized = result;
        return result;
    };

    CoInitialize(nullptr); // Como el original: sin comprobar el resultado

    if (!WMCreateSyncReader) {
        return Finish();
    }
    if (FAILED(WMCreateSyncReader(nullptr, 0, &syncReader))) {
        return Finish();
    }
    if (FAILED(syncReader->OpenStream(m_dataStream))) {
        if (syncReader) {
            syncReader->Release();
        }
        syncReader = nullptr;
        return Finish();
    }
    if (FAILED(syncReader->QueryInterface(IID_IWMProfile, (void**)&profile))) {
        return Finish();
    }
    const HRESULT selectResult = SelectStreamIndex(profile);
    if (profile) {
        profile->Release();
    }
    profile = nullptr;
    if (FAILED(selectResult)) {
        return Finish();
    }

    DWORD                outputNumber = 0;
    IWMOutputMediaProps* mediaProps   = nullptr;

    syncReader->SetStreamsSelected(1, &m_wStreamNumber, &activateStream);
    syncReader->SetReadStreamSamples(m_wStreamNumber, FALSE);
    syncReader->SetRange(0ULL, 0LL);
    syncReader->GetOutputNumberForStream(m_wStreamNumber, &outputNumber);

    if (SUCCEEDED(syncReader->GetOutputProps(outputNumber, &mediaProps))) {
        DWORD mediaTypeSize = 0;
        mediaProps->GetMediaType(nullptr, &mediaTypeSize);

        WM_MEDIA_TYPE* mediaTypeBase = (WM_MEDIA_TYPE*)CMemoryMgr::Malloc(mediaTypeSize, 0);
        mediaProps->GetMediaType(mediaTypeBase, &mediaTypeSize);
        WAVEFORMATEX* mediaMetadata = (WAVEFORMATEX*)mediaTypeBase->pbFormat;

        if (mediaMetadata && mediaMetadata->nChannels == 2 && mediaMetadata->wBitsPerSample == 16) {
            IWMHeaderInfo3* headerInfo;
            m_nSampleRate = mediaMetadata->nSamplesPerSec;

            if (SUCCEEDED(syncReader->QueryInterface(IID_IWMHeaderInfo3, (void**)&headerInfo))) {
                WORD count = 0;
                dummyBufLen = 8;
                headerInfo->GetAttributeIndices(0, L"Duration", nullptr, nullptr, &count);

                // Como el original: operator new/delete escalares (no new[]/delete[]).
                WORD* indices = (WORD*)::operator new((uint32)count * 2u);
                headerInfo->GetAttributeIndices(0, L"Duration", nullptr, indices, &count);

                WORD              dummyNameLen;
                QWORD             duration;
                WMT_ATTR_DATATYPE dataType;
                headerInfo->GetAttributeByIndexEx(0, indices[0], nullptr, &dummyNameLen, &dataType, nullptr, (BYTE*)&duration, &dummyBufLen);

                m_nLengthMs = (long)(duration / 10000ULL);
                ::operator delete(indices);
                headerInfo->Release();

                // Como el original: marca el decoder inicializado y pregunta la duración por la vtable.
                m_bInitialized = true;
                result = GetStreamLengthMs() >= 7000;
            }
        }

        CMemoryMgr::Free(mediaTypeBase);
    }
    // Como el original: libera las propiedades también cuando GetOutputProps falla.
    mediaProps->Release();
    return Finish();
}

// 0x5027d0
/**
 * Copia PCM desde IWMSyncReader y conserva bytes sobrantes en m_szBuffer para la
 * siguiente petición cuando una muestra excede el espacio disponible.
 */
size_t CAEWMADecoder::FillBuffer(void* dest, size_t size) {
    auto* const out = (uint8*)dest;
    // Como el original: las variables de la muestra se inicializan una sola vez, fuera del bucle.
    INSSBuffer* nssBuffer  = nullptr;
    DWORD       flags      = 0, outputNum = 0;
    WORD        stream     = 0;
    DWORD       bufferLen  = 0;
    BYTE*       byteBuffer = nullptr;
    size_t      written    = 0;

    if (m_nTempBufferUsed > 0) {
        if (m_nTempBufferUsed < size) {
            memcpy(out, m_szBuffer, m_nTempBufferUsed);
            written           = m_nTempBufferUsed;
            m_nTempBufferUsed = 0;
        } else {
            memcpy(out, m_szBuffer, size);
            memmove(m_szBuffer, m_szBuffer + size, m_nTempBufferUsed - size);
            written            = size;
            m_nTempBufferUsed -= size;
        }
    }

    while (written < size) {
        if (FAILED(syncReader->GetNextSample(m_wStreamNumber, &nssBuffer, &m_nSampleTime, &m_nSampleDuration, &flags, &outputNum, &stream))) {
            break;
        }
        if (stream != m_wStreamNumber) {
            continue; // Como el original: la muestra de otro stream no se libera
        }
        const size_t remaining = size - written;
        nssBuffer->GetBufferAndLength(&byteBuffer, &bufferLen); // Como el original: sin comprobar el resultado

        memcpy(out + written, byteBuffer, bufferLen < remaining ? bufferLen : remaining);
        if (bufferLen > remaining) {
            // Como el original: el sobrante se guarda desde el inicio del buffer temporal, hasta 0x20000 bytes.
            m_nTempBufferUsed = bufferLen - remaining;
            if (m_nTempBufferUsed > TEMPBUFSIZE) {
                m_nTempBufferUsed = TEMPBUFSIZE;
            }
            memcpy(m_szBuffer, byteBuffer + remaining, m_nTempBufferUsed);
        } else {
            m_nTempBufferUsed = 0;
        }
        written += bufferLen < remaining ? bufferLen : remaining;
        nssBuffer->Release();
    }

    return written;
}

// 0x502ad0
long CAEWMADecoder::GetStreamLengthMs() {
    if (m_bInitialized)
        return m_nLengthMs;

    return -1;
}

// 0x502af0
long CAEWMADecoder::GetStreamPlayTimeMs() {
    if (!m_bInitialized) {
        return -1;
    }
    // Como el original: la suma de 64 bits sin signo se carga en x87 (63 bits bajos + bit alto con fchs),
    // se multiplica por 0.0001 (0x858FC4) con la precisión del FPU y se trunca con _ftol2.
    const uint64 sum   = m_nSampleTime + m_nSampleDuration;
    int64        low63 = (int64)(sum & 0x7FFFFFFFFFFFFFFFull);
    int64        top   = (int64)(sum & 0x8000000000000000ull);
    float        scale = 0.0001f;
    float        ms;
    __asm {
        fild  qword ptr low63
        fild  qword ptr top
        fchs
        faddp st(1), st
        fmul  dword ptr scale
        fstp  dword ptr ms
    }
    return (long)(int64)ms;
}

// 0x502b50
void CAEWMADecoder::SetCursor(unsigned long pos) {
    // Como el original: pos * 10000 en 32 bits (imul), extendido sin signo.
    if (m_bInitialized)
        syncReader->SetRange((QWORD)(uint32)(pos * 10000u), 0LL);
}

// 0x502ab0
int32 CAEWMADecoder::GetSampleRate() {
    if (m_bInitialized)
        return m_nSampleRate;

    return -1;
}

// 0x502750
int32 CAEWMADecoder::GetStreamID() {
    return m_dataStream->m_nTrackId;
}

// 0x502b80
// Carga wmvcore.dll dinámicamente y verifica que WMCreateSyncReader sea realmente utilizable.
bool CAEWMADecoder::InitLibrary() {
    if (WMCreateSyncReader != nullptr)
        return true;

    // Como el original: compara el módulo con -1 (no con nulo) y no lo pone a nulo al fallar.
    wmvCoreModule = LoadLibraryA("wmvcore.dll");
    if (wmvCoreModule == (HMODULE)(-1)) {
        return false;
    }
    using WMCreateSyncReaderFunc = HRESULT(__stdcall*)(IUnknown*, DWORD, IWMSyncReader**);
    WMCreateSyncReader = (WMCreateSyncReaderFunc)GetProcAddress(wmvCoreModule, "WMCreateSyncReader");
    if (!WMCreateSyncReader) {
        FreeLibrary(wmvCoreModule);
        return false;
    }

    // MikuAuahDark: GTASA tests if WMCreateSyncReader actually works
    IWMSyncReader* reader;
    if (FAILED(WMCreateSyncReader(nullptr, 0, &reader))) {
        WMCreateSyncReader = nullptr; // Como el original: sin FreeLibrary
        return false;
    }
    reader->Release();
    return true;
}

void CAEWMADecoder::Shutdown() {
    // Free WMV core
    if (WMCreateSyncReader) {
        FreeLibrary(wmvCoreModule);
        WMCreateSyncReader = nullptr;
        wmvCoreModule = nullptr;
    }
}

// 0x502990
// Recorre el perfil hasta encontrar el primer stream cuyo tipo sea WMMEDIATYPE_Audio.
HRESULT CAEWMADecoder::SelectStreamIndex(IWMProfile* profile) {
    if (profile == nullptr)
        return E_INVALIDARG;

    DWORD   streams = 0;
    HRESULT hr      = profile->GetStreamCount(&streams);
    if (FAILED(hr)) {
        return hr; // Como el original: devuelve el error sin mirar m_wStreamNumber
    }

    IWMStreamConfig* streamConfig = nullptr;
    m_wStreamNumber = 0;
    for (DWORD i = 0; i < streams; i++) {
        if (FAILED(hr = profile->GetStream(i, &streamConfig)))
            break;

        DWORD index = 0; // Como el original: se pone a cero la palabra doble entera
        if (FAILED(hr = streamConfig->GetStreamNumber((WORD*)&index)))
            break; // Como el original: sin liberar streamConfig

        GUID streamGUID;
        if (FAILED(hr = streamConfig->GetStreamType(&streamGUID)))
            break;

        if (streamGUID == WMMEDIATYPE_Audio) {
            m_wStreamNumber = (WORD)index; // Como el original: el stream de audio no se libera
            break;
        }

        if (streamConfig) {
            streamConfig->Release();
        }
        streamConfig = nullptr;
    }

    if (m_wStreamNumber == 0)
        return E_INVALIDARG;

    return hr; // Como el original: el último HRESULT (puede ser positivo)
}

/**
 * Registra el decoder Windows Media Audio.
 */
void CAEWMADecoder::InjectHooks() {
    // Como el original: vtable 0x862D90 con 8 entradas (Initialise, FillBuffer, GetStreamLengthMs,
    // GetStreamPlayTimeMs, SetCursor, GetSampleRate, destructor con borrado, GetStreamID). Las virtuales se
    // instalan en su hueco: un hook plano saltaría al thunk virtual de la DLL y volvería al EXE (bucle).
    RH_ScopedVirtualClass(CAEWMADecoder, 0x862D90, 8);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x502720);
    RH_ScopedInstall(Destructor, 0x502760);
    RH_ScopedInstall(InitLibrary, 0x502b80);
    RH_ScopedInstall(SelectStreamIndex, 0x502990);
    RH_ScopedVMTInstall(Initialise, 0x502c60);
    RH_ScopedVMTInstall(FillBuffer, 0x5027d0);
    RH_ScopedVMTInstall(GetStreamLengthMs, 0x502ad0);
    RH_ScopedVMTInstall(GetStreamPlayTimeMs, 0x502af0);
    RH_ScopedVMTInstall(SetCursor, 0x502b50);
    RH_ScopedVMTInstall(GetSampleRate, 0x502ab0);
    RH_ScopedVMTInstall(GetStreamID, 0x502750);
}

CAEWMADecoder* CAEWMADecoder::Constructor(CAEDataStream* dataStream) {
    this->CAEWMADecoder::CAEWMADecoder(dataStream);
    return this;
}

CAEWMADecoder* CAEWMADecoder::Destructor() {
    this->CAEWMADecoder::~CAEWMADecoder();
    return this;
}
