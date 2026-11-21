/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSunbathe
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "DatosExe/RdataCadenas.h"
#include "TaskComplexSunbathe.h"
#include "TaskSimplePause.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleRunTimedAnim.h"
#include "InterestingEvents.h"

// 0x632140: whole EAX result, readonly hour/weather, no original GAME delegation.
int32 CanSunbathe() {
    const uint8 hour = *reinterpret_cast<volatile const uint8*>(0xB70153);
    if (hour < 10 || hour >= 18) {
        return 0;
    }
    const int16 weather = *reinterpret_cast<volatile const int16*>(0xC8131C);
    if (weather == 1 || weather == 3 || weather == 14 || weather == 5 ||
        weather == 10 || weather == 18 || weather == 0 || weather == 2 ||
        weather == 13 || weather == 6 || weather == 11 || weather == 17) {
        return 1;
    }
    return 0;
}


// 0x632190 (lote 197A, sin validar; cdecl, sin argumentos, `ret`, 90 bytes; el listado lo atribuia a CTaskComplexSunbathe y no es metodo: es una funcion libre; devuelve solo AL): "se pueden cargar las animaciones de
// tomar el sol". Lee el peaton del jugador en foco (PlayerInFocus BYTE en 0xB7CD74, CPlayerInfo de 0x190 bytes desde 0xB7CD98, CPed* en +0). Solo si ese peaton va en vehiculo (bit 8 de +0x46C = bInVehicle) Y tiene
// vehiculo (+0x58C) mira la velocidad (+0x44/+0x48/+0x4C): cuadrado de la magnitud sumado como el EXE, (x*x + y*y) + z*z, con todo en float (el EXE carga los tres con fld dword y redondea a 24 bits cada operacion,
// igual que el float de C++), comparado con la constante del EXE 0x863244 = 0x3D23D70B (0.0400000028, NO el float 0.04f = 0x3D23D70A): devuelve falso solo si es ESTRICTAMENTE mayor (con NaN, fcomp deja C0=C3=1 y
// el EXE devuelve verdadero, igual que `>` falso de C++). En cualquier otro caso devuelve verdadero. Colaboradores: ninguno.
bool ShouldLoadSunbatheAnims() {
    const CPed* const player = CWorld::Players[CWorld::PlayerInFocus].m_pPed;
    if (player->bInVehicle && player->m_pVehicle) {
        const CVector& v = player->m_pVehicle->m_vecMoveSpeed;
        const float squaredSpeed = (v.x * v.x + v.y * v.y) + v.z * v.z;
        if (squaredSpeed > std::bit_cast<float>(uint32{0x3D23D70B})) {
            return false;
        }
    }
    return true;
}

// 0x62F2D0 (lote 152A, banco Fn152A; cdecl, un argumento de pila de 32 bits del que solo se lee la PALABRA baja, `ret`, 49 bytes; el listado lo atribuia a CTaskComplexLeaveCarAndDie): el tiempo es "muy soleado"
// (EXTRASUNNY de LA, SMOG LA, SF, VEGAS, COUNTRYSIDE o DESERT: 0, 2, 6, 11, 13 o 17). Devuelve solo AL (el EXE hace `xor al,al` con falso y `mov eax,1` con verdadero); lo llama CanSunbathe (0x632140).
bool IsExtraSunnyWeather(int16 weather) {
    return weather == 0 || weather == 2 || weather == 13 || weather == 6 || weather == 11 || weather == 17;
}

namespace {
bool SunbatheWeatherAllows() {
    const auto hour = CClock::GetGameClockHours();
    if (hour < 10 || hour >= 18) return false;
    // This task reads the current executable weather, at C8131C.
    switch (*reinterpret_cast<const int16*>(0xC8131C)) {
    case 1: case 3: case 14: case 5: case 10: case 18:
    case 0: case 2: case 13: case 6: case 11: case 17: return true;
    default: return false;
    }
}
}

