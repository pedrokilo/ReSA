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

RwTexDictionary* RwTexDictionaryGtaStreamRead(RwStream* stream) {
    return plugin::CallAndReturn<RwTexDictionary*, 0x730FC0, RwStream*>(stream);
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
