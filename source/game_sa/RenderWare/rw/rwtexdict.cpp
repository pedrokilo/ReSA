#include "StdInc.h"

#include "rwtexdict.h"
#include "rpanisot.h"
#include "X87Math.h"
#include <platform/win/WinPlatform.h>

// 0x734940: el EXE recorre la lista del diccionario con RwTexDictionaryForAllTextures (0x7F3730) y un callback (0x734930) que guarda la primera textura y devuelve 0
// (detiene el recorrido); es decir, devuelve la primera textura de la lista o nulo si esta vacia. Aqui se lee el primer enlace del centinela directamente, como
// RwTexDictionaryFindHashNamedTexture (RwHelper.cpp); no se escribe nada ni se toca estado de RenderWare. El hook esta en RwHelper.cpp.
RwTexture* GetFirstTexture(RwTexDictionary* txd) {
    auto* const sentinel = &txd->texturesInDict.link;
    auto* const first = sentinel->next;
    if (first == sentinel) {
        return nullptr;
    }
    return reinterpret_cast<RwTexture*>(reinterpret_cast<uint8*>(first) - offsetof(RwTexture, lInDictionary));
}

// 0x734930: callback de RwTexDictionaryForAllTextures que guarda la textura recibida en *data y devuelve nulo (detiene el recorrido).
RwTexture* GetFirstTextureCallback(RwTexture* texture, void* data) {
    *static_cast<RwTexture**>(data) = texture;
    return nullptr;
}

// Como el original: contadores de depuracion del tiempo medio de lectura de texturas (se actualizan solo con el juego en marcha, gGameState == 8).
static auto& s_textureReadCount   = StaticRef<int32>(0xC87FF0);
static auto& s_textureReadAverage = StaticRef<float>(0xC87FEC);

// 0x730E60: lee una textura nativa de un stream RenderWare. Busca el chunk rwID_TEXTURENATIVE (0x15) y llama a stdFunc[rwSTANDARDNATIVETEXTUREREAD] (RwEngineInstance + 0xB0) con
// (stream, &textura, longitud); si falla, nulo. Mide el tiempo: marca de inicio = CTimer::GetCurrentTimeInCycles (0x561A80) / GetCyclesPerMillisecond (0x561A40) como cociente
// float (Rt66TextureReadStartCpp), y con gGameState == 8 una vez leida (aunque la textura salga nula) actualiza la media incremental en 0xC87FEC / 0xC87FF0
// (Rt66TextureReadEndCpp, x87 a 24 bits). Luego, con textura: filtro NEAREST (1) -> LINEAR (2) y MIPNEAREST (3) -> MIPLINEAR (4) en el byte bajo de filterAddressing y,
// con anisotropia soportada, calidad de efectos >= alta (2) y anisotropia de la textura >= 1 (byte con signo), le pone la anisotropia maxima del dispositivo.
RwTexture* RwTextureGtaStreamRead(RwStream* stream) {
    static_assert(offsetof(RwGlobals, stdFunc) + rwSTANDARDNATIVETEXTUREREAD * sizeof(RwStandardFunc) == 0xB0);

    RwUInt32 length;
    RwUInt32 version;
    if (!RwStreamFindChunk(stream, rwID_TEXTURENATIVE, &length, &version)) {
        return nullptr;
    }

    const uint32 startTime    = CTimer::GetCurrentTimeInCycles();
    const uint32 startDivider = CTimer::GetCyclesPerMillisecond();
    const float  startRatio   = Rt66TextureReadStartCpp(startTime, startDivider);

    RwTexture* texture = nullptr; // Como el original: sin inicializar (la lectura nativa lo escribe)
    if (!RwEngineInstance->stdFunc[rwSTANDARDNATIVETEXTUREREAD](stream, &texture, static_cast<RwInt32>(length))) {
        return nullptr;
    }

    if (gGameState == 8) {
        const uint32 endTime    = CTimer::GetCurrentTimeInCycles();
        const uint32 endDivider = CTimer::GetCyclesPerMillisecond();
        Rt66TextureReadEndCpp(endTime, endDivider, startRatio, &s_textureReadCount, &s_textureReadAverage);
    }

    if (!texture) {
        return nullptr;
    }

    auto* const filter = reinterpret_cast<uint8*>(&texture->filterAddressing);
    if (*filter == rwFILTERNEAREST) {
        *filter = rwFILTERLINEAR;
    } else if (*filter == rwFILTERMIPNEAREST) {
        *filter = rwFILTERMIPLINEAR;
    }

    if (anisotropySupportedByGFX) {
        if (RpAnisotTextureGetMaxAnisotropy(texture) >= 1 && g_fx.GetFxQuality() >= FX_QUALITY_HIGH) {
            RpAnisotTextureSetMaxAnisotropy(texture, RpAnisotGetMaxSupportedMaxAnisotropy());
        }
    }
    return texture;
}

