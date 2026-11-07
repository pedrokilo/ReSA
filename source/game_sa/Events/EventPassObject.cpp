/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPassObject
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPassObject.h"


namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x65DCF0 (lote 115A, banco Fn115A): cuerpo no virtual de ~CEventPassObject del EXE (thiscall, sin argumentos de pila, `ret`)
// Deja la vtable de CEventPassObject del EXE (0x86F8B8), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula en +0xC y llama al cuerpo de ~CEvent (0x4AC010), que deja la vtable de CEvent (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla.
void __fastcall PassObjectDestructorBody(CEventPassObject* self, void* /*edx*/) {
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x86F8B8;
    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0xC));
    CEventBody::EventDestructorBody(self, nullptr);
}
} // namespace

void CEventPassObject::InjectHooks() {
    RH_ScopedVirtualClass(CEventPassObject, 0x86F8B8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PassObjectDestructorBody, "Destructor", 0x65DCF0);

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