#pragma once
#include "DatosExe/DataBssE3.h"
#include "DatosExe/DataBssE1.h"

#include "Vector.h"

class CRestart {
public:
    static constexpr uint32 MAX_RESTART_POINTS{ 10u };

    static inline auto& bOverrideRespawnBasePointForMission = DatosExe::Bss::bA43248;
    static inline auto& OverrideRespawnBasePointForMission = StaticRef<CVector>(0xA4342C);
    static inline auto& OverrideHeading = DatosExe::Bss::bA43260;
    static inline auto& bOverrideRestart = DatosExe::Bss::bA43264;
    static inline auto& OverridePosition = StaticRef<CVector>(0xA43408);

    static inline auto& bFadeInAfterNextArrest = DatosExe::Bss::bA4325C;
    static inline auto& bFadeInAfterNextDeath = DatosExe::Bss::bA4325D;

    static inline auto& NumberOfPoliceRestarts = DatosExe::Bss::bA43268;
    static inline auto& PoliceRestartWhenToUse = StaticRef<int32[MAX_RESTART_POINTS]>(0xA43270);
    static inline auto& PoliceRestartHeadings = StaticRef<float[MAX_RESTART_POINTS]>(0xA43298);
    static inline auto& PoliceRestartPoints = StaticRef<CVector[MAX_RESTART_POINTS]>(0xA43390);

    static inline auto& NumberOfHospitalRestarts = DatosExe::Bss::bA4326C;
    static inline auto& HospitalRestartWhenToUse = StaticRef<int32[MAX_RESTART_POINTS]>(0xA432C0);
    static inline auto& HospitalRestartHeadings = StaticRef<float[MAX_RESTART_POINTS]>(0xA432E8);
    static inline auto& HospitalRestartPoints = StaticRef<CVector[MAX_RESTART_POINTS]>(0xA43318);

    // Script command 2271 (COMMAND_SET_EXTRA_HOSPITAL_RESTART_POINT) arguments
    static inline auto& ExtraHospitalRestartCoors = StaticRef<CVector>(0xA43414);
    static inline auto& ExtraHospitalRestartRadius = StaticRef<float>(0xA43258);
    static inline auto& ExtraHospitalRestartHeading = StaticRef<float>(0xA43254);

    // Script command 2272 (COMMAND_SET_EXTRA_POLICE_RESTART_POINT) arguments
    static inline auto& ExtraPoliceStationRestartCoors = StaticRef<CVector>(0xA43420);
    static inline auto& ExtraPoliceStationRestartRadius = StaticRef<float>(0xA43250);
    static inline auto& ExtraPoliceStationRestartHeading = StaticRef<float>(0xA4324C);

public:
    static void InjectHooks();

    static void Initialise();
    static void AddHospitalRestartPoint(const CVector& point, float angle, int32 townId);
    static void AddPoliceRestartPoint(const CVector& point, float angle, int32 townId);
    static void OverrideNextRestart(CVector const& point, float angle);
    static void CancelOverrideRestart();
    static void SetRespawnPointForDurationOfMission(CVector point);
    static void ClearRespawnPointForDurationOfMission();
    static void FindClosestHospitalRestartPoint(CVector point, CVector& outPos, float& outAngle);
    static void FindClosestPoliceRestartPoint(CVector point, CVector& storedPoint, float& outAngle);
    static bool Load();
    static bool Save();
};