// 0x730FC0: lee un diccionario de texturas de GTA. Busca el chunk STRUCT (rwID_STRUCT = 1) y lee su contenido en un marco local de 12 bytes (palabra 0 = contador de texturas
// en su word bajo; palabra 1 = longitud del chunk; palabra 2 = version, que el EXE pasa a RwStreamFindChunk): si la lectura no devuelve exactamente la longitud, devuelve nulo. La longitud se
// RELEE del marco despues de leer (el EXE la recarga de la pila: una lectura de 5 a 12 bytes la pisa). Con una longitud mayor que el marco el EXE destroza su pila; aqui se lee a un bufer
// aparte y se copian los 12 primeros bytes. Luego crea el diccionario (nulo -> nulo) y lee `contador` texturas con RwTextureGtaStreamRead (0x730E60) anadiendolas una a una; si alguna
// falla, destruye las ya anadidas con RwTexDictionaryForAllTextures y el callback 0x730E50 (RwTextureDestroy y devuelve 0) y el diccionario, y devuelve nulo. El contador es un word
// (dec word ptr): sin texturas devuelve el diccionario vacio. Con una longitud de 0 o 1 el EXE usa bytes viejos de su pila para el contador: no es un caso definido.
RwTexDictionary* RwTexDictionaryGtaStreamRead(RwStream* stream) {
    struct { uint32 header; uint32 length; uint32 version; } frame{};
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &frame.length, &frame.version)) {
        return nullptr;
    }
    const uint32 length = frame.length;
    uint32       bytesRead;
    if (length <= sizeof(frame)) {
        bytesRead = RwStreamRead(stream, &frame, length);
    } else {
        std::vector<uint8> buffer(length);
        bytesRead = RwStreamRead(stream, buffer.data(), length);
        std::memcpy(&frame, buffer.data(), sizeof(frame));
    }
    if (bytesRead != frame.length) {
        return nullptr;
    }

    RwTexDictionary* const txd = RwTexDictionaryCreate();
    if (!txd) {
        return nullptr;
    }
    uint16 remaining = static_cast<uint16>(frame.header);
    if (remaining != 0) {
        do {
            --remaining;
            RwTexture* const texture = RwTextureGtaStreamRead(stream);
            if (!texture) {
                RwTexDictionaryForAllTextures(txd, reinterpret_cast<RwTextureCallBack>(0x730E50), nullptr);
                RwTexDictionaryDestroy(txd);
                return nullptr;
            }
            RwTexDictionaryAddTexture(txd, texture);
        } while (remaining != 0);
    }
    return txd;
}

// Estado de la lectura de un TXD en dos fases (el EXE reparte la carga en dos pasadas para no provocar un pico de fotograma).
static auto& s_txdTexturesLeft = StaticRef<int32>(0x8D6088); // 0x8D6088: texturas que faltan por leer (Read1 deja la mitad; Read2 las lee y lo deja a -1)
static auto& s_txdStreamPos    = StaticRef<int32>(0xC87FE8); // 0xC87FE8: posicion del stream (Type.memory.position, +0xC) al acabar Read1

