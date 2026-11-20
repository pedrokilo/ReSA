/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorSitInChair
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include "X87Math.h"

#include "TaskInteriorSitInChair.h"

#include "AnimBlendAssociation.h"
#include "AnimManager.h"
#include "Ragdoll/IKChainManager.h"
#include "TaskTimer.h"
#include "General.h"
#include "Interior/InteriorManager_c.h"
#include "Interior/InteriorGroup_c.h"
#include "Interior/InteriorInfo_t.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"

namespace {
// Lote 176A (sin validar). La clase conserva la vtable del EXE (0x870314) y sus campos nativos como bytes (m_OriginalFields: info +8, duracion +0xC, modo +0x10, animacion +0x14, banderas +0x2A), por eso estas funciones
// se enganchan por direccion (S) en lugar de por la vtable de la DLL.
constexpr uint32 SIT_IN_CHAIR_VTABLE = 0x870314;
static_assert(CTaskInteriorSitInChair::Type == 0x57F); // GetTaskType del EXE (0x675C80)

template<typename T>
T& SitInChairField(CTaskInteriorSitInChair* self, size_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(self) + offset);
}

// 0x675C80: `mov eax, 0x57F; ret` (TASK_INTERIOR_SIT_ON_CHAIR, ranura 4 de la vtable 0x870314).
eTaskType __fastcall SitInChairGetTaskType(CTaskInteriorSitInChair* /*self*/, void* /*edx*/) {
    return CTaskInteriorSitInChair::Type;
}

// 0x675C90 (thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla y aqui se omite): cuerpo del destructor del EXE. Como el original: vtable propia (0x870314); si hay animacion (+0x14), le pone
// el callback de borrado por defecto con datos nulos (CAnimBlendAssociation::SetDeleteCallback, 0x4CEBC0, con hook; la animacion NO se suelta ni se pone a nulo el campo) y sigue con el cuerpo de ~CTask (0x61A3A0,
// CTask::Destructor, con hook). El destructor generado por el compilador dejaria la vtable de la DLL.
__declspec(noinline) void __fastcall SitInChairDestructorBody(CTaskInteriorSitInChair* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = SIT_IN_CHAIR_VTABLE;
    if (auto* const anim = SitInChairField<CAnimBlendAssociation*>(self, 0x14)) {
        anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}

// 0x675CF0 (thiscall, sin argumentos de pila, `ret`, 106 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)): Clone del EXE (ranura 1). CTask::operator new (0x61A5A0) con 0x2C bytes;
// sin memoria devuelve nulo; si no, llama al constructor (0x675C30, con hook) con la informacion (+8), la duracion (+0xC) y el modo (+0x10, byte sin signo) y devuelve lo que el constructor deja en EAX (la direccion reservada).
CTask* __fastcall SitInChairClone(CTaskInteriorSitInChair* self, void* /*edx*/) {
    void* const memory = CTask::operator new(sizeof(CTaskInteriorSitInChair));
    if (!memory) {
        return nullptr;
    }
    auto* const clone = static_cast<CTaskInteriorSitInChair*>(memory);
    return reinterpret_cast<CTask*>(static_cast<uintptr_t>(clone->Constructor(
        reinterpret_cast<void*>(static_cast<uintptr_t>(SitInChairField<uint32>(self, 0x08))),
        SitInChairField<uint32>(self, 0x0C),
        SitInChairField<uint8>(self, 0x10)
    )));
}

// 0x675D60 (thiscall, tres argumentos de pila, `ret 0xC`, 99 bytes): MakeAbortable del EXE (ranura 2). Con prioridad distinta de INMEDIATA (2) solo pide la parada (byte +0x2A = 1) y devuelve falso. Con prioridad
// inmediata: si el IKChainManager (0xC15448) esta mirando al peaton (IsLooking, 0x6181A0), AbortLookAt con 250 ms (0x618280); si hay animacion (+0x14), pone su m_BlendDelta (+0x1C) a -1000.0 (0xC47A0000),
// le pone el callback de borrado por defecto (0x4CEBC0) y borra el campo; devuelve verdadero. El evento no se usa.
bool __fastcall SitInChairMakeAbortable(CTaskInteriorSitInChair* self, void* /*edx*/, CPed* ped, eAbortPriority priority, const CEvent* /*event*/) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        SitInChairField<uint8>(self, 0x2A) = 1;
        return false;
    }
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 250);
    }
    if (auto* const anim = SitInChairField<CAnimBlendAssociation*>(self, 0x14)) {
        anim->m_BlendDelta = -1000.0f;
        anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        SitInChairField<CAnimBlendAssociation*>(self, 0x14) = nullptr;
    }
    return true;
}
// 0x676D10 (lote 204A, sin validar; el listado decia CTaskInteriorUseInfo; ranura 0 de la vtable 0x870314 de CTaskInteriorSitInChair; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x675C90;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: llama al cuerpo del destructor (0x675C90, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook)
// solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CTaskInteriorSitInChair* __fastcall SitInChairScalarDeletingDestructor(CTaskInteriorSitInChair* self, void* /*edx*/, uint8 flags) {
    SitInChairDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// Ayudantes de ProcessPed (0x676D30). El EXE llama al callback de fin de animacion 0x675DD0 (funcion propia desde el lote S1A) y a 0x59C890 (CMatrix * CVector) por direccion fija.