namespace {
// 0x632050 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexSunbathe del EXE (con hook propio (Destructor); vtable 0x86E0AC). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x632050` del EXE.
__declspec(noinline) void __fastcall CTaskComplexSunbatheDestructorBody(CTaskComplexSunbathe* self, void* /*edx*/) { self->CTaskComplexSunbathe::~CTaskComplexSunbathe(); }

// 0x638180 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexSunbathe del EXE (ranura 0 de su vtable 0x86E0AC; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x632050; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexSunbathe* __fastcall CTaskComplexSunbatheScalarDeletingDestructor(CTaskComplexSunbathe* self, void* /*edx*/, uint8 flags) {
    CTaskComplexSunbatheDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexSunbathe() { return reinterpret_cast<uintptr_t>(&CTaskComplexSunbatheDestructorBody); } // para el banco Fn145A

void CTaskComplexSunbathe::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSunbathe, 0x86e0ac, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexSunbatheScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638180); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x631F80);
    RH_ScopedInstall(Destructor, 0x632050);

    {
        RH_ScopedCategory("Tasks/TaskTypes");
        RH_ScopedGlobalInstall(CanSunbathe, 0x632140);
        RH_ScopedGlobalInstall(IsExtraSunnyWeather, 0x62F2D0); // lote 152A (banco Fn152A)
    }
    RH_ScopedGlobalInstall(ShouldLoadSunbatheAnims, 0x632190); // lote 197A (sin validar)

    RH_ScopedInstall(CreateSubTask, 0x638290); // lote 197A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x6366A0);
    RH_ScopedVMTInstall(GetTaskType, 0x632040);
    RH_ScopedVMTInstall(MakeAbortable, 0x6320F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6399F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639CB0);
    RH_ScopedVMTInstall(ControlSubTask, 0x6381A0);

}

// 0x631f80
CTaskComplexSunbathe::CTaskComplexSunbathe(CObject* towel, bool bStartStanding) :
    m_pTowel{towel},
    m_bStartStanding{bStartStanding}
{
    if (m_pTowel) {
        CEntity::SafeRegisterRef(m_pTowel);
        m_pTowel->m_nObjectType = OBJECT_TYPE_DECORATION;
    }

    const auto LoadAnim = [](int32& idx, CAnimBlock*& blk, const char* name) {
        blk = CAnimManager::GetAnimationBlock(name);
        idx = CAnimManager::GetAnimationBlockIndex(name);
    };
    LoadAnim(m_BeachAnimBlockIndex, m_pBeachAnimBlock, "beach");
    LoadAnim(m_SunbatheAnimBlockIndex, m_pSunbatheAnimBlock, "sunbathe");

    // Refs added later in another function
}

CTaskComplexSunbathe::CTaskComplexSunbathe(const CTaskComplexSunbathe& o) :
    CTaskComplexSunbathe{ o.m_pTowel, o.m_bStartStanding }
{
}

// 0x632050
CTaskComplexSunbathe::~CTaskComplexSunbathe() {
    if (m_pTowel) {
        CEntity::SafeCleanUpRef(m_pTowel);
        m_pTowel->m_nObjectType = OBJECT_TEMPORARY;
    }

    const auto UnrefAnimBlock = [](bool& refed, int32 blkIdx) {
        if (refed) {
            CAnimManager::RemoveAnimBlockRef(blkIdx);
            refed = false;
        }
    };
    UnrefAnimBlock(m_bBeachAnimsReferenced, m_BeachAnimBlockIndex);
    UnrefAnimBlock(m_bSunbatheAnimsReferenced, m_SunbatheAnimBlockIndex);
}

// 0x6320F0
bool CTaskComplexSunbathe::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (event && (event->GetEventType() == static_cast<eEventType>(2) || event->GetEventType() == static_cast<eEventType>(3))) return false;
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) return false;
    m_bAborted = true;
    return true;
}

// 0x6399F0
CTask* CTaskComplexSunbathe::CreateNextSubTask(CPed* ped) {
    if (ped->m_nPedType == static_cast<ePedType>(22) || ped->m_nPedType == static_cast<ePedType>(5)) {
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_2, ped);
    }
    auto create = [&](int type) { return CreateSubTask(static_cast<eTaskType>(type), ped); };
    switch (m_pSubTask->GetTaskType()) {
    case static_cast<eTaskType>(0xCA):
        if (!m_bBathing) {
            if (m_pSunbatheAnimBlock->IsLoaded && m_pBeachAnimBlock->IsLoaded) return create(0x1AC);
        } else if (ShouldLoadSunbatheAnims() && m_pSunbatheAnimBlock->IsLoaded) return create(0x1AE);
        return create(0x516);
    case static_cast<eTaskType>(0x1A2):
        if (!m_pSunbatheAnimBlock->IsLoaded) return create(0xCA);
        if (SunbatheWeatherAllows() && !m_BathingTimer.IsOutOfTime() && (m_SunbatherType == 0 || m_SunbatherType == 2)) return create(0x1AD);
        return create(0x1AE);
    case static_cast<eTaskType>(0x1AC):
        if (!m_pBeachAnimBlock->IsLoaded) return create(0x516);
        m_BathingTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_BathingTimer.m_nInterval = CGeneral::GetRandomIntInRange(20000, 100000);
        m_BathingTimer.m_bStarted = true;
        { auto* result = create(0x1A2); m_bBathing = true; return result; }
    case static_cast<eTaskType>(0x1AD): return create(m_pBeachAnimBlock->IsLoaded ? 0x1A2 : 0x516);
    case static_cast<eTaskType>(0x1AE):
        { auto* result = create(0x516); m_bBathing = false; return result; }
    default: return nullptr;
    }
}

