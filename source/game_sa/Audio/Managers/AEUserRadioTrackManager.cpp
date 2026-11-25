/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAEUserRadioTrackManager
 * Reconstrucción: COMPLETA — 16/16 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 16/16 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: LegacyRuntimeError
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: LegacyStdString
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: UserTracksLegacyString
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: UserTracksLegacyVector
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h" // TODO: Remove

// Definida en Attractors/PedAttractor.cpp (insert(pos, n, valor) de un vector de elementos de 4 bytes de MSVC 6).
void SArrayInsertN4(void* vector, uint32* pos, uint32 count, const uint32* value);
#include "DatosExe/RdataCadenas.h"

#include <cstdio>

// COM
#include <ShObjIdl.h>

#include "common.h"

#include "AEUserRadioTrackManager.h"

#include "AEAudioUtility.h"
#include "AEDataStream.h"
#include "AEMFDecoder.h"
#include "AEWaveDecoder.h"
#include "AEWaveDecoderEx.h"
#include "extensions/SilentPatchConfig.h"
#include "AEWMADecoder.h"
#include "AEVorbisDecoder.h"

#include "FileMgr.h"
#include "MenuManager.h"
#include "game_sa/Crt/CrtException.h"

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
 *
 * @addr 0x4F35B0
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
 *
 * @addr 0x4F3050
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
 *
 * @addr 0x4F35F0
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
        // SilentPatch (MIT, Silent): WaveProfilesFix: PCM de 8/16/24 bits, mono/estereo, hasta 48 kHz y WAV con chunks ID3/LIST; el WAV canonico de 44 bytes sigue por el decoder original
        if (SilentPatchOption("WaveProfilesFix", true) && !CAEWaveDecoderEx::IsCanonicalHeader(dataStream)) {
            return new CAEWaveDecoderEx(dataStream);
        }
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

    // SilentPatch (MIT, Silent): UserTracksFLAC: ".flac" (8/16/24 bits, mono/estereo, hasta 48 kHz) se decodifica con Media Foundation (CAEMFDecoder), que lee FLAC de forma nativa desde Windows 10
    if (_stricmp(dotPosition, ".flac") == 0 && SilentPatchOption("UserTracksFLAC", true)) {
        return AUDIO_FILE_TYPE_QUICKTIME;
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
 *
 * @addr 0x4F4A20
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
            ::operator delete(*entry);
        }
        ::operator delete(offsets.first);
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
 * push_back del vector VC6 (UserTracksLegacyVector::PushBack, 0x4F4620).
 *
 * @addr 0x4F4690
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
        offsets.PushBack(record);
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
            auto** storage = static_cast<tUserTracksInfo**>(::operator new[](size_t(capacity) * sizeof(tUserTracksInfo*)));
            if (size) {
                std::copy_n(offsets.first, size, storage);
            }
            ::operator delete(offsets.first);
            offsets.first = storage;
            offsets.last = storage + size;
            offsets.end = storage + capacity;
        }
        // The native worker deletes both these records and the vector storage
        // with the executable's CRT; allocate them from that same CRT.
        auto* record = static_cast<tUserTracksInfo*>(::operator new[](sizeof(tUserTracksInfo)));
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
 *
 * @addr 0x4F30F0
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
    char*       result = static_cast<char*>(::operator new[](str.length() + 1));

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
        NOTSA_LOG_ERR("QueryInterface(IID_IPersistFile) falló.");
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

