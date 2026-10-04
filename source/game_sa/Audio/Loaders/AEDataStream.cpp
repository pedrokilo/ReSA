/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEDataStream
 * Reconstrucción: COMPLETA — 20/20 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove

#include "common.h"

#include "AEDataStream.h"
#include "AEStreamTransformer.h"
#include "FileMgr.h"

// 0x4dc620
// Conserva una ventana [m_nStartPosition, m_nStartPosition + m_nLength) sobre el archivo.
CAEDataStream::CAEDataStream(int32 trackId, char* filename, uint32 startPosition, uint32 length, bool encrypted) {
    // The native constructor leaves the handle untouched until Initialise.
    m_pszFilename      = filename;
    m_bIsOpen          = false;
    m_nCurrentPosition = 0;
    m_nStartPosition   = startPosition;
    m_nLength          = length;
    m_nTrackId         = trackId;
    m_bIsEncrypted     = encrypted;
    m_lRefCount        = 1;
}

// 0x4dc490
// Cierra el HANDLE si sigue abierto y libera el nombre de archivo propiedad del stream.
CAEDataStream::~CAEDataStream() {
    if (m_bIsOpen) {
        Close();
    }

    if (m_pszFilename) {
        // Como el original: libera con operator delete escalar (0x8214BD), no delete[].
        operator delete(m_pszFilename);
        m_pszFilename = nullptr;
    }
}

