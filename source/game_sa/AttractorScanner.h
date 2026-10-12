/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskTimer.h"

class C2dEffect;

class CAttractorScanner {
public:
    char       field_0;
    char       _pad[3];
    CTaskTimer field_4;
    C2dEffect* m_pEffectInUse;
    int32      field_14;
    int32      field_18[10];
    int32      field_40[10];
    int32      field_68[10];

    static void InjectHooks();
    /**
     * Pone a cero los dos primeros vectores y deja las distancias en 25 (las posiciones 4 y 7, al cuadrado de 0x86C52C).
     * Devuelve EDX:EAX tal como los deja el EXE (0x5FFF90), que sus llamadores y el banco comprueban: EAX = &field_68[10]
     * y EDX = 10. Los llamadores en C++ pueden ignorar el valor.
     */
    uint64 Clear();
};

VALIDATE_SIZE(CAttractorScanner, 0x90);