constexpr int16  SIT_ANIM_IN       = 0x157; // animacion de entrada (se pide con 4.0 si no es instantanea)
constexpr int16  SIT_ANIM_LOOP     = 0x158; // en reposo
constexpr int16  SIT_ANIM_OUT      = 0x159; // salida
constexpr auto   SIT_ANIM_GROUP    = static_cast<AssocGroupId>(0x54);
static_assert(offsetof(CPed, m_vecAnimMovingShiftLocal) == 0x4D8);

// 0x675DD0 (lote S1A, sin validar; cdecl (asociacion, tarea), `ret`, 65 bytes): callback de fin de la animacion de sentarse. Guarda el identificador de la animacion (palabra +0x2C con signo) en +0x18 de la tarea; si es el de
// reposo (0x158) pone el m_BlendDelta (+0x1C) de la animacion a -1000.0f y el BYTE +0x29 de la tarea a 1; si ademas el BYTE +0x2A es distinto de cero y el valor guardado en +0x18 es la entrada (0x157) repite ambas
// escrituras; y deja el puntero de animacion (+0x14) a cero.
static void SitInChairFinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    static_assert(offsetof(CAnimBlendAssociation, m_AnimId) == 0x2C && offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C);
    auto* const task = static_cast<CTaskInteriorSitInChair*>(data);
    const int32 animId = static_cast<int16>(static_cast<AnimationId>(anim->m_AnimId));
    SitInChairField<int32>(task, 0x18) = animId;
    if (animId == 0x158) {
        anim->m_BlendDelta = -1000.0f;
        SitInChairField<uint8>(task, 0x29) = 1;
    }
    if (SitInChairField<uint8>(task, 0x2A) != 0 && SitInChairField<int32>(task, 0x18) == 0x157) {
        anim->m_BlendDelta = -1000.0f;
        SitInChairField<uint8>(task, 0x29) = 1;
    }
    SitInChairField<uint32>(task, 0x14) = 0;
}

// Identificador de la animacion como el EXE lo lee (MOVSX de una palabra con signo)
static int32 SitAnimIdOf(const CAnimBlendAssociation* assoc) {
    return static_cast<int16>(static_cast<AnimationId>(assoc->m_AnimId));
}

