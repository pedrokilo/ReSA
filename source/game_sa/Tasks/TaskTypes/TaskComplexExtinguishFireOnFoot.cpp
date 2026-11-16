// Sin PCH (CMakeLists.txt): StdInc.h redefine NOMINMAX y el aviso C4005 es error; mismo recurso que Fx.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "TaskComplexExtinguishFireOnFoot.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleGunControl.h"
#include "FireManager.h"
#include "Fire.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
static_assert(TASK_COMPLEX_EXTINGUISH_FIRE_ON_FOOT == 0x6E);   // 0x6598A0: `mov eax, 0x6E; ret`
static_assert(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL == 0x387);
static_assert(TASK_SIMPLE_GUN_CTRL == 0x3FC);
static_assert(PEDMOVE_RUN == 6);
static_assert(static_cast<int>(eGunCommand::RELOAD) == 4);

// 0x6598B0 (lote 193A, sin validar): cuerpo del destructor de CTaskComplexExtinguishFireOnFoot del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86F5FC; jmp 0x61A3D0`). Deja la vtable de la clase del EXE
// (0x86F5FC) y sigue con el cuerpo de ~CTaskComplex (0x61A3D0, con hook). Funcion propia sin expansion en linea.
__declspec(noinline) void __fastcall ExtinguishFireOnFootDestructorBody(CTaskComplexExtinguishFireOnFoot* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F5FC;
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x65A700 (lote 193A, sin validar; el listado decia CTaskComplexUseWaterCannon: es la ranura 0 de la vtable 0x86F5FC de CTaskComplexExtinguishFireOnFoot; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x6598B0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x6598B0, ExtinguishFireOnFootDestructorBody), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexExtinguishFireOnFoot* __fastcall ExtinguishFireOnFootScalarDeletingDestructor(CTaskComplexExtinguishFireOnFoot* self, void* /*edx*/, uint8 flags) {
    ExtinguishFireOnFootDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexExtinguishFireOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexExtinguishFireOnFoot, 0x86F5FC, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x659870);                                                                         // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(ExtinguishFireOnFootDestructorBody, "DestructorBody", 0x6598B0);                     // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(ExtinguishFireOnFootScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65A700); // lote 193A (sin validar)
    RH_ScopedInstall(FindNearestFire, 0x659990);                                                                     // lote 193A (sin validar)

    RH_ScopedVMTInstall(GetTaskType, 0x6598A0);          // lote 193A (sin validar): retorno de constante (TASK_COMPLEX_EXTINGUISH_FIRE_ON_FOOT = 0x6E)
    RH_ScopedVMTInstall(Clone, 0x659D70);                // lote 193A (sin validar)
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6598C0);    // lote 193A (sin validar)
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65A720);   // lote 193A (sin validar)
    RH_ScopedVMTInstall(ControlSubTask, 0x659980);       // lote 193A (sin validar)
}

// 0x659870 (lote 193A, sin validar; thiscall, un argumento de pila (puntero a CVector), `ret 4`, 43 bytes): constructor del EXE. Llama al constructor de CTaskComplex (0x61A3B0), deja la vtable de la clase y copia los tres floats
// (como tres dwords) de la posicion a +0xC; devuelve `this`. El hook recibe el puntero del EXE y lo desreferencia (el EXE tampoco lo comprueba).
CTaskComplexExtinguishFireOnFoot::CTaskComplexExtinguishFireOnFoot(const CVector& pos) :
    m_pos{pos}
{
}

// 0x659D70 (lote 193A, sin validar; ranura 1 de la vtable 0x86F5FC; thiscall, `ret`, 106 bytes con marco SEH que solo libera la memoria si el constructor lanza y aqui se omite): Clone. Reserva 0x18 bytes con CTask::operator new (0x61A5A0);
// si devuelve nulo, devuelve nulo; si no, construye la tarea con la posicion de +0xC y devuelve la tarea.
CTask* CTaskComplexExtinguishFireOnFoot::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskComplexExtinguishFireOnFoot));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskComplexExtinguishFireOnFoot(m_pos);
}

