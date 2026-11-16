/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventHitByWaterCannon
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventHitByWaterCannon.h"

namespace {
// Cuerpo no virtual de ~CEventHitByWaterCannon del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall HitByWaterCannonDestructorBody(CEventHitByWaterCannon* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B65C0 (lote 192A, sin validar): destructor con borrado de CEventHitByWaterCannon del EXE (ranura 0 de su vtable 0x85B488; thiscall, un argumento de pila, `ret 4`, 77 bytes: `call 0x4B1320; test byte [esp+..],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice)); devuelve `this`). El EXE conserva ECX tras la llamada al cuerpo (sin pila extra).
// Como el original: destruye con el cuerpo de la clase (0x4B1320), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventHitByWaterCannon* __fastcall HitByWaterCannonScalarDeletingDestructor(CEventHitByWaterCannon* self, void* /*edx*/, uint8 flags) {
    HitByWaterCannonDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace


void CEventHitByWaterCannon::InjectHooks()
{
    RH_ScopedVirtualClass(CEventHitByWaterCannon, 0x85B488, 16);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(Clone, 0x4B77B0); // lote 201A (sin validar)
    RH_ScopedNamedGlobalInstall(HitByWaterCannonDestructorBody, "Destructor", 0x4B1320);
    RH_ScopedNamedGlobalInstall(HitByWaterCannonScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B65C0); // lote 192A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B1290);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1330);
    RH_ScopedVMTInstall(GetEventType, 0x4B12E0);        // lote 166A (banco Fn166A)
    RH_ScopedVMTInstall(GetLifeTime, 0x4B12F0);         // lote 166A
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4B1300);  // lote 166A
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1310);    // lote 166A
}

// 0x4B12E0 (lote 166A, banco Fn166A; ranura 1 de la vtable 0x85B488; thiscall, `ret`): `mov eax, 0x40` (EVENT_WATER_CANNON = 64).
eEventType CEventHitByWaterCannon::GetEventType() const {
    return EVENT_WATER_CANNON;
}

// 0x4B1310 (lote 166A; ranura 2): `mov eax, 0x40` (prioridad 64).
int32 CEventHitByWaterCannon::GetEventPriority() const {
    return 64;
}

// 0x4B12F0 (lote 166A; ranura 3): `xor eax, eax` (vida util 0).
int32 CEventHitByWaterCannon::GetLifeTime() {
    return 0;
}

// 0x4B1300 (lote 166A; ranura 12): `fld dword [0x85ADBC]` (55.0f); `ret`. El resultado queda en ST0.
float CEventHitByWaterCannon::GetLocalSoundLevel() {
    return 55.0f;
}

// 0x4B1290
CEventHitByWaterCannon::CEventHitByWaterCannon(const CVector& point, const CVector& moveSpeed)
{
    m_point = point;
    m_moveSpeed = moveSpeed;
}

// 0x4B1290
CEventHitByWaterCannon* CEventHitByWaterCannon::Constructor(const CVector& point, const CVector& moveSpeed)
{
    this->CEventHitByWaterCannon::CEventHitByWaterCannon(point, moveSpeed);
    return this;
}

// 0x4B1330
bool CEventHitByWaterCannon::AffectsPed(CPed* ped)
{
    if (ped->IsAlive()) {
        CTask* task = ped->GetTaskManager().GetActiveTask();
        if (!task || task->GetTaskType() != TASK_COMPLEX_FALL_AND_GET_UP)
            return true;
    }
    return false;
}

// 0x4B77B0 (lote 201A, sin validar; thiscall, sin argumentos, `ret`, 84 bytes sin marco SEH; ranura 4 de la vtable 0x85B488; el listado decia CEventVehicleOnFire). Como el original: CEvent::operator new (0x4B5620, con hook; el EXE lo expande en
// linea con 0x4B5570 sobre el pool 0xB744AC) con 0x24 bytes; sin memoria devuelve nulo (EAX = 0). NO llama a ningun constructor: escribe a mano +4 = 0, BYTE +8 = 0, la vtable del EXE (0x85B488) y copia como palabras de 32 bits (sin pasar por x87)
// el CVector +0xC (punto) y el CVector +0x18 (velocidad); devuelve la memoria.
CEvent* CEventHitByWaterCannon::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventHitByWaterCannon));
    if (!memory) {
        return nullptr;
    }
    auto* const       dst = static_cast<uint8*>(memory);
    const auto* const src = reinterpret_cast<const uint8*>(this);
    *reinterpret_cast<uint32*>(dst + 0x4) = 0;
    *(dst + 0x8)                          = 0;
    *reinterpret_cast<uint32*>(dst + 0x0) = 0x85B488;
    for (uint32 offset = 0xC; offset < 0x24; offset += sizeof(uint32)) {
        *reinterpret_cast<uint32*>(dst + offset) = *reinterpret_cast<const uint32*>(src + offset);
    }
    return static_cast<CEvent*>(memory);
}
