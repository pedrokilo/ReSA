/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorBeInShop
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskInteriorBeInShop.h"

#include "EventInteriorUseInfo.h"

#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorGroup_c.h"

#include "Ragdoll/IKChainManager.h"

void CTaskInteriorBeInShop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorBeInShop, 0x870244, 15);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x6753E0);
    RH_ScopedInstall(Destructor, 0x675460);

    RH_ScopedVMTInstall(Clone, 0x675470);
    RH_ScopedVMTInstall(GetTaskType, 0x675450);
    RH_ScopedVMTInstall(GetWanderType, 0x675440);
    RH_ScopedVMTInstall(ScanForStuff, 0x6754D0);
}

// 0x6753E0
CTaskInteriorBeInShop::CTaskInteriorBeInShop(InteriorGroup_c* interiorGroup) :
    // Como el original: (rand() & 0xFFFF) * (1/32768) * 8 truncado (0x407180 en linea), no RandomNodeHeading
    CTaskComplexWander{PEDMOVE_WALK, (uint8)CGeneral::GetRandomIntInRange(0, 8), true, 0.2f},
    m_CurrIntGrp{interiorGroup}
{
}

// 0x675470
CTaskInteriorBeInShop::CTaskInteriorBeInShop(const CTaskInteriorBeInShop& o) :
    CTaskInteriorBeInShop{o.m_CurrIntGrp}
{
}

// 0x6754D0
void CTaskInteriorBeInShop::ScanForStuff(CPed* ped) {
    if (!m_bScanForStuff) {
        return;
    }
    InteriorInfo_t* closestIntInfo{};
    Interior_c* closestInt{};
    float unk{};
    if (!m_CurrIntGrp->FindClosestInteriorInfo(
        -1,
        ped->GetPosition(),
        2.f,
        &closestIntInfo,
        &closestInt,
        &unk
    )) {
        return;
    }
    if (closestIntInfo == m_CurrIntInfo) {
        return;
    }
    const auto PedAddUseInfoEvent = [&]{
        ped->GetEventGroup().Add(CEventInteriorUseInfo{ closestIntInfo, closestInt, 0, false });
    };
    switch (closestIntInfo->Type) {
    case eInteriorInfoType::UNK_8: {
        // Como el original: GetRandomNumberInRange(0, 100) > 50 añade el evento; si no, mira el punto
        if (CGeneral::GetRandomIntInRange(0, 100) <= 50) {
            g_ikChainMan.LookAt(
                "TaskBeInShop",
                ped,
                nullptr,
                3'000,
                eBoneTag::BONE_UNKNOWN,
                &closestIntInfo->Pos,
                true,
                0.25f,
                500,
                3,
                false
            );
        } else {
            PedAddUseInfoEvent();
        }
        break;
    }
    case eInteriorInfoType::UNK_9: {
        // Como el original: solo con GetRandomNumberInRange(0, 100) > 80
        if (CGeneral::GetRandomIntInRange(0, 100) > 80) {
            m_bScanForStuff = false;
            PedAddUseInfoEvent();
        }
        break;
    }
    }
    m_CurrIntInfo = closestIntInfo;
}