// 0x4dc410
HRESULT CAEDataStream::QueryInterface(REFIID riid, void** objout) {
    // MikuAuahDark: Despite having QueryInterface implemented,
    // I don't think CAEDataStream is meant to be QueryInterface'd.
    // See CAEDataStream::Release
    if (riid == IID_IUnknown || riid == IID_IStream) {
        // Como el original: escribe *objout antes de llamar a AddRef por la vtable.
        *objout = this;
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

// 0x4dc460
ULONG CAEDataStream::AddRef() {
    // Como el original: InterlockedIncrement (IAT 0x8580CC).
    return static_cast<ULONG>(InterlockedIncrement(reinterpret_cast<volatile LONG*>(&m_lRefCount)));
}

// 0x4dc5b0
ULONG CAEDataStream::Release() {
    // Como el original: InterlockedDecrement (IAT 0x8580D0).
    if (InterlockedDecrement(reinterpret_cast<volatile LONG*>(&m_lRefCount)) == 0) {
        delete this;
        return 0;
    }

    // MikuAuahDark: I think CAEDataStream isn't mean to be
    // QueryInterface'd? 0x4dc608 clearly says "mov eax, 0xbad"
    return 0xbad;
}

// 0x4dc320
// Adapta FillBuffer() a la interfaz COM IStream.
HRESULT CAEDataStream::Read(void* dest, ULONG size, ULONG* readed) {
    *readed = static_cast<ULONG>(FillBuffer(dest, size));
    return S_OK;
}

// 0x4dc4d0
HRESULT CAEDataStream::Write(const void* src, ULONG size, ULONG* written) {
    return E_NOTIMPL;
}

// 0x4dc340
HRESULT CAEDataStream::Seek(LARGE_INTEGER offset, DWORD whence, ULARGE_INTEGER* newOffset) {
    // C-style cast is not a good idea here
    // but I can't figure out which cast is best to preserve
    // the sign-bit of LARGE_INTEGER.
    //
    // TODO: Implement 64-bit variants?

    // Como el original: no comprueba m_bIsOpen y sólo usa la parte baja del offset.
    unsigned long pos = Seek(static_cast<long>(offset.LowPart), static_cast<int32>(whence));
    if (newOffset)
        newOffset->QuadPart = pos;

    return S_OK;
}

// 0x4dc4e0
HRESULT CAEDataStream::SetSize(ULARGE_INTEGER newSize) {
    return E_NOTIMPL;
}

// 0x4dc4f0
HRESULT CAEDataStream::CopyTo(IStream* target, ULARGE_INTEGER size, ULARGE_INTEGER* readed, ULARGE_INTEGER* written) {
    return E_NOTIMPL;
}

// 0x4dc500
HRESULT CAEDataStream::Commit(DWORD flags) {
    return E_NOTIMPL;
}

// 0x4dc510
HRESULT CAEDataStream::Revert() {
    return E_NOTIMPL;
}

// 0x4dc520
HRESULT CAEDataStream::LockRegion(ULARGE_INTEGER offset, ULARGE_INTEGER size, DWORD type) {
    return E_NOTIMPL;
}

// 0x4dc530
HRESULT CAEDataStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {
    return E_NOTIMPL;
}

// 0x4dc3a0
HRESULT CAEDataStream::Stat(STATSTG* statout, DWORD flags) {
    if (statout == nullptr || flags != STATFLAG_NONAME)
        return E_INVALIDARG;

    const auto fileSize = GetFileSize(m_pFileHandle, nullptr);
    if (fileSize == INVALID_FILE_SIZE) {
        // Como el original: macro HRESULT_FROM_WIN32 antigua, GetLastError se llama dos veces.
        if (static_cast<HRESULT>(GetLastError()) <= 0) {
            return static_cast<HRESULT>(GetLastError());
        }
        return static_cast<HRESULT>((GetLastError() & 0x0000FFFF) | 0x80070000);
    }
    memset(statout, 0, sizeof(STATSTG));
    statout->cbSize.LowPart = fileSize;
    statout->type = STGTY_STREAM;
    return S_OK;
}

// 0x4dc540
HRESULT CAEDataStream::Clone(IStream** target) {
    return E_NOTIMPL;
}

// 0x4dc1c0
/**
 * Lee desde el HANDLE respetando la ventana lógica del track y, si procede,
 * descifra in-place mediante CAEStreamTransformer usando la posición absoluta.
 */
size_t CAEDataStream::FillBuffer(void* dest, size_t size) {
    size_t sizeToRead = static_cast<size_t>((m_nCurrentPosition - m_nLength) - m_nStartPosition);
    if (size < sizeToRead)
        sizeToRead = size;

    // Como el original: el contador de ReadFile ocupa la ranura del argumento size.
    DWORD bytesRead = static_cast<DWORD>(size);
    if (!ReadFile(m_pFileHandle, dest, static_cast<DWORD>(sizeToRead), &bytesRead, nullptr)) {
        GetLastError();
    }
    size = bytesRead;

    if (m_bIsEncrypted)
        AEStreamTransformer.TransformBuffer(dest, size, m_nCurrentPosition);

    m_nCurrentPosition += static_cast<uint32>(size);
    return size;
}

// 0x4dc230
uint32 CAEDataStream::GetCurrentPosition() {
    const auto pos = SetFilePointer(m_pFileHandle, 0, nullptr, FILE_CURRENT);
    return pos - m_nStartPosition;
}

// 0x4dc250
// Traduce SEEK_SET/SEEK_END desde coordenadas relativas del track al offset real del archivo.
uint32 CAEDataStream::Seek(long offset, int32 whence) {
    if (whence == SEEK_SET) {
        offset += m_nStartPosition;
    } else if (whence == SEEK_END) {
        offset = (m_nLength + m_nStartPosition) - offset;
        whence = SEEK_SET;
    }

    m_nCurrentPosition = SetFilePointer(m_pFileHandle, offset, nullptr, whence);
    return m_nCurrentPosition - m_nStartPosition;
}

bool CAEDataStream::Close() {
    m_bIsOpen = false;
    return CloseHandle(m_pFileHandle) != FALSE;
}

// 0x4dc2b0
/**
 * Abre el archivo con CreateFileW, posiciona el cursor en m_nStartPosition y,
 * si m_nLength era cero, utiliza el tamaño completo del archivo.
 */
bool CAEDataStream::Initialise() {
    if (m_bIsOpen)
        return true;

    // Como el original: CreateFileA (IAT 0x8580A0) con el nombre ANSI tal cual.
    m_pFileHandle = CreateFileA(m_pszFilename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    const bool opened = m_pFileHandle != INVALID_HANDLE_VALUE;
    if (opened) {
        m_nCurrentPosition = SetFilePointer(m_pFileHandle, m_nStartPosition, nullptr, FILE_BEGIN);
        m_bIsOpen = true;
    }
    if (m_nLength == 0) {
        m_nLength = GetFileSize(m_pFileHandle, nullptr);
    }
    return opened;
}

/**
 * Registra la implementación Win32/IStream del stream de datos de audio.
 */
void CAEDataStream::InjectHooks() {
    // Como el original: vtable IStream en 0x85F0BC (14 entradas: IUnknown, ISequentialStream, IStream; sin destructor virtual)
    RH_ScopedVirtualClass(CAEDataStream, 0x85F0BC, 14);
    RH_ScopedCategory("Audio/Loaders");

    // Native and reconstructed methods share Win32 HANDLE ownership.

    RH_ScopedInstall(Constructor, 0x4dc620);
    RH_ScopedInstall(Destructor, 0x4dc490);
    RH_ScopedInstall(Initialise, 0x4dc2b0);
    RH_ScopedInstall(FillBuffer, 0x4dc1c0);
    RH_ScopedInstall(GetCurrentPosition, 0x4dc230);
    RH_ScopedOverloadedInstall(Seek, "OG", 0x4dc250, uint32(CAEDataStream::*)(long offset, int32 whence));

    // IStream implementation (vtable: un hook plano sobre una virtual entra en bucle). These depend on functions above so most of them don't have to be locked.
    RH_ScopedVMTInstall(Read, 0x4dc320);
    RH_ScopedVMTOverloadedInstall(Seek, "istream", 0x4dc340, HRESULT(__stdcall CAEDataStream::*)(LARGE_INTEGER, DWORD, ULARGE_INTEGER*));
    RH_ScopedVMTInstall(Stat, 0x4dc3a0, {.locked = false});
    RH_ScopedVMTInstall(QueryInterface, 0x4dc410);
    RH_ScopedVMTInstall(AddRef, 0x4dc460);
    RH_ScopedVMTInstall(Write, 0x4dc4d0);
    RH_ScopedVMTInstall(SetSize, 0x4dc4e0);
    RH_ScopedVMTInstall(CopyTo, 0x4dc4f0);
    RH_ScopedVMTInstall(Commit, 0x4dc500);
    RH_ScopedVMTInstall(Revert, 0x4dc510);
    RH_ScopedVMTInstall(LockRegion, 0x4dc520);
    RH_ScopedVMTInstall(UnlockRegion, 0x4dc530);
    RH_ScopedVMTInstall(Clone, 0x4dc540);
    RH_ScopedVMTInstall(Release, 0x4dc5b0);

    CAEStreamTransformer::InjectHooks();
}

// Como el original: el flag se copia como byte (mov cl,[esp+14h]), sin normalizar a 0/1.
CAEDataStream* CAEDataStream::Constructor(int32 trackID, char* filename, uint32 startPosition, uint32 length, bool encrypted) {
    this->CAEDataStream::CAEDataStream(trackID, filename, startPosition, length, encrypted);
    return this;
}

void CAEDataStream::Destructor() {
    this->CAEDataStream::~CAEDataStream();
}