// 0x639CB0
CTask* CTaskComplexSunbathe::CreateFirstSubTask(CPed* ped) {
    CGeneral::GetRandomNumber();
    if (ped->m_nPedType == static_cast<ePedType>(5)) {
        const auto random = CGeneral::GetRandomNumber() & 0xFFFF;
        m_SunbatherType = static_cast<eSunbatherType>(2 - int(double(random) * (1.0 / 32768.0) * -3.0));
    } else {
        m_SunbatherType = static_cast<eSunbatherType>(CPopulation::IsSunbather(static_cast<eModelID>(ped->m_nModelIndex)) ? int(CGeneral::GetRandomNumber() < 0x3FFF) : 1);
    }
    if (!m_pBeachAnimBlock->IsLoaded) CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    if (!m_pSunbatheAnimBlock->IsLoaded) CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    m_bBathing = false;
    const bool load = ShouldLoadSunbatheAnims();
    if (m_bStartStanding && load) {
        if (!m_pTowel && m_pSunbatheAnimBlock->IsLoaded) return CreateSubTask(static_cast<eTaskType>(0x1AC), ped);
        return CreateSubTask(static_cast<eTaskType>(0xCA), ped);
    }
    if (!load && !m_pBeachAnimBlock->IsLoaded) return nullptr;
    if (m_pBeachAnimBlock->IsLoaded) {
        const auto random = CGeneral::GetRandomNumber() & 0xFFFF;
        m_BathingTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_BathingTimer.m_nInterval = 20000 - int(double(random) * (1.0 / 32768.0) * -80000.0);
        m_BathingTimer.m_bStarted = true;
        m_bBathing = true;
        return CreateSubTask(static_cast<eTaskType>(0x1A2), ped);
    }
    return load ? CreateSubTask(static_cast<eTaskType>(0xCA), ped) : nullptr;
}

// 0x6381A0
CTask* CTaskComplexSunbathe::ControlSubTask(CPed* ped) {
    if (m_bAborted) return nullptr;
    if (!m_bBeachAnimsReferenced) {
        if (m_pBeachAnimBlock->IsLoaded) { CAnimManager::AddAnimBlockRef(m_BeachAnimBlockIndex); m_bBeachAnimsReferenced = true; }
        else CStreaming::RequestModel(IFPToModelId(m_BeachAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    if (!m_bSunbatheAnimsReferenced && ShouldLoadSunbatheAnims()) {
        if (m_pSunbatheAnimBlock->IsLoaded) { CAnimManager::AddAnimBlockRef(m_SunbatheAnimBlockIndex); m_bSunbatheAnimsReferenced = true; }
        else CStreaming::RequestModel(IFPToModelId(m_SunbatheAnimBlockIndex), STREAMING_KEEP_IN_MEMORY);
    }
    if (m_bBathing) {
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x528) |= 8;
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x474) |= 0x100000;
    }
    if (!SunbatheWeatherAllows()) {
        m_BathingTimer.Pause();
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x1A2)) m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
    }
    return m_pSubTask;
}

