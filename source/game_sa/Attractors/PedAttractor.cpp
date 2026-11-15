/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedAttractor
 * Reconstrucción: COMPLETA — 44/44 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 44/44 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedAttractor.h"

#include "Tasks/TaskTypes/TaskComplexGoToAttractor.h"
#include "Tasks/TaskTypes/TaskComplexWaitAtAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUsePairedAttractor.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Pools/PoolRawDelete.h"

namespace {
struct SArrayElem20 { uint32 w[5]; };
struct SArrayElem12 { uint32 w[3]; };

// Lote 205A (sin validar): 15 funciones mas del mismo bloque de plantillas de la STL de MSVC 6 (0x5EB140..0x5EB3B2, cdecl, sin llamadores fuera de las instancias de SArray; son plantillas de la STL, no metodos de CPedAttractor).
// Mismas convenciones que arriba: iteradores como punteros por valor, valor por puntero, retorno de estructura en el primer argumento. Los llamadores empujan argumentos de relleno de la plantilla (etiquetas de categoria) que estas
// funciones no leen; con cdecl el llamador los limpia, asi que basta declarar los que se leen.
// std::copy_backward de 4 bytes (0x5EB140, 0x5EB1B0 y 0x5EB260, tres instancias identicas, 40 bytes cada una): si inicio == fin no copia; *out = inicio del destino; devuelve out.
template<int Id>
uint32** __cdecl SArrayCopyBackwardStride4(uint32** out, uint32* first, uint32* last, uint32* destEnd) {
    if (first != last) {
        do {
            const uint32 value = last[-1];
            --last;
            --destEnd;
            *destEnd = value;
        } while (last != first);
    }
    *out = destEnd;
    return out;
}

// Envoltorios de copy_backward (0x5EB170 de 20 bytes y 0x5EB220 de 12 bytes, 50 bytes cada uno): reenvian (out, first, last, destEnd) y repiten destEnd como quinto argumento (el EXE empuja cinco) a la funcion de arriba
// (0x5EAC50 / 0x5EACC0, con hook) y devuelven out (en ESI). El quinto argumento no lo lee la funcion, asi que aqui se omite.
SArrayElem20** __cdecl SArrayCopyBackwardStride20Wrapper(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* destEnd) {
    // Colaborador del lote 189A (sin hook en esta base): se llama por su direccion del EXE.
    reinterpret_cast<SArrayElem20** (__cdecl*)(SArrayElem20**, SArrayElem20*, SArrayElem20*, SArrayElem20*)>(0x5EAC50)(out, first, last, destEnd);
    return out;
}
SArrayElem12** __cdecl SArrayCopyBackwardStride12Wrapper(SArrayElem12** out, SArrayElem12* first, SArrayElem12* last, SArrayElem12* destEnd) {
    // Colaborador del lote 189A (sin hook en esta base): se llama por su direccion del EXE.
    reinterpret_cast<SArrayElem12** (__cdecl*)(SArrayElem12**, SArrayElem12*, SArrayElem12*, SArrayElem12*)>(0x5EACC0)(out, first, last, destEnd);
    return out;
}

// std::fill de 12 bytes (0x5EB1E0, 50 bytes): lee el valor en cada vuelta (tres dwords en orden); con inicio == fin no lo lee.
void __cdecl SArrayFillStride12(SArrayElem12* first, SArrayElem12* last, const SArrayElem12* value) {
    for (; first != last; ++first) {
        first->w[0] = value->w[0];
        first->w[1] = value->w[1];
        first->w[2] = value->w[2];
    }
}

// std::uninitialized_fill_n de 4 bytes (0x5EB2A0, 0x5EB310 y 0x5EB390, tres instancias identicas, 35 bytes cada una): con n == 0 (comparacion SIN signo, `jbe`) no hace nada; si no, n vueltas que escriben el valor (releido en cada
// vuelta) solo si el destino no es nulo (new(puntero) T(valor)), avanzando siempre.
template<int Id>
void __cdecl SArrayFillNStride4(uint32* dest, uint32 count, const uint32* value) {
    if (count == 0) {
        return;
    }
    do {
        if (dest) {
            *dest = *value;
        }
        ++dest;
        --count;
    } while (count != 0);
}

// Igual con elementos de 20 bytes (0x5EB2E0, 46 bytes; `rep movsd` de 5 dwords) y de 12 bytes (0x5EB340, 55 bytes; tres dwords en orden).
void __cdecl SArrayFillNStride20(SArrayElem20* dest, uint32 count, const SArrayElem20* value) {
    if (count == 0) {
        return;
    }
    do {
        if (dest) {
            for (int32 i = 0; i < 5; i++) {
                dest->w[i] = value->w[i];
            }
        }
        ++dest;
        --count;
    } while (count != 0);
}
void __cdecl SArrayFillNStride12(SArrayElem12* dest, uint32 count, const SArrayElem12* value) {
    if (count == 0) {
        return;
    }
    do {
        if (dest) {
            dest->w[0] = value->w[0];
            dest->w[1] = value->w[1];
            dest->w[2] = value->w[2];
        }
        ++dest;
        --count;
    } while (count != 0);
}
} // namespace

