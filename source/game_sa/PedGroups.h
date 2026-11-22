/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssE5.h"
#include "DatosExe/DataBssE2.h"
#include "DatosExe/DataBssE1.h"

#include "Base.h"
#include "PedGroup.h"

class CPed;

class CPedGroups {
public:
    static inline auto& ScriptReferenceIndex = DatosExe::Bss::bC098D0;
    static inline auto& ms_activeGroups = StaticRef<std::array<char, 8>>(0xC098E0);
    static inline auto& ms_bIsPlayerOnAMission = DatosExe::Bss::bC098E8;
    static inline auto& ms_iNoOfPlayerKills = DatosExe::Bss::bC098EC;
    static inline auto& ms_groups = StaticRef<std::array<CPedGroup, 8>>(0xC09920);

public:
    static void InjectHooks();

    static int32 AddGroup();
    static void RemoveGroup(int32 groupID);
    static void RemoveAllFollowersFromGroup(int32 groupId);

    static void Init();

    static void RegisterKillByPlayer();

    static void CleanUpForShutDown();

    static bool IsGroupLeader(CPed* ped);

    static CPedGroup* GetPedsGroup(const CPed* ped);
    static int32 GetGroupId(const CPedGroup* pedGroup);

    static void Process();

    static bool AreInSameGroup(const CPed* ped1, const CPed* ped2);
    static bool IsInPlayersGroup(CPed* ped);

    // inlined
    static CPedGroup& GetGroup(int32 groupId);

    static auto GetActiveGroupsWithIDs() {
        return ms_groups
            | rngv::enumerate
            | rngv::filter([](auto&& p) { return ms_activeGroups[std::get<0>(p)]; });
    }
};
