#include "StdInc.h"

#include "CustomCarEnvMapPipeline.h"
#include "CustomBuildingDNPipeline.h"

using namespace notsa::envmap;

// Como el original: los datos del material se manejan como bytes crudos
// (+0/+1 escala, +2/+3 escala de traslación con signo · 8, +4 brillo sin signo · 255,
//  +5 relleno, +6 fotograma de render, +8 textura)
namespace {
uint8* Raw(CustomEnvMapPipeMaterialData* data) {
    return reinterpret_cast<uint8*>(data);
}

// Como el original: (float)(int8)b · 0,125 (flt_858C48)
float FromFixed8(uint8 b) {
    return (float)(int8)b * 0.125f;
}

// Como el original: (float)b · (1/255) (flt_859A3C)
float FromFixed255(uint8 b) {
    return (float)b * (1.0f / 255.0f);
}

// Como el original: _ftol2 (0x821B40) de x · 8 (flt_859000), byte bajo
uint8 ToFixed8(float x) {
    return (uint8)(int64)(x * 8.0f);
}

// Como el original: _ftol2 de x · 255 (flt_859AAC), byte bajo
uint8 ToFixed255(float x) {
    return (uint8)(int64)(x * 255.0f);
}
} // namespace

// 0x5D9570
CustomEnvMapPipeMaterialData* CCustomCarEnvMapPipeline::DuplicateCustomEnvMapPipeMaterialData(CustomEnvMapPipeMaterialData** data) {
    if (*data == &fakeEnvMapPipeMatData) {
        // Como el original: si el pool está lleno el puntero queda nulo
        *data = static_cast<CustomEnvMapPipeMaterialData*>(PoolNew<sizeof(CustomEnvMapPipeMaterialData)>(m_gEnvMapPipeMatDataPool));
        if (!*data) {
            return nullptr;
        }
        std::memcpy(*data, &fakeEnvMapPipeMatData, sizeof(CustomEnvMapPipeMaterialData)); // 3 dwords, relleno incluido
    }
    return *data;
}

// 0x5D8BD0
void* CCustomCarEnvMapPipeline::pluginEnvMatConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, object, offsetInObject) = &fakeEnvMapPipeMatData;
    return object;
}

// 0x5D95B0
void* CCustomCarEnvMapPipeline::pluginEnvMatDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    auto& matData = *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, object, offsetInObject);
    if (matData != &fakeEnvMapPipeMatData && matData) {
        PoolDelete<sizeof(CustomEnvMapPipeMaterialData)>(m_gEnvMapPipeMatDataPool, matData);
        matData = nullptr;
    }
    return object;
}

// 0x5D9600
void* CCustomCarEnvMapPipeline::pluginEnvMatCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject) {
    auto* const srcMatData = *RWPLUGINOFFSETCONST(CustomEnvMapPipeMaterialData*, srcObject, offsetInObject);
    auto&       dstMatData = *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, dstObject, offsetInObject);
    if (srcMatData) {
        if (!dstMatData) {
            dstMatData = static_cast<CustomEnvMapPipeMaterialData*>(PoolNew<sizeof(CustomEnvMapPipeMaterialData)>(m_gEnvMapPipeMatDataPool));
            if (!dstMatData) {
                dstMatData = &fakeEnvMapPipeMatData;
                return dstObject;
            }
        }
        // Como el original: copia `sizeInObject` bytes (el tamaño del plugin, no el de los datos), también sobre el falso
        std::memcpy(dstMatData, srcMatData, (uint32)sizeInObject);
    }
    return dstObject;
}

// 0x5D9660
RwStream* CCustomCarEnvMapPipeline::pluginEnvMatStreamReadCB(RwStream* stream, int32 binaryLength, void* object, int32 offsetInObject, int32 sizeInObject) {
    EnvMapPipeMaterialDataBuffer buffer;
    RwStreamRead(stream, &buffer, binaryLength);

    // 0x5D8BE0 - Como el original: conversión con _ftol2; el fotograma de render queda a 0.
    // En el EXE el byte de relleno +5 es basura de la pila; aquí se toma el del falso para que la
    // comparación siguiente dependa solo de los campos.
    uint8 data[sizeof(CustomEnvMapPipeMaterialData)];
    data[0] = ToFixed8(buffer.Scale.x);
    data[1] = ToFixed8(buffer.Scale.y);
    data[2] = ToFixed8(buffer.TranslationScale.x);
    data[3] = ToFixed8(buffer.TranslationScale.y);
    data[4] = ToFixed255(buffer.Shininess);
    data[5] = Raw(&fakeEnvMapPipeMatData)[5];
    *reinterpret_cast<uint16*>(data + 6) = 0;
    *reinterpret_cast<RwTexture**>(data + 8) = buffer.Texture;

    auto& slot = *RWPLUGINOFFSET(CustomEnvMapPipeMaterialData*, object, offsetInObject);
    if (std::memcmp(data, &fakeEnvMapPipeMatData, sizeof(data)) == 0) {
        slot = &fakeEnvMapPipeMatData;
    } else {
        slot = static_cast<CustomEnvMapPipeMaterialData*>(PoolNew<sizeof(CustomEnvMapPipeMaterialData)>(m_gEnvMapPipeMatDataPool));
        if (slot) {
            std::memcpy(slot, data, sizeof(data));
        } else {
            slot = &fakeEnvMapPipeMatData;
        }
    }
    return stream;
}

