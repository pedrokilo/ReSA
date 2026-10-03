/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorGoToInfo
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskInteriorGoToInfo.h"
#include "TaskComplexFollowNodeRoute.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleGoToPoint.h"

#include "Interior_c.h"
#include "InteriorInfo_t.h"
#include "InteriorManager_c.h"

void CTaskInteriorGoToInfo::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorGoToInfo, 0x8702bc, 11);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x6757D0);
    RH_ScopedInstall(Destructor, 0x675810);

    RH_ScopedVMTInstall(Clone, 0x675820);
    RH_ScopedVMTInstall(GetTaskType, 0x675800);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x675890);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x676640);
    RH_ScopedVMTInstall(ControlSubTask, 0x675A30);
}

// 0x6757D0
CTaskInteriorGoToInfo::CTaskInteriorGoToInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, bool bDoInstantly) :
    m_Int{interior},
    m_IntInfo{interiorInfo},
    m_bDoInstantly{bDoInstantly}
{
}

// 0x675820
CTaskInteriorGoToInfo::CTaskInteriorGoToInfo(const CTaskInteriorGoToInfo& o) :
    CTaskInteriorGoToInfo{o.m_IntInfo, o.m_Int, o.m_bDoInstantly}
{
}   

namespace {
// Como el original: CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0, 0))
float InteriorInfoHeading(const InteriorInfo_t* info) {
    return CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(info->Dir.x, info->Dir.y, 0.f, 0.f));
}
} // namespace

// 0x675890
CTask* CTaskInteriorGoToInfo::CreateNextSubTask(CPed* ped) {
    if (!m_IntInfo) {
        return nullptr;
    }
    // Como el original: dos llamadas virtuales a GetTaskType
    if (m_pSubTask->GetTaskType() != TASK_COMPLEX_FOLLOW_NODE_ROUTE && m_pSubTask->GetTaskType() != TASK_SIMPLE_GO_TO_POINT) {
        return nullptr;
    }
    const CVector intPos = m_IntInfo->Pos;
    const auto&   pedPos = ped->GetPosition();
    const float   dy     = pedPos.y - intPos.y;
    const float   dx     = pedPos.x - intPos.x;
    // Como el original: NaN también crea la ruta (constante 0x863244)
    if (!(dx * dx + dy * dy <= std::bit_cast<float>(0x3D23D70Bu))) {
        return new CTaskComplexFollowNodeRoute{ PEDMOVE_WALK, intPos, 0.2f, 3.f, 2.f, false, -1, true };
    }
    return new CTaskSimpleAchieveHeading{ InteriorInfoHeading(m_IntInfo), 0.5f, 0.2f };
}

// 0x676640
CTask* CTaskInteriorGoToInfo::CreateFirstSubTask(CPed* ped) {
    if (!m_IntInfo) {
        return nullptr;
    }

    if (m_bDoInstantly) {
        ped->SetPosn(m_IntInfo->Pos);

        const auto rz = InteriorInfoHeading(m_IntInfo);
        ped->m_fAimingRotation = rz; // Como el original: primero 0x55C y luego 0x558
        ped->m_fCurrentRotation = rz;
        ped->SetHeading(rz);

        return nullptr;
    }

    const auto pedsInterior = g_interiorMan.GetPedsInterior(ped);
    if (pedsInterior == m_Int) {
        return new CTaskSimpleGoToPoint{ PEDMOVE_WALK, m_IntInfo->Pos, 0.2f, false, false };
    }

    // Como el original: acceso directo al nodo, sin comprobar que el área esté cargada
    const auto nodeAddr = m_Int->GetNodeAddress();
    const CVector nodePos = ThePaths.m_pPathNodes[nodeAddr.m_wAreaId][nodeAddr.m_wNodeId].GetPosition();
    const auto tFollowNodeRoute = new CTaskComplexFollowNodeRoute{ PEDMOVE_WALK, nodePos, 0.2f, 3.f, 2.f, false, -1, true };
    tFollowNodeRoute->SetStartNode(pedsInterior->GetNodeAddress()); // Como el original: el nodo inicial es el del interior del peatón (sin comprobar nulo)
    return tFollowNodeRoute;
}

// 0x675A30
CTask* CTaskInteriorGoToInfo::ControlSubTask(CPed* ped) {
    return m_IntInfo
        ? m_pSubTask
        : nullptr;
}
