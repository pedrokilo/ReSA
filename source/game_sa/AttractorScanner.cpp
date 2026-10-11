#include "StdInc.h"

#include "AttractorScanner.h"

__declspec(naked) void CAttractorScanner::Clear() {
    __asm {
        push        esi
        xor         esi,esi
        xor         edx,edx
        lea         eax,[ecx+68h]
        jmp         attractorClear5_5FFFA0
        lea         ebx,[ebx+00000000h]
attractorClear5_5FFFA0:
        cmp         edx,4
        mov         dword ptr [eax-50h],esi
        mov         dword ptr [eax-28h],esi
        fld         dword ptr ds:[0086C52Ch]
        fmul        dword ptr ds:[0086C52Ch]
        fstp        dword ptr [eax]
        je          attractorClear5_5FFFC4
        cmp         edx,7
        je          attractorClear5_5FFFC4
        mov         dword ptr [eax],41C80000h
attractorClear5_5FFFC4:
        inc         edx
        add         eax,4
        cmp         edx,0Ah
        jl          attractorClear5_5FFFA0
        pop         esi
        ret
    }
}

void CAttractorScanner::InjectHooks() {
    RH_ScopedClass(CAttractorScanner);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(Clear, 0x5FFF90);
}
