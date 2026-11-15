/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedAttractorManager
 * Reconstrucción: COMPLETA — 17/17 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 17/17 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedAttractorManager.h"
#include <TaskTypes/TaskComplexUseAttractor.h>
#include <TaskTypes/TaskComplexWaitAtAttractor.h>
#include "Scripted2dEffects.h"
#include "PedAttractor.h"
#include "PedAtmAttractor.h"
#include "PedSeatAttractor.h"
#include "PedStopAttractor.h"
#include "PedPizzaAttractor.h"
#include "PedShelterAttractor.h"
#include "PedTriggerScriptAttractor.h"
#include "PedLookAtAttractor.h"
#include "PedScriptedAttractor.h"
#include "PedParkAttractor.h"
#include "PedStepAttractor.h"

namespace {
// Lote 205A (sin validar): las tres instancias de plantilla del EXE sobre el vector de punteros a atractor (CPedAttractor*) que no se expanden en linea; las llaman las funciones de la clase (0x5EBA30, 0x5EBCB0 y 0x5EBD70, ya reconstruidas, con
// su propia version de plantilla en la cabecera) una vez por cada una de las diez listas del gestor. Son metodos del gestor que NO leen `this` (ECX llega sin usar); el resto de argumentos va en la pila y los limpia el propio metodo
// (`ret 8` / `ret 8` / `ret 10h`), asi que se declaran __stdcall con el vector (SArray, +4 = inicio, +8 = fin) como ultimo argumento. El numero de elementos se calcula como (fin - inicio) >> 2 con signo (0 si no hay inicio).

// 0x5EB5F0 (67 bytes, `ret 8`; plantilla RemoveEffect<CPedAttractor>, argumentos (efecto, vector)): para cada atractor cuyo efecto (+4) es el pedido llama a AbortPedTasks (0x5EAF60, ECX = atractor). Devuelve siempre falso. El EXE vuelve a leer el
// inicio del vector en cada vuelta (indice con signo, `jl`), asi que aqui tambien.
bool __stdcall AttractorsRemoveEffect(const C2dEffectPedAttractor* fx, const SArray<CPedAttractor*>* attractors) {
    const int32 count = attractors->_First ? int32(attractors->_Last - attractors->_First) : 0;
    for (int32 i = 0; i < count; i++) {
        CPedAttractor* const attractor = attractors->_First[i];
        if (attractor->m_Fx == fx) {
            attractor->AbortPedTasks();
        }
    }
    return false;
}

// 0x5EB640 (75 bytes, `ret 8`; plantilla IsPedRegistered<CPedAttractor>, argumentos (peaton, vector)): verdadero en cuanto un atractor responde IsRegisteredWithPed (0x5EB4C0, ECX = atractor, el peaton en la pila); si no, falso.
// El EXE lee el inicio una sola vez y avanza el puntero.
bool __stdcall AttractorsIsPedRegistered(CPed* ped, const SArray<CPedAttractor*>* attractors) {
    CPedAttractor* const* cursor = attractors->_First;
    const int32 count = cursor ? int32(attractors->_Last - cursor) : 0;
    for (int32 i = 0; i < count; i++, cursor++) {
        if ((*cursor)->IsRegisteredWithPed(ped)) {
            return true;
        }
    }
    return false;
}

// 0x5EB690 (93 bytes, `ret 10h`; plantilla IsPedRegisteredWithEffect<CPedAttractor>, argumentos (peaton, efecto, entidad, vector)): verdadero en cuanto un atractor con ese efecto (+4) y esa entidad (+8, comparada en bruto) responde
// IsRegisteredWithPed para el peaton; si no, falso. Misma lectura unica del inicio que la anterior.
bool __stdcall AttractorsIsPedRegisteredWithEffect(CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity, const SArray<CPedAttractor*>* attractors) {
    CPedAttractor* const* cursor = attractors->_First;
    const int32 count = cursor ? int32(attractors->_Last - cursor) : 0;
    for (int32 i = 0; i < count; i++, cursor++) {
        CPedAttractor* const attractor = *cursor;
        if (attractor->m_Fx == fx && attractor->GetEntity() == entity) {
            if (attractor->IsRegisteredWithPed(ped)) {
                return true;
            }
        }
    }
    return false;
}
// Lote 206A (sin validar): instancias de plantilla de CPedAttractorManager del EXE (`this` en ECX sin usar, argumentos en la pila, `ret 0Ch` o `ret 10h`; el codigo es identico para los diez tipos de atractor porque
// solo lee campos de la base CPedAttractor, asi que el enlazador del EXE dejo una sola copia y el listado las atribuia a CPedAttractor). Como no leen `this`, son funciones stdcall sin objeto; los tres primeros
// argumentos son los de las plantillas del .h (FindAssociatedAttractor, GetPedUsingEffect, GetRelevantAttractor, BroadcastDeparture y DeRegisterPed) y el ultimo es la lista (SArray) por referencia.
// La lista se recorre por indice con el tamano con signo del EXE ((fin - inicio) >> 2, 0 si el inicio es nulo).
using AttractorList = SArray<CPedAttractor*>;

// 0x5EB6F0 (68 bytes, `ret 0Ch`): primer atractor de la lista cuyo efecto (+4) y entidad (+8) son los dados; nulo si no hay. No llama a nada.
CPedAttractor* __stdcall FindAssociatedAttractorList(const C2dEffectPedAttractor* fx, const CEntity* entity, const AttractorList& attractors) {
    const auto count = static_cast<int32>(attractors.size());
    for (int32 i = 0; i < count; i++) {
        CPedAttractor* const a = attractors.begin()[i];
        if (a->GetEffect() == fx && a->GetEntity() == entity) {
            return a;
        }
    }
    return nullptr;
}

// 0x5EB740 (97 bytes, `ret 0Ch`): como la anterior, pero devuelve el primer peaton de la cola de llegada (+0x20, GetHeadOfQueue en linea, sin llamarla) del primer atractor que coincide; nulo si la cola esta vacia
// (o sin reservar) o si no hay atractor. Con el primero que coincide para: no sigue buscando aunque su cola este vacia.
CPed* __stdcall GetPedUsingEffectList(const C2dEffectPedAttractor* fx, const CEntity* entity, const AttractorList& attractors) {
    const auto count = static_cast<int32>(attractors.size());
    for (int32 i = 0; i < count; i++) {
        CPedAttractor* const a = attractors.begin()[i];
        if (a->GetEffect() == fx && a->GetEntity() == entity) {
            const auto& queue = a->GetArrivedPeds();
            if (queue.empty()) {
                return nullptr;
            }
            return queue.begin()[0];
        }
    }
    return nullptr;
}

// 0x5EB7B0 (95 bytes, `ret 10h`): primer atractor de la lista con ese efecto y esa entidad en el que ademas esta registrado el peaton (CPedAttractor::IsRegisteredWithPed 0x5EB4C0, con hook, solo si los dos
// primeros campos coinciden); nulo si ninguno. Los argumentos son (peaton, efecto, entidad, lista).
const CPedAttractor* __stdcall GetRelevantAttractorList(const CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity, const AttractorList& attractors) {
    const auto count = static_cast<int32>(attractors.size());
    for (int32 i = 0; i < count; i++) {
        const CPedAttractor* const a = attractors.begin()[i];
        if (a->GetEffect() == fx && a->GetEntity() == entity && a->IsRegisteredWithPed(ped)) {
            return a;
        }
    }
    return nullptr;
}

// Busca `attractor` en la lista como el EXE (por indice, con el tamano con signo); devuelve si esta.
bool AttractorListContains(const AttractorList& attractors, const CPedAttractor* attractor) {
    const auto count = static_cast<int32>(attractors.size());
    for (int32 i = 0; i < count; i++) {
        if (attractors.begin()[i] == attractor) {
            return true;
        }
    }
    return false;
}

// Quita `attractor` de la lista si esta (busca desde el inicio hasta el fin; desplaza el resto hacia delante y baja el fin en un puntero) y despues lo borra con su destructor virtual con borrado (ranura 1 de la
// vtable, argumento 1), este o no en la lista. Es el final comun de BroadcastDeparture y DeRegisterPed cuando el atractor se queda sin peatones registrados.
void RemoveFromListAndDelete(AttractorList& attractors, CPedAttractor* attractor) {
    if (const auto it = rng::find(attractors, attractor); it != attractors.end()) {
        attractors.erase(it);
    }
    delete attractor;
}

// 0x5EC660 (217 bytes, `ret 0Ch`): (peaton, atractor, lista). Falso si el atractor es nulo o no esta en la lista. Llama a BroadcastDeparture del atractor por la vtable (ranura 5, con el peaton) y, si
// no le queda ningun peaton registrado (llegados + en camino), lo quita de la lista y lo borra. Devuelve verdadero.
bool __stdcall BroadcastDepartureList(CPed* ped, CPedAttractor* attractor, AttractorList& attractors) {
    if (!attractor) {
        return false;
    }
    if (!AttractorListContains(attractors, attractor)) {
        return false;
    }
    attractor->BroadcastDeparture(ped);
    if (attractor->GetNoOfRegisteredPeds() == 0) {
        RemoveFromListAndDelete(attractors, attractor);
    }
    return true;
}

// 0x5EC740 (217 bytes, `ret 0Ch`): igual que la anterior pero llama a CPedAttractor::DeRegisterPed (0x5EC5B0, con hook, no virtual) en lugar de BroadcastDeparture.
bool __stdcall DeRegisterPedList(CPed* ped, CPedAttractor* attractor, AttractorList& attractors) {
    if (!attractor) {
        return false;
    }
    if (!AttractorListContains(attractors, attractor)) {
        return false;
    }
    attractor->DeRegisterPed(ped);
    if (attractor->GetNoOfRegisteredPeds() == 0) {
        RemoveFromListAndDelete(attractors, attractor);
    }
    return true;
}
} // namespace