static_assert(offsetof(RwStream, Type) == 0xC);

static uint32 GtaStreamPosition(const RwStream* stream) {
    return stream->Type.memory.position;
}

// 0x731070: primera mitad de la lectura de un TXD de GTA. Pone a 0 el contador 0x8D6088 y busca el chunk STRUCT (misma lectura del marco local de 12 bytes que RwTexDictionaryGtaStreamRead
// 0x730FC0: palabra 0 = contador de texturas (word bajo), palabra 1 = longitud, palabra 2 = version; la longitud se RELEE del marco tras leer). Crea el diccionario (nulo -> nulo), guarda en
// 0x8D6088 la mitad del contador (n >> 1) y lee texturas con RwTextureGtaStreamRead (0x730E60) anadiendolas una a una mientras las restantes (word, `dec ax`) sean MAS que 0x8D6088, que se
// RELEE en cada vuelta. Si una textura falla destruye las ya anadidas (ForAllTextures con el callback 0x730E50) y el diccionario y devuelve nulo (0x8D6088 queda en la mitad). Si acaba bien guarda la
// posicion del stream en 0xC87FE8, deja en 0x8D6088 las texturas restantes y devuelve el diccionario. Longitud 0 o 1: el EXE usa bytes viejos de su pila (no definido); con mas de 12 se lee a un bufer aparte.
RwTexDictionary* RwTexDictionaryGtaStreamRead1(RwStream* stream) {
    struct { uint32 header; uint32 length; uint32 version; } frame{};
    s_txdTexturesLeft = 0;
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &frame.length, &frame.version)) {
        return nullptr;
    }
    const uint32 length = frame.length;
    uint32       bytesRead;
    if (length <= sizeof(frame)) {
        bytesRead = RwStreamRead(stream, &frame, length);
    } else {
        std::vector<uint8> buffer(length);
        bytesRead = RwStreamRead(stream, buffer.data(), length);
        std::memcpy(&frame, buffer.data(), sizeof(frame));
    }
    if (bytesRead != frame.length) {
        return nullptr;
    }

    RwTexDictionary* const txd = RwTexDictionaryCreate();
    if (!txd) {
        return nullptr;
    }
    uint16      remaining = static_cast<uint16>(frame.header);
    const int32 half      = remaining >> 1;
    s_txdTexturesLeft     = half;
    if (remaining > half) {
        do {
            RwTexture* const texture = RwTextureGtaStreamRead(stream);
            if (!texture) {
                RwTexDictionaryForAllTextures(txd, reinterpret_cast<RwTextureCallBack>(0x730E50), nullptr);
                RwTexDictionaryDestroy(txd);
                return nullptr;
            }
            RwTexDictionaryAddTexture(txd, texture);
            --remaining;
        } while (static_cast<int32>(remaining) > s_txdTexturesLeft);
    }
    s_txdStreamPos    = static_cast<int32>(GtaStreamPosition(stream));
    s_txdTexturesLeft = remaining;
    return txd;
}

