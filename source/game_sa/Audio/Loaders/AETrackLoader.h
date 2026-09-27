#pragma once
#include "AEDataStream.h"

/**
 * Localización de un track dentro de uno de los StreamPack.
 */
struct tTrackLookup {
    uint8 m_nbPackId;
    uint32 m_nOffset;
    uint32 m_nSize;
};
VALIDATE_SIZE(tTrackLookup, 0xC);

struct StreamPack {
    char m_szName[16];
};
VALIDATE_SIZE(StreamPack, 0x10);

/**
 * Metadata que precede a los bytes de audio de cada track.
 *
 * Incluye hasta 1000 marcas de beat y campos adicionales conservados por compatibilidad.
 */
struct tTrackInfo {
    struct tBeat {
        uint32 m_nTime{};
        uint32 m_nKey{};
    };
    std::array<tBeat, 1000>   m_aBeats;

    std::array<uint32, 16> field_1F40;
    uint16 field_1F80;
};
VALIDATE_SIZE(tTrackInfo, 0x1F84);
VALIDATE_SIZE(tTrackInfo::tBeat, 0x8);

/**
 * Base que resuelve track IDs a secciones de archivos AUDIO\\STREAMS.
 *
 * Las tablas m_paTrackLookups/m_paStreamPacks pertenecen a la subclase que las carga;
 * GetDataStream() usa esas tablas para construir una vista cifrada de un track concreto.
 */
class CAETrackLoader {
public:
    bool          m_bInitialised;
    uint32        m_nTrackCount;
    uint32        m_nStreamPackCount;
    tTrackLookup* m_paTrackLookups; //!< Tabla track -> pack/offset/tamaño.
    StreamPack*   m_paStreamPacks;  //!< Nombres de los paquetes físicos.
    bool          m_bStreamingFromDVD;
    char*         m_pszDvdDrivePath;

public:
    static void InjectHooks();

    CAEDataStream* GetDataStream(uint32 trackId);
};
VALIDATE_SIZE(CAETrackLoader, 0x1C);
