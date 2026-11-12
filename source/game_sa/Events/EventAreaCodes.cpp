/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventAreaCodes
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
#include "EventAreaCodes.h"


namespace CEventAreaCodesBody {
// 0x4B2210 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventAreaCodes que usan los destructores del EXE. Deja la vtable de CEventAreaCodes del EXE (0x85B7C0), libera la referencia de m_ped (+0xC) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall AreaCodesDestructorBody(CEventAreaCodes* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B7C0;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace CEventAreaCodesBody

namespace {
// 0x4B6A30 (lote 184A, sin validar; el listado decia CEventPotentialWalkIntoFire): destructor con borrado de CEventAreaCodes del EXE (ranura 0 de su vtable 0x85B7C0; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B2210` (cuerpo del
// destructor, con hook propio); con el bit 0 del BYTE de banderas libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`). Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0). Como CEventDamage (lote 176A), usa CEvent::operator delete (0x4B5630, con hook).
__declspec(noinline) CEventAreaCodes* __fastcall AreaCodesScalarDeletingDestructor(CEventAreaCodes* self, void* /*edx*/, uint8 flags) {
    CEventAreaCodesBody::AreaCodesDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventAreaCodes::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAreaCodes, 0x85B7C0, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(CEventAreaCodesBody::AreaCodesDestructorBody, "Destructor", 0x4B2210);
    RH_ScopedNamedGlobalInstall(AreaCodesScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6A30); // lote 184A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B2190);
    RH_ScopedVMTInstall(GetEventType, 0x4B21F0); // lote 144A (banco Fn144A): retorno de constante (67), ranura 1 de la vtable 0x85B7C0
    RH_ScopedVMTInstall(GetLifeTime, 0x4B2200);  // lote 144A: 0, ranura 3
    RH_ScopedVMTInstall(AffectsPed, 0x4B2270);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B2350);
}

// 0x4B2190
CEventAreaCodes::CEventAreaCodes(CPed* ped)
{
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

CEventAreaCodes::~CEventAreaCodes()
{
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B2190
CEventAreaCodes* CEventAreaCodes::Constructor(CPed* ped)
{
    this->CEventAreaCodes::CEventAreaCodes(ped);
    return this;
}

// 0x4B2270
bool CEventAreaCodes::AffectsPed(CPed* ped)
{
    if (!m_ped)
        return false;

    CPed* targetPed = nullptr;
    if (const auto killPedOnFoot = ped->GetTaskManager().Find<CTaskComplexKillPedOnFoot>()) {
        targetPed = killPedOnFoot->m_target;
        if (targetPed != m_ped) {
            if (const auto arrestPed = ped->GetTaskManager().Find<CTaskComplexArrestPed>()) {
                targetPed = arrestPed->m_PedToArrest;
            }
        }
    }

    if (targetPed == m_ped) {
        if (m_ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_GOTO_DOOR_AND_OPEN))
            return true;
        if (ped->m_pContactEntity && m_ped->m_pContactEntity) {
            if (ped->m_pContactEntity->GetAreaCode() == m_ped->m_pContactEntity->GetAreaCode())
                return false;
        }
        if (m_ped->IsAlive()
            && ped->IsAlive()
            && (!ped->m_pContactEntity || ped->m_pContactEntity->GetAreaCode() != AREA_CODE_13)
            && (!m_ped->m_pContactEntity || m_ped->m_pContactEntity->GetAreaCode() != AREA_CODE_13))
        {
            return true;
        }
    }
    return false;
}

// 0x4B2350
bool CEventAreaCodes::TakesPriorityOver(const CEvent& refEvent)
{
    if (CEventHandler::IsTemporaryEvent(refEvent))
        return true;
    return CEvent::TakesPriorityOver(refEvent);
}