// 0x731150: segunda mitad. Salta en el stream hasta la posicion guardada por Read1 (RwStreamSkip con 0xC87FE8 - posicion actual), lee las texturas que quedan en 0x8D6088 (lo decrementa ANTES
// de cada lectura y lo relee al acabar la vuelta) con el mismo tratamiento de fallo que Read1 y, al final, decrementa 0x8D6088 una vez mas (queda en -1) y devuelve `txd`.
RwTexDictionary* RwTexDictionaryGtaStreamRead2(RwStream* stream, RwTexDictionary* txd) {
    const int32 savedPosition = s_txdStreamPos;
    RwStreamSkip(stream, static_cast<RwUInt32>(savedPosition - static_cast<int32>(GtaStreamPosition(stream))));
    if (s_txdTexturesLeft != 0) {
        do {
            s_txdTexturesLeft = s_txdTexturesLeft - 1;
            RwTexture* const texture = RwTextureGtaStreamRead(stream);
            if (!texture) {
                RwTexDictionaryForAllTextures(txd, reinterpret_cast<RwTextureCallBack>(0x730E50), nullptr);
                RwTexDictionaryDestroy(txd);
                return nullptr;
            }
            RwTexDictionaryAddTexture(txd, texture);
        } while (s_txdTexturesLeft != 0);
    }
    s_txdTexturesLeft = s_txdTexturesLeft - 1;
    return txd;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Lectura de un clump de GTA en dos fases (0x72E570 / 0x72E620 / 0x72E700). Estado del EXE: cabecera STRUCT del clump (3 enteros), lista de frames (rwFrameList), posicion del stream tras
// Read1 y lista de geometrias ({vector, contador}). Los cuatro ayudantes (0x72E140, 0x72E220, 0x72E270 y 0x72E360, convencion de registros propia: ESI/EBX/EDI) solo los usan estas tres
// funciones (y la funcion muerta 0x72E3A0, sin llamadores), por lo que son funciones privadas de este archivo.
namespace {
struct GtaClumpHeader {
    int32 numAtomics;
    int32 numLights;
    int32 numCameras;
};
struct GtaGeometryList {
    RpGeometry** geometries;
    int32        numGeometries;
};
struct GtaAtomicInfo { // chunk STRUCT de un atomico: indice de frame, indice de geometria, flags, sin uso
    int32 frameIndex;
    int32 geometryIndex;
    int32 flags;
    int32 unused;
};
static_assert(sizeof(GtaClumpHeader) == 12 && sizeof(GtaAtomicInfo) == 16);
}

static auto& s_clumpHeader       = StaticRef<GtaClumpHeader>(0xC87AE8);
static auto& s_clumpFrameList    = StaticRef<rwFrameList>(0xC87AF4);
static auto& s_clumpStreamPos    = StaticRef<int32>(0xC87AFC);
static auto& s_clumpGeometryList = StaticRef<GtaGeometryList>(0xC87B00);
static auto& s_geometriesInPass1 = StaticRef<int32>(0x8D607C); // 0x8D607C: geometrias que lee la primera fase (la mitad del contador, division con signo)

// 0x72E360: destruye las geometrias de la lista (RpGeometryDestroy a cada una no nula), libera el vector y pone el contador a 0. Con contador 0 no libera nada.
static void RpClumpGtaGeometryListDestroy(GtaGeometryList& list) {
    if (list.numGeometries != 0) {
        for (int32 i = 0; i < list.numGeometries; ++i) {
            if (RpGeometry* const geometry = list.geometries[i]) {
                RpGeometryDestroy(geometry);
            }
        }
        CMemoryMgr::Free(list.geometries);
    }
    list.numGeometries = 0;
}

// 0x72E140: primera fase de la lista de geometrias. 0x8D607C = 0; chunk STRUCT con un contador de 4 bytes; 0x8D607C = contador / 2 (con signo); reserva contador * 4 bytes (hint 0, nulo -> nulo)
// puestos a cero y lee las primeras 0x8D607C geometrias (chunk GEOMETRY + RpGeometryStreamRead; si alguna falla, nulo sin liberar nada). Con contador <= 0 el vector queda nulo.
static GtaGeometryList* RpClumpGtaGeometryListStreamRead1(RwStream* stream, GtaGeometryList* list) {
    RwUInt32 length;
    RwUInt32 version;
    s_geometriesInPass1 = 0;
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version)) {
        return nullptr;
    }
    int32 count;
    if (RwStreamRead(stream, &count, sizeof(count)) != sizeof(count)) {
        return nullptr;
    }
    s_geometriesInPass1 = count / 2;
    list->numGeometries = count;
    if (count > 0) {
        const uint32 bytes = static_cast<uint32>(count) * sizeof(RpGeometry*);
        list->geometries   = static_cast<RpGeometry**>(CMemoryMgr::Malloc(bytes, 0));
        if (!list->geometries) {
            return nullptr;
        }
        std::memset(list->geometries, 0, bytes);
    } else {
        list->geometries = nullptr;
    }
    for (int32 i = 0; i < s_geometriesInPass1; ++i) {
        if (!RwStreamFindChunk(stream, rwID_GEOMETRY, nullptr, &version)) {
            return nullptr;
        }
        RpGeometry* const geometry = RpGeometryStreamRead(stream);
        list->geometries[i]        = geometry;
        if (!geometry) {
            return nullptr;
        }
    }
    return list;
}

