/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorBeInHouse
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskInteriorBeInHouse.h"
#include "TaskInteriorUseInfo.h"

#include "Interior/InteriorGroup_c.h"
#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"

void CTaskInteriorBeInHouse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorBeInHouse, 0x8701e8, 11);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x674FC0);
    RH_ScopedInstall(Destructor, 0x675000);

    RH_ScopedInstall(GetInfoForPedToUse, 0x6750A0);
    RH_ScopedVMTInstall(Clone, 0x675010);
    RH_ScopedVMTInstall(GetTaskType, 0x674FF0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x676200);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6762B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x675090);
}

// 0x674FC0
CTaskInteriorBeInHouse::CTaskInteriorBeInHouse(InteriorGroup_c* intGrp) :
    m_IntGrp{intGrp}
{
}

// NOTSA
CTaskInteriorBeInHouse::CTaskInteriorBeInHouse(const CTaskInteriorBeInHouse& o) :
    CTaskInteriorBeInHouse{o.m_IntGrp}
{
}

// 0x6750A0
void CTaskInteriorBeInHouse::GetInfoForPedToUse(CPed* ped, int32* outDuration) {
    using enum eInteriorInfoType;

    // Como el original: tipos de banda 7..16 comparados con signo.
    const auto pedType = (int32)ped->m_nPedType;
    const bool isGang  = pedType >= (int32)PED_TYPE_GANG1 && pedType <= (int32)PED_TYPE_GANG10;
    const int32 chanceA = isGang ? 0 : 90;
    const int32 chanceB = isGang ? 100 : 10;

    // Como el original: el EXE borra la info antes de buscar y solo escribe la duración en cada rama.
    m_IntInfo = nullptr;

    const auto Find = [this](eInteriorInfoType type) {
        return m_IntGrp->FindInteriorInfo(type, &m_IntInfo, &m_Int);
    };

    // Como el original: (rand() & 0xFFFF) * (1/32768) * 100 truncado.
    const int32 rnd = (int32)((float)CGeneral::GetRandomNumber() * (1.0f / 32768.0f) * 100.0f);
    if (rnd < chanceA) {
        if (CGeneral::GetRandomNumber() < 0x3FFF) {
            if (!Find(UNK_3)) {
                Find(UNK_4);
            }
        } else {
            if (!Find(UNK_4)) {
                Find(UNK_3);
            }
        }
        *outDuration = -1;
    }
    if (m_IntInfo) {
        return;
    }

    if (rnd < chanceA + chanceB) {
        Find(UNK_1);
        *outDuration = -1;
    }
    if (m_IntInfo) {
        return;
    }

    if (rnd < chanceA + chanceB) {
        Find(UNK_5);
        // Como el original: la duración (y su aleatorio) se calcula aunque no se encuentre la info.
        *outDuration = 5000 - (int32)((float)CGeneral::GetRandomNumber() * (1.0f / 32768.0f) * -25000.0f);
    }
    if (m_IntInfo) {
        return;
    }

    Find(UNK_1);
    *outDuration = -1;
    if (m_IntInfo) {
        return;
    }

    Find(UNK_2);
    *outDuration = -1;
}

// 0x676200
CTask* CTaskInteriorBeInHouse::CreateNextSubTask(CPed* ped) {
    // Como el original: marca el flag 0x474 bit 7 del peatón y la siguiente info no es instantánea.
    ped->bHasJustSoughtCover = true;
    int32 duration;
    GetInfoForPedToUse(ped, &duration);
    return new CTaskInteriorUseInfo{ m_IntInfo, m_Int, duration, false };
}

// 0x6762B0
CTask* CTaskInteriorBeInHouse::CreateFirstSubTask(CPed* ped) {
    int32 duration;
    GetInfoForPedToUse(ped, &duration);
    return new CTaskInteriorUseInfo{ m_IntInfo, m_Int, duration, true };
}
