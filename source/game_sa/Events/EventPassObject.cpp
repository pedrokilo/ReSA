/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPassObject
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPassObject.h"


void CEventPassObject::InjectHooks() {
    RH_ScopedVirtualClass(CEventPassObject, 0x86F8B8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x65DC70);
    RH_ScopedVMTInstall(IsValid, 0x4B1700);
}

// 0x65DC70
CEventPassObject* CEventPassObject::Constructor(CPed* giver, bool dontPassObject) { this->CEventPassObject::CEventPassObject(giver, dontPassObject); return this; }

// 0x65DC70
CEventPassObject::CEventPassObject(CPed* giver, bool dontPassObject) :
    m_giver{giver},
    m_dontPassObject{dontPassObject}
{
    CEntity::SafeRegisterRef(m_giver);
}

CEventPassObject::~CEventPassObject() {
    CEntity::SafeCleanUpRef(m_giver);
}

// 0x4B1700
bool CEventPassObject::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();

    if (CEvent::IsValid(ped))
        return true;

    return false;
}