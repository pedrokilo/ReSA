/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEUserRadioTrackManager
 * Reconstrucción: COMPLETA — 15/15 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 15/15 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove

#include <cstdio>

// COM
#include <ShObjIdl.h>

#include "common.h"

#include "AEUserRadioTrackManager.h"

#include "AEAudioUtility.h"
#include "AEDataStream.h"
#include "AEMFDecoder.h"
#include "AEWaveDecoder.h"
#include "AEWMADecoder.h"
#include "AEVorbisDecoder.h"

#include "FileMgr.h"
#include "MenuManager.h"

#include <reversiblebugfixes/Bugs.hpp>

auto& AEUserRadioTrackManager = StaticRef<CAEUserRadioTrackManager>(0xB6B970);

#ifdef USERTRACK_FLAC_SUPPORT
// NOTSA: copia propia de la tabla del EXE con FLAC al final.
static auto audioExtensionTypes = std::to_array<tAudioExtensionType>({
    {".ogg", AUDIO_FILE_TYPE_VORBIS},
    {".mp3", AUDIO_FILE_TYPE_QUICKTIME}, // replaced to AUDIO_FILE_TYPE_WMA if QuickTime is not installed
    {".wav", AUDIO_FILE_TYPE_WAV},
    {".wma", AUDIO_FILE_TYPE_WMA},
    {".wmv", AUDIO_FILE_TYPE_WMA},
    {".aac", AUDIO_FILE_TYPE_QUICKTIME},
    {".m4a", AUDIO_FILE_TYPE_QUICKTIME},
    {".mov", AUDIO_FILE_TYPE_QUICKTIME},
    {".flac", AUDIO_FILE_TYPE_FLAC},
});
#else
// Como el original: la tabla del EXE (0x8CBB28, 8 entradas de 12 bytes, también ".mov"). Initialise reescribe el
// tipo de ".mp3" (0x8CBB3C) cuando falta el decoder QuickTime/MediaFoundation.
static auto& audioExtensionTypes = StaticRef<std::array<tAudioExtensionType, 8>>(0x8CBB28);
#endif

// 0x4f35b0
/**
 * Carga el índice persistente de user tracks e inicializa los decoders disponibles.
 * Si Media Foundation no está disponible, reasigna MP3 al decoder WMA compatible.
 */
bool CAEUserRadioTrackManager::Initialise() {
    m_bUserTracksLoadedCopy = m_bUserTracksLoaded = ReadUserTracks();

    // Como el original: solo se escriben los decoders 0..4 (dword en +8 y byte en +0xC); 5..7 no se tocan.
    std::fill_n(m_baDecodersSupported, AUDIO_FILE_TYPE_QUICKTIME + 1, false);

    m_baDecodersSupported[AUDIO_FILE_TYPE_VORBIS] = true;
    m_baDecodersSupported[AUDIO_FILE_TYPE_WAV] = true;
    m_baDecodersSupported[AUDIO_FILE_TYPE_WMA] = CAEWMADecoder::InitLibrary();
    m_baDecodersSupported[AUDIO_FILE_TYPE_QUICKTIME] = CAEMFDecoder::InitLibrary();

    if (!m_baDecodersSupported[AUDIO_FILE_TYPE_QUICKTIME]) {
        // change MP3 decoder from QuickTime to WMA (sin registro: spdlog reserva memoria)
        audioExtensionTypes[1].type = AUDIO_FILE_TYPE_WMA; // ".mp3"
    }
    return true;
}

// 0x4f2fb0
void CAEUserRadioTrackManager::Shutdown() {
    if (m_bUserTracksLoaded && m_pUserTracksInfo) {
        CMemoryMgr::Free(m_pUserTracksInfo);
        // Como el original no se pone a nulo; NOTSA: con el arreglo se evita la doble liberación en ReadUserTracks.
        if (notsa::bugfixes::GenericUB) {
            m_pUserTracksInfo = nullptr;
        }
    }

    CAEWMADecoder::Shutdown();
    CAEMFDecoder::Shutdown();
}

// 0x4f3050
/**
 * Recupera desde sa-ufiles.dat la cadena de ruta correspondiente al registro indicado.
 * La cadena devuelta se reserva dinámicamente y pasa al decoder/CAEDataStream que la adopta.
 */