namespace {
// Lote 189A (sin validar): 12 funciones de plantilla de la STL de MSVC 6 (std::fill, std::uninitialized_copy, std::copy y std::copy_backward) que el EXE no expande en linea en el codigo de los vectores de los atractores
// (0x5EAAA0..0x5EAF04, cdecl, sin llamadores fuera de las instancias de SArray). El listado las atribuia a CPedAttractorPool; son el mismo bloque que SArraySizeStride20/12 (0x5E9800, PedShelterAttractor.cpp).
// Los iteradores son punteros (4 bytes) pasados por valor; el valor a rellenar se pasa por puntero; las dos copias que devuelven un iterador lo escriben en el puntero que reciben primero (retorno de estructura) y lo devuelven.
// Cada direccion de hook tiene su propia instancia de plantilla (RH no admite dos hooks hacia la misma funcion de la DLL: el cuerpo de la segunda y la tercera se resolvia a la entrada del EXE de la primera; lo detecto el banco Fn189A).
// Cada direccion de hook tiene su propia instancia de plantilla (RH no admite dos hooks hacia la misma funcion de la DLL: el cuerpo de la segunda y la tercera se resolvia a la entrada del EXE de la primera; lo detecto el banco Fn189A).
// Elementos de 4 bytes (SArray<puntero>), de 20 bytes (CPedTaskPair) y de 12 bytes (CVector). Las instancias identicas de distinto tipo de elemento tienen su propia direccion (A, B y C).

// std::fill: lee el valor en cada vuelta; con inicio == fin no lo lee. EAX queda en fin, nadie lo usa.
template<int Id>
void __cdecl SArrayFillStride4(uint32* first, uint32* last, const uint32* value) {
    for (; first != last; ++first) {
        *first = *value;
    }
}
void __cdecl SArrayFillStride20(SArrayElem20* first, SArrayElem20* last, const SArrayElem20* value) {
    for (; first != last; ++first) {
        *first = *value;
    }
}

// std::uninitialized_copy: construye en el destino solo si no es nulo (new(puntero) T(*origen)); devuelve el final del destino en EAX.
template<int Id>
uint32* __cdecl SArrayUninitCopyStride4(const uint32* first, const uint32* last, uint32* dest) {
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
    }
    return dest;
}
SArrayElem20* __cdecl SArrayUninitCopyStride20(const SArrayElem20* first, const SArrayElem20* last, SArrayElem20* dest) {
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
    }
    return dest;
}
SArrayElem12* __cdecl SArrayUninitCopyStride12(const SArrayElem12* first, const SArrayElem12* last, SArrayElem12* dest) {
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
    }
    return dest;
}

// std::copy (hacia delante, con las mismas comprobaciones de rango vacio que el EXE): *out = final del destino; devuelve out.
SArrayElem20** __cdecl SArrayCopyStride20(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* dest) {
    if (first == last) {
        *out = dest;
        return out;
    }
    do {
        *dest = *first;
        ++first;
        ++dest;
    } while (first != last);
    *out = dest;
    return out;
}

