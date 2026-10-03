/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorBeInOffice
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

#include "TaskInteriorBeInOffice.h"
#include "TaskInteriorUseInfo.h"

#include "InteriorGroup_c.h"
#include "InteriorInfo_t.h"

void CTaskInteriorBeInOffice::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorBeInOffice, 0x870218, 11);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");

    RH_ScopedInstall(Constructor, 0x675220);
    RH_ScopedInstall(Destructor, 0x675260);

    
    RH_ScopedInstall(GetInfoForPedToUse, 0x675300);
    RH_ScopedVMTInstall(Clone, 0x675270);
    RH_ScopedVMTInstall(GetTaskType, 0x675250);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x676350);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x676400);
    RH_ScopedVMTInstall(ControlSubTask, 0x6752F0);
}

// 0x675220
CTaskInteriorBeInOffice::CTaskInteriorBeInOffice(InteriorGroup_c* ig) :
    m_IntGrp{ ig }
{
}

CTaskInteriorBeInOffice::CTaskInteriorBeInOffice(const CTaskInteriorBeInOffice& o) :
    CTaskInteriorBeInOffice{ o.m_IntGrp }
{
}

// 0x675300
void CTaskInteriorBeInOffice::GetInfoForPedToUse(int32* outDur) {
    using enum eInteriorInfoType;

    // Como el original: la duración se calcula dentro del bucle (con sus aleatorios) en cada intento.
    do {
        const auto type = m_CurrIntInfo && m_CurrIntInfo->Type == UNK_6 ? UNK_7 : UNK_6;
        m_IntGrp->FindInteriorInfo(type, &m_CurrIntInfo, &m_CurrInt);
        if (type == UNK_6) {
            if ((int32)((float)CGeneral::GetRandomNumber() * (1.0f / 32768.0f) * 100.0f) > 20) {
                *outDur = -1;
            } else {
                *outDur = 5000 - (int32)((float)CGeneral::GetRandomNumber() * (1.0f / 32768.0f) * -25000.0f);
            }
        } else if (type == UNK_7) {
            *outDur = 5000 - (int32)((float)CGeneral::GetRandomNumber() * (1.0f / 32768.0f) * -5000.0f);
        }
    } while (!m_CurrIntInfo);
}

// 0x676350
CTask* CTaskInteriorBeInOffice::CreateNextSubTask(CPed* ped) {
    // Como el original: marca el flag 0x474 bit 7 del peatón y la siguiente info no es instantánea.
    ped->bHasJustSoughtCover = true;
    int32 dur;
    GetInfoForPedToUse(&dur);
    return new CTaskInteriorUseInfo{ m_CurrIntInfo, m_CurrInt, dur, false };
}

// 0x676400
CTask* CTaskInteriorBeInOffice::CreateFirstSubTask(CPed* ped) {
    int32 dur;
    GetInfoForPedToUse(&dur);
    return new CTaskInteriorUseInfo{ m_CurrIntInfo, m_CurrInt, dur, true };
}
