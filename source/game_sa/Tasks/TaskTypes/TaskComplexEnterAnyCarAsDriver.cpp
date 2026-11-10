/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexEnterAnyCarAsDriver
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexEnterAnyCarAsDriver.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleNone.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x63D100 (lote 141A, banco Fn141A): cuerpo no virtual de ~CTaskComplexEnterAnyCarAsDriver del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86EA14; jmp 0x61A3D0 (~CTaskComplex, con hook)`). Deja la vtable de la clase del EXE (0x86EA14) y sigue con ese cuerpo.
__declspec(noinline) void __fastcall EnterAnyCarAsDriverDestructorBody(CTaskComplexEnterAnyCarAsDriver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86EA14;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x6434F0 (lote 149A, banco Fn149A): destructor con borrado de CTaskComplexEnterAnyCarAsDriver del EXE (ranura 0 de su vtable 0x86EA14; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x63D100; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x63D100), libera
// `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexEnterAnyCarAsDriver* __fastcall EnterAnyCarAsDriverScalarDeletingDestructor(CTaskComplexEnterAnyCarAsDriver* self, void* /*edx*/, uint8 flags) {
    EnterAnyCarAsDriverDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x63D0D0 (lote 149A, banco Fn149A; thiscall, sin argumentos de pila, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x61A3B0; mov [esi],0x86EA14; mov eax,esi; pop esi; ret`): constructor de
// CTaskComplexEnterAnyCarAsDriver del EXE. Llama al cuerpo del constructor de CTaskComplex (0x61A3B0, con hook: pone +4 y +8 a 0 y deja la vtable de CTaskComplex), deja la vtable de la clase del EXE (0x86EA14)
// y devuelve `this`. Funcion propia sin expansion en linea para que el banco pueda sustituir el colaborador.
__declspec(noinline) CTaskComplexEnterAnyCarAsDriver* __fastcall EnterAnyCarAsDriverConstructorBody(CTaskComplexEnterAnyCarAsDriver* self, void* /*edx*/) {
    CTaskComplexBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x86EA14;
    return self;
}
} // namespace

uintptr_t Lote149ABody_CTaskComplexEnterAnyCarAsDriver() { return reinterpret_cast<uintptr_t>(&EnterAnyCarAsDriverDestructorBody); } // para el banco Fn149A

void CTaskComplexEnterAnyCarAsDriver::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterAnyCarAsDriver, 0x86EA14, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(EnterAnyCarAsDriverDestructorBody, "Destructor", 0x63D100); // lote 141A (banco Fn141A)
    RH_ScopedNamedGlobalInstall(EnterAnyCarAsDriverScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6434F0); // lote 149A (banco Fn149A)
    RH_ScopedNamedGlobalInstall(EnterAnyCarAsDriverConstructorBody, "Constructor", 0x63D0D0);                       // lote 149A (banco Fn149A)

    RH_ScopedVMTInstall(Clone, 0x63DE60);
    RH_ScopedVMTInstall(GetTaskType, 0x63D0F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63D110);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643510);
    RH_ScopedVMTInstall(ControlSubTask, 0x63D120);
}

// 0x643510
CTask* CTaskComplexEnterAnyCarAsDriver::CreateFirstSubTask(CPed* ped) {
    // Try entering the closest vehicle (If it's stealable)
    if (const auto closestVeh = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange()) {
        if (CCarEnterExit::IsVehicleStealable(closestVeh, ped)) {
            return new CTaskComplexEnterCarAsDriver{ closestVeh };
        }
    }

    // Como el original: recorre las 16 entradas del escáner y solo acepta vehículos robables;
    // la distancia es (ped - veh) con la suma (z*z + y*y) + x*x del exe.
    auto closestDistSq = FLT_MAX;
    CVehicle* closest{};
    for (auto* const entity : ped->GetIntelligence()->GetVehicleScanner().m_apEntities) {
        if (!entity) {
            continue;
        }
        auto* const vehicle = entity->AsVehicle();
        const CVector& vehPos = vehicle->GetPosition();
        const CVector& pedPos = ped->GetPosition();
        const float dx = pedPos.x - vehPos.x;
        const float dy = pedPos.y - vehPos.y;
        const float dz = pedPos.z - vehPos.z;
        const float distSq = dz * dz + dy * dy + dx * dx;
        if (distSq < closestDistSq && CCarEnterExit::IsVehicleStealable(vehicle, ped)) {
            closestDistSq = distSq;
            closest = vehicle;
        }
    }

    if (closest) {
        return new CTaskComplexEnterCarAsDriver{ closest };
    }
    return new CTaskSimpleNone{};
}