// std::copy_backward: recorre desde el final; *out = inicio del destino; devuelve out.
SArrayElem20** __cdecl SArrayCopyBackwardStride20(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* destEnd) {
    if (first == last) {
        *out = destEnd;
        return out;
    }
    do {
        --last;
        --destEnd;
        *destEnd = *last;
    } while (last != first);
    *out = destEnd;
    return out;
}
SArrayElem12** __cdecl SArrayCopyBackwardStride12(SArrayElem12** out, SArrayElem12* first, SArrayElem12* last, SArrayElem12* destEnd) {
    if (first == last) {
        *out = destEnd;
        return out;
    }
    do {
        --last;
        --destEnd;
        *destEnd = *last;
    } while (last != first);
    *out = destEnd;
    return out;
}

// Lote 206A (sin validar): envolturas stdcall de std::uninitialized_copy (0x5EB930, 0x5EB960, 0x5EB990, 0x5EB9D0 y 0x5EBA00; `ret 0Ch`, tres argumentos de pila (origen, fin, destino)). Cada una reserva en la pila un marco
// de 0x14 bytes con los tres argumentos y un hueco sin inicializar (el ECX de entrada, basura), llama a la funcion cdecl de la tabla de 189A (0x5EAE10 / 0x5EAE40 / 0x5EAE70 / 0x5EAEA0 / 0x5EAEE0), la limpia con
// `add esp, 14h` y devuelve su EAX (el final del destino). No usan `this`. Las tres de elemento de 4 bytes (0x5EB930, 0x5EB990 y 0x5EBA00) comparten cuerpo, como las funciones de 189A.
template<int Id>
uint32* __stdcall SArrayUninitCopyWrapStride4(const uint32* first, const uint32* last, uint32* dest) {
    return SArrayUninitCopyStride4<Id>(first, last, dest);
}
SArrayElem20* __stdcall SArrayUninitCopyWrapStride20(const SArrayElem20* first, const SArrayElem20* last, SArrayElem20* dest) {
    return SArrayUninitCopyStride20(first, last, dest);
}
SArrayElem12* __stdcall SArrayUninitCopyWrapStride12(const SArrayElem12* first, const SArrayElem12* last, SArrayElem12* dest) {
    return SArrayUninitCopyStride12(first, last, dest);
}

// 0x5EC820 (39 bytes, thiscall sin argumentos de pila, `ret`): liberar el almacenamiento de un vector SArray de elementos sin destructor (el inicio esta en +4, el fin en +8 y el limite en +0xC): si el inicio no es nulo
// lo libera con operator delete del EXE (0x8214BD) y pone los tres punteros a cero. Es el mismo trozo que el destructor de los vectores de CPedAttractor 0x5EC410 trae en linea.
void __fastcall SArrayTidyStride4(SArray<uint32>* self, void* /*edx*/) {
    self->clear();
}

// 0x5EC4B0 (76 bytes, thiscall con el BYTE de banderas en la pila, `ret 4`): destructor con borrado de CPedAttractor del EXE (ranura 1 de su vtable 0x86C538, tras GetType). Llama al cuerpo del destructor (0x5EC410, con
// hook: aqui el destructor de la clase llamado sin pasar por la vtable) y, solo si el bit 0 del BYTE de banderas esta puesto, libera `this` en el pool de atractores (0xB744C4) EN LINEA (indice = (this - almacenamiento) / 0xC4 con signo,
// estado |= 0x80, cursor = min con signo), como CPedAttractor::operator delete (0x5EAFE0, con hook). Devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CPedAttractor* __fastcall PedAttractorScalarDeletingDestructor(CPedAttractor* self, void* /*edx*/, uint8 flags) {
    self->CPedAttractor::~CPedAttractor();
    if (flags & 1) {
        CPedAttractor::operator delete(self);
    }
    return self;
}
} // namespace

void CPedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CPedAttractor, 0x86C538, 6);
    RH_ScopedCategory("Attractors");

    // Lote 189A (sin validar): ayudantes de plantilla de los vectores (ver arriba)
    RH_ScopedNamedGlobalInstall(SArrayFillStride4<0>, "SArrayFillStride4A", 0x5EAAA0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride20, "SArrayFillStride20", 0x5EAAC0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride4<1>, "SArrayFillStride4B", 0x5EAAF0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride4<2>, "SArrayFillStride4C", 0x5EAB10);
    RH_ScopedNamedGlobalInstall(SArrayCopyStride20, "SArrayCopyStride20", 0x5EAB80);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride20, "SArrayCopyBackwardStride20", 0x5EAC50);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride12, "SArrayCopyBackwardStride12", 0x5EACC0);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride4<0>, "SArrayUninitCopyStride4A", 0x5EAE10);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride20, "SArrayUninitCopyStride20", 0x5EAE40);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride4<1>, "SArrayUninitCopyStride4B", 0x5EAE70);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride12, "SArrayUninitCopyStride12", 0x5EAEA0);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride4<2>, "SArrayUninitCopyStride4C", 0x5EAEE0);
    // Lote 206A (sin validar): envolturas stdcall de las copias anteriores, vaciado de vector y destructor con borrado
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride4<0>, "SArrayUninitCopyWrapStride4A", 0x5EB930);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride20, "SArrayUninitCopyWrapStride20", 0x5EB960);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride4<1>, "SArrayUninitCopyWrapStride4B", 0x5EB990);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride12, "SArrayUninitCopyWrapStride12", 0x5EB9D0);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride4<2>, "SArrayUninitCopyWrapStride4C", 0x5EBA00);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4", 0x5EC820);
    RH_ScopedNamedGlobalInstall(PedAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5EC4B0);

    // Lote 205A (sin validar): mas ayudantes de plantilla de los vectores (ver arriba)
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride4<0>, "SArrayCopyBackwardStride4A", 0x5EB140);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride20Wrapper, "SArrayCopyBackwardStride20Wrapper", 0x5EB170);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride4<1>, "SArrayCopyBackwardStride4B", 0x5EB1B0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride12, "SArrayFillStride12", 0x5EB1E0);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride12Wrapper, "SArrayCopyBackwardStride12Wrapper", 0x5EB220);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride4<2>, "SArrayCopyBackwardStride4C", 0x5EB260);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride4<0>, "SArrayFillNStride4A", 0x5EB2A0);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride20, "SArrayFillNStride20", 0x5EB2E0);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride4<1>, "SArrayFillNStride4B", 0x5EB310);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride12, "SArrayFillNStride12", 0x5EB340);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride4<2>, "SArrayFillNStride4C", 0x5EB390);

    RH_ScopedInstall(operator delete, 0x5EAFE0); // lote 205A (sin validar)
    RH_ScopedInstall(Constructor, 0x5EDFB0);
    RH_ScopedInstall(Destructor, 0x5EC410);

    RH_ScopedInstall(SetTaskForPed, 0x5EECA0);
    RH_ScopedInstall(RegisterPed, 0x5EEE30);
    RH_ScopedInstall(DeRegisterPed, 0x5EC5B0);
    RH_ScopedInstall(IsRegisteredWithPed, 0x5EB4C0);
    // Como el original: las virtuales se enganchan por la vtable (huecos 2-5 de 0x86C538); con RH_ScopedInstall
    // el símbolo de la DLL es un thunk de llamada virtual que vuelve a la entrada enganchada -> bucle infinito
    RH_ScopedVMTInstall(IsAtHeadOfQueue, 0x5EB530);
    RH_ScopedInstall(GetTaskForPed, 0x5EC500);
    RH_ScopedInstall(GetQueueSlot, 0x5EB550);
    //RH_ScopedInstall(GetNoOfRegisteredPeds, 0xdeadbeef, { .reversed = false }); // Address incorrect
    RH_ScopedInstall(GetHeadOfQueue, 0x5EB590);
    RH_ScopedInstall(GetTailOfQueue, 0x5EB5B0);
    RH_ScopedInstall(HasEmptySlot, 0x5EAF10);
    //RH_ScopedInstall(ComputeFreeSlot, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeDeltaPos, 0x5E9600);
    RH_ScopedInstall(ComputeDeltaHeading, 0x5E9640);
    RH_ScopedInstall(ComputeAttractTime, 0x5E95E0);
    RH_ScopedVMTOverloadedInstall(ComputeAttractPos, "v", 0x5EA110, void(CPedAttractor::*)(int32, CVector&));
    RH_ScopedVMTOverloadedInstall(ComputeAttractHeading, "v", 0x5EA1C0, void(CPedAttractor::*)(int32, float&));
    RH_ScopedVMTInstall(BroadcastDeparture, 0x5EF160);
    RH_ScopedInstall(BroadcastArrival, 0x5EEF80);
    RH_ScopedInstall(AbortPedTasks, 0x5EAF60);
}