// 0x72E220: segunda fase. Lee las geometrias desde el indice 0x8D607C hasta el contador de la lista (relee el contador y el vector en cada vuelta); si una falla, nulo.
static GtaGeometryList* RpClumpGtaGeometryListStreamRead2(RwStream* stream, GtaGeometryList* list) {
    RwUInt32 version;
    int32    i = s_geometriesInPass1;
    if (i < list->numGeometries) {
        do {
            if (!RwStreamFindChunk(stream, rwID_GEOMETRY, nullptr, &version)) {
                return nullptr;
            }
            RpGeometry* const geometry = RpGeometryStreamRead(stream);
            list->geometries[i]        = geometry;
            if (!geometry) {
                return nullptr;
            }
            ++i;
        } while (i < list->numGeometries);
    }
    return list;
}

// 0x72E270: lee un atomico. Chunk STRUCT y lectura de `longitud` bytes (la longitud se relee: debe coincidir con lo leido) en 16 bytes {frame, geometria, flags, sin uso}; RpAtomicCreate (nulo -> nulo);
// flags (byte bajo) a atomic->object.flags (+2); con frames en la lista, RpAtomicSetFrame con frames[frame] (sin acotar); con geometrias en la lista, RpAtomicSetGeometry(geometrias[geometria], 0)
// y listo. Sin geometrias en la lista: chunk GEOMETRY + RpGeometryStreamRead (si falla se destruye el atomico y nulo), RpAtomicSetGeometry y RpGeometryDestroy (libera la referencia extra).
static RpAtomic* RpClumpGtaAtomicStreamRead(RwStream* stream, const GtaGeometryList* geometryList, const rwFrameList* frameList) {
    RwUInt32      length;
    RwUInt32      version;
    GtaAtomicInfo info{};
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version)) {
        return nullptr;
    }
    RwUInt32 bytesRead;
    if (length <= sizeof(info)) {
        bytesRead = RwStreamRead(stream, &info, length);
    } else {
        std::vector<uint8> buffer(length);
        bytesRead = RwStreamRead(stream, buffer.data(), length);
        std::memcpy(&info, buffer.data(), sizeof(info));
    }
    if (bytesRead != length) {
        return nullptr;
    }

    RpAtomic* const atomic = RpAtomicCreate();
    if (!atomic) {
        return nullptr;
    }
    atomic->object.object.flags = static_cast<RwUInt8>(info.flags);
    if (frameList->numFrames != 0) {
        RpAtomicSetFrame(atomic, frameList->frames[info.frameIndex]);
    }
    if (geometryList->numGeometries != 0) {
        RpAtomicSetGeometry(atomic, geometryList->geometries[info.geometryIndex], 0);
        return atomic;
    }
    if (!RwStreamFindChunk(stream, rwID_GEOMETRY, nullptr, &version)) {
        RpAtomicDestroy(atomic);
        return nullptr;
    }
    RpGeometry* const geometry = RpGeometryStreamRead(stream);
    if (!geometry) {
        RpAtomicDestroy(atomic);
        return nullptr;
    }
    RpAtomicSetGeometry(atomic, geometry, 0);
    RpGeometryDestroy(geometry);
    return atomic;
}