char* CAEUserRadioTrackManager::GetTrackPath(int32 trackID) const {
    if (trackID >= static_cast<int32>(m_nUserTracksCount))
        return nullptr;

    CFileMgr::SetDirMyDocuments();
    FILESTREAM ufiles = CFileMgr::OpenFile("sa-ufiles.dat", "rb");
    CFileMgr::SetDir("");

    // Como el original solo falla con un handle negativo (jge); NOTSA: con el arreglo también falla con nulo.
    if (notsa::bugfixes::GenericCrashing ? ufiles == nullptr : static_cast<int32>(reinterpret_cast<intptr_t>(ufiles)) < 0)
        return nullptr;

    char* dest = new char[m_pUserTracksInfo[trackID].pathLength + 1];

    CFileMgr::Seek(ufiles, m_pUserTracksInfo[trackID].startUfilesPath, SEEK_SET);
    CFileMgr::Read(ufiles, dest, m_pUserTracksInfo[trackID].pathLength);
    CFileMgr::CloseFile(ufiles);

    // NULL-terminate the path
    dest[m_pUserTracksInfo[trackID].pathLength] = 0;
    return dest;
}

// 0x4f35f0
/**
 * Selecciona y construye el decoder apropiado según tUserTracksInfo::fileType.
 *
 * Los decoders basados en CAEDataStream adoptan el stream y su filename; la ruta
 * Media Foundation se entrega directamente a CAEMFDecoder.
 */
CAEStreamingDecoder* CAEUserRadioTrackManager::LoadUserTrack(int32 trackID) {
    if (!m_bUserTracksLoaded)
        return nullptr;

    if (trackID < 0 || trackID >= m_nUserTracksCount)
        return nullptr;

    if (!m_baDecodersSupported[m_pUserTracksInfo[trackID].fileType])
        return nullptr;

    char* filename = GetTrackPath(trackID);
    if (filename == nullptr)
        return nullptr;

    // QuickTime decoder doesn't need the data stream
    if (m_pUserTracksInfo[trackID].fileType == AUDIO_FILE_TYPE_QUICKTIME) {
        // MikuAuahDark: We've agreed to replace QuickTime with MediaFoundation
        return new CAEMFDecoder(filename, trackID);
    }

    CAEDataStream* dataStream = new CAEDataStream(trackID, filename, 0, 0, false);

    CFileMgr::SetDirMyDocuments();
    const bool dataStreamInitialized = dataStream->Initialise();
    CFileMgr::SetDir("");

    if (!dataStreamInitialized) {
        delete dataStream;
        return nullptr;
    }

    switch (m_pUserTracksInfo[trackID].fileType) {
    case AUDIO_FILE_TYPE_VORBIS:
        return new CAEVorbisDecoder(dataStream, true);
    case AUDIO_FILE_TYPE_WAV:
        return new CAEWaveDecoder(dataStream);
    case AUDIO_FILE_TYPE_WMA:
        return new CAEWMADecoder(dataStream);
    default:
        // Como el original: AUDIO_FILE_TYPE_UNKNOWN (0) y los tipos desconocidos liberan el stream.
        delete dataStream;
        return nullptr;
    }
}

// 0x4f2fd0
// Recarga sa-utrax.dat como array contiguo de tUserTracksInfo, liberando cualquier copia previa.
bool CAEUserRadioTrackManager::ReadUserTracks() {
    CFileMgr::SetDirMyDocuments();
    auto file = CFileMgr::OpenFile("sa-utrax.dat", "rb");
    CFileMgr::SetDir("");

    // Como el original: el handle tiene que ser positivo (jg); si no, sale sin cerrar.
    if (static_cast<int32>(reinterpret_cast<intptr_t>(file)) <= 0)
        return false;

    const auto size = CFileMgr::GetTotalSize(file);
    if (size == 0) {
        CFileMgr::CloseFile(file);
        return false;
    }

    // NOTSA: Don't leak memory in case of multiple calls.
    if (notsa::bugfixes::GenericUB && m_pUserTracksInfo) {
        CMemoryMgr::Free(m_pUserTracksInfo);
    }

    m_nUserTracksCount = static_cast<uint16>(static_cast<uint32>(size) / sizeof(tUserTracksInfo));
    m_pUserTracksInfo = (tUserTracksInfo*)CMemoryMgr::Malloc(size, 0);

    CFileMgr::Read(file, m_pUserTracksInfo, size);
    CFileMgr::CloseFile(file);

    return true;
}

