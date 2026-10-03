/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEMP3TrackLoader
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "AEMP3TrackLoader.h"
#include "app.h"
#include "FileMgr.h"
#include "MemoryMgr.h"

/**
 * Registra carga de tablas de tracks y detección de la fuente local/DVD.
 */
void CAEMP3TrackLoader::InjectHooks() {
    RH_ScopedClass(CAEMP3TrackLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Constructor, 0x4E0930);
    RH_ScopedInstall(Deconstructor, 0x4E0940);
    RH_ScopedInstall(Initialise, 0x4E0C50);
    RH_ScopedInstall(LoadStreamPackTable, 0x4E0970);
    RH_ScopedInstall(LoadTrackLookupTable, 0x4E09F0);
    RH_ScopedInstall(GetTrackInfo, 0x4E0A70);
    RH_ScopedInstall(IsCurrentAudioStreamAvailable, 0x4E0BD0);
}

// 0x4E0930
CAEMP3TrackLoader::CAEMP3TrackLoader() {
    m_bInitialised = false;
    m_pszDvdDrivePath = nullptr;
}

// 0x4E0940
CAEMP3TrackLoader::~CAEMP3TrackLoader() {
    if (m_bInitialised) {
        CMemoryMgr::Free(m_paTrackLookups);
        CMemoryMgr::Free(m_paStreamPacks);
    }

    if (m_pszDvdDrivePath) {
        delete m_pszDvdDrivePath;
    }
}

// 0x4E0C50
/**
 * Carga STRMPAKS.DAT/TRAKLKUP.DAT y comprueba primero AUDIO\STREAMS local.
 * Si no está disponible, intenta resolver la ruta de medios del DVD.
 */
bool CAEMP3TrackLoader::Initialise() {
    if (!LoadStreamPackTable() || !LoadTrackLookupTable())
        return false;

    m_bStreamingFromDVD = false;

    // Como el original: ruta en un bloque de operator new de strlen(nombre) + 47 bytes, con lstrcpyA/lstrcatA.
    const char* packName = m_paStreamPacks[0].m_szName;
    char* path = static_cast<char*>(operator new(lstrlenA(packName) + 47));
    lstrcpyA(path, "AUDIO\\STREAMS\\");
    lstrcatA(path, m_paStreamPacks[0].m_szName);

    // Como el original: el FILE* se comprueba con signo (test eax,eax / jle).
    auto file = CFileMgr::OpenFile(path, "r");
    if ((int32)(uintptr_t)file > 0) {
        m_bStreamingFromDVD = false;
    } else {
        // Como el original: la ruta del DVD se resuelve aquí mismo (no llama a IsCurrentAudioStreamAvailable).
        m_pszDvdDrivePath = getDvdGamePath();
        if (!m_pszDvdDrivePath) {
            operator delete(path);
            return false;
        }
        lstrcpyA(path, m_pszDvdDrivePath);
        lstrcatA(path, "AUDIO\\STREAMS\\");
        lstrcatA(path, m_paStreamPacks[0].m_szName);
        file = CFileMgr::OpenFile(path, "r");
        if ((int32)(uintptr_t)file <= 0) {
            operator delete(path);
            return false;
        }
        m_bStreamingFromDVD = true;
    }
    CFileMgr::CloseFile(file);
    operator delete(path);
    m_bInitialised = true;
    return true;
}

