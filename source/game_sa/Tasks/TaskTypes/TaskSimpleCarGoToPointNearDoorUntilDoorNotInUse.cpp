/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "TaskSimpleGoToPoint.h"
#include "TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "TaskSimpleStandStill.h"

namespace {
// 0x646E50 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse del EXE (con hook propio (Destructor); vtable 0x86EDE0). Funcion propia sin expansion en linea para que el banco pueda sustituirla;
// la llama el destructor con borrado como el `call 0x646E50` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* self, void* /*edx*/) { self->CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::~CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(); }

// 0x64B480 (lote 150A, banco Fn150A): destructor con borrado de CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse del EXE (ranura 0 de su vtable 0x86EDE0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x646E50; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* __fastcall CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseScalarDeletingDestructor(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote150ABody_CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() { return reinterpret_cast<uintptr_t>(&CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseDestructorBody); } // para el banco Fn150A

void CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse, 0x86ede0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUseScalarDeletingDestructor, "ScalarDeletingDestructor", 0x64B480); // lote 150A (banco Fn150A)

    RH_ScopedInstall(Constructor, 0x646D70);
    RH_ScopedInstall(Destructor, 0x646E50);

    RH_ScopedVMTInstall(Clone, 0x646EC0);
    RH_ScopedVMTInstall(GetTaskType, 0x646E30);
    RH_ScopedVMTInstall(MakeAbortable, 0x646E40);
    RH_ScopedVMTInstall(ProcessPed, 0x64B4A0);

    RH_ScopedInstall(ComputeTarget, 0x646F40); // lote 176A (sin validar)
}

// 0x646D70
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(
    CVehicle*      vehicle,
    int32          targetDoor,
    const CVector& targetPt,
    eMoveState     moveState
) :
    m_TaskGoToPoint{ new CTaskSimpleGoToPoint{ moveState, targetPt } },
    m_TaskWaitForDoor{ new CTaskSimpleCarWaitForDoorNotToBeInUse{ vehicle, (uint32)targetDoor } }
{
}

// 0x646EC0
// Como el original: los datos salen de las subtareas del objeto copiado (antes se leian las del objeto nuevo, sin inicializar)
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse(const CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse& o) :
    CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse{
        o.m_TaskWaitForDoor->m_veh,
        static_cast<int32>(o.m_TaskWaitForDoor->m_doorsToWaitFor[0]),
        o.m_TaskGoToPoint->m_vecTargetPoint,
        o.m_TaskGoToPoint->m_moveState
    }
{
}

// 0x646E50
CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::~CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse() {
    delete m_TaskGoToPoint;
    delete m_TaskWaitForDoor;
}

// 0x64B4A0
bool CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ProcessPed(CPed* ped) {
    m_TaskGoToPoint->UpdatePoint(ComputeTarget());
    if (!m_TaskGoToPoint->ProcessPed(ped)) {
        return m_TaskWaitForDoor->ProcessPed(ped);
    }
    CTaskSimpleStandStill{-1}.ProcessPed(ped);
    return true;
}

// 0x646F40 (lote 176A, sin validar; thiscall con retorno de CVector por puntero oculto, un argumento de pila, `ret 4`, 158 bytes): objetivo del peaton = posicion del coche + (posicion de la puerta - posicion del coche) * 2.
// Como el original, en este orden: GetPositionToOpenCarDoor (0x64E740) con el coche (+8 de la subtarea de espera) y la puerta (+0xC); DESPUES se lee la posicion del coche (matriz +0x14 si existe; si no, la
// posicion propia +4) y se copia a locales; x87 a 24 bits: cada diferencia (puerta - coche) se calcula sin guardar, se duplica (fadd st,st: exacto) y se suma a la posicion del coche, guardando cada componente como float.
#pragma float_control(precise, on, push)
CVector CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget() const {
    const auto    veh     = m_TaskWaitForDoor->m_veh;
    const CVector doorPos = CCarEnterExit::GetPositionToOpenCarDoor(veh, m_TaskWaitForDoor->m_doorsToWaitFor[0]);
    const CVector vehPos  = m_TaskWaitForDoor->m_veh->GetPosition();
    // Como el EXE: la diferencia en z se guarda como float antes de duplicarla y el doble de la diferencia en x se guarda como float antes de sumar la posicion; la y no se guarda (el doble queda en un registro con exponente ampliado)
    volatile float dz = doorPos.z - vehPos.z;
    volatile float x2 = (doorPos.x - vehPos.x) * 2.f;
    return CVector{
        x2 + vehPos.x,
        vehPos.y + (doorPos.y - vehPos.y) * 2.f,
        vehPos.z + dz * 2.f
    };
}
#pragma float_control(pop)