// 0x4f4ba0
// Arranca una única exploración asíncrona y expone su estado sin bloquear al llamador.
bool CAEUserRadioTrackManager::ScanUserTracks() {
    switch (m_nUserTracksScanState) {
    case USER_TRACK_SCAN_OFF:
        m_nUserTracksScanState = USER_TRACK_SCAN_IN_PROGRESS;
        m_hwndUserTracksScanThreadHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)WriteUserTracksThread, this, 0, &m_nUserTracksScanThreadID);

        [[fallthrough]];
    case USER_TRACK_SCAN_IN_PROGRESS:
        return false;
    case USER_TRACK_SCAN_COMPLETE:
    case USER_TRACK_SCAN_ERROR:
        return true;
    default:
        return false;
    }
}

// 0x4f32f0
void CAEUserRadioTrackManager::DeleteUserTracksInfo() {
    // Como el original: DeleteFileA relativo al directorio de Mis documentos.
    TerminateThread(m_hwndUserTracksScanThreadHandle, 0);
    CFileMgr::SetDirMyDocuments();
    DeleteFileA("sa-ufiles.dat");
    DeleteFileA("sa-utrax.dat");
    CFileMgr::SetDir("");

    m_nUserTracksScanState = USER_TRACK_SCAN_OFF;
}

// 0x4f3340
void CAEUserRadioTrackManager::SetUserTrackIndex(int32 index) {
    if (index != -1)
        FrontEndMenuManager.m_UserTrackIndex = index;
}

// 0x4f3250
// Aplica el modo secuencial/aleatorio evitando repetir inmediatamente la pista anterior.
int32 CAEUserRadioTrackManager::SelectUserTrackIndex() const {
    if (m_nUserTracksCount == 0 || FrontEndMenuManager.m_RadioMode < 0)
        return -1;

    switch (FrontEndMenuManager.m_RadioMode) {
    case RADIO_MODE_RADIO:
    case RADIO_MODE_RANDOM: {
        // Como el original: siempre pide al menos un número (con una sola pista sale 0) y solo repite mientras
        // haya más de una pista y salga el índice actual.
        int32 index;
        do {
            index = CAEAudioUtility::GetRandomNumberInRange<int32>(0, m_nUserTracksCount - 1);
        } while (m_nUserTracksCount > 1 && index == static_cast<int32>(FrontEndMenuManager.m_UserTrackIndex));

        FrontEndMenuManager.m_UserTrackIndex = index;
        return index;
    }
    case RADIO_MODE_SEQUENTIAL:
        // Como el original: división sin signo
        return FrontEndMenuManager.m_UserTrackIndex = (FrontEndMenuManager.m_UserTrackIndex + 1) % static_cast<uint32>(m_nUserTracksCount);
    default:
        return -1;
    }
}

// 0x4f31f0
eAudioFileType CAEUserRadioTrackManager::GetAudioFileType(const char* filename) {
    const char* dotPosition = strrchr(filename, '.');

    if (dotPosition == nullptr)
        return AUDIO_FILE_TYPE_UNKNOWN;

    for (const tAudioExtensionType& audioExtType : audioExtensionTypes) {
        if (_stricmp(audioExtType.extension, dotPosition) == 0)
            return audioExtType.type;
    }

    return AUDIO_FILE_TYPE_UNKNOWN;
}

// 0x4f32c0
bool CAEUserRadioTrackManager::IsShortcut(const char* path) {
    const char* dot = strrchr(path, '.');
    return dot ? _stricmp(dot, ".lnk") == 0 : false;
}

// 0x4f3330
uint8 CAEUserRadioTrackManager::GetUserTrackPlayMode() {
    return (uint8)FrontEndMenuManager.m_RadioMode;
}