void CPedAttractorManager::InjectHooks() {
    RH_ScopedClass(CPedAttractorManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RestoreStuffFromMem, 0x5EF710);
    //RH_ScopedInstall(BroadcastArrival<CPedAttractor>, 0x5EF920); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(BroadcastArrival, "", 0x5EFE20, bool(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    //RH_ScopedInstall(BroadcastDeparture<CPedAttractor>, 0x5EC660); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(BroadcastDeparture, "", 0x5EC980, bool(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    //RH_ScopedInstall(DeRegisterPed<CPedAttractor>, 0x5EC740); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(DeRegisterPed, "", 0x5EC850, bool(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    //RH_ScopedInstall(RemoveEffect<CPedAttractor>, 0x5EB5F0); // Can't hook this because template function will (probably) be wrong
    RH_ScopedNamedGlobalInstall(AttractorsRemoveEffect, "RemoveEffectCPedAttractor", 0x5EB5F0); // lote 205A (sin validar): funcion suelta __stdcall
    RH_ScopedOverloadedInstall(RemoveEffect, "", 0x5EBA30, bool(CPedAttractorManager::*)(const C2dEffectPedAttractor* fx));
    //RH_ScopedInstall(IsPedRegistered<CPedAttractor>, 0x5EB640); // Can't hook this because template function will (probably) be wrong
    RH_ScopedNamedGlobalInstall(AttractorsIsPedRegistered, "IsPedRegisteredCPedAttractor", 0x5EB640); // lote 205A (sin validar): funcion suelta __stdcall
    RH_ScopedOverloadedInstall(IsPedRegisteredWithEffect, "Ped", 0x5EBCB0, bool(CPedAttractorManager::*)(CPed*));
    RH_ScopedOverloadedInstall(IsPedRegisteredWithEffect, "Effect", 0x5EBD70, bool(CPedAttractorManager::*)(CPed*, const C2dEffectPedAttractor*, const CEntity*));
    //RH_ScopedInstall(IsPedRegisteredWithEffect<CPedAttractor>, 0x5EB690); // Can't hook this because template function will (probably) be wrong
    RH_ScopedNamedGlobalInstall(AttractorsIsPedRegisteredWithEffect, "IsPedRegisteredWithEffectCPedAttractor", 0x5EB690); // lote 205A (sin validar): funcion suelta __stdcall
    //RH_ScopedInstall(FindAssociatedAttractor<CPedAttractor>, 0x5EB6F0); // Can't hook this because template function will (probably) be wrong
    RH_ScopedInstall(HasQueueTailArrivedAtSlot, 0x5EBBA0);
    RH_ScopedInstall(HasEmptySlot, 0x5EBB00);
    //RH_ScopedInstall(GetPedUsingEffect<CPedAttractor>, 0x5EB740);
    RH_ScopedOverloadedInstall(GetPedUsingEffect, "", 0x5EBE50, CPed*(CPedAttractorManager::*)(const C2dEffectPedAttractor*, const CEntity*));
    //RH_ScopedInstall(GetRelevantAttractor<CPedAttractor>, 0x5EB7B0);
    RH_ScopedOverloadedInstall(GetRelevantAttractor, "", 0x5EBF50, const CPedAttractor*(CPedAttractorManager::*)(const CPed*, const C2dEffectPedAttractor*, const CEntity*));
    //RH_ScopedInstall(ComputeEffectPos, 0x5E96C0); // Different signature, cant hook
    //RH_ScopedInstall(ComputeEffectUseDir, 0x5E96E0); // Different signature, cant hook
    //RH_ScopedInstall(ComputeEffectQueueDir, 0x5E9730); // Different signature, cant hook
    //RH_ScopedInstall(ComputeEffectForwardDir, 0x5E9780); // Different signature, cant hook
    //RH_ScopedInstall(RegisterPed<CPedAttractor>, 0x5EF980); // Can't hook this because template function will (probably) be wrong
    RH_ScopedInstall(RegisterPedWithAttractor, 0x5EFCA0);
    RH_ScopedInstall(IsApproachable, 0x5EA220);
    RH_ScopedGlobalInstall(GetPedAttractorManager, 0x5EE190);
    RH_ScopedNamedGlobalInstall(BroadcastDepartureList, "BroadcastDepartureList", 0x5EC660); // lote 206A (sin validar): instancia de la plantilla del EXE, ver arriba
    RH_ScopedNamedGlobalInstall(DeRegisterPedList, "DeRegisterPedList", 0x5EC740); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(FindAssociatedAttractorList, "FindAssociatedAttractorList", 0x5EB6F0); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(GetPedUsingEffectList, "GetPedUsingEffectList", 0x5EB740); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(GetRelevantAttractorList, "GetRelevantAttractorList", 0x5EB7B0); // lote 206A (sin validar)
}

