#include "StdInc.h"

#include "CustomCarEnvMapPipeline.h"

using namespace notsa::envmap;

// 0x5D8D40
void* CCustomCarEnvMapPipeline::pluginSpecMatConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    *RWPLUGINOFFSET(CustomSpecMapPipeMaterialData*, object, ms_specularMapPluginOffset) = nullptr;
    return object;
}

// 0x5D97D0
void* CCustomCarEnvMapPipeline::pluginSpecMatDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    auto& data = *RWPLUGINOFFSET(CustomSpecMapPipeMaterialData*, object, ms_specularMapPluginOffset);
    if (data) {
        if (data->Texture) {
            RwTextureDestroy(data->Texture);
            data->Texture = nullptr;
        }
        PoolDelete<sizeof(CustomSpecMapPipeMaterialData)>(m_gSpecMapPipeMatDataPool, data);
        data = nullptr; // Como el original: se limpia el puntero del plugin
    }
    return object;
}

// 0x5D9830
void* CCustomCarEnvMapPipeline::pluginSpecMatCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject) {
    auto* const src = SpecMapPlGetData(static_cast<const RpMaterial*>(srcObject));
    auto*&      dst = SpecMapPlGetData(static_cast<RpMaterial*>(dstObject));
    if (src) {
        if (!dst) {
            dst = static_cast<CustomSpecMapPipeMaterialData*>(PoolNew<sizeof(CustomSpecMapPipeMaterialData)>(m_gSpecMapPipeMatDataPool));
            if (!dst) {
                return dstObject;
            }
        }
        // Como el original: la textura anterior del destino no se libera y solo se suma una referencia a la nueva
        dst->Specularity = src->Specularity;
        dst->Texture     = src->Texture;
        if (auto* const t = src->Texture) {
            ++t->refCount;
        }
    }
    return dstObject;
}

// 0x5D9880
RwStream* CCustomCarEnvMapPipeline::pluginSpecMatStreamReadCB(RwStream* stream, int32 binaryLength, void* object, int32 offsetInObject, int32 sizeInObject) {
    auto& slot = SpecMapPlGetData(static_cast<RpMaterial*>(object));

    SpecMapPipeMaterialDataBuffer buffer;
    RwStreamRead(stream, &buffer, binaryLength);

    if (buffer.Specularity != 0.0f) { // Como el original: NaN también entra
        if (auto* const tex = RwTextureRead(buffer.TexName, nullptr)) {
            // Como el original: si el pool está lleno el puntero queda nulo (y la textura leída sin dueño)
            slot = static_cast<CustomSpecMapPipeMaterialData*>(PoolNew<sizeof(CustomSpecMapPipeMaterialData)>(m_gSpecMapPipeMatDataPool));
            if (slot) {
                slot->Texture     = tex;
                slot->Specularity = buffer.Specularity;
            }
        }
    }

    return stream;
}

// 0x5D8D60
RwStream* CCustomCarEnvMapPipeline::pluginSpecMatStreamWriteCB(RwStream* stream, int32 binaryLength, const void* object, int32 offsetInObject, int32 sizeInObject) {
    auto* const data = SpecMapPlGetData(static_cast<const RpMaterial*>(object));

    // Como el original: búfer a cero, nombre copiado con strncpy de 24 bytes solo si hay textura
    SpecMapPipeMaterialDataBuffer buffer;
    std::memset(&buffer, 0, sizeof(buffer));
    if (data) {
        buffer.Specularity = data->Specularity;
        if (auto* const t = data->Texture) {
            std::strncpy(buffer.TexName, RwTextureGetName(t), sizeof(buffer.TexName));
        }
    }
    RwStreamWrite(stream, &buffer, binaryLength);

    return stream;
}

// 0x5D8DD0
int32 CCustomCarEnvMapPipeline::pluginSpecMatStreamGetSizeCB(const void* object, int32 offsetInObject, int32 sizeInObject) {
    return ms_specularMapPluginOffset == -1 ? -1 : sizeof(SpecMapPipeMaterialDataBuffer);
}

// 0x5D8B90
float CCustomCarEnvMapPipeline::GetFxSpecSpecularity(const RpMaterial* material) {
    auto* const data = SpecMapPlGetData(material);
    return data ? data->Specularity : 0.0f;
}

// 0x5D8B50
RwTexture* CCustomCarEnvMapPipeline::GetFxSpecTexture(const RpMaterial* material) {
    auto* const data = SpecMapPlGetData(material);
    return data ? data->Texture : nullptr;
}

// 0x5D8B00
void CCustomCarEnvMapPipeline::SetFxSpecTexture(RpMaterial* material, RwTexture* texture) {
    auto* const data = SpecMapPlGetData(material);
    if (!data) {
        return;
    }

    if (texture) {
        if (auto* const old = data->Texture) {
            RwTextureDestroy(old);
            data->Texture = nullptr;
        }
        data->Texture = texture;
        ++texture->refCount;
    }

    // Como el original: direccionamiento U/V envolvente (byte +0x51 = 0x11) y filtro lineal (byte +0x50 = 2)
    if (auto* const t = data->Texture) {
        reinterpret_cast<uint8*>(t)[0x51] = 0x11;
        reinterpret_cast<uint8*>(t)[0x50] = rwFILTERLINEAR;
    }
}

// 0x5D8B70
void CCustomCarEnvMapPipeline::SetFxSpecSpecularity(RpMaterial* material, float value) {
    auto* const data = SpecMapPlGetData(material);
    if (data) {
        data->Specularity = value;
    }
}
