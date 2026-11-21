/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleBeHitWhileMoving
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "game_sa/Crt/CrtState.h"
#include "game_sa/Crt/CrtMath.h"
#include "game_sa/Crt/CrtFile.h"

#include "TaskSimpleBeHitWhileMoving.h"
#include "AnimBlendAssociation.h"
#include "AnimBlendHierarchy.h"
#include "X87Math.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// Callbacks de fin de animacion del EXE, pasados por su direccion del EXE como el original: 0x4D1390 (CDefaultAnimCallback::DefaultAnimCB, `ret`) y 0x61FF30 (callback propio de la tarea: pone el puntero de la animacion
// (+0x14) a cero y el BYTE +8 a 1; sin reconstruir en este lote).
using AnimFinishCallback = void (*)(CAnimBlendAssociation*, void*);
const AnimFinishCallback kExeDefaultAnimCB      = CDefaultAnimCallback::DefaultAnimCB; // 0x4D1390 (funcion propia, lote S1A)

// 0x61FF30 (lote S1A, sin validar; cdecl (asociacion, tarea), `ret`, 16 bytes): callback de fin de animacion de la tarea. No lee la asociacion: pone el puntero de la animacion (+0x14) a nulo y el BYTE +8 a 1.
void BeHitFinishAnimCB(CAnimBlendAssociation* /*anim*/, void* data) {
    auto* const task = static_cast<CTaskSimpleBeHitWhileMoving*>(data);
    task->m_Anim       = nullptr;
    task->m_bIsFinished = true;
}
const AnimFinishCallback kExeBeHitFinishAnimCB  = BeHitFinishAnimCB;

static_assert(TASK_SIMPLE_BE_HIT_WHILE_MOVING == 0x3F1); // 0x61FE60: `mov eax, 3F1h; ret`

// rand() del CRT DEL EXE (0x821B1E): avanza la semilla del juego (la de la DLL no).
int32 ExeRand() {
    return ExeCrt::Rand();
}

// 0x61FE70 (lote 193A, sin validar): cuerpo del destructor de CTaskSimpleBeHitWhileMoving del EXE (thiscall, sin argumentos de pila, `ret`, con marco SEH que solo desenrolla y aqui se omite; ranura 0 de la vtable 0x86D7FC).
// Deja la vtable de la clase del EXE (0x86D7FC); si hay animacion (puntero en +0x14) pone su m_BlendDelta (+0x1C) a -4.0f (0xC0800000) y le pone la funcion de fin por defecto del EXE (CAnimBlendAssociation::SetFinishCallback
// 0x4CEBE0 con 0x4D1390 y datos nulos); y sigue con el cuerpo de ~CTask (0x61A3A0, con hook). La clase es una plantilla sin campos: el puntero +0x14 se lee por desplazamiento.
__declspec(noinline) void __fastcall BeHitWhileMovingDestructorBody(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D7FC;
    auto* const anim = *reinterpret_cast<CAnimBlendAssociation**>(reinterpret_cast<uint8*>(self) + 0x14);
    if (anim) {
        static_assert(offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C);
        anim->m_BlendDelta = -4.0f;
        anim->SetFinishCallback(kExeDefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}

// 0x625BA0 (lote 193A, sin validar; el listado decia CTaskSimpleThrowProjectile: es la ranura 0 de la vtable 0x86D7FC de CTaskSimpleBeHitWhileMoving; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x61FE70; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x61FE70, aqui BeHitWhileMovingDestructorBody), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (clase sin reconstruir: sin vtable exportada).
CTaskSimpleBeHitWhileMoving* __fastcall BeHitWhileMovingScalarDeletingDestructor(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, uint8 flags) {
    BeHitWhileMovingDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x61FE30 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; thiscall, dos argumentos de pila, `ret 8`, 42 bytes; devuelve `this`): constructor de la clase del EXE. Base CTaskSimple (0x61A390, con hook), el BYTE +8 y
// el dword +0x14 a cero, la vtable 0x86D7FC (la del EXE: la de la DLL no vale, la clase no esta reconstruida), +0xC = primer argumento y +0x10 = segundo.
CTaskSimpleBeHitWhileMoving* __fastcall BeHitWhileMovingConstructor(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, int32 mode, int32 dirn) {
    CTaskSimpleBody::Constructor(self, nullptr);
    self->m_bIsFinished = false;
    self->m_Anim        = nullptr;
    *reinterpret_cast<uint32*>(self) = 0x86D7FC;
    self->m_nMode = mode;
    self->m_nDirn = dirn;
    return self;
}

// 0x61FE60 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 4 (GetTaskType) de la vtable 0x86D7FC; thiscall, `ret`, 6 bytes: `mov eax, 3F1h; ret`): no lee el objeto. Se engancha como S (sirve a la vtable y
// a las llamadas directas) con una funcion suelta porque la clase no esta reconstruida.
eTaskType __fastcall BeHitWhileMovingGetTaskType(void* /*self*/, void* /*edx*/) {
    return TASK_SIMPLE_BE_HIT_WHILE_MOVING;
}

// 0x61FEE0 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 6 (MakeAbortable) de la vtable 0x86D7FC; thiscall, tres argumentos de pila (peaton, prioridad, evento), `ret 0Ch`, 76 bytes): con prioridad 1 (URGENT) o 2
// (IMMEDIATE), si hay animacion (+0x14) le pone el DefaultAnimCB del EXE (0x4D1390) como callback de fin con dato nulo (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0) y deja el puntero a nulo; devuelve verdadero. Con cualquier otra
// prioridad y animacion, activa el bit 2 (ANIMATION_IS_BLEND_AUTO_REMOVE) de sus banderas (+0x2E) y pone su m_BlendDelta (+0x1C) a -4.0f (0xC0800000); devuelve falso. El peaton y el evento no se leen.
bool __fastcall BeHitWhileMovingMakeAbortable(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, CPed* /*ped*/, eAbortPriority priority, const CEvent* /*event*/) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        if (self->m_Anim) {
            self->m_Anim->SetFinishCallback(kExeDefaultAnimCB, nullptr);
            self->m_Anim = nullptr;
        }
        return true;
    }
    if (self->m_Anim) {
        self->m_Anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        self->m_Anim->m_BlendDelta = -4.0f;
    }
    return false;
}