// 0x5EE190
CPedAttractorManager* GetPedAttractorManager() {
    // Como el original: se crea con operator new la primera vez y se guarda en 0xC09840 (nunca se libera)
    auto& manager = StaticRef<CPedAttractorManager*>(0xC09840);
    if (!manager) {
        void* const mem = ::operator new(sizeof(CPedAttractorManager));
        manager = mem ? new (mem) CPedAttractorManager : nullptr;
    }
    return manager;
}

// 0x5EF710
void CPedAttractorManager::RestoreStuffFromMem() {
    // Como el original: libera el almacenamiento de las 10 listas...
    m_Seats.clear();
    m_ATMs.clear();
    m_Stops.clear();
    m_Pizzas.clear();
    m_Shelters.clear();
    m_TriggerScripts.clear();
    m_LookAts.clear();
    m_Scripted.clear();
    m_Parks.clear();
    m_Steps.clear();

    // ...y las rellena con los atractores vivos del pool, del último slot al primero
    auto* const pool = GetPedAttractorPool();
    for (auto i = pool->GetSize(); i != 0;) {
        CPedAttractor* const a = pool->GetAt(--i);
        if (!a) {
            continue;
        }
        switch (a->GetType()) {
        case PED_ATTRACTOR_ATM:            m_ATMs.emplace_back(static_cast<CPedATMAttractor*>(a)); break;
        case PED_ATTRACTOR_SEAT:           m_Seats.emplace_back(static_cast<CPedSeatAttractor*>(a)); break;
        case PED_ATTRACTOR_STOP:           m_Stops.emplace_back(static_cast<CPedStopAttractor*>(a)); break;
        case PED_ATTRACTOR_PIZZA:          m_Pizzas.emplace_back(static_cast<CPedPizzaAttractor*>(a)); break;
        case PED_ATTRACTOR_SHELTER:        m_Shelters.emplace_back(static_cast<CPedShelterAttractor*>(a)); break;
        case PED_ATTRACTOR_TRIGGER_SCRIPT: m_TriggerScripts.emplace_back(static_cast<CPedTriggerScriptAttractor*>(a)); break;
        case PED_ATTRACTOR_LOOK_AT:        m_LookAts.emplace_back(static_cast<CPedLookAtAttractor*>(a)); break;
        case PED_ATTRACTOR_SCRIPTED:       m_Scripted.emplace_back(static_cast<CPedScriptedAttractor*>(a)); break;
        case PED_ATTRACTOR_PARK:           m_Parks.emplace_back(static_cast<CPedParkAttractor*>(a)); break;
        case PED_ATTRACTOR_STEP:           m_Steps.emplace_back(static_cast<CPedStepAttractor*>(a)); break;
        default:                           break;
        }
    }
}

