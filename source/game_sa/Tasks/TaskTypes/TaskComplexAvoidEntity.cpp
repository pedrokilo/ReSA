/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexAvoidEntity
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "TaskComplexAvoidEntity.h"
#include "Entity/Entity.h"
#include "Entity/Ped/Ped.h"
#include "Collision/ColSphere.h"
#include "Models/ModelInfo.h"
#include "PedGeometryAnalyser.h"

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

// 0x66F6A0 (lote 205A, sin validar; el listado decia CTaskComplexWanderGang; ranura 0 de la vtable 0x86FF00 de la clase del EXE; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x66AAE0; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`): destructor con borrado. Como el original: destruye con el cuerpo de la clase (0x66AAE0), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si
// el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (clase sin reconstruir: RH_ScopedVMTInstall no es aplicable).
__declspec(noinline) CTaskComplex* __fastcall AvoidEntityScalarDeletingDestructor(CTaskComplex* self, void* /*edx*/, uint8 flags) {
    // Cuerpo 0x66AAE0 (lote 203A, sin hook en esta base): se llama por su direccion del EXE.
    reinterpret_cast<void(__fastcall*)(CTaskComplex*, void*)>(0x66AAE0)(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x66F6C0 (lote 205A, sin validar; el listado decia CTaskComplexWanderGang; thiscall con `this` SIN USAR (ECX llega pero no se lee; la llama 0x66FD30 con ECX = this de la tarea), dos argumentos de pila, `ret 8`, 1541 bytes): calcula la esfera que
// engloba a hasta 16 peatones (el segundo argumento apunta a 16 punteros a CPed contiguos, dos filas de 8; los nulos se saltan) y la escribe en la CColSphere del primer argumento. Como el original:
//  1) suma las posiciones (GetPosition: matriz +0x14 -> +0x30, o +4 sin matriz) de los no nulos en el orden x, y, z, cada suma redondeada a float, y cuenta (int32);
//  2) centro.x = suma.x * (1.0f / n) y centro.y = suma.y * (1.0f / n) (fild n; fdivr [0x858624]: 1.0f / n; con n = 0 sale infinito/NaN, sin proteccion); centro.z = 0 (la suma z se descarta);
//  3) maximo de T = (dy*dy + dx*dx) + r*r con d = posicion - centro (z no interviene) y r = radio de la esfera de la CColModel del modelo del peaton (indice de modelo con SIGNO de 16 bits, ms_modelInfoPtrs 0xA9B0C8, +0x14 = CColModel, +0x24 = radio; sin comprobar
//     nulos), partiendo de 0.0f (0x858B50) y SOLO si T > maximo (con NaN no entra);
//  4) CColSphere::Set (0x40FD10, ECX = salida) con radio = sqrt(maximo) + 0.7f (0x86FCC8), centro, material 0, pieza 0 e iluminacion 0xFF.
// Sin valor de retorno util (EAX queda como lo deja Set; nadie lo usa). x87 a 24 bits: todas las operaciones son de float simple (fsqrt a 24 bits = sqrtf).
__declspec(noinline) void __fastcall AvoidEntityComputeSphere(void* /*self*/, void* /*edx*/, CColSphere* out, CPed* const* peds) {
    static constexpr float ONE       = 1.0f;                                 // 0x858624
    static constexpr float ZERO      = 0.0f;                                 // 0x858B50
    static constexpr float RADIUS_PAD = std::bit_cast<float>(0x3F333333u);   // 0x86FCC8 (0.7f)

    // El EXE guarda cada suma parcial como float (fld sum; fadd pos; fstp sum): un desbordamiento a infinito en una suma intermedia no se recupera despues; `volatile` obliga a ese guardado.
    volatile float sumX = 0.0f, sumY = 0.0f, sumZ = 0.0f;
    int32   count = 0;
    for (int32 i = 0; i < 16; i++) {
        CPed* const ped = peds[i];
        if (!ped) {
            continue;
        }
        count++;
        const CVector& pos = ped->GetPosition();
        sumX = sumX + pos.x;
        sumY = sumY + pos.y;
        sumZ = sumZ + pos.z;
    }

    const float inv = ONE / static_cast<float>(count);
    CVector     center;
    center.x = sumX * inv;
    center.y = sumY * inv;
    center.z = 0.0f;

    float maxDistSq = ZERO;
    for (int32 i = 0; i < 16; i++) {
        CPed* const ped = peds[i];
        if (!ped) {
            continue;
        }
        const CVector& pos    = ped->GetPosition();
        const float    dx     = pos.x - center.x;
        const float    dy     = pos.y - center.y;
        const float    radius = CModelInfo::ms_modelInfoPtrs[static_cast<int16>(ped->m_nModelIndex)]->GetColModel()->GetBoundRadius();
        const float    distSq = dy * dy + dx * dx + radius * radius;
        if (distSq > maxDistSq) {
            maxDistSq = distSq;
        }
    }

    out->Set(std::sqrt(maxDistSq) + RADIUS_PAD, center, static_cast<eSurfaceType>(0), 0, tColLighting{ 0xFF });
}

// 0x66FD30 (lote 205A, sin validar; el listado decia CTaskComplexWanderGang; thiscall, un argumento de pila (el peaton), `ret 4`, 108 bytes; lo llama CreateFirstSubTask 0x672530 tras copiar la posicion del peaton a +0x14): ruta alrededor de la entidad guardada
// (+0x0C, tratada como CPed). Como el original: arma en la pila 16 punteros a CPed con el primero = la entidad (+0x0C) y los otros quince a cero, calcula la esfera con 0x66F6C0 (ECX = this), pone el z del centro a la z del peaton (GetPosition) y devuelve lo que
// devuelve CPedGeometryAnalyser::ComputeRouteRoundSphere (0x5F1890, cdecl, seis argumentos: peaton, esfera, inicio = +0x14, destino = +0x20, salida = +0x38, salida = +0x2C).
__declspec(noinline) bool __fastcall AvoidEntityComputeRouteRoundSphere(CTaskComplex* self, void* /*edx*/, CPed* ped) {
    CPed* peds[16]{};
    peds[0] = FieldAt<CPed*>(self, 0x0C);
    CColSphere sphere;
    AvoidEntityComputeSphere(self, nullptr, &sphere, peds);
    // Como el original (mov eax,[pos+8]; mov [esp+..],eax): copia de dword, sin pasar por la x87 (que silenciaria un NaN senalizante)
    std::memcpy(&sphere.m_vecCenter.z, &ped->GetPosition().z, sizeof(float));
    return CPedGeometryAnalyser::ComputeRouteRoundSphere(
        *ped,
        sphere,
        FieldAt<CVector>(self, 0x14),
        FieldAt<CVector>(self, 0x20),
        FieldAt<CVector>(self, 0x38),
        FieldAt<CVector>(self, 0x2C)
    );
}
} // namespace

void CTaskComplexAvoidEntity::InjectHooks() {
    RH_ScopedClass(CTaskComplexAvoidEntity); // clase aun sin reconstruir (sin vtable exportada): solo hooks de funciones sueltas por direccion; lote 203A (sin validar)
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(AvoidEntityConstructor, "Constructor", 0x66AA20);
    RH_ScopedNamedGlobalInstall(AvoidEntityGetTaskType, "GetTaskType", 0x66AAD0);
    RH_ScopedNamedGlobalInstall(AvoidEntityDestructorBody, "DestructorBody", 0x66AAE0);
    RH_ScopedNamedGlobalInstall(AvoidEntityScalarDeletingDestructor, "ScalarDeletingDestructor", 0x66F6A0);    // lote 205A
    RH_ScopedNamedGlobalInstall(AvoidEntityComputeSphere, "ComputeSphere", 0x66F6C0);                          // lote 205A
    RH_ScopedNamedGlobalInstall(AvoidEntityComputeRouteRoundSphere, "ComputeRouteRoundSphere", 0x66FD30);      // lote 205A
    // TODO: instalar el resto (Clone 0x66D0C0, MakeAbortable 0x66AD40, CreateNextSubTask 0x66AD70, CreateFirstSubTask 0x672530, ControlSubTask 0x66ADE0) conforme se reconstruyan.
}
