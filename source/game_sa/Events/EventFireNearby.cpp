/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventFireNearby
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventFireNearby.h"

namespace {
// 0x4B1F80
// Cuerpo no virtual de ~CEventFireNearby del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall FireNearbyDestructorBody(CEventFireNearby* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventFireNearby::InjectHooks() {
    RH_ScopedVirtualClass(CEventFireNearby, 0x85B6E8, 17);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(CloneEditable, 0x4B7A70); // lote 201A (sin validar)
    RH_ScopedNamedGlobalInstall(FireNearbyDestructorBody, "Destructor", 0x4B1F80);

    RH_ScopedInstall(Constructor, 0x4B1F10);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1F90); // lote 184A (sin validar): ranura 5
}



CEventFireNearby::CEventFireNearby(const CVector& position) : CEventEditableResponse() {
    m_position = position;
}

// 0x4B1F90 (lote 184A, sin validar; el listado decia CEventPotentialWalkIntoFire; ranura 5 de la vtable 0x85B6E8; thiscall, un puntero en la pila, `ret 4`, 45 bytes): si el gestor de tareas del peaton (inteligencia +0x47C, +4)
// tiene una tarea activa de tipo 0x25C (TASK_COMPLEX_EXTINGUISH_FIRES) con CTaskManager::FindActiveTaskByType (0x681740, con hook) devuelve falso; si no, devuelve CPed::IsAlive (0x5E0170, con hook; ECX = peaton).
static_assert(TASK_COMPLEX_EXTINGUISH_FIRES == 0x25C, "el EXE empuja 0x25C a FindActiveTaskByType");
// 0x4B1F90
bool CEventFireNearby::AffectsPed(CPed* ped) {
    if (ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_EXTINGUISH_FIRES)) {
        return false;
    }
    return ped->IsAlive();
}

// 0x4B1F10
CEventFireNearby* CEventFireNearby::Constructor(const CVector& position) {
    this->CEventFireNearby::CEventFireNearby(position);
    return this;
}

// 0x4B7A70 (lote 201A, sin validar; thiscall, sin argumentos, `ret`, 76 bytes sin marco SEH; ranura 16 de la vtable 0x85B6E8; el listado decia CEventVehicleOnFire). Como el original: CEvent::operator new (0x4B5620, con hook; el EXE lo expande en
// linea con 0x4B5570 sobre el pool 0xB744AC) con 0x20 bytes; sin memoria devuelve nulo (EAX = 0). NO llama a ningun constructor: escribe a mano +4 = 0, BYTE +8 = 0, la parte de CEventEditableResponse (BYTE +0xC = 1, WORD +0xE = 0xC8, WORD +0x10 = 0xFFFF),
// la vtable del EXE (0x85B6E8) y copia como palabras de 32 bits el CVector +0x14 (posicion); devuelve la memoria. Los bytes de relleno +9..+0xB y +0x12..+0x13 no se escriben.
CEventFireNearby* CEventFireNearby::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventFireNearby));
    if (!memory) {
        return nullptr;
    }
    auto* const       dst = static_cast<uint8*>(memory);
    const auto* const src = reinterpret_cast<const uint8*>(this);
    *reinterpret_cast<uint32*>(dst + 0x4)  = 0;
    *(dst + 0x8)                           = 0;
    *(dst + 0xC)                           = 1;
    *reinterpret_cast<uint16*>(dst + 0xE)  = 0xC8;
    *reinterpret_cast<uint16*>(dst + 0x10) = 0xFFFF;
    *reinterpret_cast<uint32*>(dst + 0x0)  = 0x85B6E8;
    for (uint32 offset = 0x14; offset < 0x20; offset += sizeof(uint32)) {
        *reinterpret_cast<uint32*>(dst + offset) = *reinterpret_cast<const uint32*>(src + offset);
    }
    return static_cast<CEventFireNearby*>(memory);
}