// 0x61FF40 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; thiscall, un argumento de pila (el peaton), `ret 4`, 836 bytes): inicia la animacion. Todas las comprobaciones, los rand() del CRT del EXE (0x821B1E, en
// el orden del EXE y solo los que el EXE llama) y las llamadas siguen el desensamblado:
//  - "especial" solo con +0xC == 4 o 2: no jugador y rand() impar, o (jugador, o rand() par) y (rand() & 7) == 0.
//  - NO especial: si no es jugador y rand() es par, solo `GetRandomIntInRange(1000, 3000)` (0x407180, resultado descartado) y salta al callback; si no, id = 0x1D/0x1E/0x1F segun la direccion +0x10 (1/2/3) y 0x1C para cualquier otro.
//  - especial, segun la direccion: 1 -> mezcla id 0x19; 2 -> con rand() impar reutiliza el id 0x22, si no mezcla 0x1A; 3 -> mezcla 0x1B; otra -> con +0xC == 2 mezcla 0x14, si no rand() impar reutiliza 0x25, si no rand() impar reutiliza 0x20, si no mezcla 0x13.
//  - "reutiliza id": RpAnimBlendClumpGetAssociation (0x4D68B0) del clump (+0x18) -> +0x14; si no hay asociacion o su m_BlendDelta (+0x1C) es MENOR que cero (ordenado: con NaN reutiliza) se mezcla una nueva
//    (CAnimManager::BlendAnimation 0x4D4610 con (clump, grupo 0, id, 8.0f)); despues SetCurrentTime(0.0f) (0x4CEA80) y las banderas |= 1 y |= 8.
//  - "mezcla id": si el peaton esta enganchado a otro cuerpo (dword +0xFC) salta al callback; si no ClearLookFlag (0x5E1950) y ClearAimFlag (0x5DEF20); con id != 0xBF (siempre) se busca la asociacion: si existe, SetCurrentTime(0.0f),
//    bandera 1, m_BlendAmount (+0x18) = 0 y m_BlendDelta = 8.0f (NO se guarda en +0x14); si no existe, BlendAnimation (grupo 0, 8.0f) -> +0x14. (La rama con id == 0xBF, de los ids 0x6E/0x6F para el jugador, es inalcanzable aqui: se conserva.)
//    Despues el tiempo de inconsciencia del peaton (+0x744) segun la asociacion encontrada `a`: jugador sin `a` o no jugador sin `a`: (semilla (word +0x20) + contador de fotogramas) % 1000 + tiempo + 1000 (entero);
//    jugador con `a` de id 0x6E o 0x6F: ((total * 1000 + tiempo) - actual * 1000) + 100; jugador con otra `a`: (total * 1000 + tiempo) + 500; no jugador con `a`: ((resto + total * 1000) + tiempo) + 500 (todo en x87, X87Math.cpp).
//  - Siempre termina con SetFinishCallback (0x4CEBE0) sobre la animacion de +0x14 (que puede ser nula en las ramas que no la fijan, como en el original) con el callback 0x61FF30 y `this` como dato.
void __fastcall BeHitWhileMovingStartAnim(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, CPed* ped) {
    static_assert(offsetof(CPed, m_nUnconsciousTimer) == 0x744);
    static_assert(offsetof(CEntity, m_nRandomSeed) == 0x20);
    static_assert(offsetof(CPhysical, m_pAttachedTo) == 0xFC);

    const auto finishWithCallback = [&] {
        self->m_Anim->SetFinishCallback(kExeBeHitFinishAnimCB, self);
    };
    // 0x61FFAA / 0x62021C: asociacion existente (si no se esta desvaneciendo) o animacion nueva, y 0x620257: se reinicia y se marca
    const auto acquireAndStart = [&](int32 animId) {
        self->m_Anim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<uint32>(animId));
        if (!self->m_Anim || self->m_Anim->m_BlendDelta < 0.0f) {
            self->m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, static_cast<AnimationId>(animId), 8.0f);
        }
        self->m_Anim->SetCurrentTime(0.0f);
        self->m_Anim->m_Flags |= ANIMATION_IS_PLAYING;
        self->m_Anim->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
    };

    bool special = false;
    if (self->m_nMode == 4 || self->m_nMode == 2) {
        if (!ped->IsPlayer() && (ExeRand() & 1)) {
            special = true;
        } else {
            special = (ExeRand() & 7) == 0;
        }
    }

    if (!special) {
        if (!ped->IsPlayer() && !(ExeRand() & 1)) {
            CGeneral::GetRandomIntInRange(1000, 3000); // 0x407180: el resultado se descarta
            finishWithCallback();
            return;
        }
        int32 animId;
        switch (self->m_nDirn) {
        case 1:  animId = 0x1D; break;
        case 2:  animId = 0x1E; break;
        case 3:  animId = 0x1F; break;
        default: animId = 0x1C; break;
        }
        acquireAndStart(animId);
        finishWithCallback();
        return;
    }

    int32 blendId    = 0;
    int32 existingId = 0;
    bool  reuse      = false;
    switch (self->m_nDirn) {
    case 1:
        blendId = 0x19;
        break;
    case 2:
        if (ExeRand() & 1) {
            existingId = 0x22;
            reuse      = true;
        } else {
            blendId = 0x1A;
        }
        break;
    case 3:
        blendId = 0x1B;
        break;
    default:
        if (self->m_nMode == 2) {
            blendId = 0x14;
        } else if (ExeRand() & 1) {
            existingId = 0x25;
            reuse      = true;
        } else if (ExeRand() & 1) {
            existingId = 0x20;
            reuse      = true;
        } else {
            blendId = 0x13;
        }
        break;
    }
    if (reuse) {
        acquireAndStart(existingId);
        finishWithCallback();
        return;
    }

    if (ped->m_pAttachedTo) {
        finishWithCallback();
        return;
    }
    ped->ClearLookFlag(); // 0x5E1950
    ped->ClearAimFlag();  // 0x5DEF20

    CAnimBlendAssociation* found = nullptr;
    if (blendId != 0xBF) {
        found = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), static_cast<uint32>(blendId));
        if (found) {
            found->SetCurrentTime(0.0f);
            found->m_Flags |= ANIMATION_IS_PLAYING;
            found->m_BlendAmount = 0.0f;
            found->m_BlendDelta  = 8.0f;
        } else {
            self->m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, static_cast<AnimationId>(blendId), 8.0f);
        }
    } else if (ped->IsPlayer()) { // inalcanzable (blendId nunca es 0xBF), conservado
        found = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), 0x6Eu);
        if (!found) {
            found = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), 0x6Fu);
        }
    }

    const auto timeMs = static_cast<int32>(CTimer::m_snTimeInMilliseconds);
    // (semilla + contador de fotogramas) % 1000 sin signo, como `div ecx` del EXE
    const auto quickRemainder = [&] {
        return static_cast<uint32>(ped->m_nRandomSeed) + CTimer::m_FrameCounter;
    };
    if (ped->IsPlayer()) {
        if (!found) {
            ped->m_nUnconsciousTimer = quickRemainder() % 1000u + CTimer::m_snTimeInMilliseconds + 1000u;
        } else {
            const auto animIdWord = static_cast<uint16>(found->GetAnimId());
            const float total     = found->m_BlendHier->m_fTotalTime;
            if (animIdWord == 0x6E || animIdWord == 0x6F) {
                ped->m_nUnconsciousTimer = static_cast<uint32>(BeHitMovingUnconsciousRemainingCpp(total, found->m_CurrentTime, timeMs));
            } else {
                ped->m_nUnconsciousTimer = static_cast<uint32>(BeHitMovingUnconsciousPlayerCpp(total, timeMs));
            }
        }
    } else {
        if (!found) {
            ped->m_nUnconsciousTimer = quickRemainder() % 1000u + CTimer::m_snTimeInMilliseconds + 1000u;
        } else {
            const auto remainder = static_cast<int32>(quickRemainder() % 1000u);
            const float total    = found->m_BlendHier->m_fTotalTime;
            ped->m_nUnconsciousTimer = static_cast<uint32>(BeHitMovingUnconsciousPedCpp(remainder, total, timeMs));
        }
    }
    finishWithCallback();
}