// 0x5EAFD0
void* CPedAttractor::operator new(unsigned size) {
    return GetPedAttractorPool()->New();
}

// 0x5EAFE0
// Lote 205A (sin validar; cdecl con un argumento de pila, `ret`, 49 bytes): libera la ranura de `object` en el pool de atractores (puntero global 0xB744C4, leido en cada llamada) EN LINEA, igual que el resto de operator delete
// del EXE: indice = (object - almacenamiento) / 0xC4 con signo (los slots miden 0xC4, no sizeof(CPedAttractor)), estados[indice] |= 0x80 y, si indice < cursor (con signo), cursor = indice. No comprueba nulo ni rellena el almacenamiento.
void CPedAttractor::operator delete(void* object) {
    PoolFreeSlotRaw(GetPedAttractorPool(), object, 0xC4);
}

// 0x5EDFB0
CPedAttractor::CPedAttractor(
    C2dEffectPedAttractor* fx,
    CEntity*               entity,
    eMoveState             moveState,
    size_t                 maxNoOfPeds,
    float                  spacing,
    float                  achieveQueueTime,
    float                  achieveQueueShuffleTime,
    float                  arriveRange,
    float                  headingRange,
    float                  deltaPos,
    float                  deltaHeading
) :
    m_Fx{fx},
    m_Entity{entity},
    m_MoveState{moveState},
    m_MaxNumPeds{maxNoOfPeds},
    m_Spacing{spacing},
    m_AchieveQueueTime{achieveQueueTime},
    m_AchieveQueueShuffleTime{achieveQueueShuffleTime},
    m_ArriveRange{arriveRange},
    m_HeadingRange{headingRange},
    m_DeltaPos{deltaPos},
    m_DeltaHeading{deltaHeading}
{
    const CMatrix mat = entity
        ? entity->GetMatrix()
        : CMatrix::Unity();
    m_Pos      = CPedAttractorManager::ComputeEffectPos(fx, mat);
    m_QueueDir = CPedAttractorManager::ComputeEffectQueueDir(fx, mat);
    m_UseDir   = CPedAttractorManager::ComputeEffectUseDir(fx, mat);

    strcpy(m_ScriptName, fx->m_szScriptName); // Como el original: strcpy (0x821BB5); strcpy_s rellenaba con 0xFE en Debug
}

void CPedAttractor::Shutdown() {
    ms_tasks.clear();
}

// 0x5EECA0
void CPedAttractor::SetTaskForPed(CPed* ped, CTask* task) {
    const auto numPairs = (int32)m_PedTaskPairs.size();
    for (int32 i = 0; i < numPairs; ++i) {
        auto& pair = m_PedTaskPairs.begin()[i];
        if (pair.Ped != ped) {
            continue;
        }
        if (!pair.UsedTask && pair.Task) { // 0x5EED79
            if (const auto erased = rng::find(ms_tasks, pair.Task); erased != ms_tasks.end()) { ms_tasks.erase(erased); } // Como el original: no borra si no está // 0x5EEDA0, 0x5EEDAD
            delete pair.Task; // 0x5EEDD1
        }
        pair.Task     = task;
        pair.UsedTask = false;
        ms_tasks.emplace_back(task); // Como el original: la tarea nueva se apunta en ms_tasks (0x5EEDF2)
        return;
    }
    // Como el original (0x5EED19): par nuevo {ped, tarea, -1, 0, false} y la tarea también va a ms_tasks
    m_PedTaskPairs.emplace_back(CPedTaskPair{ .Ped = ped, .Task = task, .Slot = TASK_SECONDARY_INVALID, .MatchID = 0, .UsedTask = false });
    ms_tasks.emplace_back(task);
}