namespace {
// Lote 209A (sin validar): bloque de la STL de MSVC 9 que el EXE enlaza junto a CAEUserRadioTrackManager (0x4F3A3E..0x4F4D10); el listado lo atribuia entero a esa clase. NO son metodos suyos: son std::string (+4 buffer de 16 bytes o puntero,
// +0x14 longitud, +0x18 capacidad; el campo +0 no se escribe), std::runtime_error (std::exception de 12 bytes con un std::string en +0xC; vtables 0x887D5C y, la derivada length_error, 0x887D68) y las plantillas de std::vector de punteros
// (ver UserTracksLegacyVector). Las funciones de std::string se reconocen por la comparacion `cmp [x+18h],10h` (capacidad >= 16 -> datos por puntero). Colaboradores sin reconstruir, llamados por direccion fija: CRT 0x82119A operator new,
// 0x8214BD operator delete (::operator new / delete del proyecto), 0x821255 std::string::_Xlen, 0x8211FD std::string::_Xran, 0x822A20 memmove, 0x822D97 / 0x822DE5 / 0x822E2F constructor, constructor de copia y destructor de
// std::exception, 0x822D5D _CxxThrowException; std::string::_Copy 0x4F39C0 (la contienen 0x4F3A3E y 0x4F3AAD).
struct LegacyString {
    uint32 allocator; // +0, sin inicializar por los constructores
    union {
        char  sso[16];
        char* ptr;
    };
    uint32 size; // +0x14
    uint32 res;  // +0x18, capacidad (0xF = buffer interno)