// 0x4f4a20
/**
 * Worker que reconstruye sa-ufiles.dat y sa-utrax.dat a partir de User Tracks.
 *
 * La compatibilidad con el vector/CRT de VC6 obliga a usar los alloc/free del EXE
 * para los registros que cruzan esa frontera ABI.
 */
DWORD __stdcall CAEUserRadioTrackManager::WriteUserTracksThread(CAEUserRadioTrackManager* self) {
    if (notsa::bugfixes::CAEUserRadioTrackManager_UnicodeUserTracks) {
        self->WriteUserTracksThread_Unicode();
        return 0;
    }

    CoInitialize(nullptr);

    UserTracksLegacyVector offsets;
    uint32 length = 0;

    CFileMgr::SetDirMyDocuments();
    const auto ufiles = CFileMgr::OpenFile("sa-ufiles.dat", "wb");
    if (!ufiles) {
        // Como el original: sale sin volver al directorio raíz ni llamar a CoUninitialize.
        self->m_nUserTracksScanState = USER_TRACK_SCAN_ERROR;
        return 0;
    }

    char* dir = new char[lstrlenA(CFileMgr::ms_dirName) + 15];
    lstrcpyA(dir, CFileMgr::ms_dirName);
    lstrcatA(dir, "\\User Tracks\\");
    const auto amountOfTracks = self->WriteUserTracksFile(dir, offsets, ufiles, length, 0);
    delete[] dir;
    CFileMgr::CloseFile(ufiles);

    // Como el original: no comprueba que sa-utrax.dat se abra, escribe dos veces el registro cuando solo hay uno
    // y libera cada registro justo después de escribirlo.
    const auto utrax = CFileMgr::OpenFile("sa-utrax.dat", "wb");
    if (amountOfTracks == 1) {
        CFileMgr::Write(utrax, offsets.first[0], sizeof(tUserTracksInfo));
    }
    for (uint32 i = 0; offsets.first && i < static_cast<uint32>(offsets.last - offsets.first); i++) {
        CFileMgr::Write(utrax, offsets.first[i], sizeof(tUserTracksInfo));
        operator delete(offsets.first[i]);
    }
    CFileMgr::CloseFile(utrax);
    CFileMgr::SetDir("");

    FrontEndMenuManager.m_UserTrackIndex = 0;
    self->m_nUserTracksScanState = USER_TRACK_SCAN_COMPLETE;
    CoUninitialize();

    if (offsets.first) {
        operator delete(offsets.first);
    }
    return 0;
}

// NOTSA: variante UTF-8 del hilo (bugfix CAEUserRadioTrackManager_UnicodeUserTracks).
void CAEUserRadioTrackManager::WriteUserTracksThread_Unicode() {
    VERIFY(SUCCEEDED(CoInitialize(nullptr)));

    // Open sa-ufiles.dat
    CFileMgr::SetDirMyDocuments();

    if (const auto ufiles = CFileMgr::OpenFile("sa-ufiles.dat", "wb")) {
        uint32 dummy{};
        UserTracksLegacyVector offsets;

        // Use the executable's ABI so the worker and scanner can be switched
        // independently. Each record and the vector storage use its CRT.
        const auto directory = UnicodeToUTF8((fs::path(CFileMgr::ms_dirName) / "User Tracks").wstring());
        const auto amountOfTracks = WriteUserTracksFile(directory.c_str(), offsets, ufiles, dummy, 0);
        CFileMgr::CloseFile(ufiles);

        // Open sa-utrax.dat containing the offsets
        if (const auto utrax = CFileMgr::OpenFile("sa-utrax.dat", "wb")) {
            if (amountOfTracks == 1) {
                CFileMgr::Write(utrax, *offsets.first, sizeof(tUserTracksInfo));
            }
            for (auto** entry = offsets.first; entry != offsets.last; ++entry) {
                CFileMgr::Write(utrax, *entry, sizeof(tUserTracksInfo));
            }

            CFileMgr::CloseFile(utrax);

            FrontEndMenuManager.m_UserTrackIndex = 0;
            m_nUserTracksScanState = USER_TRACK_SCAN_COMPLETE;

        } else {
            // NOTSA(MikuAuahDark): GTASA doesn't check if sa-utrax.dat fails to open
            m_nUserTracksScanState = USER_TRACK_SCAN_ERROR;
        }
        for (auto** entry = offsets.first; entry != offsets.last; ++entry) {
            plugin::Call<0x8214BD>(*entry);
        }
        plugin::Call<0x8214BD>(offsets.first);
    } else {
        m_nUserTracksScanState = USER_TRACK_SCAN_ERROR;
    }

    // NOTSA: Read user tracks back
    m_bUserTracksLoaded = ReadUserTracks();
    m_bUserTracksLoadedCopy = m_bUserTracksLoaded;

    CFileMgr::SetDir("");
    CoUninitialize();
}