// Como el original: CAnimManager::BlendAnimation (0x4D4610, con hook) con el clump del peaton (+0x18, sin comprobar el tipo), el grupo 0x54 y el identificador pedido; la asociacion nueva queda en +0x14 con el
// callback de fin 0x675DD0 y `this` como datos (CAnimBlendAssociation::SetFinishCallback, 0x4CEBE0, con hook).
static CAnimBlendAssociation* SitInChairBlendAnim(CTaskInteriorSitInChair* self, CPed* ped, int16 animId, float blendDelta) {
    auto* const assoc = CAnimManager::BlendAnimation(reinterpret_cast<RpClump*>(ped->GetRwObject()), SIT_ANIM_GROUP, static_cast<AnimationId>(animId), blendDelta);
    SitInChairField<CAnimBlendAssociation*>(self, 0x14) = assoc;
    assoc->SetFinishCallback(SitInChairFinishAnimCB, self);
    return assoc;
}

// CTaskTimer::Start (con el guardia de signo del EXE): si la duracion (+8) no es negativa pone inicio = hora actual (0xB7CB84), intervalo = duracion y arrancado.
static void SitInChairStartTimer(CTaskInteriorSitInChair* self) {
    SitInChairField<CTaskTimer>(self, 0x1C).Start(SitInChairField<int32>(self, 0x08));
}

