/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPlayerCommandToGroup
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
#include "EventPlayerCommandToGroup.h"


namespace CEventPlayerCommandBody {
// 0x4B2470 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventPlayerCommandToGroup que usan los destructores del EXE. Deja la vtable de CEventPlayerCommandToGroup del EXE (0x85B800), libera la referencia de m_target (+0x18) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall PlayerCommandToGroupDestructorBody(CEventPlayerCommandToGroup* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B800;
    CEntity::SafeCleanUpRef(self->m_target);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace CEventPlayerCommandBody

namespace {
// 0x4B6A80 (lote 184A, sin validar; el listado decia CEventPotentialWalkIntoFire): destructor con borrado de CEventPlayerCommandToGroup del EXE (ranura 0 de su vtable 0x85B800; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B2470` (cuerpo del
// destructor, con hook propio); con el bit 0 del BYTE de banderas libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`). Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0). Como CEventDamage (lote 176A), usa CEvent::operator delete (0x4B5630, con hook).
__declspec(noinline) CEventPlayerCommandToGroup* __fastcall PlayerCommandToGroupScalarDeletingDestructor(CEventPlayerCommandToGroup* self, void* /*edx*/, uint8 flags) {
    CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPlayerCommandToGroup::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroup, 0x85B800, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody, "Destructor", 0x4B2470);
    RH_ScopedNamedGlobalInstall(PlayerCommandToGroupScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6A80); // lote 184A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B23D0);
    RH_ScopedVMTInstall(GetLifeTime, 0x4B2440);       // lote 138A (banco Fn138A): retorno de constante
    RH_ScopedVMTInstall(AffectsPed, 0x4B2450);        // lote 138A: siempre false (`xor al,al; ret 4`)
    RH_ScopedVMTInstall(GetSourceEntity, 0x4B2460);   // lote 138A: devuelve m_target (+0x18)
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B2520); // lote 138A: siempre true (`mov al,1; ret 4`)
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B24D0);
}

// 0x4B23D0
CEventPlayerCommandToGroup::CEventPlayerCommandToGroup(ePlayerGroupCommand command, CPed* target)
{
    m_command = command;
    m_target = target;
    CEntity::SafeRegisterRef(m_target);
}

CEventPlayerCommandToGroup::~CEventPlayerCommandToGroup()
{
    CEntity::SafeCleanUpRef(m_target);
}

// 0x4B23D0
CEventPlayerCommandToGroup* CEventPlayerCommandToGroup::Constructor(ePlayerGroupCommand command, CPed* target)
{
    this->CEventPlayerCommandToGroup::CEventPlayerCommandToGroup(command, target);
    return this;
}

// 0x4B24D0
bool CEventPlayerCommandToGroup::AffectsPedGroup(CPedGroup* pedGroup)
{
    // Como el original: sin lider o con lider que no es el jugador devuelve false
    CPed* leader = pedGroup->GetMembership().GetLeader();
    if (!leader || !leader->IsPlayer())
        return false;
    for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        CPed* member = pedGroup->GetMembership().GetMember(i);
        if (member && member->GetAreaCode() != leader->GetAreaCode())
            return false;
    }
    return true;
}
