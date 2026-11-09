/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartnerDeal
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

#include "TaskComplexPartnerDeal.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleRunAnim.h"

namespace {
// 0x684200 (lote 122A, banco Fn122A): cuerpo no virtual de ~CTaskComplexPartnerDeal del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x870754 ; jmp 0x683A40`).
// Deja la vtable de CTaskComplexPartnerDeal del EXE (0x870754) y salta a ~CTaskComplexPartner (0x683A40), que desde el lote 123A (banco Fn123A) esta en C++: llamada directa.
// Lo llaman el funclet de limpieza (SEH) y el destructor con borrado.
void __fastcall PartnerDealDestructorBody(CTaskComplexPartnerDeal* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870754;
    CTaskComplexPartnerBody::Destructor(self, nullptr);
}
} // namespace

void CTaskComplexPartnerDeal::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexPartnerDeal, 0x870754, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(PartnerDealDestructorBody, "Destructor", 0x684200);
    RH_ScopedInstall(Constructor, 0x684190);
    RH_ScopedVMTInstall(GetTaskType, 0x681DF0);
    RH_ScopedVMTInstall(Clone, 0x684D10); // lote 132A (banco Fn132A)
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6823B0);
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x6823C0);
    RH_ScopedVMTInstall(GetPartnerSequence, 0x682440);
}

CTaskComplexPartnerDeal::CTaskComplexPartnerDeal(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, true, 1, point)
{
    m_taskId = TASK_COMPLEX_PARTNER_DEAL;
    // Como el original: copia los 6 bytes de "gangs" (dword + word desde 0x86F92C) sin tocar el resto del búfer
    strcpy(m_animBlockName, "gangs");
}

// 0x684D10 (lote 132A, banco Fn132A): Clone del EXE (thiscall, sin argumentos, `ret`, marco SEH que solo libera la memoria si el constructor lanza: se omite). Reserva 0x74 bytes con CTask::operator new
// (0x61A5A0); si devuelve nulo, devuelve nulo. Si no, llama al constructor (0x684190) sobre la memoria con (nombre de comando +0x14, companero +0x38, lider +0x58 sin signo, multiplicador +0x3C y el punto
// +0x40 por valor) y devuelve lo que el constructor deja en EAX.
CTask* CTaskComplexPartnerDeal::Clone() const {
    auto* const memory = static_cast<CTaskComplexPartnerDeal*>(CTask::operator new(sizeof(CTaskComplexPartnerDeal)));
    if (!memory) {
        return nullptr;
    }
    return memory->Constructor(m_commandName, m_partner, m_leadSpeaker, m_distanceMultiplier, m_point);
}

CTaskComplexPartnerDeal* CTaskComplexPartnerDeal::Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, CVector point)
{
    this->CTaskComplexPartnerDeal::CTaskComplexPartnerDeal(commandName, partner, leadSpeaker, distanceMultiplier, point);
    return this;
}

// 0x6823B0
CTask* CTaskComplexPartnerDeal::CreateFirstSubTask(CPed* ped)
{
    return CTaskComplexPartner::CreateFirstSubTask(ped); // Como el original: salto directo a 0x681F20
}

// 0x6823C0
void CTaskComplexPartnerDeal::StreamRequiredAnims()
{
    // Como el original: sin referencia, se pide/referencia el bloque si hay que cargar las animaciones de bandas;
    // con referencia, se suelta si ya no hay que cargarlas (el bloque cargado se compara con == 1)
    if (reinterpret_cast<const uint8&>(m_requiredAnimsStreamedIn) == 0) { // Como el original: se prueba el byte entero
        if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            return;
        }
        const auto blkIdx = CAnimManager::GetAnimationBlockIndex(m_animBlockName);
        if (reinterpret_cast<const uint8&>(CAnimManager::GetAnimBlocks()[blkIdx].IsLoaded) != 1) {
            CStreaming::RequestModel(IFPToModelId(blkIdx), STREAMING_KEEP_IN_MEMORY);
            return;
        }
        CAnimManager::AddAnimBlockRef(blkIdx);
        m_requiredAnimsStreamedIn = true;
    } else {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            return;
        }
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_animBlockName));
        m_requiredAnimsStreamedIn = false;
    }
}

// 0x682440
CTaskComplexSequence* CTaskComplexPartnerDeal::GetPartnerSequence()
{
    auto* seq = new CTaskComplexSequence; // Como el original: sin inicialización por valor
    seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f, 0.02f)); // 0x86FC7C y 0x3CA3D70A
    // Como el original: el byte m_leadSpeaker se prueba entero (cualquier valor distinto de 0 es verdadero)
    const bool lead = reinterpret_cast<const uint8&>(m_leadSpeaker) != 0;
    seq->AddTask(new CTaskSimpleRunAnim(ANIM_GROUP_GANGS, lead ? ANIM_ID_DEALER_DEAL : ANIM_ID_DRUGS_BUY, 4.0f, false));
    return seq;
}