// 0x676D30 (lote 204A, sin validar; el listado decia CTaskInteriorUseInfo; ranura 7 (ProcessPed) de la vtable 0x870314 de CTaskInteriorSitInChair; thiscall, un argumento de pila (el peaton), `ret 4`, 1423 bytes). Campos
// nativos (la clase conserva los bytes del EXE): duracion +8 (int32), informacion del interior +0xC (InteriorInfo_t*: posicion +4, direccion +0x10), modo instantaneo +0x10 (byte), animacion actual +0x14, ultima animacion
// +0x18 (int32, -1 al construir), temporizador +0x1C (inicio, intervalo, arrancado: CTaskTimer), banderas +0x28 (hay que recolocar), +0x29 y +0x2A (parada pedida). Como el original y en este orden: id = identificador de la animacion
// actual (int16 con signo, -1 sin animacion); CPed::SetMoveState(PEDMOVE_STILL) (0x5DEC00); con +0x29 y sin la asociacion 0x158 en el clump (RpAnimBlendClumpGetAssociation, 0x4D68B0) devuelve verdadero; con parada pedida (+0x2A):
// sin las animaciones del interior cargadas (g_interiorMan.AreAnimsLoaded(0), 0x5980F0) devuelve verdadero, con la animacion 0x157 pone su m_BlendDelta a -8.0 (0xC1000000), con la 0x159 y sin +0x28 le pone el callback de borrado
// por defecto (0x4CEBC0), pide la 0x158 (BlendAnimation con 1000.0), marca +0x28 y devuelve falso, con la 0x158 pone su m_Speed (+0x24) a 3.0. Sin animacion y con las animaciones cargadas: sin ultima animacion (-1) pide la
// 0x159 (1000.0, con temporizador y +0x28) si es instantanea o la 0x157 (4.0) si no, y con ultima = 0x157 pide la 0x159 con temporizador y +0x28. Con animacion y +0x28: recoloca al peaton (con 0x159 o 0x158 la posicion pasa a ser
// matriz * (vector fijo 0xC18C78) con la z original y, con 0x159, la rotacion = LimitRadianAngle(rotacion actual + pi), puesta en +0x558, +0x55C y en la matriz o el heading); SetPosn (0x4241C0); +0x28 = 0. Con el temporizador arrancado y
// vencido (CTaskTimer::IsOutOfTime, 0x420E30) y la animacion no 0x158 (sin comprobar nulo, como el EXE) pide la 0x158 con 1000.0 y marca +0x28. Con la animacion 0x157 (sin comprobar nulo) orienta al peaton hacia la informacion del
// interior: d = pos_info - pos_peaton (floats), L = |d| (raiz, guardada como float), m = (L < 0.02 ? L : 0.02) y u = d * (1/L) * m; guarda en +0x4D8 el producto escalar con el eje derecho de la matriz y en +0x4DC con el eje avance (cada
// uno como (z + y) + x), y pone +0x55C = LimitRadianAngle(GetRadianAngleBetweenPoints(dir.x, dir.y, 0, 0)). Al final, con la animacion 0x159: si el IKChainManager mira al peaton devuelve falso; si no, con probabilidad 19/1000 (aleatorio
// 0..1000 > 980, 0x407180) y con un grupo de interior del peaton (0x598240) cuya informacion mas cercana a 10.0 (FindClosestInteriorInfo, 0x594A50) exista, LookAt (0x618970) "TaskSitInChair" de 10000 a 20000 ms hacia la posicion de la
// informacion; sin animacion 0x159, AbortLookAt (0x618280, 250) si miraba. Devuelve falso siempre salvo en los dos primeros retornos tempranos.
#pragma float_control(precise, on, push)
// 0x676D30
bool __fastcall SitInChairProcessPed(CTaskInteriorSitInChair* self, void* /*edx*/, CPed* ped) {
    auto* const anim0 = SitInChairField<CAnimBlendAssociation*>(self, 0x14);
    const int32 animId = anim0 ? SitAnimIdOf(anim0) : -1;
    ped->SetMoveState(PEDMOVE_STILL);

    if (SitInChairField<uint8>(self, 0x29)) {
        if (!RpAnimBlendClumpGetAssociation(reinterpret_cast<RpClump*>(ped->GetRwObject()), 0x158u)) {
            return true;
        }
    }
    if (SitInChairField<uint8>(self, 0x2A)) {
        if (!g_interiorMan.AreAnimsLoaded(0)) {
            return true;
        }
        if (animId == SIT_ANIM_IN) {
            SitInChairField<CAnimBlendAssociation*>(self, 0x14)->m_BlendDelta = -8.0f; // 0xC1000000
        } else if (animId == SIT_ANIM_OUT) {
            if (!SitInChairField<uint8>(self, 0x28)) {
                SitInChairField<CAnimBlendAssociation*>(self, 0x14)->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                SitInChairBlendAnim(self, ped, SIT_ANIM_LOOP, 1000.0f);
                SitInChairField<uint8>(self, 0x28) = 1;
                return false;
            }
        } else if (animId == SIT_ANIM_LOOP) {
            SitInChairField<CAnimBlendAssociation*>(self, 0x14)->m_Speed = 3.0f; // 0x40400000
        }
    }

    if (!SitInChairField<CAnimBlendAssociation*>(self, 0x14)) {
        if (g_interiorMan.AreAnimsLoaded(0)) {
            const int32 last = SitInChairField<int32>(self, 0x18);
            if (last == -1) {
                if (SitInChairField<uint8>(self, 0x10)) {
                    SitInChairStartTimer(self);
                    SitInChairBlendAnim(self, ped, SIT_ANIM_OUT, 1000.0f);
                    SitInChairField<uint8>(self, 0x28) = 1;
                } else {
                    SitInChairBlendAnim(self, ped, SIT_ANIM_IN, 4.0f); // 0x40800000
                }
            } else if (last == SIT_ANIM_IN) {
                SitInChairStartTimer(self);
                SitInChairBlendAnim(self, ped, SIT_ANIM_OUT, 1000.0f);
                SitInChairField<uint8>(self, 0x28) = 1;
            }
        }
    } else {
        if (SitInChairField<uint8>(self, 0x28)) {
            // Posicion del peaton (matriz +0x30 o placement +4) copiada a V; W = z original
            CVector     V = ped->GetPosition();
            uint32 Wbits; // como el EXE: copias por dword (una copia por la x87 silenciaria un NaN senalizante)
            std::memcpy(&Wbits, &V.z, sizeof(Wbits));
            if (animId == SIT_ANIM_OUT) {
                // 0x59C890 (matriz * vector fijo 0xC18C78) por direccion fija; la matriz no se comprueba (como el EXE)
                CVector T;
                plugin::CallAndReturn<CVector*, 0x59C890, CVector*, const CMatrix*, const CVector*>(&T, ped->m_matrix, &StaticRef<CVector>(0xC18C78));
                const float rotation = ped->m_fCurrentRotation + 3.14159274f; // 0x858CB8
                std::memcpy(&V, &T, sizeof(V));
                const float limited = CGeneral::LimitRadianAngle(rotation); // 0x53CB50
                ped->m_fAimingRotation  = limited;
                ped->m_fCurrentRotation = limited;
                if (ped->m_matrix) {
                    ped->m_matrix->SetRotateZOnly(limited); // 0x59B020
                } else {
                    ped->m_placement.m_fHeading = limited;
                }
            } else if (animId == SIT_ANIM_LOOP) {
                CVector T;
                plugin::CallAndReturn<CVector*, 0x59C890, CVector*, const CMatrix*, const CVector*>(&T, ped->m_matrix, &StaticRef<CVector>(0xC18C78));
                std::memcpy(&V, &T, sizeof(V));
            }
            std::memcpy(&V.z, &Wbits, sizeof(Wbits));
            ped->SetPosn(V); // 0x4241C0
            SitInChairField<uint8>(self, 0x28) = 0;
        }

        if (SitInChairField<uint8>(self, 0x24)) {
            if (SitInChairField<CTaskTimer>(self, 0x1C).IsOutOfTime()) {
                if (SitAnimIdOf(SitInChairField<CAnimBlendAssociation*>(self, 0x14)) != SIT_ANIM_LOOP) {
                    SitInChairField<CAnimBlendAssociation*>(self, 0x14)->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                    SitInChairBlendAnim(self, ped, SIT_ANIM_LOOP, 1000.0f);
                    SitInChairField<uint8>(self, 0x28) = 1;
                }
            }
        }

        if (SitAnimIdOf(SitInChairField<CAnimBlendAssociation*>(self, 0x14)) == SIT_ANIM_IN) {
            const InteriorInfo_t* const info = SitInChairField<InteriorInfo_t*>(self, 0x0C);
            const CVector P = info->Pos;
            const CVector& pos = ped->GetPosition();
            // 0x40FE60 (info - peaton, cada componente guardada como float) y raiz de la suma de cuadrados (z, y, x)
            const float dx = P.x - pos.x;
            const float dy = P.y - pos.y;
            const float dz = P.z - pos.z;
            // Los valores que el EXE mantiene en registros x87 (suma de cuadrados, 1/L, dx*inv, los productos por M y los productos escalares) son `double` (24 bits de mantisa por la palabra de control y exponente ampliado: con
            // coordenadas enormes un float desbordaria o se haria denormal donde el EXE no); los que guarda como float (L, dy*inv y dz*inv) son `float`.
            const double sum = (double(dz) * dz + double(dy) * dy) + double(dx) * dx;
            const float  L   = static_cast<float>(std::sqrt(sum));          // fsqrt, guardado como float
            const double M   = (L < 0.02f) ? double(L) : double(0.02f);       // 0x858B38; fcomp + test ah,5 + jp: L >= 0.02 y NaN usan 0.02
            const double inv = 1.0 / double(L);                               // 0x858624 / L
            const double nx  = double(dx) * inv;
            const float  ny  = static_cast<float>(double(dy) * inv);
            const float  nz  = static_cast<float>(double(dz) * inv);
            const double X1  = nx * M;
            const double Y1  = double(ny) * M;
            const double Z1  = double(nz) * M;
            const CMatrix& matrix = *ped->m_matrix;
            const CVector  right  = matrix.GetRight();
            const CVector  fwd    = matrix.GetForward();
            const float dotRight = static_cast<float>((double(right.z) * Z1 + double(right.y) * Y1) + X1 * double(right.x));
            ped->m_vecAnimMovingShiftLocal.x = dotRight; // +0x4D8
            const float dotFwd = static_cast<float>((double(fwd.z) * Z1 + double(fwd.y) * Y1) + double(fwd.x) * X1);
            ped->m_vecAnimMovingShiftLocal.y = dotFwd;   // +0x4DC
            // Como el EXE: los cuatro argumentos de 0x53CBE0 se empujan como dwords crudos (pasarlos como float por la x87 silenciaria un NaN senalizante)
            uint32 dirX, dirY;
            std::memcpy(&dirX, &info->Dir.x, sizeof(dirX));
            std::memcpy(&dirY, &info->Dir.y, sizeof(dirY));
            const float angle = reinterpret_cast<float(__cdecl*)(uint32, uint32, uint32, uint32)>(0x53CBE0)(dirX, dirY, 0u, 0u); // CGeneral::GetRadianAngleBetweenPoints
            ped->m_fAimingRotation = CGeneral::LimitRadianAngle(angle); // 0x53CB50 // 0x53CBE0, 0x53CB50
        }
    }

    auto* const anim = SitInChairField<CAnimBlendAssociation*>(self, 0x14);
    if (anim && SitAnimIdOf(anim) == SIT_ANIM_OUT) {
        if (g_ikChainMan.IsLooking(ped)) { // 0x6181A0
            return false;
        }
        if (CGeneral::GetRandomIntInRange(0, 1000) > 980) { // 0x407180
            if (InteriorGroup_c* const group = g_interiorMan.GetPedsInteriorGroup(ped)) { // 0x598240
                InteriorInfo_t* info     = nullptr;
                Interior_c*     interior = nullptr;
                float           distance;
                group->FindClosestInteriorInfo(0, ped->GetPosition(), 10.0f, &info, &interior, &distance); // 0x594A50
                if (info) {
                    g_ikChainMan.LookAt("TaskSitInChair", ped, nullptr, CGeneral::GetRandomIntInRange(10000, 20000), BONE_UNKNOWN, &info->Pos, false, 0.25f, 500, 3, false); // 0x618970
                }
            }
        }
        return false;
    }
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 250);
    }
    return false;
}
#pragma float_control(pop)
} // namespace

