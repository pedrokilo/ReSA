/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventDontJoinPlayerGroup
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
#include "EventDontJoinPlayerGroup.h"


namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x609140 (lote 115A, banco Fn115A): cuerpo no virtual de ~CEventDontJoinPlayerGroup del EXE (thiscall, sin argumentos de pila, `ret`)
// Deja la vtable de CEventDontJoinPlayerGroup del EXE (0x86D0E0), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula en +0xC y llama al cuerpo de ~CEvent (0x4AC010), que deja la vtable de CEvent (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla.
void __fastcall DontJoinPlayerGroupDestructorBody(CEventDontJoinPlayerGroup* self, void* /*edx*/) {
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x86D0E0;
    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0xC));
    CEventBody::EventDestructorBody(self, nullptr);
}
} // namespace

void CEventDontJoinPlayerGroup::InjectHooks()
{
    RH_ScopedVirtualClass(CEventDontJoinPlayerGroup, 0x86D0E0, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(DontJoinPlayerGroupDestructorBody, "Destructor", 0x609140);

    RH_ScopedInstall(Constructor, 0x6090E0);
}

// 0x6090E0
CEventDontJoinPlayerGroup::CEventDontJoinPlayerGroup(CPed* player)
{
    m_player = player;
    CEntity::SafeRegisterRef(m_player);
}

CEventDontJoinPlayerGroup::~CEventDontJoinPlayerGroup()
{
    CEntity::SafeCleanUpRef(m_player);
}

// 0x6090E0
CEventDontJoinPlayerGroup* CEventDontJoinPlayerGroup::Constructor(CPed* player)
{
    this->CEventDontJoinPlayerGroup::CEventDontJoinPlayerGroup(player);
    return this;
}

