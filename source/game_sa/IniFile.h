/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataInicializadaA.h"

class CIniFile {
public:
    static inline auto& PedNumberMultiplier = DatosExe::Data::v8CDF14;
    static inline auto& CarNumberMultiplier = DatosExe::Data::v8CDF18;

public:
    static void InjectHooks();

    static void LoadIniFile();
};
