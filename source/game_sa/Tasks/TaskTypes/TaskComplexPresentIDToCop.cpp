#include "StdInc.h"

#include "TaskComplexPresentIDToCop.h"

#include "TaskSimpleHandsUp.h"
#include "TaskSimpleAchieveHeading.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x659240 (lote 198A, sin validar; el listado decia CTaskComplexMedicTreatInjuredPed: es la clase CTaskComplexPresentIDToCop, ranura 0 de la vtable 0x86F544): cuerpo del destructor del EXE (thiscall, sin argumentos de pila, `ret`,
// 90 bytes con marco SEH que solo desenrolla y aqui se omite). Como el original: deja la vtable de la clase del EXE (0x86F544); si la referencia al policia (+0xC) no es nula, CEntity::CleanUpOldReference (0x571A00, con hook) con la
// direccion de +0xC (sin poner el puntero a cero); y sigue con ~CTaskComplex (0x61A3D0, con hook). Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall PresentIDToCopDestructorBody(CTaskComplexPresentIDToCop* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F544;
    CEntity::SafeCleanUpRef(self->m_pCop);
    CTaskComplexBody::Destructor(self, nullptr);
}

// 0x65A0F0 (lote 198A, sin validar; el listado decia CTaskComplexMedicTreatInjuredPed: es la ranura 0 de la vtable 0x86F544 de CTaskComplexPresentIDToCop; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x659240; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x659240, PresentIDToCopDestructorBody), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexPresentIDToCop* __fastcall PresentIDToCopScalarDeletingDestructor(CTaskComplexPresentIDToCop* self, void* /*edx*/, uint8 flags) {
    PresentIDToCopDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexPresentIDToCop::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPresentIDToCop, 0x86F544, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(PresentIDToCopDestructorBody, "DestructorBody", 0x659240);                    // lote 198A (sin validar)
    RH_ScopedNamedGlobalInstall(PresentIDToCopScalarDeletingDestructor, "ScalarDeletingDestructor", 0x65A0F0); // lote 198A (sin validar)
    RH_ScopedInstall(Constructor, 0x6591D0);                                                                   // lote 198A (sin validar)
    RH_ScopedInstall(CreateSubTask, 0x65A110);                                                                 // lote 198A (sin validar)
    RH_ScopedInstall(ComputeHeading, 0x6592A0);                                                                // lote 198A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x659230);                                                                // lote 198A (sin validar): retorno de constante (106)
    RH_ScopedVMTInstall(Clone, 0x659B60);                                                                      // lote 198A (sin validar)
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65AFE0);                                                          // lote 198A (sin validar)
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65B050);                                                         // lote 198A (sin validar)
    RH_ScopedVMTInstall(ControlSubTask, 0x65B070);                                                             // lote 198A (sin validar)
}

CTaskComplexPresentIDToCop* CTaskComplexPresentIDToCop::Constructor(CPed* cop) {
    this->CTaskComplexPresentIDToCop::CTaskComplexPresentIDToCop(cop);
    return this;
}

// 0x6591D0 (thiscall, un argumento de pila, `ret 4`, 83 bytes con SEH): ~CTaskComplex (0x61A3B0) -> vtable -> +0xC = policia -> CEntity::RegisterReference (0x571B70) con ECX = el policia y la direccion de +0xC. El EXE NO comprueba que
// el policia sea no nulo antes de registrar la referencia (a diferencia de CTaskComplexMedicTreatInjuredPed): con nulo accede a memoria invalida igual que el original.
CTaskComplexPresentIDToCop::CTaskComplexPresentIDToCop(CPed* cop) : CTaskComplex() {
    m_pCop = cop;
    CEntity::RegisterReference(m_pCop);
}

// Destructor de C++ (el del EXE es PresentIDToCopDestructorBody / 0x659240)
CTaskComplexPresentIDToCop::~CTaskComplexPresentIDToCop() {
    CEntity::SafeCleanUpRef(m_pCop);
}

// 0x659B60 (thiscall, sin argumentos, `ret`, 96 bytes con SEH): reserva 0x10 bytes con CTask::operator new (0x61A5A0); sin memoria devuelve nulo sin leer campos; si no, el constructor con el policia actual (+0xC).
CTask* CTaskComplexPresentIDToCop::Clone() const {
    return new CTaskComplexPresentIDToCop(m_pCop);
}

