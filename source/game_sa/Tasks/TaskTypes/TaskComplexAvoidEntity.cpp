#include "StdInc.h"

#include "TaskComplexAvoidEntity.h"
#include "Entity/Entity.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx);  // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// Lote 203A (sin validar). La clase del EXE con la vtable 0x86FF00 (TASK_COMPLEX_AVOID_ENTITY = 0x3AB, 0x54 bytes, base CTaskComplex) sigue sin reconstruir como clase (sin vtable exportada): se enganchan sus funciones sueltas y los campos se leen por
// desplazamiento. Disposicion deducida del desensamblado (constructor 0x66AA20, Clone 0x66D0C0 que reserva 0x54 bytes y copia estos campos, destructor 0x66AAE0):
//   +0x0C CEntity* (referencia registrada) · +0x10 dword (primer argumento del constructor; el tipo no se deduce) · +0x20 y +0x2C dos CVector (copias del tercer argumento) · +0x44 y +0x48 dwords a cero · +0x4C y +0x4D bytes a cero · +0x50 bit 0 a cero.
template<typename T>
T& FieldAt(void* object, uint32 offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(object) + offset);
}

static_assert(TASK_COMPLEX_AVOID_ENTITY == 0x3AB); // 0x66AAD0: `mov eax, 3ABh; ret`

// 0x66AA20 (lote 203A, sin validar; el listado decia CTaskComplexAvoidOtherPedWhileWandering; thiscall, tres argumentos de pila (un dword, la entidad y un puntero a CVector), `ret 0Ch`, 161 bytes con marco SEH que solo desenrolla (se omite); devuelve `this`):
// constructor de la clase del EXE. Orden del EXE: base CTaskComplex (0x61A3B0, con hook); +0x10 = arg1 y +0x0C = arg2 (en bruto); vtable 0x86FF00; copia el vector apuntado por arg3 a +0x20 y de nuevo a +0x2C (tres dwords cada vez, sin tocar el flotante);
// +0x44 y +0x48 a cero, los BYTE +0x4C y +0x4D a cero y el bit 0 del BYTE +0x50 a cero; AL FINAL, si la entidad no es nula, CEntity::RegisterReference (0x571B70, con hook) con la direccion de +0x0C. Solo lo llama Clone (0x66D0C0).
CTaskComplex* __fastcall AvoidEntityConstructor(CTaskComplex* self, void* /*edx*/, uint32 arg1, CEntity* entity, const CVector* position) {
    CTaskComplexBody::Constructor(self, nullptr);
    FieldAt<uint32>(self, 0x10)   = arg1;
    FieldAt<CEntity*>(self, 0x0C) = entity;
    FieldAt<uint32>(self, 0x00)   = 0x86FF00;
    std::memcpy(&FieldAt<uint8>(self, 0x20), position, 12);
    std::memcpy(&FieldAt<uint8>(self, 0x2C), position, 12);
    FieldAt<uint32>(self, 0x44) = 0;
    FieldAt<uint32>(self, 0x48) = 0;
    FieldAt<uint8>(self, 0x4C)  = 0;
    FieldAt<uint8>(self, 0x4D)  = 0;
    FieldAt<uint8>(self, 0x50) &= 0xFE;
    if (CEntity* const registered = FieldAt<CEntity*>(self, 0x0C)) {
        registered->RegisterReference(&FieldAt<CEntity*>(self, 0x0C));
    }
    return self;
}

// 0x66AAD0 (lote 203A, sin validar; el listado decia CTaskComplexAvoidOtherPedWhileWandering; ranura 4 (GetTaskType) de la vtable 0x86FF00; thiscall, sin argumentos, `ret`, 6 bytes: `mov eax, 3ABh; ret`): no lee el objeto. Se engancha como S (la entrada sirve a la vtable
// y a las llamadas directas) con una funcion suelta porque la clase no esta reconstruida.
eTaskType __fastcall AvoidEntityGetTaskType(void* /*self*/, void* /*edx*/) {
    return TASK_COMPLEX_AVOID_ENTITY;
}

// 0x66AAE0 (lote 203A, sin validar; el listado decia CTaskComplexAvoidOtherPedWhileWandering; thiscall, sin argumentos, `ret`, 90 bytes con marco SEH que solo desenrolla (se omite)): cuerpo del destructor de la clase del EXE (lo llama la ranura 0, 0x66F6A0).
// Deja la vtable 0x86FF00; si la entidad guardada (+0x0C) no es nula, CEntity::CleanUpOldReference (0x571A00, con hook) con la direccion de +0x0C (la referencia NO se pone a nulo); y termina con el cuerpo de CTaskComplex (0x61A3D0, con hook).
void __fastcall AvoidEntityDestructorBody(CTaskComplex* self, void* /*edx*/) {
    FieldAt<uint32>(self, 0x00) = 0x86FF00;
    if (CEntity* const registered = FieldAt<CEntity*>(self, 0x0C)) {
        registered->CleanUpOldReference(&FieldAt<CEntity*>(self, 0x0C));
    }
    CTaskComplexBody::Destructor(self, nullptr);
}
} // namespace

void CTaskComplexAvoidEntity::InjectHooks() {
    RH_ScopedClass(CTaskComplexAvoidEntity); // clase aun sin reconstruir (sin vtable exportada): solo hooks de funciones sueltas por direccion; lote 203A (sin validar)
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(AvoidEntityConstructor, "Constructor", 0x66AA20);
    RH_ScopedNamedGlobalInstall(AvoidEntityGetTaskType, "GetTaskType", 0x66AAD0);
    RH_ScopedNamedGlobalInstall(AvoidEntityDestructorBody, "DestructorBody", 0x66AAE0);
    // TODO: instalar el resto (Clone 0x66D0C0, MakeAbortable 0x66AD40, CreateNextSubTask 0x66AD70, CreateFirstSubTask 0x672530, ControlSubTask 0x66ADE0) conforme se reconstruyan.
}
