/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeanOnVehicle
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
#include "EventLeanOnVehicle.h"

namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x65DC10 (lote 168A, banco Fn168A): cuerpo del destructor de CEventLeanOnVehicle del EXE (thiscall, sin argumentos de pila, `ret`, 90 bytes con marco SEH que solo desenrolla). Deja la vtable de la clase del EXE (0x86F878; la de la
// DLL no vale para los llamadores del EXE), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula del vehiculo (+0xC) y llama al cuerpo de ~CEvent (0x4AC010, con hook propio). Funcion propia sin expansion en linea
// para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x65DC10` del EXE. (El listado lo atribuia a CEventPassObject.)
__declspec(noinline) void __fastcall LeanOnVehicleDestructorBody(CEventLeanOnVehicle* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F878;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x65DBF0 (lote 168A, banco Fn168A): destructor con borrado de CEventLeanOnVehicle del EXE (ranura 0 de su vtable 0x86F878; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x65DC10;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x65DC10), libera `this` con CEvent::operator delete (0x4B5630, con hook)
// solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventLeanOnVehicle* __fastcall LeanOnVehicleScalarDeletingDestructor(CEventLeanOnVehicle* self, void* /*edx*/, uint8 flags) {
    LeanOnVehicleDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventLeanOnVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CEventLeanOnVehicle, 0x86F878, 16);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(LeanOnVehicleDestructorBody, "DestructorBody", 0x65DC10); // lote 168A (banco Fn168A)
    RH_ScopedNamedGlobalInstall(LeanOnVehicleScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65DBF0); // lote 168A (banco Fn168A)
    RH_ScopedInstall(Constructor, 0x65DAF0);
    RH_ScopedVMTInstall(GetEventType, 0x65DB50);      // lote 168A (banco Fn168A): retorno de constante
    RH_ScopedVMTInstall(GetLifeTime, 0x65DB60);       // lote 168A (banco Fn168A): retorno de constante
    RH_ScopedVMTInstall(Clone, 0x65DB70);             // lote 168A (banco Fn168A)
    RH_ScopedVMTInstall(AffectsPed, 0x65DBE0);        // lote 168A (banco Fn168A): retorno de constante
    RH_ScopedVMTInstall(GetEventPriority, 0x65DE10);  // lote 168A (banco Fn168A): retorno de constante
    RH_ScopedVMTInstall(IsValid, 0x4B16C0);
}

// 0x65DB50 (lote 168A, banco Fn168A; thiscall, `ret`, 6 bytes: `mov eax,5Ch`): EVENT_LEAN_ON_VEHICLE (92); no lee el objeto (ranura 1 de la vtable 0x86F878).
eEventType CEventLeanOnVehicle::GetEventType() const {
    return EVENT_LEAN_ON_VEHICLE;
}

// 0x65DE10 (lote 168A, banco Fn168A; thiscall, `ret`, 6 bytes: `mov eax,0Ch`): prioridad 12; no lee el objeto (ranura 2). (El listado lo atribuia a CEventPassObject; la entrada esta en la vtable de CEventLeanOnVehicle.)
int32 CEventLeanOnVehicle::GetEventPriority() const {
    return 12;
}

// 0x65DB60 (lote 168A, banco Fn168A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3).
int32 CEventLeanOnVehicle::GetLifeTime() {
    return 0;
}

// 0x65DB70 (lote 168A, banco Fn168A; thiscall, sin argumentos, `ret`, 102 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x86F878).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo; si no, llama al constructor (0x65DAF0, con hook; ECX = memoria) con el vehiculo (+0xC) y la duracion (+0x10),
// ambos como dwords completos, y devuelve lo que el constructor deja en EAX.
CEventLeanOnVehicle* CEventLeanOnVehicle::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventLeanOnVehicle));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventLeanOnVehicle*>(memory)->Constructor(m_vehicle, m_leanAnimDurationInMs);
}

// 0x65DBE0 (lote 168A, banco Fn168A; thiscall, un puntero de pila, `ret 4`, 5 bytes: `mov al,1`): siempre verdadero; no lee ni el objeto ni el peaton (ranura 5).
bool CEventLeanOnVehicle::AffectsPed(CPed* ped) {
    return true;
}

// 0x65DAF0
CEventLeanOnVehicle* CEventLeanOnVehicle::Constructor(CVehicle* vehicle, int32 leanAnimDurationInMs) { this->CEventLeanOnVehicle::CEventLeanOnVehicle(vehicle, leanAnimDurationInMs); return this; }

// 0x65DAF0
CEventLeanOnVehicle::CEventLeanOnVehicle(CVehicle* vehicle, int32 leanAnimDurationInMs) : CEvent() {
    m_vehicle = vehicle;
    m_leanAnimDurationInMs = leanAnimDurationInMs;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventLeanOnVehicle::~CEventLeanOnVehicle() {
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x4B16C0
bool CEventLeanOnVehicle::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();

    // Como el original: CEvent::IsValid en linea (la llamada virtual sin calificar se llamaba a si misma sin fin)
    if (CEvent::IsValid(ped))
        return true;

    return false;
}