    [[nodiscard]] char* Data() { return res >= 0x10 ? ptr : sso; }
    [[nodiscard]] const char* Data() const { return res >= 0x10 ? ptr : sso; }
};
VALIDATE_SIZE(LegacyString, 0x1C);

struct LegacyRuntimeError {
    uint32       vtable;  // +0
    const char*  what;    // +4  (std::exception::_m_what)
    uint32       doFree;  // +8
    LegacyString message; // +0xC
};
VALIDATE_SIZE(LegacyRuntimeError, 0x28);

// `rep movsd` (n / 4 dwords) y `rep movsb` (n % 4 bytes), como el EXE (sin superposicion en estos usos).
void CopyBytesLikeExe(char* dst, const char* src, uint32 n) {
    __movsd(reinterpret_cast<unsigned long*>(dst), reinterpret_cast<const unsigned long*>(src), n >> 2);
    __movsb(reinterpret_cast<unsigned char*>(dst) + (n & ~3u), reinterpret_cast<const unsigned char*>(src) + (n & ~3u), n & 3u);
}

[[noreturn]] void StringXlen() { // 0x821255, no vuelve
    ExeCrt::StringXlen(); // CRT3A: CrtException.cpp
    __assume(0);
}
[[noreturn]] void StringXran() { // 0x8211FD, no vuelve
    ExeCrt::StringXran(); // CRT3A: CrtException.cpp
    __assume(0);
}

// 0x4F39C0 (no listada; la contienen 0x4F3A3E y 0x4F3AAD, trozos con el marco EBP de esa funcion y su manejador de excepciones): std::string::_Copy(nuevaCapacidad, longitudAnterior), thiscall, `ret 8`.
void StringCopyGrow(LegacyString* self, uint32 newSize, uint32 oldLength) {
    reinterpret_cast<UserTracksLegacyString*>(self)->Reallocate(newSize, oldLength); // mismo tipo de 0x1C bytes (std::string de MSVC 2010)
}

// 0x4F3BA0 (lote 209A, sin validar; 35 bytes, thiscall sin argumentos, `ret`): std::string::_Tidy(): con capacidad >= 16 libera el buffer con operator delete (0x8214BD); despues capacidad = 0xF, longitud = 0 y primer byte del buffer
// interno = 0 (esto ultimo escribe en el primer byte de la union aunque el puntero acabe de liberarse).
void __fastcall StringTidy(LegacyString* self, void* /*edx*/) {
    if (self->res >= 0x10) {
        ::operator delete(self->ptr);
    }
    self->res    = 0xF;
    self->size   = 0;
    self->sso[0] = 0;
}

// 0x4F3BD0 (lote 209A, sin validar; 190 bytes, thiscall con (nuevoTamano, vaciar), `ret 8`; el BYTE `vaciar` se lee de la pila): std::string::_Grow. Devuelve (nuevoTamano != 0) en EAX (0 o 1).
//  - nuevoTamano > 0xFFFFFFFE (sin signo): std::string::_Xlen (0x821255).
//  - capacidad < nuevoTamano: _Copy (0x4F39C0) con (nuevoTamano, vaciar ? 0 : longitud actual).
//  - si no, con vaciar y nuevoTamano < 16: libera si habia buffer, capacidad 0xF, longitud 0, primer byte 0.
//  - si no, con nuevoTamano == 0: longitud 0 y terminador en la posicion 0 del buffer activo; devuelve 0.
bool __fastcall StringGrow(LegacyString* self, void* /*edx*/, uint32 newSize, bool trim) {
    if (newSize > 0xFFFFFFFEu) {
        StringXlen();
    }
    if (self->res < newSize) {
        StringCopyGrow(self, newSize, trim ? 0 : self->size);
        return newSize != 0;
    }
    if (trim && newSize < 0x10) {
        if (self->res >= 0x10) {
            ::operator delete(self->ptr);
        }
        self->res    = 0xF;
        self->size   = 0;
        self->sso[0] = 0;
        return newSize != 0;
    }
    if (newSize == 0) {
        self->size      = 0;
        self->Data()[0] = 0;
        return false;
    }
    return true;
}

// 0x4F3DB0 (lote 209A, sin validar; 181 bytes, thiscall con (posicion, cantidad), `ret 8`): std::string::erase(pos, n). pos > longitud: std::string::_Xran (0x8211FD). n se limita a longitud - pos (sin signo). Si el n efectivo es > 0: memmove
// (0x822A20) de la cola sobre la posicion (sin el terminador), nueva longitud = longitud - n; si es > 0xFFFFFFFE: _Xlen; si la capacidad no la alcanza: _Copy(nuevaLongitud, longitud anterior) y despues longitud y terminador; con nueva
// longitud 0 solo pone longitud 0 y terminador en el inicio del buffer activo. Devuelve this.
LegacyString* __fastcall StringErase(LegacyString* self, void* /*edx*/, uint32 pos, uint32 n) {
    if (self->size < pos) {
        StringXran();
    }
    const uint32 remaining = self->size - pos;
    const uint32 count     = remaining < n ? remaining : n;
    if (count > 0) {
        char* const base = self->Data();
        std::memmove(base + pos, base + pos + count, remaining - count);
        const uint32 newSize = self->size - count;
        if (newSize > 0xFFFFFFFEu) {
            StringXlen();
        }
        if (self->res < newSize) {
            StringCopyGrow(self, newSize, self->size);
        } else if (newSize == 0) {
            self->size      = 0;
            self->Data()[0] = 0;
            return self;
        }
        self->size            = newSize;
        self->Data()[newSize] = 0;
    }
    return self;
}

// 0x4F3E80 (lote 209A, sin validar; 166 bytes, thiscall con (cadena, posicion, cantidad), `ret 0Ch`): std::string::assign(const string&, pos, n). pos > longitud de la otra: _Xran. n se limita a su longitud - pos. Si es la misma cadena:
// erase(pos + n, -1) y erase(0, pos) (0x4F3DB0). Si no: _Grow(n, true) (0x4F3BD0) y, si devuelve cierto, copia n bytes (rep movsd/movsb) desde su buffer + pos, longitud = n y terminador. Devuelve this.
LegacyString* __fastcall StringAssignString(LegacyString* self, void* /*edx*/, const LegacyString* str, uint32 pos, uint32 n) {
    if (str->size < pos) {
        StringXran();
    }
    uint32 len = str->size - pos;
    if (n < len) {
        len = n;
    }
    if (self == str) {
        StringErase(self, nullptr, pos + len, 0xFFFFFFFFu);
        StringErase(self, nullptr, 0, pos);
        return self;
    }
    if (StringGrow(self, nullptr, len, true)) {
        CopyBytesLikeExe(self->Data(), str->Data() + pos, len);
        self->size        = len;
        self->Data()[len] = 0;
    }
    return self;
}

// 0x4F3F60 (lote 209A, sin validar; 159 bytes, thiscall con (texto, cantidad), `ret 8`): std::string::assign(const char*, n). Si el texto cae dentro del propio buffer (inicio del buffer <= texto < inicio + longitud, comparaciones sin signo)
// delega en assign(esta cadena, texto - buffer, n) (0x4F3E80). Si no: _Grow(n, true) y, si devuelve cierto, copia n bytes (rep movsd/movsb) al buffer activo, longitud = n y terminador. Devuelve this.
LegacyString* __fastcall StringAssignChars(LegacyString* self, void* /*edx*/, const char* text, uint32 n) {
    const uint32 data = reinterpret_cast<uint32>(self->Data());
    const uint32 from = reinterpret_cast<uint32>(text);
    if (!(data > from) && !(from >= self->size + data)) {
        return StringAssignString(self, nullptr, self, from - data, n);
    }
    if (StringGrow(self, nullptr, n, true)) {
        CopyBytesLikeExe(self->Data(), text, n);
        self->size      = n;
        self->Data()[n] = 0;
    }
    return self;
}

// 0x4F4170 (lote 209A, sin validar; 57 bytes, thiscall con (texto), `ret 4`): std::string::string(const char*): capacidad 0xF, longitud 0, primer byte 0 (el campo +0 no se escribe); longitud del texto con un recorrido byte a byte y
// assign(texto, longitud) (0x4F3F60). Devuelve this.
LegacyString* __fastcall StringFromChars(LegacyString* self, void* /*edx*/, const char* text) {
    self->res    = 0xF;
    self->size   = 0;
    self->sso[0] = 0;
    const uint32 length = static_cast<uint32>(std::strlen(text));
    StringAssignChars(self, nullptr, text, length);
    return self;
}

// 0x4F4090 (lote 209A, sin validar; 95 bytes con marco SEH que solo destruye la base si la asignacion lanzara: omitido; thiscall con (const std::string&), `ret 4`): std::runtime_error::runtime_error(const string&):
// std::exception::exception() (0x822D97), vtable 0x887D5C, el string en +0xC con capacidad 0xF, longitud 0 y primer byte 0 y assign(mensaje, 0, -1) (0x4F3E80). Devuelve this.
LegacyRuntimeError* __fastcall RuntimeErrorFromString(LegacyRuntimeError* self, void* /*edx*/, const LegacyString* message) {
    reinterpret_cast<notsa::StdException*>(self)->Construct(); // CRT3A: CrtException.cpp
    self->vtable         = 0x887D5C;
    self->message.res    = 0xF;
    self->message.size   = 0;
    self->message.sso[0] = 0;
    StringAssignString(&self->message, nullptr, message, 0, 0xFFFFFFFFu);
    return self;
}

// 0x4F4230 (lote 209A, sin validar; 101 bytes con marco SEH que solo destruye la base: omitido; thiscall con (const runtime_error&), `ret 4`): constructor de copia de std::runtime_error: constructor de copia de std::exception (0x822DE5),
// vtable 0x887D5C, el string en +0xC vacio (capacidad 0xF, longitud 0, primer byte 0) y assign(string del otro, 0, -1). Devuelve this.
LegacyRuntimeError* __fastcall RuntimeErrorCopy(LegacyRuntimeError* self, void* /*edx*/, const LegacyRuntimeError* other) {
    reinterpret_cast<notsa::StdException*>(self)->CopyConstruct(*reinterpret_cast<const notsa::StdException*>(other)); // CRT3A: CrtException.cpp
    self->vtable         = 0x887D5C;
    self->message.res    = 0xF;
    self->message.size   = 0;
    self->message.sso[0] = 0;
    StringAssignString(&self->message, nullptr, &other->message, 0, 0xFFFFFFFFu);
    return self;
}

// 0x4F3D10 y 0x4F4130 (lote 209A, sin validar; 47 bytes cada una, mismos bytes salvo el destino del salto; thiscall sin argumentos, `ret`): cuerpo del destructor de std::runtime_error: vtable 0x887D5C, destruye el string (libera si la
// capacidad es >= 16; capacidad 0xF, longitud 0, primer byte 0) y salta (`jmp`) al destructor de std::exception (0x822E2F). Una sola funcion C++ para las dos direcciones.
void __fastcall RuntimeErrorDestructorBody(LegacyRuntimeError* self, void* /*edx*/) {
    self->vtable = 0x887D5C;
    if (self->message.res >= 0x10) {
        ::operator delete(self->message.ptr);
    }
    self->message.res    = 0xF;
    self->message.size   = 0;
    self->message.sso[0] = 0;
    reinterpret_cast<notsa::StdException*>(self)->Destruct(); // CRT3A: CrtException.cpp
}

// 0x4F3D50 (lote 209A, sin validar; 14 bytes, thiscall sin argumentos, `ret`): std::runtime_error::what(): c_str() del string en +0xC (puntero si la capacidad es >= 16, si no el buffer interno).
const char* __fastcall RuntimeErrorWhat(const LegacyRuntimeError* self, void* /*edx*/) {
    return self->message.Data();
}

// 0x4F3D60 y 0x4F4110 (lote 209A, sin validar; 27 bytes cada una, thiscall con el BYTE de banderas en la pila, `ret 4`): destructor con borrado de std::runtime_error (0x4F3D60, cuerpo 0x4F3D10) y de la clase derivada con vtable 0x887D68
// (0x4F4110, cuerpo 0x4F4130, el mismo codigo): cuerpo del destructor y, solo si el bit 0 de las banderas esta puesto, operator delete (0x8214BD). Devuelve this.
LegacyRuntimeError* __fastcall RuntimeErrorScalarDeletingDestructor(LegacyRuntimeError* self, void* /*edx*/, uint8 flags) {
    RuntimeErrorDestructorBody(self, nullptr);
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}

// 0x4F41B0 (lote 209A, sin validar; 122 bytes con marco SEH que destruye el string y el objeto temporales al desenrollar: aqui NO se destruyen, se pierden sus bloques de memoria dinamica; thiscall con ECX sin leer, sin argumentos, no vuelve):
// std::vector<T>::_Xlen(): construye un std::string con el texto de 0x860AB0 ("vector<T> too long"; longitud por recorrido byte a byte, assign 0x4F3F60), con el un std::runtime_error (0x4F4090), pone la vtable 0x887D68 (la clase
// derivada, std::length_error) y lanza el objeto con _CxxThrowException (0x822D5D, stdcall) y el ThrowInfo 0x893634.
[[noreturn]] void VectorXlenImpl(void* /*self*/) {
    LegacyString text;
    text.res    = 0xF;
    text.size   = 0;
    text.sso[0] = 0;
    const char* const message = DatosExe::Rdata::kS860AB0;
    StringAssignChars(&text, nullptr, message, static_cast<uint32>(std::strlen(message)));
    LegacyRuntimeError error;
    RuntimeErrorFromString(&error, nullptr, &text);
    error.vtable = 0x887D68;
    reinterpret_cast<void(__stdcall*)(void*, const void*)>(0x822D5D)(&error, reinterpret_cast<const void*>(0x893634));
    __assume(0);
}

// Cuerpo enganchado en 0x4F41B0 (lote 209A). El cuerpo vive en VectorXlenImpl, que no tiene hook: asi las instancias de abajo no dependen del estado del hook de esta funcion.
[[noreturn]] void __fastcall VectorXlen(void* self, void* /*edx*/) {
    VectorXlenImpl(self);
}

// Lote 219A: una instancia propia por direccion de hook (0x5ECAB0, 0x5ECB30, 0x5ECBB0, 0x5ECC30 y 0x5ECCB0; 0x4F41B0 conserva la funcion base): RH no admite dos hooks hacia la misma funcion de la DLL.
// Llaman a VectorXlenImpl (sin hook): con el hook de VectorXlen apagado, VectorXlen salta al codigo del EXE y la instancia no probaria el C++.
template<int Id>
[[noreturn]] void __fastcall VectorXlenInst(void* self, void* /*edx*/) {
    VectorXlenImpl(self);
}
} // namespace