// Como el original: los switch del EXE sobre el tipo devuelven un valor fijo fuera de 0..9 (sin UNREACHABLE)
static bool IsValidAttractorType(int32 type) {
    return static_cast<uint32>(type) <= PED_ATTRACTOR_STEP;
}

// 0x5EFE20
bool CPedAttractorManager::BroadcastArrival(CPed* ped, CPedAttractor* attractor) {
    if (!attractor || !IsPedRegisteredWithEffect(ped)) {
        return false;
    }
    const auto type = attractor->GetType();
    return IsValidAttractorType(type) && VisitAttractorsOfType(type, [&](auto&& attractors) {
        return BroadcastArrival(ped, attractor, attractors);
    });
}

// 0x5EC980
bool CPedAttractorManager::BroadcastDeparture(CPed* ped, CPedAttractor* attractor) {
    if (!attractor || !IsPedRegisteredWithEffect(ped)) {
        return false;
    }
    const auto type = attractor->GetType();
    return IsValidAttractorType(type) && VisitAttractorsOfType(type, [&](auto&& attractors) {
        return BroadcastDeparture(ped, attractor, attractors);
    });
}

// 0x5EC850
bool CPedAttractorManager::DeRegisterPed(CPed* ped, CPedAttractor* attractor) {
    if (!attractor || !IsPedRegisteredWithEffect(ped)) {
        return false;
    }
    const auto type = attractor->GetType();
    return IsValidAttractorType(type) && VisitAttractorsOfType(type, [&](auto&& attractors) {
        return DeRegisterPed(ped, attractor, attractors);
    });
}