// 0x620290 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 7 (ProcessPed) de la vtable 0x86D7FC; thiscall, un argumento de pila, `ret 4`, 34 bytes): con el BYTE +8 puesto devuelve verdadero; si no, sin animacion
// (+0x14 nulo) llama a StartAnim (0x61FF40, con hook); devuelve falso.
bool __fastcall BeHitWhileMovingProcessPed(CTaskSimpleBeHitWhileMoving* self, void* /*edx*/, CPed* ped) {
    if (self->m_bIsFinished) {
        return true;
    }
    if (!self->m_Anim) {
        BeHitWhileMovingStartAnim(self, nullptr, ped);
    }
    return false;
}

// 0x623190 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 1 (Clone) de la vtable 0x86D7FC; thiscall, sin argumentos de pila, `ret`, 130 bytes con marco SEH que solo desenrolla): CTask::operator new (0x61A5A0) con
// 0x18 bytes; si no hay memoria devuelve nulo; si la hay, lee +0x10 y +0xC de `this` y escribe en la tarea nueva: el cuerpo de CTaskSimple (0x61A390, con hook), +0x10, +0xC, la vtable 0x86D7FC, el BYTE +8 y el dword +0x14 a cero (los
// mismos valores que deja el constructor 0x61FE30). No copia la animacion ni el estado terminado.
CTask* __fastcall BeHitWhileMovingClone(const CTaskSimpleBeHitWhileMoving* self, void* /*edx*/) {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleBeHitWhileMoving));
    if (!memory) {
        return nullptr;
    }
    const int32 dirn = self->m_nDirn;
    const int32 mode = self->m_nMode;
    auto* const task = static_cast<CTaskSimpleBeHitWhileMoving*>(memory);
    CTaskSimpleBody::Constructor(task, nullptr);
    task->m_nDirn = dirn;
    task->m_nMode = mode;
    *reinterpret_cast<uint32*>(task) = 0x86D7FC;
    task->m_bIsFinished = false;
    task->m_Anim        = nullptr;
    return task;
}
} // namespace

void CTaskSimpleBeHitWhileMoving::InjectHooks() {
    RH_ScopedClass(CTaskSimpleBeHitWhileMoving); // clase aun sin reconstruir: sin vtable exportada, solo hooks de funciones sueltas
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(BeHitWhileMovingConstructor, "Constructor", 0x61FE30); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingGetTaskType, "GetTaskType", 0x61FE60); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingDestructorBody, "DestructorBody", 0x61FE70); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingMakeAbortable, "MakeAbortable", 0x61FEE0); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingStartAnim, "StartAnim", 0x61FF40); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingProcessPed, "ProcessPed", 0x620290); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingClone, "Clone", 0x623190); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitWhileMovingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x625BA0); // lote 193A (sin validar)
    RH_ScopedNamedGlobalInstall(BeHitFinishAnimCB, "FinishAnimCB", 0x61FF30); // lote S1A (sin validar)
    // TODO: instalar las funciones conforme se reconstruyan.
}