// std::string / std::runtime_error del EXE para Crt/CrtException.cpp (lanzadores _Xran y _Xlen): construyen y destruyen con los cuerpos C++ de arriba, sin pasar por las direcciones del EXE.
void ExeStdStringFromChars(void* string, const char* text) {
    StringFromChars(static_cast<LegacyString*>(string), nullptr, text); // 0x4F4170
}
void ExeStdRuntimeErrorFromString(void* error, const void* string) {
    RuntimeErrorFromString(static_cast<LegacyRuntimeError*>(error), nullptr, static_cast<const LegacyString*>(string)); // 0x4F4090
}
void ExeStdStringTidy(void* string) {
    StringTidy(static_cast<LegacyString*>(string), nullptr); // 0x4F3BA0
}

// std::vector<T>::_Xlen del EXE para PedAttractor.cpp (sus instancias de la STL de MSVC 6 lo lanzan sin pasar por la direccion del EXE); lanza el length_error del original.
[[noreturn]] void ExeVectorXlen() {
    VectorXlenImpl(nullptr);
}

// 0x4F4320 (lote 209A, sin validar; 645 bytes con marco SEH que solo libera el bloque nuevo si una copia lanzara: omitido; thiscall con (posicion, cantidad, const T&), `ret 0Ch`): std::vector<T*>::insert(pos, n, valor) de VC9 (vector de
// punteros de 4 bytes; disposicion +4 inicio, +8 fin, +0xC limite). Como el original:
//  - El valor se copia a una variable local al entrar (su direccion es el argumento de relleno de todas las llamadas). capacidad = inicio ? (limite - inicio) >> 2 : 0 (desplazamiento con signo). Con n == 0 no hace nada.
//  - tamano = inicio ? (fin - inicio) >> 2 : 0; si (0x3FFFFFFF - tamano) < n (sin signo): VectorXlen (0x4F41B0).
//  - Capacidad insuficiente (capacidad < tamano + n): nueva = (0x3FFFFFFF - capacidad / 2 < capacidad) ? 0 : capacidad + capacidad / 2; si es menor que tamano + n, la nueva es tamano + n; reserva nueva * 4 con operator new (0x82119A);
//    copia [inicio, pos) (0x4F3940), rellena n con el valor (0x4F3B30), copia [pos, fin) detras (0x4F3940), libera el bloque viejo (0x8214BD) y coloca inicio, fin = inicio + (n + tamano anterior) y limite = inicio + nueva * 4.
//  - Con capacidad: si hay menos de n elementos tras pos, copia la cola a pos + n (0x4F3CE0), rellena n - cola desde fin (0x4F3D80), fin += n y rellena [pos, fin anterior) (0x4F3AE0); si no, copia los ultimos n a fin (0x4F3CE0),
//    fin = su retorno, copia hacia atras [pos, fin anterior - n) hasta fin anterior (0x4F3B00) y rellena [pos, pos + n) (0x4F3AE0).
void UserTracksLegacyVector::Insert(tUserTracksInfo** pos, uint32 count, tUserTracksInfo* const& value) {
    // El cuerpo es el de la instancia de PedAttractor.cpp (misma plantilla de MSVC con elementos de 4 bytes): se reutiliza sin pasar por las direcciones del EXE.
    SArrayInsertN4(this, reinterpret_cast<uint32*>(pos), count, reinterpret_cast<const uint32*>(&value));
}

