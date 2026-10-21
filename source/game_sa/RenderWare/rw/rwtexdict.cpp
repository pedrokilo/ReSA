#include "StdInc.h"

#include "rwtexdict.h"

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

RwTexture* GetFirstTextureCallback(RwTexture* texture, void* data) {
    return plugin::CallAndReturn<RwTexture*, 0x734930, RwTexture*, void*>(texture, data);
}

RwTexture* RwTextureGtaStreamRead(RwStream* stream) {
    return plugin::CallAndReturn<RwTexture*, 0x730E60, RwStream*>(stream);
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

RwTexDictionary* RwTexDictionaryGtaStreamRead1(RwStream* stream) {
    return plugin::CallAndReturn<RwTexDictionary*, 0x731070, RwStream*>(stream);
}

RwTexDictionary* RwTexDictionaryGtaStreamRead2(RwStream* stream, RwTexDictionary* txd) {
    return plugin::CallAndReturn<RwTexDictionary*, 0x731150, RwStream*, RwTexDictionary*>(stream, txd);
}

bool RpClumpGtaStreamRead1(RwStream* stream) {
    return plugin::CallAndReturn<bool, 0x72E570, RwStream*>(stream);
}

RpClump* RpClumpGtaStreamRead2(RwStream* stream) {
    return plugin::CallAndReturn<RpClump*, 0x72E620, RwStream*>(stream);
}
