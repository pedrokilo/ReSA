/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimBlendClumpData
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AnimBlendClumpData.h"

void CAnimBlendClumpData::InjectHooks() {
    RH_ScopedClass(CAnimBlendClumpData);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Constructor, 0x4CF0E0);
    RH_ScopedInstall(Destructor, 0x4CF100);
    RH_ScopedInstall(SetNumberOfBones, 0x4CF140);
    RH_ScopedInstall(ForAllFrames, 0x4CF190);
    RH_ScopedInstall(LoadFramesIntoSPR, 0x4CF1D0);
    RH_ScopedInstall(ForAllFramesInSPR, 0x4CF1E0);
}

// 0x4CF0E0
CAnimBlendClumpData::CAnimBlendClumpData() {
    m_NumFrameData   = 0;
    m_PedPosition = nullptr;
    m_FrameDatas      = nullptr;
}

// 0x4CF100
CAnimBlendClumpData::~CAnimBlendClumpData() {
    m_AnimList.Remove();
    if (m_FrameDatas) {
        CMemoryMgr::FreeAlign(&m_FrameDatas);
    }
}

// 0x4CF140
void CAnimBlendClumpData::SetNumberOfBones(uint32 numBones) {
    if (m_FrameDatas) {
        CMemoryMgr::FreeAlign(&m_FrameDatas);
    }
    m_NumFrameData = numBones;
    m_FrameDatas = static_cast<AnimBlendFrameData*>(CMemoryMgr::MallocAlign(sizeof(AnimBlendFrameData) * numBones, 64));
}

// 0x4CF190
void CAnimBlendClumpData::ForAllFrames(void (*callback)(AnimBlendFrameData*, void*), void* data) {
    for (auto& frame : std::span{ m_FrameDatas, m_NumFrameData }) {
        callback(&frame, data);
    }
}

// 0x4CF1D0
void CAnimBlendClumpData::LoadFramesIntoSPR() {
    // NOP
}

// 0x4CF1E0
void CAnimBlendClumpData::ForAllFramesInSPR(void (*callback)(AnimBlendFrameData*, void*), void* data, uint32 a3) {
    // NOP
}
