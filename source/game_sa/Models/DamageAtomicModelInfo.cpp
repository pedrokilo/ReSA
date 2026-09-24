/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDamageAtomicModelInfo
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "DamageAtomicModelInfo.h"
#include "CustomBuildingRenderer.h"
#include "CarFXRenderer.h"

void CDamageAtomicModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CDamageAtomicModelInfo, 0x85BC30, 16);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(Init, 0x4C48B0);
    RH_ScopedVMTInstall(AsDamageAtomicModelInfoPtr, 0x4C55C0);
    RH_ScopedVMTInstall(DeleteRwObject, 0x4C49D0);
    // clang moment: RH_ScopedVirtualOverloadedInstall(CreateInstance, "void", 0x4C4960, RwObject * (CDamageAtomicModelInfo::*)());
    // clang moment: RH_ScopedVirtualOverloadedInstall(CreateInstance, "rwmat", 0x4C4910, RwObject * (CDamageAtomicModelInfo::*)(RwMatrix*));

    RH_ScopedInstall(SetDamagedAtomic, 0x4C48D0);
}

// 0x4C55C0
CDamageAtomicModelInfo* CDamageAtomicModelInfo::AsDamageAtomicModelInfoPtr()
{
    return this;
}

// 0x4C48B0
void CDamageAtomicModelInfo::Init()
{
    CBaseModelInfo::Init();
    m_pDamagedAtomic = nullptr;
}

// 0x4C49D0
void CDamageAtomicModelInfo::DeleteRwObject()
{
    if (m_pDamagedAtomic) {
        auto frame = RpAtomicGetFrame(m_pDamagedAtomic);
        RpAtomicDestroy(m_pDamagedAtomic);
        RwFrameDestroy(frame);
        m_pDamagedAtomic = nullptr;
    }

    CAtomicModelInfo::DeleteRwObject();
}

RwObject* CDamageAtomicModelInfo::CreateInstance()
{
    if (!CDamageAtomicModelInfo::ms_bCreateDamagedVersion)
        return CAtomicModelInfo::CreateInstance();

    if (!m_pDamagedAtomic)
        return nullptr;

    auto atomic = RpAtomicClone(m_pDamagedAtomic);
    auto frame = RwFrameCreate();
    RpAtomicSetFrame(atomic, frame);
    return reinterpret_cast<RwObject*>(atomic);
}

RwObject* CDamageAtomicModelInfo::CreateInstance(RwMatrix* matrix)
{
    if (!CDamageAtomicModelInfo::ms_bCreateDamagedVersion)
        return CAtomicModelInfo::CreateInstance(matrix);

    if (!m_pDamagedAtomic)
        return nullptr;

    auto atomic = RpAtomicClone(m_pDamagedAtomic);
    auto frame = RwFrameCreate();
    memcpy(RwFrameGetMatrix(frame), matrix, sizeof(*RwFrameGetMatrix(frame)));
    RpAtomicSetFrame(atomic, frame);
    return reinterpret_cast<RwObject*>(atomic);
}

void CDamageAtomicModelInfo::SetDamagedAtomic(RpAtomic* atomic)
{
    m_pDamagedAtomic = atomic;
    if (CCustomBuildingRenderer::IsCBPCPipelineAttached(atomic))
    {
        CCustomBuildingRenderer::AtomicSetup(atomic);
    }
    else if (CCarFXRenderer::IsCCPCPipelineAttached(atomic))
    {
        CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB(atomic, nullptr);
    }
}
