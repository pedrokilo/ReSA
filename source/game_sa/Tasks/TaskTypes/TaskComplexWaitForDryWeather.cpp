/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWaitForDryWeather
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexWaitForDryWeather.h"
#include "TaskSimpleStandStill.h"
#include "Weather.h"

namespace {
// 0x632830 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexWaitForDryWeather del EXE (con hook propio (Destructor); vtable 0x86E160). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x632830` del EXE.
__declspec(noinline) void __fastcall CTaskComplexWaitForDryWeatherDestructorBody(CTaskComplexWaitForDryWeather* self, void* /*edx*/) { self->CTaskComplexWaitForDryWeather::~CTaskComplexWaitForDryWeather(); }

// 0x638900 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexWaitForDryWeather del EXE (ranura 0 de su vtable 0x86E160; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x632830; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexWaitForDryWeather* __fastcall CTaskComplexWaitForDryWeatherScalarDeletingDestructor(CTaskComplexWaitForDryWeather* self, void* /*edx*/, uint8 flags) {
    CTaskComplexWaitForDryWeatherDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexWaitForDryWeather() { return reinterpret_cast<uintptr_t>(&CTaskComplexWaitForDryWeatherDestructorBody); } // para el banco Fn145A

void CTaskComplexWaitForDryWeather::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitForDryWeather, 0x86e160, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexWaitForDryWeatherScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638900); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x632800);
    RH_ScopedInstall(Destructor, 0x632830);

    RH_ScopedVMTInstall(Clone, 0x636A10);
    RH_ScopedVMTInstall(GetTaskType, 0x632820);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632840);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632860);
    RH_ScopedVMTInstall(ControlSubTask, 0x6328F0);
}

CTaskComplexWaitForDryWeather::CTaskComplexWaitForDryWeather(const CTaskComplexWaitForDryWeather& o) :
    CTaskComplexWaitForDryWeather{}
{
}

// 0x632840
CTask* CTaskComplexWaitForDryWeather::CreateNextSubTask(CPed* ped) {
    return CWeather::Rain >= 0.2f
            ? CreateFirstSubTask(ped)
            : nullptr;
}   

// 0x632860
CTask* CTaskComplexWaitForDryWeather::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 1000 };
}