// 0x5EEE30
bool CPedAttractor::RegisterPed(CPed* ped) {
    if (const auto it = rng::find(m_AttractPeds, ped); it != m_AttractPeds.end()) { // Como el original: si ya estaba, lo quita y devuelve false
        m_AttractPeds.erase(it);
        return false;
    }
    if ((int32)(m_ArrivedPeds.size() + m_AttractPeds.size()) >= (int32)m_MaxNumPeds) { // Como el original: con signo (jge)
        return false;
    }
    m_AttractPeds.emplace_back(ped); // Como el original: entra en m_AttractPeds (0x5EEED1), no en la cola
    const auto slot = (int32)m_ArrivedPeds.size();
    CVector pos;
    ComputeAttractPos(slot, pos);
    float heading;
    ComputeAttractHeading(slot, heading);
    const float time = m_AchieveQueueTime;
    SetTaskForPed(ped, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, m_MoveState });
    return true;
}

// 0x5EC5B0
bool CPedAttractor::DeRegisterPed(CPed* ped) {
    if (const auto erased = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); erased != m_PedTaskPairs.end()) { m_PedTaskPairs.erase(erased); } // Como el original: no borra si no está
    if (const auto it = rng::find(m_AttractPeds, ped); it != m_AttractPeds.end()) { // Como el original: busca y borra en m_AttractPeds (0x5EC600)
        m_AttractPeds.erase(it);
        return true;
    }
    return BroadcastDeparture(ped);
}

// 0x5EB4C0
bool CPedAttractor::IsRegisteredWithPed(const CPed* ped) const {
    return rng::contains(m_AttractPeds, ped)
        || rng::contains(m_ArrivedPeds, ped);
}

// 0x5EB530
bool CPedAttractor::IsAtHeadOfQueue(CPed* ped) {
    return *m_ArrivedPeds.begin() == ped; // Como el original: lee el primer hueco aunque la cola esté vacía (o sin reservar)
}

// 0x5EC500
CTask* CPedAttractor::GetTaskForPed(CPed* ped) {
    const auto it = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped);
    if (it == m_PedTaskPairs.end()) {
        return nullptr;
    }
    if (!it->UsedTask) {
        // Como el original (0x5EC55D): solo se quita si está en la lista; si no, no se borra nada
        if (const auto t = rng::find(ms_tasks, it->Task); t != ms_tasks.end()) {
            ms_tasks.erase(t);
        }
    }
    it->UsedTask = true;
    return it->Task;
}

// 0x5EB550
int32 CPedAttractor::GetQueueSlot(const CPed* ped) {
    const auto numArrived = (int32)m_ArrivedPeds.size();
    for (int32 i = 0; i < numArrived; ++i) {
        if (m_ArrivedPeds.begin()[i] == ped) {
            return i;
        }
    }
    return -1; // Como el original: -1 si no está en la cola
}

// 0x5EB590
CPed* CPedAttractor::GetHeadOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.front();
}

// 0x5EB5B0
CPed* CPedAttractor::GetTailOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.back();
}

int32 CPedAttractor::ComputeFreeSlot() {
    assert(0);
    return 0;
}

// 0x5E9600
float CPedAttractor::ComputeDeltaPos() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaPos, m_DeltaPos);
}

// 0x5E9640
float CPedAttractor::ComputeDeltaHeading() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaHeading, m_DeltaHeading);
}

// inlined
// 0x5E95E0
void CPedAttractor::ComputeAttractTime(int32 unused, bool time1_or_time2, float& outTime) const {
    if (time1_or_time2)
        outTime = m_AchieveQueueShuffleTime;
    else
        outTime = m_AchieveQueueTime;
}

// 0x5EA110
void CPedAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) {
    if (m_Fx) {
        outPos = m_Pos - m_QueueDir * (float)pedId * m_Spacing; // Como el original: (dir · id) · separación

        if (pedId) {
            outPos.x += ComputeDeltaPos();
            outPos.y += ComputeDeltaPos();
        }
    }
}

// bQueue - bad name?
// 0x5EA1C0
void CPedAttractor::ComputeAttractHeading(int32 bQueue, float& heading) {
    if (m_Fx) {
        const CVector dir = bQueue ? m_QueueDir : m_UseDir; // Como el original: copia la dirección sin modificar m_UseDir
        heading = CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.0f, 0.0f);
        if (bQueue)
            heading += CPedAttractor::ComputeDeltaHeading();
    }
}

