/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAETrackLoader
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "AETrackLoader.h"
#include "FileMgr.h"

/**
 * Registra la creación de CAEDataStream para tracks empaquetados.
 */
void CAETrackLoader::InjectHooks() {
    RH_ScopedClass(CAETrackLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(GetDataStream, 0x4E0D20);
}

/**
 * Construye un stream lógico sobre la sección de audio de un track.
 *
 * Salta sizeof(tTrackInfo) porque esa cabecera se consume por separado. La cadena
 * path pasa a ser propiedad de CAEDataStream, que también aplica el transformador
 * al tratar estos tracks como cifrados.
 */
CAEDataStream* CAETrackLoader::GetDataStream(uint32 trackId) {
    if (trackId >= m_nTrackCount)
        return nullptr;

    const auto packId = m_paTrackLookups[trackId].m_nbPackId;
    if (packId > m_nStreamPackCount)
        return nullptr;

    const auto length = [&] {
        if (m_bStreamingFromDVD) {
            return strlen(m_pszDvdDrivePath) + strlen(m_paStreamPacks[packId].m_szName) + 21;
        }
        return strlen(m_paStreamPacks[packId].m_szName) + 21;
    }();

    char* path = new char[length]; // owned by to be ctor'd CAEDataStream.
    strcpy_s(path, length,
        std::format("{}\\{}",
            !m_bStreamingFromDVD ? "AUDIO\\STREAMS\\" : std::string{m_pszDvdDrivePath} + "AUDIO\\STREAMS\\",
            m_paStreamPacks[packId].m_szName
        ).c_str()
    );

    auto* stream = new CAEDataStream(
        trackId,
        path,
        m_paTrackLookups[trackId].m_nOffset + sizeof(tTrackInfo),
        m_paTrackLookups[trackId].m_nSize,
        true
    );

    CFileMgr::SetDir("");
    if (!stream->Initialise()) {
        delete stream;
        stream = nullptr;
    }

    return stream;
}