void CTaskInteriorSitInChair::InjectHooks() {
    RH_ScopedClass(CTaskInteriorSitInChair);
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x675C80);
    RH_ScopedInstall(Constructor, 0x675C30);
    RH_ScopedNamedGlobalInstall(SitInChairGetTaskType, "GetTaskType", 0x675C80);         // lote 176A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairDestructorBody, "Destructor", 0x675C90);       // lote 176A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairClone, "Clone", 0x675CF0);                     // lote 176A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairMakeAbortable, "MakeAbortable", 0x675D60);     // lote 176A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairScalarDeletingDestructor, "ScalarDeletingDestructor", 0x676D10); // lote 204A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairProcessPed, "ProcessPed", 0x676D30);           // lote 204A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairFinishAnimCB, "FinishAnimCB", 0x675DD0);       // lote S1A (sin validar)
}

// Como el original: conserva la vtable EXE para los demás métodos. Deja EAX = this, ECX = (info con el byte bajo = modo) y EDX = duracion.
#pragma runtime_checks("", off)
// 0x675C30
uint32 CTaskInteriorSitInChair::Constructor(void* info, uint32 duration, uint8 mode) {
    auto* const fields = reinterpret_cast<uint8*>(this);
    *reinterpret_cast<uint32*>(fields + 0x04) = 0;
    *reinterpret_cast<uint32*>(fields + 0x00) = 0x870314;
    *reinterpret_cast<uint32*>(fields + 0x1C) = 0;
    *reinterpret_cast<uint32*>(fields + 0x20) = 0;
    fields[0x24] = 0;
    fields[0x25] = 0;
    *reinterpret_cast<uint32*>(fields + 0x08) = static_cast<uint32>(reinterpret_cast<uintptr_t>(info));
    *reinterpret_cast<uint32*>(fields + 0x14) = 0;
    fields[0x29] = 0;
    fields[0x28] = 0;
    fields[0x2A] = 0;
    *reinterpret_cast<uint32*>(fields + 0x0C) = duration;
    fields[0x10] = mode;
    *reinterpret_cast<uint32*>(fields + 0x18) = 0xFFFFFFFFu;
    const uint32 infoBits = static_cast<uint32>(reinterpret_cast<uintptr_t>(info));
    return ResidueRegsCpp((infoBits & 0xFFFFFF00u) | mode, duration, static_cast<uint32>(reinterpret_cast<uintptr_t>(this)));
}
#pragma runtime_checks("", restore)
