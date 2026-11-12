/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderEnteredCarAsDriver
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventLeaderEnteredCarAsDriver.h"

namespace CEventEditableBody {
void __fastcall EditableResponseDestructorBody(CEventEditableResponse* self, void* edx); // EventEditableResponse.cpp: 0x4AC480
}

namespace {
// 0x48E220 (lote 169A, banco Fn169A): cuerpo no virtual de ~CEventLeaderEnteredCarAsDriver del EXE (thiscall, sin argumentos de pila, `ret`, 90 bytes con marco SEH que solo desenrolla). Deja la vtable de la clase del
// EXE (0x85A198), libera con CEntity::CleanUpOldReference (0x571A00, con hook) la referencia no nula del vehiculo (+0x14) y llama al cuerpo de ~CEventEditableResponse (0x4AC480, con hook), que deja la vtable de
// CEvent. El destructor generado por el compilador dejaria la vtable de la DLL. Funcion propia sin expansion en linea para que el banco pueda sustituirla; el marco SEH se omite.
__declspec(noinline) void __fastcall LeaderEnteredCarAsDriverDestructorBody(CEventLeaderEnteredCarAsDriver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85A198;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);
}

// 0x48E320 (lote 169A, banco Fn169A): destructor con borrado de CEventLeaderEnteredCarAsDriver del EXE (ranura 0 de su vtable 0x85A198; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x48E220; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x48E220),
// libera `this` con CEvent::operator delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CEventLeaderEnteredCarAsDriver* __fastcall LeaderEnteredCarAsDriverScalarDeletingDestructor(CEventLeaderEnteredCarAsDriver* self, void* /*edx*/, uint8 flags) {
    LeaderEnteredCarAsDriverDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace


void CEventLeaderEnteredCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderEnteredCarAsDriver, 0x85A198, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(LeaderEnteredCarAsDriverDestructorBody, "DestructorBody", 0x48E220);                                   // lote 169A (banco Fn169A)
    RH_ScopedNamedGlobalInstall(LeaderEnteredCarAsDriverScalarDeletingDestructor, "ScalarDeletingDestructor", 0x48E320);               // lote 169A

    RH_ScopedInstall(Constructor, 0x48E1C0);
    RH_ScopedVMTInstall(GetEventType, 0x48E280);                    // lote 169A (banco Fn169A): ranura 1 de la vtable 0x85A198
    RH_ScopedVMTInstall(GetEventPriority, 0x48E340);                // lote 169A: ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x48E290);                     // lote 169A: ranura 3
    RH_ScopedVMTInstall(AffectsPed, 0x48E300);                      // lote 169A: ranura 5
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x48E310);     // lote 169A: ranura 15
    RH_ScopedVMTInstall(CloneEditable, 0x48E2A0);                   // lote 169A: ranura 16
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B0EF0);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B0EE0); // lote 139A (banco Fn139A): siempre true (`mov al,1; ret 4`); el listado la atribuia a CEventVehicleDied
}

// 0x48E280 (lote 169A, banco Fn169A; thiscall, `ret`, 6 bytes: `mov eax,36h`): EVENT_LEADER_ENTERED_CAR_AS_DRIVER (54); no lee el objeto.
eEventType CEventLeaderEnteredCarAsDriver::GetEventType() const {
    return EVENT_LEADER_ENTERED_CAR_AS_DRIVER;
}

// 0x48E340 (lote 169A, banco Fn169A; thiscall, `ret`, 6 bytes: `mov eax,2Eh`): prioridad 46; no lee el objeto.
int32 CEventLeaderEnteredCarAsDriver::GetEventPriority() const {
    return 46;
}

// 0x48E290 (lote 169A, banco Fn169A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto.
int32 CEventLeaderEnteredCarAsDriver::GetLifeTime() {
    return 0;
}

// 0x48E300 (lote 169A, banco Fn169A; thiscall, un argumento de pila, `ret 4`, 5 bytes: `xor al,al`): siempre falso; no lee el objeto ni el ped.
bool CEventLeaderEnteredCarAsDriver::AffectsPed(CPed* /*ped*/) {
    return false;
}

// 0x48E310 (lote 169A, banco Fn169A; thiscall, `ret`, 3 bytes: `mov al,1`): siempre verdadero; no lee el objeto.
bool CEventLeaderEnteredCarAsDriver::CanBeInterruptedBySameEvent() {
    return true;
}

// 0x48E2A0 (lote 169A, banco Fn169A; thiscall, sin argumentos, `ret`, 96 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x85A198).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x18 bytes; sin memoria devuelve nulo; si no, llama al constructor (0x48E1C0, con hook; aqui Constructor, sin expansion en linea) con el
// vehiculo guardado (+0x14) y devuelve su resultado.
CEventLeaderEnteredCarAsDriver* CEventLeaderEnteredCarAsDriver::CloneEditable() const noexcept {
    auto* const object = static_cast<CEventLeaderEnteredCarAsDriver*>(CEvent::operator new(sizeof(CEventLeaderEnteredCarAsDriver)));
    return object ? object->Constructor(m_vehicle) : nullptr;
}

// 0x48E1C0
CEventLeaderEnteredCarAsDriver::CEventLeaderEnteredCarAsDriver(CVehicle* vehicle)
{
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventLeaderEnteredCarAsDriver::~CEventLeaderEnteredCarAsDriver()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

// 0x48E1C0
CEventLeaderEnteredCarAsDriver* CEventLeaderEnteredCarAsDriver::Constructor(CVehicle* vehicle)
{
    this->CEventLeaderEnteredCarAsDriver::CEventLeaderEnteredCarAsDriver(vehicle);
    return this;
}

// 0x4B0EF0
bool CEventLeaderEnteredCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup)
{
    if (m_vehicle && pedGroup->m_bMembersEnterLeadersVehicle) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (!member)
                continue;

            if (!member->bInVehicle
                || member->m_pVehicle != m_vehicle
                || member->GetTaskManager().Find<TASK_COMPLEX_LEAVE_CAR>())
            {
                return true;
            }
        }
    }
    return false;
}