// 0x638290 (lote 197A, sin validar; thiscall, dos argumentos de pila (tipo de tarea y peaton; el peaton no se lee), `ret 8`, 725 bytes con marco SEH que solo libera la memoria si un constructor lanza (se omite)).
// Crea la subtarea de tipo `type`. Si ya hay subtarea (+8) y su GetTaskType (ranura 4) coincide, la devuelve sin crear nada. Tipos: 0xCA pausa (CTaskSimplePause 0x18 bytes), 0x1A2 animacion temporizada
// (CTaskSimpleRunTimedAnim 0x34), 0x1AC/0x1AD/0x1AE animaciones (CTaskSimpleRunAnim 0x20); cualquier otro (incluido 0x516) devuelve nulo. Cada tarea: CTask::operator new (0x61A5A0) y, si hay memoria, constructor
// del EXE; sin memoria devuelve nulo. El ORDEN de los efectos laterales es el del EXE: en 0xCA primero se reserva y despues se pide el azar (solo con memoria); en 0x1A2 y 0x1AD el azar se pide ANTES de reservar.
CTask* CTaskComplexSunbathe::CreateSubTask(eTaskType type, CPed* /*ped*/) {
    if (m_pSubTask && m_pSubTask->GetTaskType() == type) {
        return m_pSubTask;
    }

    // Reserva con CTask::operator new y construye solo con memoria (el EXE devuelve nulo sin memoria); los argumentos ya estan evaluados.
    const auto RunAnim = [&](AnimationId animId, const char* name, bool holdLastFrame) -> CTask* {
        void* const memory = CTask::operator new(sizeof(CTaskSimpleRunAnim));
        // Como el EXE (0x61A900, con hook): el constructor enganchado y su EAX como resultado
        return memory
            ? static_cast<CTaskSimpleRunAnim*>(memory)->Constructor2(static_cast<AssocGroupId>(0x30), animId, 4.0f, static_cast<int32>(type), name, holdLastFrame)
            : nullptr;
    };

    switch (static_cast<int32>(type)) {
    case 0xCA: { // pausa: espera de 10 s si falta algun bloque de animaciones necesario; si no, entre 1 y 5 s
        // Como el EXE: !(bloque de tomar el sol cargado) && ShouldLoad..., o !(bloque de playa cargado)
        const bool mustWait = (!m_pSunbatheAnimBlock->IsLoaded && ShouldLoadSunbatheAnims()) || !m_pBeachAnimBlock->IsLoaded;
        void* const memory = CTask::operator new(sizeof(CTaskSimplePause));
        if (!memory) {
            return nullptr;
        }
        const int32 durationMs = mustWait ? 10000 : CGeneral::GetRandomIntInRange(1000, 5000);
        return static_cast<CTaskSimplePause*>(memory)->Constructor(durationMs); // 0x48E750 (con hook): su EAX como resultado
    }
    case 0x1A2: { // tomar el sol: animacion temporizada
        const bool isSunbatheType = m_SunbatherType == SUNBATHER_MALE_1 || m_SunbatherType == SUNBATHER_FEMALE_1;
        int32 durationMs;
        if (m_pSunbatheAnimBlock->IsLoaded && isSunbatheType) {
            // Como el EXE: tiempo restante + 1000 (suma de 32 bits), tope 12000 (comparacion con signo) y azar entre 3000 y ese valor;
            // el EXE llama a GetTimeLeft (0x62EB80) una segunda vez cuando no supera el tope
            int32 maxMs = static_cast<int32>(static_cast<uint32>(m_BathingTimer.GetTimeLeft()) + 1000u);
            if (maxMs > 12000) {
                maxMs = 12000;
            } else {
                maxMs = static_cast<int32>(static_cast<uint32>(m_BathingTimer.GetTimeLeft()) + 1000u);
            }
            durationMs = CGeneral::GetRandomIntInRange(3000, maxMs);
        } else {
            durationMs = static_cast<int32>(static_cast<uint32>(m_BathingTimer.GetTimeLeft()) + 1000u);
        }
        const auto animId = static_cast<AnimationId>(static_cast<int32>(m_SunbatherType) + 0xEF);
        void* const memory = CTask::operator new(sizeof(CTaskSimpleRunTimedAnim));
        return memory
            ? static_cast<CTaskSimpleRunTimedAnim*>(memory)->Constructor(static_cast<AssocGroupId>(0x2F), animId, 4.0f, -4.0f, static_cast<uint32>(durationMs), static_cast<eTaskType>(0x1A2), DatosExe::Rdata::kS86E098 /*"sunbathe"*/, true) // 0x61ABE0 (con hook): su EAX como resultado
            : nullptr;
    }
    case 0x1AC: // empezar a tomar el sol
        return RunAnim(static_cast<AnimationId>(static_cast<int32>(m_SunbatherType) + 0xF4), DatosExe::Rdata::kS86E654 /*"start sunbathing"*/, true);
    case 0x1AD: { // reposo: tipos 0 y 2 eligen una de tres animaciones al azar; los demas usan el propio valor del tipo de tarea (0x1AD) como animacion, como el EXE
        int32 animId;
        if (m_SunbatherType == SUNBATHER_MALE_1) {
            animId = CGeneral::GetRandomIntInRange(0, 3) + 0xFE;
        } else if (m_SunbatherType == SUNBATHER_FEMALE_1) {
            animId = CGeneral::GetRandomIntInRange(0, 3) + 0x101;
        } else {
            animId = static_cast<int32>(type);
        }
        return RunAnim(static_cast<AnimationId>(animId), DatosExe::Rdata::kS86E644 /*"idle sunbathing"*/, true);
    }
    case 0x1AE: // dejar de tomar el sol (sin mantener el ultimo fotograma)
        return RunAnim(static_cast<AnimationId>(static_cast<int32>(m_SunbatherType) + 0xF9), DatosExe::Rdata::kS86E634 /*"stop sunbathing"*/, false);
    default: // incluye 0x516
        return nullptr;
    }
}
