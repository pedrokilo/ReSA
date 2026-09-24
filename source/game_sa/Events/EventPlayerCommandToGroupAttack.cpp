/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPlayerCommandToGroupAttack
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPlayerCommandToGroupAttack.h"

void CEventPlayerCommandToGroupAttack::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroupAttack, 0x86C710, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5F6340);
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B2530);
}

// 0x5F6340
CEventPlayerCommandToGroupAttack* CEventPlayerCommandToGroupAttack::Constructor(CPed* target)
{
    this->CEventPlayerCommandToGroupAttack::CEventPlayerCommandToGroupAttack(target);
    return this;
}

// 0x4B2530
bool CEventPlayerCommandToGroupAttack::AffectsPedGroup(CPedGroup* pedGroup)
{
    bool affectsPedGroup = CEventPlayerCommandToGroup::AffectsPedGroup(pedGroup);
    if (affectsPedGroup) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (member && !member->bDoesntListenToPlayerGroupCommands)
                return true;
        }
        return false;
    }
    return affectsPedGroup;
}