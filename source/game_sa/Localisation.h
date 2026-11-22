/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssE1.h"

class CLocalisation {
public:
    static inline auto& nastyGame = DatosExe::Bss::bB9B7EC;
    static inline auto& germanGame = DatosExe::Bss::bB9B7ED;
    static inline auto& frenchGame = DatosExe::Bss::bB9B7EE;

public:
    static void InjectHooks();

    static void Initialise();
    static void SetNormalGame();
    static void SetFrenchGame();
    static void SetGermanGame();
    static bool GermanGame();
    static bool FrenchGame();
    static bool Metric();
    static bool Blood();
    static bool Porn();
    static bool ScreamsFromKills();
    static bool Prostitutes();
    static bool KickingWhenDown();
    static bool ShootLimbs();
    static bool KnockDownPeds();
    static bool KillFrenzy();
    static bool StealFromDeadPed();
    static bool KillPeds();
    static bool PedsOnFire();
};