#include "StdInc.h"

#include "CustomCarEnvMapPipeline.h"

using namespace notsa::envmap;

// 0x5D8D30
void* CCustomCarEnvMapPipeline::pluginEnvAtmConstructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    // Como el original: usa el offset recibido
    *RWPLUGINOFFSET(CustomEnvMapPipeAtomicData*, object, offsetInObject) = nullptr;
    return object;
}

// 0x5D9730
void* CCustomCarEnvMapPipeline::pluginEnvAtmDestructorCB(void* object, int32 offsetInObject, int32 sizeInObject) {
    auto& data = *RWPLUGINOFFSET(CustomEnvMapPipeAtomicData*, object, offsetInObject);
    if (data) {
        PoolDelete<sizeof(CustomEnvMapPipeAtomicData)>(m_gEnvMapPipeAtmDataPool, data);
        data = nullptr;
    }
    return object;
}

// 0x5D9780
void* CCustomCarEnvMapPipeline::pluginEnvAtmCopyConstructorCB(void* dstObject, const void* srcObject, int32 offsetInObject, int32 sizeInObject) {
    auto* const src = *RWPLUGINOFFSETCONST(CustomEnvMapPipeAtomicData*, srcObject, offsetInObject);
    auto&       dst = *RWPLUGINOFFSET(CustomEnvMapPipeAtomicData*, dstObject, offsetInObject);
    if (src) {
        // Como el original: reserva inline sin valores por defecto y copia `sizeInObject` bytes (el tamaño del plugin)
        if (!dst) {
            dst = static_cast<CustomEnvMapPipeAtomicData*>(PoolNew<sizeof(CustomEnvMapPipeAtomicData)>(m_gEnvMapPipeAtmDataPool));
        }
        if (dst) {
            std::memcpy(dst, src, (uint32)sizeInObject);
        }
    }
    return dstObject;
}

// 0x5D96F0
CustomEnvMapPipeAtomicData* CCustomCarEnvMapPipeline::AllocEnvMapPipeAtomicData(RpAtomic* atomic) {
    auto*& data = EnvMapAtmPlGetData(atomic);
    if (!data) {
        data = static_cast<CustomEnvMapPipeAtomicData*>(PoolNew<sizeof(CustomEnvMapPipeAtomicData)>(m_gEnvMapPipeAtmDataPool));
        if (data) {
            SetCustomEnvMapPipeAtomicDataDefaults(data);
        }
    }
    return data;
}

// 0x5D8D20
void CCustomCarEnvMapPipeline::SetCustomEnvMapPipeAtomicDataDefaults(CustomEnvMapPipeAtomicData* data) {
    data->OffsetU = 0.0f;
    data->PrevPosY = 0.0f;
    data->PrevPosX = 0.0f;
}