// 0x4f4690
/**
 * Recorre `dir` (FindFirstFileA "\*.*") y añade a sa-ufiles.dat cada archivo de audio reconocido.
 *
 * Como el original: se salta todo lo que empieza por '.', baja a las carpetas con profundidad + 1 (hasta 15) y, solo
 * en el primer nivel, resuelve los .lnk (si apuntan a una carpeta la recorre con profundidad 1). La longitud de cada
 * ruta se guarda en 16 bits (movzx) y cada registro se reserva con el operator new del EXE y se añade con el
 * push_back del vector VC6 (0x4F4620).
 */
int32 CAEUserRadioTrackManager::WriteUserTracksFile(const char* dir, UserTracksLegacyVector& offsets, FILE* file, uint32& currentLength, int32 depth) {
    if (notsa::bugfixes::CAEUserRadioTrackManager_UnicodeUserTracks) {
        return WriteUserTracksFile_Unicode(dir, offsets, file, currentLength, depth);
    }

    if (depth >= 15)
        return 0;

    int32 numTracks = 0;

    // Builds `dir` + separator + `name` with the original's buffer size.
    const auto JoinPath = [dir](const char* name) {
        char* path = new char[lstrlenA(dir) + lstrlenA(name) + 2];
        lstrcpyA(path, dir);
        lstrcatA(path, "\\");
        lstrcatA(path, name);
        return path;
    };
    // Records the track: start offset, 16-bit length and type, the path bytes and the vector entry.
    const auto AddTrack = [&](const char* path) {
        const uint32 pathLen = static_cast<uint16>(lstrlenA(path));
        auto* record = static_cast<tUserTracksInfo*>(operator new(sizeof(tUserTracksInfo)));
        record->startUfilesPath = currentLength;
        record->pathLength      = pathLen;
        record->fileType        = GetAudioFileType(path);
        CFileMgr::Write(file, path, pathLen);
        return std::pair{ record, pathLen };
    };
    const auto PushBack = [&offsets](tUserTracksInfo* record) {
        plugin::CallMethod<0x4F4620, UserTracksLegacyVector*, tUserTracksInfo**>(&offsets, &record);
    };

    char* pattern = new char[lstrlenA(dir) + 6];
    lstrcpyA(pattern, dir);
    lstrcatA(pattern, "\\*.*");
    WIN32_FIND_DATAA findData;
    const HANDLE find = FindFirstFileA(pattern, &findData);
    delete[] pattern;
    if (find == INVALID_HANDLE_VALUE)
        return 0;

    do {
        if (findData.cFileName[0] == '.')
            continue;

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char* path = JoinPath(findData.cFileName);
            numTracks += WriteUserTracksFile(path, offsets, file, currentLength, depth + 1);
            delete[] path;
            continue;
        }

        const char* dot = depth == 0 ? strrchr(findData.cFileName, '.') : nullptr;
        if (dot && _stricmp(dot, ".lnk") == 0) {
            char* link = JoinPath(findData.cFileName);
            char* target = ResolveShortcut(link);
            delete[] link;
            if (!target)
                continue;

            const DWORD attributes = GetFileAttributesA(target);
            if (attributes != INVALID_FILE_ATTRIBUTES) {
                if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
                    numTracks += WriteUserTracksFile(target, offsets, file, currentLength, 1);
                } else if (GetAudioFileType(target) != AUDIO_FILE_TYPE_UNKNOWN) {
                    const auto [record, pathLen] = AddTrack(target);
                    PushBack(record);
                    numTracks++;
                    currentLength += pathLen;
                }
            }
            delete[] target;
            continue;
        }

        if (GetAudioFileType(findData.cFileName) != AUDIO_FILE_TYPE_UNKNOWN) {
            char* path = JoinPath(findData.cFileName);
            const auto [record, pathLen] = AddTrack(path);
            delete[] path;
            PushBack(record);
            numTracks++;
            currentLength += pathLen;
        }
    } while (FindNextFileA(find, &findData));

    FindClose(find);
    return numTracks;
}