// 0x4F4620: push_back del vector VC6 de elementos de 4 bytes (thiscall, ECX = vector, un argumento en pila: referencia al valor, ret 4). Con el almacenamiento reservado y
// sitio libre (first != 0 y (last - first) / 4 < (end - first) / 4, cocientes con signo comparados sin signo) construye el elemento en `last` con 0x4F3B30 (cdecl:
// destino, 1, &valor, this, &valor) y guarda en last la direccion leida ANTES de la llamada mas 4; si no (sin almacenamiento o lleno) llama a insert(last, 1, &valor)
// del EXE (0x4F4320, thiscall, ret 0Ch), que reserva con el operator new del EXE y recoloca los punteros. Ningun llamador del EXE lee EAX/ECX/EDX.
void UserTracksLegacyVector::PushBack(tUserTracksInfo* const& value) {
    tUserTracksInfo** const begin = first;
    int32 size = 0;
    if (begin) {
        size = static_cast<int32>(reinterpret_cast<uint32>(last) - reinterpret_cast<uint32>(begin)) >> 2;
    }
    if (begin) {
        const int32 capacity = static_cast<int32>(reinterpret_cast<uint32>(end) - reinterpret_cast<uint32>(begin)) >> 2;
        if (static_cast<uint32>(size) < static_cast<uint32>(capacity)) {
            tUserTracksInfo** const slot = last;
            *slot = value; // construye el elemento en el fin (el destino no es nulo)
            last = slot + 1;
            return;
        }
    }
    Insert(last, 1, value);
}