// 0x5EBA30
// all xrefs are dead functions
bool CPedAttractorManager::RemoveEffect(const C2dEffectPedAttractor* fx) {
    if (!IsValidAttractorType(static_cast<uint8>(fx->m_nAttractorType))) {
        return false;
    }
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        return RemoveEffect(fx, attractors);
    });
}

// 0x5EBCB0
bool CPedAttractorManager::IsPedRegisteredWithEffect(CPed* ped) {
    return IsPedRegistered(ped, m_Seats)
        || IsPedRegistered(ped, m_ATMs)
        || IsPedRegistered(ped, m_Stops)
        || IsPedRegistered(ped, m_Pizzas)
        || IsPedRegistered(ped, m_Shelters)
        || IsPedRegistered(ped, m_TriggerScripts)
        || IsPedRegistered(ped, m_LookAts)
        || IsPedRegistered(ped, m_Scripted)
        || IsPedRegistered(ped, m_Parks)
        || IsPedRegistered(ped, m_Steps);
}

// 0x5EBD70
bool CPedAttractorManager::IsPedRegisteredWithEffect(CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity) {
    return IsPedRegisteredWithEffect(ped, fx, entity, m_Seats)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_ATMs)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Stops)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Pizzas)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Shelters)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_TriggerScripts)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_LookAts)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Scripted)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Parks)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Steps);
}

// notsa
CPedAttractor* CPedAttractorManager::FindAssociatedAttractor(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        return notsa::cast_if_present<CPedAttractor>(FindAssociatedAttractor(fx, entity, attractors));
    });
}

// 0x5EBBA0
bool CPedAttractorManager::HasQueueTailArrivedAtSlot(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    // Como el original: efecto nulo o que no es de atractor -> false; tipo fuera de 0..9 -> true
    if (!fx || fx->m_Type != EFFECT_ATTRACTOR) {
        return false;
    }
    if (!IsValidAttractorType(static_cast<uint8>(fx->m_nAttractorType))) {
        return true;
    }
    const auto* const attractor = FindAssociatedAttractor(fx, entity);
    if (!attractor) {
        return true;
    }
    const auto queueSize = attractor->GetSizeOfQueue();
    const auto* const pedAtQueueTail = attractor->GetTailOfQueue();
    if (!pedAtQueueTail) {
        return true;
    }
    if (pedAtQueueTail == attractor->GetHeadOfQueue()) { // Queue has 1 ped only
        return pedAtQueueTail->GetTaskManager().Find<CTaskComplexUseAttractor>(true) != nullptr;
    }
    const auto* const tWaitAtAttractor = pedAtQueueTail->GetTaskManager().Find<CTaskComplexWaitAtAttractor>(true);
    return tWaitAtAttractor
        && tWaitAtAttractor->m_slot == static_cast<int32>(queueSize - 1);
}

