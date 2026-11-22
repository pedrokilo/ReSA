#pragma once
#include "StdInc.h"
class D3DResourceSystem {
public:
    static int32 CreateTexture(int32 width, int32 height, int32 levels, uint32 format, void** ppTexture); // 0x730510 (del EXE en el banco)
    static void DestroyTexture(void* texture);
};