// 0x4F3940 (lote 208A, sin validar; cdecl, tres argumentos (origen, fin, destino) mas argumentos de relleno de la plantilla que no lee, `ret`, 37 bytes): std::uninitialized_copy de punteros de 4 bytes de
// la STL de MSVC 6 de los vectores del gestor de user tracks. Con origen == fin devuelve el destino sin tocar nada; si no, por cada elemento escribe `*destino = *origen` solo si el destino NO es nulo
// (new(puntero) T(valor)) y avanza SIEMPRE ambos; devuelve el final del destino (EAX). Mismo cuerpo que SArrayUninitCopyStride4 (0x5EAE10) de PedAttractor.cpp, que es una funcion anonima de ese archivo.
static uint32* __cdecl UserTracksUninitCopy(const uint32* first, const uint32* last, uint32* dest) {
    if (first == last) {
        return dest;
    }
    do {
        if (dest) {
            *dest = *first;
        }
        ++first;
        ++dest;
    } while (first != last);
    return dest;
}

// 0x4F39C0 (lote 208A, sin validar; thiscall, dos argumentos de pila (nuevo tamano, longitud actual), `ret 8`; el listado daba 85 bytes porque el EXE parte la funcion en tres trozos no contiguos: el cuerpo
// 0x4F39C0..0x4F3A14, el handler de catch(...) 0x4F3A15 y la continuacion 0x4F3A3E..0x4F3AAA, mas el desenlace 0x4F3AAD): std::string::_Copy del EXE. Capacidad nueva = nuevoTamano | 0xF (si pasa de
// 0xFFFFFFFE se queda en nuevoTamano) y se reservan capacidad + 1 bytes con el operator new DEL EXE (0x82119A). Si esa reserva lanza: capacidad = nuevoTamano y se reserva de nuevo; si tambien lanza se
// hace _Tidy(true) en linea (libera el puntero si la capacidad >= 16 con operator delete del EXE 0x8214BD, capacidad = 0xF, tamano = 0, buf[0] = 0) y se relanza. Despues: si la longitud actual > 0 (sin signo) copia esa longitud desde
// el bufer actual (puntero si capacidad >= 16, si no el bufer interno) con `rep movsd` + `rep movsb`; libera el bloque anterior si la capacidad actual >= 16; guarda el puntero nuevo en +4
// (el EXE escribe antes buf[0] = 0, que el dword pisa), capacidad en +0x18, tamano (la longitud actual) en +0x14 y escribe el cero final en [capacidad >= 16 ? puntero nuevo : buf][longitud]. El EXE tiene el
// handler de catch como funcion aparte (0x4F3A15) que no se puede enganchar: queda cubierto con este try/catch. Los marcos de SEH solo desenrollan.
void UserTracksLegacyString::Reallocate(uint32 newSize, uint32 oldLength) {
    uint32 newCapacity = newSize | 0xF;
    if (newCapacity > 0xFFFFFFFEu) {
        newCapacity = newSize;
    }

    char* newPtr;
    try {
        newPtr = ((char*(__cdecl*)(uint32))0x82119A)(newCapacity + 1);
    } catch (...) {
        newCapacity = newSize;
        try {
            newPtr = ((char*(__cdecl*)(uint32))0x82119A)(newCapacity + 1);
        } catch (...) {
            if (capacity >= 0x10) {
                ::operator delete(ptr);
            }
            capacity = 0xF;
            size     = 0;
            buf[0]   = 0;
            throw;
        }
    }

    if (oldLength > 0) {
        const char* const source = capacity >= 0x10 ? ptr : buf;
        std::memcpy(newPtr, source, oldLength);
    }
    if (capacity >= 0x10) {
        ::operator delete(ptr);
    }
    buf[0]   = 0;
    ptr      = newPtr;
    capacity = newCapacity;
    size     = oldLength;
    (newCapacity >= 0x10 ? newPtr : buf)[oldLength] = 0;
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
    RH_ScopedGlobalInstall(UserTracksUninitCopy, 0x4F3940); // lote 208A (sin validar)

    {
        RH_ScopedClass(UserTracksLegacyVector);
        RH_ScopedInstall(PushBack, 0x4F4620);
        RH_ScopedInstall(Insert, 0x4F4320); // lote 209A (sin validar)
    }

    // Lote 209A (sin validar): std::string y std::runtime_error de la STL de MSVC 9 enlazados junto a esta clase (ver el bloque de arriba); el listado los atribuia a CAEUserRadioTrackManager.
    {
        RH_ScopedNamespaceName("LegacyStdString");
        RH_ScopedNamedGlobalInstall(StringTidy, "Tidy", 0x4F3BA0);
        RH_ScopedNamedGlobalInstall(StringGrow, "Grow", 0x4F3BD0);
        RH_ScopedNamedGlobalInstall(StringErase, "Erase", 0x4F3DB0);
        RH_ScopedNamedGlobalInstall(StringAssignString, "AssignString", 0x4F3E80);
        RH_ScopedNamedGlobalInstall(StringAssignChars, "AssignChars", 0x4F3F60);
        RH_ScopedNamedGlobalInstall(StringFromChars, "ConstructFromChars", 0x4F4170);
    }
    {
        RH_ScopedNamespaceName("LegacyRuntimeError");
        RH_ScopedNamedGlobalInstall(RuntimeErrorDestructorBody, "DestructorBodyA", 0x4F3D10);
        RH_ScopedNamedGlobalInstall(RuntimeErrorWhat, "What", 0x4F3D50);
        RH_ScopedNamedGlobalInstall(RuntimeErrorScalarDeletingDestructor, "ScalarDeletingDestructorA", 0x4F3D60);
        RH_ScopedNamedGlobalInstall(RuntimeErrorFromString, "ConstructFromString", 0x4F4090);
        RH_ScopedNamedGlobalInstall(RuntimeErrorScalarDeletingDestructor, "ScalarDeletingDestructorB", 0x4F4110);
        RH_ScopedNamedGlobalInstall(RuntimeErrorDestructorBody, "DestructorBodyB", 0x4F4130);
        RH_ScopedNamedGlobalInstall(RuntimeErrorCopy, "CopyConstructor", 0x4F4230);
        RH_ScopedNamedGlobalInstall(VectorXlen, "VectorXlen", 0x4F41B0);
        // Lote 219A (sin validar): cinco instancias gemelas de vector::_Xlen junto a los vectores de CPedAttractor y de CPedAttractorManager (0x5ECAB0, 0x5ECB30, 0x5ECBB0, 0x5ECC30, 0x5ECCB0; el listado decia CPedAttractorManager).
        // Mismo texto (0x860AB0), mismas llamadas (0x4F3F60, 0x4F4090, vtable 0x887D68, ThrowInfo 0x893634, _CxxThrowException 0x822D5D); solo cambia la direccion de la tabla SEH. Cada direccion tiene su instancia propia de plantilla (VectorXlenInst<Id>) con el mismo cuerpo C++ que la de arriba.
        RH_ScopedNamedGlobalInstall(VectorXlenInst<1>, "VectorXlenB", 0x5ECAB0);
        RH_ScopedNamedGlobalInstall(VectorXlenInst<2>, "VectorXlenC", 0x5ECB30);
        RH_ScopedNamedGlobalInstall(VectorXlenInst<3>, "VectorXlenD", 0x5ECBB0);
        RH_ScopedNamedGlobalInstall(VectorXlenInst<4>, "VectorXlenE", 0x5ECC30);
        RH_ScopedNamedGlobalInstall(VectorXlenInst<5>, "VectorXlenF", 0x5ECCB0);
    }

    {
        RH_ScopedClass(UserTracksLegacyString);
        RH_ScopedInstall(Reallocate, 0x4F39C0); // lote 208A (sin validar); 0x4F3A15 es su handler de catch (no enganchable)
    }
}