// NOTSA: variante UTF-8 de 0x4F4690. Mantiene la ABI del vector VC6 y traduce internamente desde std::vector.
int32 CAEUserRadioTrackManager::WriteUserTracksFile_Unicode(const char* dir, UserTracksLegacyVector& offsets, FILE* file, uint32& currentLength, int32 depth) {
    std::vector<tUserTracksInfo> found;
    size_t length = currentLength;
    const auto count = WriteUserTracksFile(UTF8ToUnicode(dir), length, file, found, depth);
    currentLength = static_cast<uint32>(length);
    for (const auto& track : found) {
        if (offsets.last == offsets.end) {
            const auto size = offsets.first ? offsets.last - offsets.first : 0;
            const auto capacity = std::max<ptrdiff_t>(1, size * 2);
            auto** storage = plugin::CallAndReturn<tUserTracksInfo**, 0x821195>(size_t(capacity) * sizeof(tUserTracksInfo*));
            if (size) {
                std::copy_n(offsets.first, size, storage);
            }
            plugin::Call<0x8214BD>(offsets.first);
            offsets.first = storage;
            offsets.last = storage + size;
            offsets.end = storage + capacity;
        }
        // The native worker deletes both these records and the vector storage
        // with the executable's CRT; allocate them from that same CRT.
        auto* record = plugin::CallAndReturn<tUserTracksInfo*, 0x821195>(sizeof(tUserTracksInfo));
        *record = track;
        *offsets.last++ = record;
    }
    return count;
}

int32 CAEUserRadioTrackManager::WriteUserTracksFile(const std::wstring& dir, size_t& currentLength, auto file, std::vector<tUserTracksInfo>& offsets, int32 depth) {
    // Limit folder scan to 16 folders deep
    if (depth >= 15 || !fs::exists(dir))
        return 0;

    int32 numTracks{};
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().filename().native().starts_with(L'.')) {
            continue;
        }
        if (fs::is_directory(entry)) {
            // NOTE: STD can also do recursive search, maybe use that?
            numTracks += WriteUserTracksFile(entry.path(), currentLength, file, offsets, depth + 1);
            continue;
        }

        auto path = entry.path();
#ifdef WIN32
        if (depth == 0 && _wcsicmp(path.extension().c_str(), L".lnk") == 0)
            path = ResolveShortcut(path.wstring());
#endif

        // A shortcut can refer to a directory as well as a file.
        if (fs::is_directory(path)) {
            numTracks += WriteUserTracksFile(path, currentLength, file, offsets, depth + 1);
            continue;
        }

        if (!fs::is_regular_file(path)) {
            continue;
        }

        // NOTSA: Use UTF-8 generic path (POSIX like) instead of Windows paths.
        const auto pathStr = UnicodeToUTF8(path.generic_wstring());
        const auto fileType = GetAudioFileType(pathStr.c_str());
        if (fileType != AUDIO_FILE_TYPE_UNKNOWN) {
            size_t pathLen = pathStr.length();
            CFileMgr::Write(file, pathStr.c_str(), pathLen);
            offsets.push_back({currentLength, pathLen, fileType});
            currentLength += pathLen;
            numTracks++;
        }
    }

    return numTracks;
}

// 0x4f30f0
/**
 * Resuelve un .lnk con IShellLinkA/IPersistFile.
 *
 * Como el original: la ruta se pasa a ancho con MultiByteToWideChar (CP_ACP), GetPath pide la ruta corta
 * (SLGP_SHORTPATH) en un buffer de MAX_PATH reservado con el new[] del EXE, y si GetPath falla devuelve nulo
 * sin liberar ese buffer.
 */