// 0x5EF160
bool CPedAttractor::BroadcastDeparture(CPed* ped) {
    // Como el original: el peatón que se va se busca en la cola (m_ArrivedPeds)
    const auto numArrived = (int32)m_ArrivedPeds.size();
    int32 idx = 0;
    for (; idx < numArrived; ++idx) {
        if (m_ArrivedPeds.begin()[idx] == ped) {
            break;
        }
    }
    if (idx >= numArrived) {
        return false;
    }
    // Los que van detrás avanzan un hueco: esperan 2000 · i ms y van al hueco i - 1 (0x5EF1C6)
    for (int32 i = idx + 1, slot = idx; i < numArrived; ++i, ++slot) {
        CPed* const queuedPed = m_ArrivedPeds.begin()[i];
        CVector pos;
        ComputeAttractPos(slot, pos);
        float heading;
        ComputeAttractHeading(slot, heading);
        const float time = m_AchieveQueueShuffleTime;
        auto* const seq = new CTaskComplexSequence;
        seq->AddTask(new CTaskSimpleStandStill{ 2000 * i, false, false, 8.0f });
        seq->AddTask(new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK });
        SetTaskForPed(queuedPed, seq);
    }
    if (const auto erased = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); erased != m_PedTaskPairs.end()) { m_PedTaskPairs.erase(erased); } // Como el original: no borra si no está // 0x5EF2F6
    m_ArrivedPeds.erase(m_ArrivedPeds.begin() + idx); // 0x5EF334
    const auto numAttract = (int32)m_AttractPeds.size(); // 0x5EF371
    for (int32 i = 0; i < numAttract; ++i) {
        CPed* const attractedPed = m_AttractPeds.begin()[i];
        const auto slot = (int32)m_ArrivedPeds.size();
        CVector pos;
        ComputeAttractPos(slot, pos);
        float heading;
        ComputeAttractHeading(slot, heading);
        const float time = m_AchieveQueueTime;
        SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK });
    }
    return true;
}

// 0x5EEF80
bool CPedAttractor::BroadcastArrival(CPed* ped) {
    if (!rng::contains(m_ArrivedPeds, ped)) {
        m_ArrivedPeds.emplace_back(ped); // 0x5EEFCB
        if (const auto erased = rng::find(m_AttractPeds, ped); erased != m_AttractPeds.end()) { m_AttractPeds.erase(erased); } // Como el original: no borra si no está // 0x5EF002
        for (auto it = m_AttractPeds.begin(); it != m_AttractPeds.end();) { // 0x5EF013: relee el final en cada vuelta
            CPed* const attractedPed = *it++;
            const auto slot = (int32)m_ArrivedPeds.size();
            CVector pos;
            ComputeAttractPos(slot, pos);
            float heading;
            ComputeAttractHeading(slot, heading);
            const float time = m_AchieveQueueTime;
            SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK }); // Como el original: PEDMOVE_WALK (push 4) // 0x5EF07B
        }
    }

    if (!IsAtHeadOfQueue(ped)) { // 0x5EF129
        SetTaskForPed(ped, new CTaskComplexWaitAtAttractor{ this, GetQueueSlot(ped) });
    } else if (GetType() == PED_ATTRACTOR_SCRIPTED) { // 0x5EF0D6 - inverted (!)
        SetTaskForPed(ped, new CTaskComplexUsePairedAttractor{ this });
    } else { // 0x5EF0FA
        SetTaskForPed(ped, new CTaskComplexUseAttractor{ this });
    }

    return true;
}

// 0x5EAF60
void CPedAttractor::AbortPedTasks() {
    const auto count = static_cast<int32>(m_PedTaskPairs.size());
    for (int32 i = 0; i < count; ++i) {
        auto& p = m_PedTaskPairs._First[i];
        p.Ped = nullptr;
        // Como el original: el destructor todavía ve el puntero Task.
        delete p.Task;
        p.Task = nullptr;
        p.Slot = TASK_SECONDARY_INVALID;
    }
    // TODO/NOTE: Why not clear the array?
}