// 0x65AFE0 (thiscall, un argumento de pila, `ret 4`, 97 bytes): pide el tipo de la subtarea actual (+8, llamada virtual, sin comprobar nulo). TASK_SIMPLE_HANDS_UP (0x19D) -> CreateSubTask(TASK_FINISHED, ped) (nulo);
// TASK_SIMPLE_ACHIEVE_HEADING (0x386) -> el peaton dice CTX_GLOBAL_CRIMINAL_PLEAD (0x45; CPed::Say 0x5EFFE0 con retraso 0, probabilidad 1.0f y las tres banderas a falso) y luego CreateSubTask(TASK_SIMPLE_HANDS_UP, ped);
// cualquier otro tipo -> nulo.
CTask* CTaskComplexPresentIDToCop::CreateNextSubTask(CPed* ped) {
    const eTaskType subTaskType = m_pSubTask->GetTaskType();
    if (subTaskType == TASK_SIMPLE_HANDS_UP) {
        return CreateSubTask(TASK_FINISHED, ped);
    }
    if (subTaskType == TASK_SIMPLE_ACHIEVE_HEADING) {
        ped->Say(CTX_GLOBAL_CRIMINAL_PLEAD, 0, 1.0f, false, false, false);
        return CreateSubTask(TASK_SIMPLE_HANDS_UP, ped);
    }
    return nullptr;
}

// 0x65B050 (thiscall, un argumento de pila, `ret 4`, 18 bytes): CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped).
CTask* CTaskComplexPresentIDToCop::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask(TASK_SIMPLE_ACHIEVE_HEADING, ped);
}

// 0x65B070 (thiscall, un argumento de pila, `ret 4`, 31 bytes): con policia (+0xC no nulo) devuelve la subtarea actual (+8); sin policia, CreateSubTask(TASK_FINISHED, ped) (nulo).
CTask* CTaskComplexPresentIDToCop::ControlSubTask(CPed* ped) {
    if (m_pCop) {
        return m_pSubTask;
    }
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x65A110 (thiscall, dos argumentos de pila, `ret 8`, 259 bytes con SEH que solo libera la memoria si el constructor lanzara). La memoria se reserva ANTES de evaluar los argumentos del constructor.
//  - TASK_SIMPLE_HANDS_UP (0x19D): 0x34 bytes, CTaskSimpleHandsUp con duracion 3000 (el EXE mete en linea el constructor 0x48E970: anim grupo 0, ANIM_ID_HANDSUP 0x8E, mezcla 4.0 / -4.0, tipo 0x19D, "HandsUp").
//  - TASK_SIMPLE_ACHIEVE_HEADING (0x386): 0x18 bytes, CTaskSimpleAchieveHeading(ComputeHeading(ped), 0.5f (0x86FC7C), 0.2f (0x86FC80)).
//  - TASK_FINISHED (0x516) y cualquier otro: nulo.
CTask* CTaskComplexPresentIDToCop::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_HANDS_UP:
        return new CTaskSimpleHandsUp(3000);
    case TASK_SIMPLE_ACHIEVE_HEADING:
        return new CTaskSimpleAchieveHeading(ComputeHeading(ped), 0.5f, 0.2f);
    default:
        return nullptr;
    }
}

// 0x6592A0 (thiscall, un argumento de pila, `ret 4`, 99 bytes): rumbo del policia hacia el peaton = CGeneral::LimitRadianAngle (0x53CB50) de CGeneral::GetRadianAngleBetweenPoints (0x53CBE0) con (policia.x, policia.y, ped.x, ped.y); las
// posiciones salen de la matriz (+0x30) o, sin matriz, de +4. El resultado se guarda como float antes de normalizarlo. Sin comprobar que el policia sea no nulo.
float CTaskComplexPresentIDToCop::ComputeHeading(CPed* ped) {
    const CVector& pedPos = ped->GetPosition();
    const CVector& copPos = m_pCop->GetPosition();
    const float angle = CGeneral::GetRadianAngleBetweenPoints(copPos.x, copPos.y, pedPos.x, pedPos.y);
    return CGeneral::LimitRadianAngle(angle);
}