// 0x72E570: primera fase de la lectura de un clump. Chunk STRUCT y lectura de 12 bytes en la cabecera 0xC87AE8 (atomicos, luces, camaras); chunk FRAMELIST y _rwFrameListStreamRead sobre 0xC87AF4
// (si falla: false sin mas); chunk GEOMETRYLIST (si falta: se libera la lista de frames y false); primera fase de la lista de geometrias en 0xC87B00 (si falla: se libera la lista de frames y false,
// sin liberar las geometrias). Con exito guarda la posicion del stream en 0xC87AFC y devuelve true.
bool RpClumpGtaStreamRead1(RwStream* stream) {
    RwUInt32 length;
    RwUInt32 version;
    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version)) {
        return false;
    }
    if (RwStreamRead(stream, &s_clumpHeader, sizeof(GtaClumpHeader)) != sizeof(GtaClumpHeader)) {
        return false;
    }
    if (!RwStreamFindChunk(stream, rwID_FRAMELIST, nullptr, &version)) {
        return false;
    }
    if (!_rwFrameListStreamRead(stream, &s_clumpFrameList)) {
        return false;
    }
    if (!RwStreamFindChunk(stream, rwID_GEOMETRYLIST, nullptr, &version) || !RpClumpGtaGeometryListStreamRead1(stream, &s_clumpGeometryList)) {
        _rwFrameListDeinitialize(&s_clumpFrameList);
        return false;
    }
    s_clumpStreamPos = static_cast<int32>(GtaStreamPosition(stream));
    return true;
}

// 0x72E620: segunda fase. RpClumpCreate (nulo -> nulo); RwStreamSkip hasta la posicion guardada por Read1 (0xC87AFC - posicion actual); resto de la lista de geometrias; el frame raiz del clump
// (parent, +4) = frames[0]; y, por cada atomico de la cabecera (relee el contador en cada vuelta), chunk ATOMIC + atomico + RpClumpAddAtomic. Cualquier fallo libera la lista de geometrias y la
// de frames, destruye el clump y devuelve nulo. Con exito libera la lista de geometrias y la de frames (el clump ya tiene las referencias) y devuelve el clump.
RpClump* RpClumpGtaStreamRead2(RwStream* stream) {
    RpClump* const clump = RpClumpCreate();
    if (!clump) {
        return nullptr;
    }
    RwStreamSkip(stream, static_cast<RwUInt32>(s_clumpStreamPos - static_cast<int32>(GtaStreamPosition(stream))));

    const auto fail = [&]() -> RpClump* {
        RpClumpGtaGeometryListDestroy(s_clumpGeometryList);
        _rwFrameListDeinitialize(&s_clumpFrameList);
        RpClumpDestroy(clump);
        return nullptr;
    };
    if (!RpClumpGtaGeometryListStreamRead2(stream, &s_clumpGeometryList)) {
        return fail();
    }
    clump->object.parent = s_clumpFrameList.frames[0];
    RwUInt32 version;
    for (int32 i = 0; i < s_clumpHeader.numAtomics; ++i) {
        if (!RwStreamFindChunk(stream, rwID_ATOMIC, nullptr, &version)) {
            return fail();
        }
        RpAtomic* const atomic = RpClumpGtaAtomicStreamRead(stream, &s_clumpGeometryList, &s_clumpFrameList);
        if (!atomic) {
            return fail();
        }
        RpClumpAddAtomic(clump, atomic);
    }
    RpClumpGtaGeometryListDestroy(s_clumpGeometryList);
    _rwFrameListDeinitialize(&s_clumpFrameList);
    return clump;
}

// 0x72E700: cancela una lectura en curso: libera la lista de geometrias y la de frames y pone a 0 el contador de frames (0xC87AF8).
void RpClumpGtaCancelStream() {
    RpClumpGtaGeometryListDestroy(s_clumpGeometryList);
    _rwFrameListDeinitialize(&s_clumpFrameList);
    s_clumpFrameList.numFrames = 0;
}
