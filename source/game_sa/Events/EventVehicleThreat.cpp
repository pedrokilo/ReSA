/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleThreat
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
#include "EventVehicleThreat.h"

namespace {
// 0x4AFBF0 (lote 178A, sin validar): cuerpo del destructor de CEventVehicleThreat del EXE (thiscall, sin argumentos de pila, `ret`, 81 bytes con marco SEH que solo desenrolla). Como el original: deja la vtable de la
// clase del EXE (0x85B298), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula del vehiculo (+0x14, ECX = vehiculo, un argumento de pila = &m_Vehicle) y deja la vtable de CEvent del EXE
// (0x85AB38; no llama a 0x4AC010, el EXE lo expande en linea). Funcion propia sin expansion en linea; la llama el destructor con borrado del EXE (0x4B6350) como `call 0x4AFBF0`. Mismo cuerpo que
// AcquaintancePedDestructorBody (0x4AF890) con otra vtable.
__declspec(noinline) void __fastcall VehicleThreatDestructorBody(CEventVehicleThreat* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B298;
    CEntity::SafeCleanUpRef(self->m_Vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventVehicleThreat::InjectHooks() {
    RH_ScopedVirtualClass(CEventVehicleThreat, 0x85B298, 17); // lote 187A (sin validar): vtable del EXE, CloneEditable en la ranura 16
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(VehicleThreatDestructorBody, "DestructorBody", 0x4AFBF0); // lote 178A (sin validar)
    RH_ScopedInstall(Constructor, 0x4AFB50);                                              // lote 178A (sin validar)
    RH_ScopedVMTInstall(CloneEditable, 0x4B7520);                                         // lote 187A (sin validar): ranura 16
}

// 0x4B7520 (lote 187A, sin validar; el listado decia CEventAttractor; thiscall, sin argumentos, `ret`, 97 bytes con marco SEH que solo desenrolla la reserva; ranura 16 de la vtable 0x85B298). Como el original:
// CEvent::operator new (0x4B5620; el EXE lleva en linea el CEventPool::New 0x4B5570); sin memoria devuelve nulo; si no, llama al constructor (0x4AFB50, con hook; ECX = memoria) con el vehiculo (+0x14) y devuelve
// lo que deja en EAX (la memoria).
CEventEditableResponse* CEventVehicleThreat::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventVehicleThreat));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventVehicleThreat*>(memory)->Constructor(m_Vehicle);
}

// 0x4AFB50 (lote 178A, sin validar; thiscall, un argumento de pila, `ret 4`, 102 bytes con marco SEH que solo desenrolla): constructor del EXE con el de CEvent y el de CEventEditableResponse expandidos en linea.
// Deja +4 = 0 (dword, m_nTimeActive), +8 = 0 (byte, m_bValid), +0xC = 1 (byte, m_bAddToEventGroup), +0xE = 200 (word, TASK_NONE), +0x10 = 0xFFFF (word), la vtable del EXE (0x85B298), guarda el vehiculo en +0x14
// y lo registra con CEntity::RegisterReference (0x571B70, un argumento de pila = &m_Vehicle) solo si no es nulo. Devuelve this.
// Banco Fn178A: con la forma exacta del EXE (escrituras en este orden). El constructor de C++ dejaria la vtable de la DLL en lugar de la 0x85B298 del EXE.
CEventVehicleThreat* CEventVehicleThreat::Constructor(CVehicle* vehicle) {
    m_nTimeActive = 0;
    m_bValid = false;
    m_bAddToEventGroup = true;
    m_TaskId = TASK_NONE;
    m_FacialExpressionType = eFacialExpression::NONE;
    *reinterpret_cast<uint32*>(this) = 0x85B298;
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
    return this;
}

CEventVehicleThreat::CEventVehicleThreat(CVehicle* vehicle) : CEventEditableResponse() {
    m_Vehicle = vehicle;
    CEntity::SafeRegisterRef(m_Vehicle);
}

CEventVehicleThreat::~CEventVehicleThreat() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

bool CEventVehicleThreat::AffectsPed(CPed* ped) {
    return ped->IsAlive();
}