// 0x5D8CD0
RwStream* CCustomCarEnvMapPipeline::pluginEnvMatStreamWriteCB(RwStream* stream, int32 binaryLength, const void* object, int32 offsetInObject, int32 sizeInObject) {
    // 0x5D8C50 - Como el original: usa el offset recibido
    const auto* const raw = reinterpret_cast<const uint8*>(*RWPLUGINOFFSETCONST(CustomEnvMapPipeMaterialData*, object, offsetInObject));

    EnvMapPipeMaterialDataBuffer buffer;
    buffer.Scale.x            = FromFixed8(raw[0]);
    buffer.Scale.y            = FromFixed8(raw[1]);
    buffer.TranslationScale.x = FromFixed8(raw[2]);
    buffer.TranslationScale.y = FromFixed8(raw[3]);
    buffer.Shininess          = FromFixed255(raw[4]);
    buffer.Texture            = *reinterpret_cast<RwTexture* const*>(raw + 8);
    RwStreamWrite(stream, &buffer, binaryLength);

    return stream;
}

// 0x5D8D10
int32 CCustomCarEnvMapPipeline::pluginEnvMatStreamGetSizeCB(const void* object, int32 offsetInObject, int32 sizeInObject) {
    return object
        ? sizeof(EnvMapPipeMaterialDataBuffer)
        : 0;
}

// 0x5D6F90
float CCustomCarEnvMapPipeline::GetFxEnvScaleX(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    return data ? FromFixed8(Raw(data)[0]) : 0.0f; // Como el original: 0 sin datos
}

// 0x5D6FC0
float CCustomCarEnvMapPipeline::GetFxEnvScaleY(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    return data ? FromFixed8(Raw(data)[1]) : 0.0f;
}

// 0x5D6F40
void CCustomCarEnvMapPipeline::SetFxEnvScale(RpMaterial* material, float x, float y) {
    // Como el original: duplica los datos falsos antes de escribir (0x5D9570)
    if (auto* const data = DuplicateCustomEnvMapPipeMaterialData(&EnvMapPlGetData(material))) {
        Raw(data)[0] = ToFixed8(x);
        Raw(data)[1] = ToFixed8(y);
    }
}

// 0x5D7040
float CCustomCarEnvMapPipeline::GetFxEnvTransSclX(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    return data ? FromFixed8(Raw(data)[2]) : 0.0f;
}

// 0x5D7070
float CCustomCarEnvMapPipeline::GetFxEnvTransSclY(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    return data ? FromFixed8(Raw(data)[3]) : 0.0f;
}

// 0x5D6FF0
void CCustomCarEnvMapPipeline::SetFxEnvTransScl(RpMaterial* material, float x, float y) {
    if (auto* const data = DuplicateCustomEnvMapPipeMaterialData(&EnvMapPlGetData(material))) {
        Raw(data)[2] = ToFixed8(x);
        Raw(data)[3] = ToFixed8(y);
    }
}

// 0x5D8AD0
float CCustomCarEnvMapPipeline::GetFxEnvShininess(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    return data ? FromFixed255(Raw(data)[4]) : 0.0f; // Como el original: byte sin signo · (1/255)
}

// 0x5D70A0
void CCustomCarEnvMapPipeline::SetFxEnvShininess(RpMaterial* material, float value) {
    if (auto* const data = DuplicateCustomEnvMapPipeMaterialData(&EnvMapPlGetData(material))) {
        Raw(data)[4] = ToFixed255(value);
    }
}

// 0x5D6F20
RwTexture* CCustomCarEnvMapPipeline::GetFxEnvTexture(RpMaterial* material) {
    auto* const data = EnvMapPlGetData(material);
    return data ? data->Texture : nullptr;
}

// 0x5DA230
void CCustomCarEnvMapPipeline::SetFxEnvTexture(RpMaterial* material, RwTexture* texture) {
    auto*& data = EnvMapPlGetData(material);

    if (data == &fakeEnvMapPipeMatData) {
        // Como el original: si el pool está lleno el puntero queda nulo
        data = static_cast<CustomEnvMapPipeMaterialData*>(PoolNew<sizeof(CustomEnvMapPipeMaterialData)>(m_gEnvMapPipeMatDataPool));
        if (!data) {
            return;
        }
        std::memcpy(data, &fakeEnvMapPipeMatData, sizeof(CustomEnvMapPipeMaterialData));
    }

    if (!data) {
        return;
    }

    if (texture) {
        data->Texture = texture;
    } else {
        // Como el original: lee directamente la textura del primer pase de MatFX (sin comprobar el efecto)
        data->Texture = material
            ? *reinterpret_cast<RwTexture**>(*RWPLUGINOFFSET(uint8*, material, MatFXMaterialDataOffset) + 4)
            : nullptr;
        if (!data->Texture || FromFixed255(Raw(data)[4]) == 0.0f) {
            PoolDelete<sizeof(CustomEnvMapPipeMaterialData)>(m_gEnvMapPipeMatDataPool, data);
            data = &fakeEnvMapPipeMatData;
        }
    }

    // Como el original: direccionamiento U/V envolvente (byte +0x51 = 0x11) y filtro lineal (byte +0x50 = 2)
    if (auto* const t = data->Texture) {
        reinterpret_cast<uint8*>(t)[0x51] = 0x11;
        reinterpret_cast<uint8*>(t)[0x50] = rwFILTERLINEAR;
    }
}

// 0x5D8BB0
void CCustomCarEnvMapPipeline::SetCustomEnvMapPipeMaterialDataDefaults(CustomEnvMapPipeMaterialData* data) {
    // Como el original: escalas a 1 (8), brillo 1 (0xFF), textura y fotograma a 0; el byte de relleno +5 no se toca
    auto* const raw = Raw(data);
    raw[0] = raw[1] = raw[2] = raw[3] = 8;
    raw[4] = 0xFF;
    *reinterpret_cast<RwTexture**>(raw + 8) = nullptr;
    *reinterpret_cast<uint16*>(raw + 6) = 0;
}
