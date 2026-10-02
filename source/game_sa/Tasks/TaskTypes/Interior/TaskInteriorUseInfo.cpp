/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorUseInfo
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskInteriorUseInfo.h"
#include "TaskInteriorGoToInfo.h"
#include "TaskInteriorLieInBed.h"
#include "TaskInteriorSitAtDesk.h"
#include "TaskInteriorShopKeeper.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleRunTimedAnim.h"
#include "TaskComplexSequence.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorInfo_t.h"
#include "Interior/InteriorGroup_c.h"
#include "Interior/InteriorManager_c.h"

void CTaskInteriorUseInfo::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorUseInfo, 0x8702e8, 11);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x675A50);
    RH_ScopedInstall(Destructor, 0x675A90);

    RH_ScopedVMTInstall(Clone, 0x675AB0);
    RH_ScopedVMTInstall(GetTaskType, 0x675A80);
    RH_ScopedVMTInstall(MakeAbortable, 0x675B30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x676880);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x675B60);
    RH_ScopedVMTInstall(ControlSubTask, 0x675C00);
}

// 0x675A50
CTaskInteriorUseInfo::CTaskInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, int32 duration, bool bDoInstantly) :
    m_IntInfo{interiorInfo},
    m_Int{interior},
    m_Dur{duration},
    m_bDoInstantly{bDoInstantly}
{
}

// 0x675AB0
CTaskInteriorUseInfo::CTaskInteriorUseInfo(const CTaskInteriorUseInfo& o) :
    CTaskInteriorUseInfo{o.m_IntInfo, o.m_Int, o.m_Dur, o.m_bDoInstantly}
{
}

// 0x675A90
CTaskInteriorUseInfo::~CTaskInteriorUseInfo() {
    m_IntInfo->IsInUse = false;
}

// 0x675B30
bool CTaskInteriorUseInfo::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (m_IntInfo->EntityIgnoredCollision) {
        ped->m_pEntityIgnoredCollision = nullptr;
    }
    return CTaskComplex::MakeAbortable(ped, priority, event);
}

// 0x676880
CTask* CTaskInteriorUseInfo::CreateNextSubTask(CPed* ped) {
    if (!m_IntInfo) {
        return nullptr;
    }

    if (m_pSubTask->GetTaskType() == TASK_INTERIOR_GOTO_INFO) {
        switch (m_IntInfo->Type) {
        case eInteriorInfoType::UNK_3:
            return new CTaskInteriorLieInBed{ m_Dur, m_IntInfo, false, m_bDoInstantly };
        case eInteriorInfoType::UNK_4:
            return new CTaskInteriorLieInBed{ m_Dur, m_IntInfo, true, m_bDoInstantly };
        case eInteriorInfoType::UNK_1: {
            // CTaskInteriorSitInChair is not reversed yet; construct it through the original constructor.
            auto* const task = static_cast<CTask*>(CTask::operator new(0x2C));
            return plugin::CallMethodAndReturn<CTask*, 0x675C30, CTask*, int32, InteriorInfo_t*, bool>(task, m_Dur, m_IntInfo, m_bDoInstantly);
        }
        case eInteriorInfoType::UNK_2:
            return new CTaskSimpleStandStill{ 9'999'999, false, false, 8.f };
        case eInteriorInfoType::UNK_5:
            if (!g_interiorMan.AreAnimsLoaded(0)) {
                break;
            }
            return new CTaskSimpleRunTimedAnim{ ANIM_GROUP_INT_HOUSE, ANIM_ID_WASH_UP, m_bDoInstantly ? 1000.f : 4.f, 4.f, (uint32)m_Dur, false };
        case eInteriorInfoType::UNK_6:
            return new CTaskInteriorSitAtDesk{ m_Dur, m_IntInfo, m_bDoInstantly };
        case eInteriorInfoType::UNK_7:
            if (!g_interiorMan.AreAnimsLoaded(2)) {
                break;
            }
            return new CTaskSimpleStandStill{ 5000, false, false, 8.f };
        case eInteriorInfoType::UNK_8:
            if (!g_interiorMan.AreAnimsLoaded(1)) {
                break;
            }
            ped->Say(CTX_GLOBAL_SHOP_BROWSE);
            return new CTaskSimpleRunAnim{ ANIM_GROUP_INT_SHOP, ANIM_ID_SHOP_SHELF, 4.f, false };
        case eInteriorInfoType::UNK_9: {
            if (!g_interiorMan.AreAnimsLoaded(1)) {
                break;
            }
            const auto shopKeeper = m_Int->m_pGroup->GetPed(0);
            const auto keeperTask = shopKeeper->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT);
            if (keeperTask->GetTaskType() != TASK_INTERIOR_SHOPKEEPER) {
                break;
            }
            static_cast<CTaskInteriorShopKeeper*>(keeperTask)->StartServeAnim();
            ped->Say(CTX_GLOBAL_SHOP_BUY);
            // Sin `{}`: el constructor es `= default` y la inicialización por valor
            // pondría a cero también el relleno; el original (0x632BD0) no lo toca.
            auto* const seq = new CTaskComplexSequence;
            seq->AddTask(new CTaskSimpleRunAnim{ ANIM_GROUP_INT_SHOP, ANIM_ID_SHOP_PAY, 4.f, false });
            return seq;
        }
        case eInteriorInfoType::UNK_10:
            return new CTaskSimpleStandStill{ 0, true, false, 8.f };
        }
    }

    if (m_IntInfo->EntityIgnoredCollision) {
        ped->m_pEntityIgnoredCollision = nullptr;
    }
    return nullptr;
}

// 0x675B60
CTask* CTaskInteriorUseInfo::CreateFirstSubTask(CPed* ped) {
    if (!m_IntInfo) {
        return nullptr;
    }
    m_IntInfo->IsInUse = true;
    return new CTaskInteriorGoToInfo{ m_IntInfo, m_Int, m_bDoInstantly };
}

// 0x675C00
CTask* CTaskInteriorUseInfo::ControlSubTask(CPed* ped) {
    if (!m_IntInfo) {
        return nullptr;
    }
    if (m_IntInfo->EntityIgnoredCollision) {
        ped->m_pEntityIgnoredCollision = m_IntInfo->EntityIgnoredCollision;
    }
    return m_pSubTask;
}
