/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseWaterCannon
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexUseWaterCannon.h"

#include "TaskSimpleCarDrive.h"
#include "TaskSimpleCarDriveTimed.h"

namespace {
// 0x6597B0 (lote 153A, banco Fn153A): cuerpo del destructor de CTaskComplexUseWaterCannon del EXE (sin hook; vtable 0x86F5D0). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor
// con borrado como el `call 0x6597B0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexUseWaterCannonDestructorBody(CTaskComplexUseWaterCannon* self, void* /*edx*/) { self->CTaskComplexUseWaterCannon::~CTaskComplexUseWaterCannon(); }

// 0x65A620 (lote 153A, banco Fn153A): destructor con borrado de CTaskComplexUseWaterCannon del EXE (ranura 0 de su vtable 0x86F5D0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6597B0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexUseWaterCannon* __fastcall CTaskComplexUseWaterCannonScalarDeletingDestructor(CTaskComplexUseWaterCannon* self, void* /*edx*/, uint8 flags) {
    CTaskComplexUseWaterCannonDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote153ABody_CTaskComplexUseWaterCannon() { return reinterpret_cast<uintptr_t>(&CTaskComplexUseWaterCannonDestructorBody); } // para el banco Fn153A

void CTaskComplexUseWaterCannon::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexUseWaterCannon, 0x86F5D0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexUseWaterCannonScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65A620); // lote 153A (banco Fn153A)
    RH_ScopedInstall(Constructor, 0x659780);
    //VTABLE
    RH_ScopedVMTInstall(Clone, 0x659D10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6597D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6597C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x65A640);
}

CTaskComplexUseWaterCannon* CTaskComplexUseWaterCannon::Constructor(CFire* pFire)
{
    this->CTaskComplexUseWaterCannon::CTaskComplexUseWaterCannon(pFire);
    return this;
}

// 0x659780
CTaskComplexUseWaterCannon::CTaskComplexUseWaterCannon(CFire* pFire)
{
    m_pFire = pFire;
}

// 0x65A620
CTaskComplexUseWaterCannon::~CTaskComplexUseWaterCannon()
{
}

// 0x659D10
CTask* CTaskComplexUseWaterCannon::Clone() const {
    return new CTaskComplexUseWaterCannon(m_pFire);
}

// 0x6597C0
CTask* CTaskComplexUseWaterCannon::CreateNextSubTask(CPed* ped)
{
    // Como el original: llama a GetTaskType de la subtarea y descarta el resultado
    (void)m_pSubTask->GetTaskType();
    return nullptr;
}

// 0x6597D0
CTask* CTaskComplexUseWaterCannon::CreateFirstSubTask(CPed* ped)
{
    return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
}

// 0x65A640
CTask* CTaskComplexUseWaterCannon::ControlSubTask(CPed* ped)
{
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE)
    {
        if (ped->m_pVehicle->GetStatus() == STATUS_PHYSICS && m_pFire->IsActive())
            ped->m_pVehicle->AsAutomobile()->FireTruckControl(m_pFire);
        else
            return new CTaskSimpleCarDriveTimed(ped->m_pVehicle, 0);
    }

    return m_pSubTask;
}