// 0x4E0970
// Carga la tabla fija de nombres de paquetes de streaming en memoria propia.
bool CAEMP3TrackLoader::LoadStreamPackTable(void) {
    // Como el original: API Win32; si el fichero no existe devuelve false sin crear nada ni tocar la tabla.
    const HANDLE file = CreateFileA("AUDIO\\CONFIG\\STRMPAKS.DAT", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return false;

    DWORD bytesRead;
    const DWORD size = GetFileSize(file, nullptr);
    m_paStreamPacks = (StreamPack*)CMemoryMgr::Malloc(size, 0);
    ReadFile(file, m_paStreamPacks, size, &bytesRead, nullptr);
    CloseHandle(file);
    m_nStreamPackCount = bytesRead / sizeof(StreamPack); // Como el original: cuenta con los bytes leídos.
    return true;
}

// 0x4E09F0
// Carga para cada track su pack, offset y tamaño dentro del archivo de streams.
bool CAEMP3TrackLoader::LoadTrackLookupTable(void) {
    // Como el original: API Win32; si el fichero no existe devuelve false sin crear nada ni tocar la tabla.
    const HANDLE file = CreateFileA("AUDIO\\CONFIG\\TRAKLKUP.DAT", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return false;

    DWORD bytesRead;
    const DWORD size = GetFileSize(file, nullptr);
    m_paTrackLookups = (tTrackLookup*)CMemoryMgr::Malloc(size, 0);
    ReadFile(file, m_paTrackLookups, size, &bytesRead, nullptr);
    m_nTrackCount = size / sizeof(tTrackLookup); // Como el original: cuenta con el tamaño del fichero.
    CloseHandle(file);
    return true;
}

// 0x4E0A70
/**
 * Lee únicamente la cabecera tTrackInfo almacenada al principio del track cifrado.
 *
 * La cadena path se transfiere a CAEDataStream; el stream temporal la destruye al
 * finalizar. El tTrackInfo devuelto sí pasa a ser responsabilidad del llamador.
 */
tTrackInfo* CAEMP3TrackLoader::GetTrackInfo(uint32 trackId) {
    // Como el original: compara con '>' (acepta trackId == m_nTrackCount).
    if (trackId > m_nTrackCount)
        return nullptr;

    const auto packId = m_paTrackLookups[trackId].m_nbPackId;
    if (packId > m_nStreamPackCount)
        return nullptr;

    int32 length;
    if (m_bStreamingFromDVD) {
        const int32 nameLength = lstrlenA(m_paStreamPacks[packId].m_szName);
        length = lstrlenA(m_pszDvdDrivePath) + nameLength + 21;
    } else {
        length = lstrlenA(m_paStreamPacks[packId].m_szName) + 21;
    }

    // Como el original: operator new escalar (lo libera CAEDataStream con operator delete) y lstrcpyA/lstrcatA.
    char* path = static_cast<char*>(operator new(length));
    if (m_bStreamingFromDVD) {
        lstrcpyA(path, m_pszDvdDrivePath);
        lstrcatA(path, "AUDIO\\STREAMS\\");
    } else {
        lstrcpyA(path, "AUDIO\\STREAMS\\");
    }
    lstrcatA(path, m_paStreamPacks[m_paTrackLookups[trackId].m_nbPackId].m_szName);

    // Como el original: reserva primero tTrackInfo (sin inicializar) y después el stream.
    auto* trackInfo = static_cast<tTrackInfo*>(operator new(sizeof(tTrackInfo)));
    void* streamMem = operator new(sizeof(CAEDataStream));
    const auto& lookup = m_paTrackLookups[trackId];
    auto* stream = streamMem ? new (streamMem) CAEDataStream(trackId, path, lookup.m_nOffset, lookup.m_nSize, true) : nullptr;

    stream->Initialise();
    stream->FillBuffer(trackInfo, sizeof(tTrackInfo));
    if (stream) {
        stream->~CAEDataStream();
        operator delete(stream);
    }

    return trackInfo;
}

// 0x4E0BD0
// Obtiene la ruta del medio y valida que el primer stream pack exista allí.
bool CAEMP3TrackLoader::IsCurrentAudioStreamAvailable() {
    m_pszDvdDrivePath = getDvdGamePath();
    if (m_pszDvdDrivePath) {
        // Como el original: ruta del DVD + "AUDIO\\STREAMS\\" sin separador extra, en un búfer de pila.
        char path[256];
        lstrcpyA(path, m_pszDvdDrivePath);
        lstrcatA(path, "AUDIO\\STREAMS\\");
        lstrcatA(path, m_paStreamPacks[0].m_szName);
        if (auto file = CFileMgr::OpenFile(path, "r"); (int32)(uintptr_t)file > 0) {
            // Como el original: no marca m_bInitialised.
            m_bStreamingFromDVD = true;
            CFileMgr::CloseFile(file);
            return m_bStreamingFromDVD;
        }
    }

    m_bStreamingFromDVD = false;
    return m_bStreamingFromDVD;
}
