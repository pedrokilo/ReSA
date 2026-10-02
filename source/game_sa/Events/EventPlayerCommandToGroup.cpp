/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPlayerCommandToGroup
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPlayerCommandToGroup.h"


void CEventPlayerCommandToGroup::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroup, 0x85B800, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B23D0);
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
    CPed* leader = pedGroup->GetMembership().GetLeader();
    if (leader && leader->IsPlayer()) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (member && member->GetAreaCode() != leader->GetAreaCode()) 
                return false;
        }
    }
    return true;
}
