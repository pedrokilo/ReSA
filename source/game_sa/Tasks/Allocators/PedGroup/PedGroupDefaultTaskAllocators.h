#pragma once

#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"
#include "./PedGroupDefaultTaskAllocatorType.h"
#include "./PedGroupDefaultTaskAllocatorFollowAnyMeans.h"
#include "./PedGroupDefaultTaskAllocatorFollowLimited.h"
#include "./PedGroupDefaultTaskAllocatorStandStill.h"
#include "./PedGroupDefaultTaskAllocatorChat.h"
#include "./PedGroupDefaultTaskAllocatorSitInLeaderCar.h"
#include "./PedGroupDefaultTaskAllocatorRandom.h"

class CPedGroupDefaultTaskAllocators {
private:
    static inline const auto& ms_FollowAnyMeansAllocator = StaticRef<CPedGroupDefaultTaskAllocatorFollowAnyMeans>(0xC09908);
    static inline const auto& ms_FollowLimitedAllocator  = StaticRef<CPedGroupDefaultTaskAllocatorFollowLimited>(0xC0990C);
    static inline const auto& ms_StandStillAllocator     = StaticRef<CPedGroupDefaultTaskAllocatorStandStill>(0xC09910);
    static inline const auto& ms_ChatAllocator           = StaticRef<CPedGroupDefaultTaskAllocatorChat>(0xC09914);
    static inline const auto& ms_SitInLeaderCarAllocator = StaticRef<CPedGroupDefaultTaskAllocatorSitInLeaderCar>(0xC0991C);
    static inline const auto& ms_RandomAllocator         = StaticRef<CPedGroupDefaultTaskAllocatorRandom>(0xC09918);

public:
    static const CPedGroupDefaultTaskAllocator& Get(ePedGroupDefaultTaskAllocatorType type) {
        using enum ePedGroupDefaultTaskAllocatorType;
        switch (type) {
        case FOLLOW_ANY_MEANS:  return ms_FollowAnyMeansAllocator;
        case FOLLOW_LIMITED:    return ms_FollowLimitedAllocator;
        case STAND_STILL:       return ms_StandStillAllocator;
        case CHAT:              return ms_ChatAllocator;
        case SIT_IN_LEADER_CAR: return ms_SitInLeaderCarAllocator;
        case RANDOM:            return ms_RandomAllocator;
        default:                NOTSA_UNREACHABLE();
        }
    }

    // 0x5F7200 (lote 151A, banco Fn151A; el listado decia CPedGroupDefaultTaskAllocatorSitInLeaderCar): forma del EXE de Get (cdecl, un dword de pila, `ret`, 54 bytes + tabla de saltos de 6 entradas): devuelve la
    // direccion del asignador global (0xC09908 + {0, 4, 8, 0xC, 0x14, 0x10} segun el tipo 0..5) y 0 con cualquier otro valor (comparacion SIN signo: `cmp ecx, 5; ja`). Su unico llamador (0x5FBB77,
    // CPedGroupIntelligence::SetDefaultTaskAllocatorType) guarda ECX del llamador en EDX antes de la llamada y lo recupera despues: EDX debe salir intacto (el EXE solo escribe ECX y EAX). Se define en
    // PedGroupDefaultTaskAllocators.cpp, que se compila optimizado por pragma (CMakeLists.txt: /Zi y sin comprobaciones de ejecucion), para que el codigo no use EDX ni lleve marco.
    static const CPedGroupDefaultTaskAllocator* GetExe(uint32 type);

public:
    static void InjectHooks() {
        RH_ScopedClass(CPedGroupDefaultTaskAllocators);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedNamedInstall(GetExe, "Get", 0x5F7200); // lote 151A (banco Fn151A)
    }
};