// 0x5EBB00
bool CPedAttractorManager::HasEmptySlot(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    // Como el original: efecto nulo o que no es de atractor -> false; tipo fuera de 0..9 -> true
    if (!fx || fx->m_Type != EFFECT_ATTRACTOR) {
        return false;
    }
    if (!IsValidAttractorType(static_cast<uint8>(fx->m_nAttractorType))) {
        return true;
    }
    const auto* const attractor = FindAssociatedAttractor(fx, entity);
    return !attractor || attractor->HasEmptySlot();
}

// 0x5EBE50
CPed* CPedAttractorManager::GetPedUsingEffect(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    if (!IsValidAttractorType(static_cast<uint8>(fx->m_nAttractorType))) {
        return nullptr;
    }
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        return GetPedUsingEffect(fx, entity, attractors);
    });
}

// 0x5EBF50
const CPedAttractor* CPedAttractorManager::GetRelevantAttractor(const CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity) {
    const auto SearchIn = [this, ped, fx, entity](auto&& attractors) {
        return GetRelevantAttractor(ped, fx, entity, attractors);
    };
    // Como el original: busca primero en los cajeros y luego en los asientos
    const CPedAttractor* p;
    if (   (p = SearchIn(m_ATMs))
        || (p = SearchIn(m_Seats))
        || (p = SearchIn(m_Stops))
        || (p = SearchIn(m_Pizzas))
        || (p = SearchIn(m_Shelters))
        || (p = SearchIn(m_TriggerScripts))
        || (p = SearchIn(m_LookAts))
        || (p = SearchIn(m_Scripted))
        || (p = SearchIn(m_Parks))
        || (p = SearchIn(m_Steps))
    ) {
        return p;
    }
    return nullptr;
}

// 0x5EFCA0
CPedAttractor* CPedAttractorManager::RegisterPedWithAttractor(CPed* ped, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState ms) {
    if (CScripted2dEffects::GetIndex(fx) < 0 && !CEventAttractor::IsEffectActive(entity, fx)) {
        return nullptr;
    }
    if (IsPedRegisteredWithEffect(ped, fx, entity)) {
        return nullptr;
    }
    // Como el original: la comprobación del tipo de efecto va después (devuelve nulo en vez de assert)
    if (fx->m_Type != EFFECT_ATTRACTOR || !IsValidAttractorType(static_cast<uint8>(fx->m_nAttractorType))) {
        return nullptr;
    }
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        // Como el original: con el pool lleno RegisterPed devuelve nulo (notsa::cast hacía assert con nulo)
        return static_cast<CPedAttractor*>(RegisterPed(ped, fx, entity, ms, attractors));
    });
}

// Como el original: CVector::FromMultiply (0x59C670), con el orden de sumas del EXE
static CVector TransformPointOG(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z,
    };
}

// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE
static CVector TransformVectorOG(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// 0x5EA220
bool CPedAttractorManager::IsApproachable(C2dEffectPedAttractor* fx, const CMatrix& mat, int32 slot, CPed* ped) {
    if (fx->m_nAttractorType == PED_ATTRACTOR_SHELTER) {
        const CVector fxPos = TransformPointOG(mat, fx->m_Pos);
        return CWorld::GetIsLineOfSightClear(ped->GetPosition(), fxPos, true, false, false, false, false, false, false);
    }

    const CVector fxFwd = TransformVectorOG(mat, fx->m_vecForwardDir);
    const CVector fxPos = TransformPointOG(mat, fx->m_Pos);
    const CVector& pedPos = ped->GetPosition();

    // Como el original: dos productos escalares separados (orden de sumas del EXE); el peatón debe estar delante
    const float fxDot  = fxPos.z * fxFwd.z + fxPos.y * fxFwd.y + fxPos.x * fxFwd.x;
    const float pedDot = fxFwd.z * pedPos.z + fxFwd.x * pedPos.x + fxFwd.y * pedPos.y;
    if (!(pedDot - fxDot > 0.f)) {
        return false;
    }

    // Como el original: dirección 2D normalizada hacia el peatón, cuyo producto con la dirección del efecto debe ser > 0.25
    // (no una distancia mínima)
    CVector dir{ pedPos.x - fxPos.x, pedPos.y - fxPos.y, 0.f };
    dir.Normalise();
    if (!(dir.y * fxFwd.y + fxFwd.z * dir.z + dir.x * fxFwd.x > 0.25f)) {
        return false;
    }
    return CPedGeometryAnalyser::IsWanderPathClear(pedPos, fxPos, 2.f, 0) == CPedGeometryAnalyser::WanderPathClearness::CLEAR;
}
