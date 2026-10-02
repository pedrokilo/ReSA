/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventNewGangMember
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventNewGangMember.h"


void CEventNewGangMember::InjectHooks()
{
    RH_ScopedVirtualClass(CEventNewGangMember, 0x86D0A0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x608F70);
}

// 0x608F70
CEventNewGangMember::CEventNewGangMember(CPed* member)
{
    m_member = member;
    CEntity::SafeRegisterRef(m_member);
}

CEventNewGangMember::~CEventNewGangMember()
{
    CEntity::SafeCleanUpRef(m_member);
}

// 0x608F70
CEventNewGangMember* CEventNewGangMember::Constructor(CPed* member)
{
    this->CEventNewGangMember::CEventNewGangMember(member);
    return this;
}