char* CAEUserRadioTrackManager::ResolveShortcut(const char* path) {
    if (notsa::bugfixes::CAEUserRadioTrackManager_UnicodeUserTracks) {
        return ResolveShortcut_Unicode(path);
    }

    IShellLinkA* shellLink;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (void**)&shellLink)))
        return nullptr;

    IPersistFile* persistFile;
    if (FAILED(shellLink->QueryInterface(IID_IPersistFile, (void**)&persistFile))) {
        shellLink->Release();
        return nullptr;
    }

    WCHAR widePath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, path, -1, widePath, MAX_PATH);
    if (FAILED(persistFile->Load(widePath, STGM_READ))) {
        persistFile->Release();
        shellLink->Release();
        return nullptr;
    }

    char* target = new char[MAX_PATH];
    WIN32_FIND_DATAA findData;
    const HRESULT result = shellLink->GetPath(target, MAX_PATH, &findData, SLGP_SHORTPATH);
    persistFile->Release();
    shellLink->Release();
    return FAILED(result) ? nullptr : target;
}

// NOTSA: variante UTF-8 de 0x4F30F0 (bugfix CAEUserRadioTrackManager_UnicodeUserTracks).
char* CAEUserRadioTrackManager::ResolveShortcut_Unicode(const char* path) {
    std::string str = UnicodeToUTF8(ResolveShortcut(UTF8ToUnicode(path)));
    char*       result = plugin::CallAndReturn<char*, 0x821195>(str.length() + 1);

    memcpy(result, str.c_str(), str.length() + 1);
    return result;
}

std::wstring CAEUserRadioTrackManager::ResolveShortcut(const std::wstring& path) {
    IShellLinkW* shellLink{};
    IPersistFile* persistFile{};

    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&shellLink))) {
        NOTSA_UNREACHABLE("CoCreateInstance(CLSID_ShellLink) failed");
    }

    if (FAILED(shellLink->QueryInterface(IID_IPersistFile, (void**)&persistFile))) {
        NOTSA_LOG_ERR("QueryInterface(IID_IPersistFile) failed.");
        shellLink->Release();
        return {};
    }

    std::wstring target(MAX_PATH, L'\0');
    WIN32_FIND_DATAW findData{};
    if (FAILED(persistFile->Load(path.c_str(), STGM_READ)) || FAILED(shellLink->GetPath(target.data(), MAX_PATH, &findData, 0))) {
        persistFile->Release();
        shellLink->Release();
        NOTSA_UNREACHABLE("Load or GetPath failed");
    }

    persistFile->Release();
    shellLink->Release();

    target.resize(wcslen(target.c_str()));
    return target;
}

void CAEUserRadioTrackManager::InjectHooks() {
    RH_ScopedClass(CAEUserRadioTrackManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Initialise, 0x4f35b0);
    RH_ScopedInstall(Shutdown, 0x4f2fb0);
    RH_ScopedInstall(ReadUserTracks, 0x4f2fd0);
    RH_ScopedInstall(GetTrackPath, 0x4f3050);
    RH_ScopedOverloadedInstall(ResolveShortcut, "", 0x4f30f0, char* (CAEUserRadioTrackManager::*)(const char*));
    RH_ScopedInstall(GetAudioFileType, 0x4f31f0);
    RH_ScopedInstall(SelectUserTrackIndex, 0x4f3250);
    RH_ScopedInstall(DeleteUserTracksInfo, 0x4f32f0);
    RH_ScopedInstall(IsShortcut, 0x4f32c0);
    RH_ScopedInstall(LoadUserTrack, 0x4f35f0);
    RH_ScopedInstall(GetUserTrackPlayMode, 0x4f3330);
    RH_ScopedInstall(SetUserTrackIndex, 0x4f3340);
    RH_ScopedOverloadedInstall(WriteUserTracksFile, "", 0x4f4690, int32 (CAEUserRadioTrackManager::*)(const char*, UserTracksLegacyVector&, FILE*, uint32&, int32));
    RH_ScopedInstall(WriteUserTracksThread, 0x4f4a20);
    RH_ScopedInstall(ScanUserTracks, 0x4f4ba0);
}