// 0x6598C0 (lote 193A, sin validar; ranura 8 (CreateNextSubTask) de la vtable 0x86F5FC; thiscall, un argumento de pila (el ped), `ret 4`, 180 bytes con marco SEH que solo libera la memoria si el constructor lanza y aqui se omite): pide el tipo
// de la subtarea (llamada virtual, ranura 4, sin comprobar si es nula). Con TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL (0x387) reserva 0x3C bytes (CTask::operator new, 0x61A5A0) y, si no es nulo, construye una CTaskSimpleGunControl (0x61F3F0,
// con hook) con (entidad nula, objetivo = &m_pos (+0xC), destino nulo, orden eGunCommand 4 (RELOAD), rafaga 1, duracion -1) y la devuelve; con memoria nula devuelve nulo. Con TASK_SIMPLE_GUN_CTRL (0x3FC) devuelve CreateFirstSubTask del ped
// (llamada virtual, ranura 9, con el mismo argumento). Con cualquier otro tipo, nulo.
CTask* CTaskComplexExtinguishFireOnFoot::CreateNextSubTask(CPed* ped) {
    const eTaskType subTaskType = m_pSubTask->GetTaskType();
    if (subTaskType == TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) {
        void* const memory = CTask::operator new(sizeof(CTaskSimpleGunControl));
        if (!memory) {
            return nullptr;
        }
        return static_cast<CTaskSimpleGunControl*>(memory)->Constructor(nullptr, &m_pos, nullptr, eGunCommand::RELOAD, 1, -1); // el constructor enganchado 0x61F3F0; devuelve su EAX
    }
    if (subTaskType == TASK_SIMPLE_GUN_CTRL) {
        return CreateFirstSubTask(ped);
    }
    return nullptr;
}

// 0x65A720 (lote 193A, sin validar; ranura 9 (CreateFirstSubTask) de la vtable 0x86F5FC; thiscall, un argumento de pila (el ped), `ret 4`, 155 bytes con marco SEH que solo libera la memoria si el constructor lanza y aqui se omite): busca el fuego mas
// cercano (0x659990); si no hay, devuelve nulo. Si lo hay, copia su posicion (como tres dwords) a +0xC, reserva 0x28 bytes (CTask::operator new, 0x61A5A0; nulo -> nulo) y construye una CTaskComplexGoToPointAndStandStill (0x668120, con hook)
// con (PEDMOVE_RUN = 6, posicion del propio fuego (no la copia), radio 2.0f (0x40000000), radio de estado 2.0f ([0x86FC88]), false, false) y la devuelve.
CTask* CTaskComplexExtinguishFireOnFoot::CreateFirstSubTask(CPed* ped) {
    CFire* const fire = FindNearestFire(ped);
    if (!fire) {
        return nullptr;
    }
    const CVector& firePos = fire->GetPosition();
    m_pos = firePos;
    void* const memory = CTask::operator new(sizeof(CTaskComplexGoToPointAndStandStill));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskComplexGoToPointAndStandStill*>(memory)->Constructor(PEDMOVE_RUN, firePos, 2.0f, 2.0f, false, false); // el constructor enganchado 0x668120; devuelve su EAX
}

// 0x659980 (lote 193A, sin validar; ranura 10 (ControlSubTask) de la vtable 0x86F5FC; thiscall, un argumento de pila (el ped, que no se lee), `ret 4`, 6 bytes: `mov eax,[ecx+8]; ret 4`): devuelve la subtarea actual.
CTask* CTaskComplexExtinguishFireOnFoot::ControlSubTask(CPed* /*ped*/) {
    return m_pSubTask;
}

// 0x659990 (lote 193A, sin validar; thiscall con `this` (ECX) sin usar, un argumento de pila (el ped), `ret 4`, 120 bytes): posicion del ped (matriz +0x14 + 0x30 o posicion propia +4); CFireManager::FindNearestFire (0x538F40, con hook;
// gFireManager 0xB71F80) con (posicion, bCheckIsBeingExtinguished = false, bCheckWasCreatedByScript = true); si no hay fuego, nulo. La distancia al cuadrado se calcula con la posicion del ped releida y el orden x87 del EXE: dx = ped.x -
// fuego.x, dy, dz como floats; ((dz*dz + dx*dx) + dy*dy) con cada operacion redondeada a float (24 bits); devuelve el fuego si la suma es MENOR que 100.0f (0x858628) en comparacion ordenada (con NaN, nulo), si no nulo.
CFire* CTaskComplexExtinguishFireOnFoot::FindNearestFire(CPed* ped) {
    CFire* const fire = gFireManager.FindNearestFire(ped->GetPosition(), false, true);
    if (!fire) {
        return nullptr;
    }
    const CVector& pedPos = ped->GetPosition();
    const CVector& firePos = fire->GetPosition();
    const float dx = pedPos.x - firePos.x;
    const float dy = pedPos.y - firePos.y;
    const float dz = pedPos.z - firePos.z;
    const float zz = dz * dz;
    const float xx = dx * dx;
    const float zzPlusXx = zz + xx;
    const float yy = dy * dy;
    const float distanceSquared = zzPlusXx + yy;
    return distanceSquared < 100.0f ? fire : nullptr;
}
