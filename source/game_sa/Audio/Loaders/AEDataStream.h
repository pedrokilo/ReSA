#pragma once

#include <atomic>

#include <objidl.h>

/**
 * IStream de sólo lectura sobre una sección de un archivo de audio.
 *
 * Posee el HANDLE y el nombre de archivo, mantiene un rango lógico por track y puede
 * aplicar el XOR de CAEStreamTransformer durante las lecturas de recursos cifrados.
 */
class CAEDataStream : public IStream {
public:
    HANDLE     m_pFileHandle; //!< HANDLE Win32 propiedad del stream mientras m_bIsOpen.
    char*      m_pszFilename; //!< Cadena dinámica propiedad de la instancia.
    bool       m_bIsOpen;
    uint32     m_nCurrentPosition;
    uint32     m_nStartPosition; //!< Offset inicial del track dentro del archivo.
    uint32     m_nLength;        //!< Longitud lógica del track.
    uint32     m_nTrackId;
    bool       m_bIsEncrypted;
    uint32     m_lRefCount;

public:
    CAEDataStream(int32 trackId, char* filename, uint32 startPosition, uint32 length, bool encrypted);
    ~CAEDataStream();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** objout) override;
    ULONG STDMETHODCALLTYPE   AddRef() override;
    ULONG STDMETHODCALLTYPE   Release() override;
    HRESULT STDMETHODCALLTYPE Read(void* dest, ULONG size, ULONG* readed) override;
    HRESULT STDMETHODCALLTYPE Write(const void* src, ULONG size, ULONG* written) override;
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER offset, DWORD whence, ULARGE_INTEGER* newOffset) override;
    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER newSize) override;
    HRESULT STDMETHODCALLTYPE CopyTo(IStream* target, ULARGE_INTEGER size, ULARGE_INTEGER* readed, ULARGE_INTEGER* written) override;
    HRESULT STDMETHODCALLTYPE Commit(DWORD flags) override;
    HRESULT STDMETHODCALLTYPE Revert() override;
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER offset, ULARGE_INTEGER size, DWORD type) override;
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override;
    HRESULT STDMETHODCALLTYPE Stat(STATSTG* statout, DWORD flags) override;
    HRESULT STDMETHODCALLTYPE Clone(IStream** target) override;

    bool   Initialise();
    size_t FillBuffer(void* dest, size_t size);
    uint32 GetCurrentPosition();
    uint32 Seek(long offset, int32 whence);
    bool   Close();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEDataStream* Constructor(int32 trackID, char* filename, uint32 startPosition, uint32 length, bool encrypted);
    void           Destructor();
};

VALIDATE_SIZE(CAEDataStream, 0x28);
