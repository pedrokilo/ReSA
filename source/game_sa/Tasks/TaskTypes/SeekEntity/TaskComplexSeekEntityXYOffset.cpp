#include "StdInc.h"
#include "TaskComplexSeekEntityXYOffset.h"

void CTaskComplexSeekEntityXYOffset::InjectHooks() {

}

// 0x4BC470: shared SeekEntity prefix used for every calculator specialization.
template<> __declspec(naked) void CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D(float) {
    __asm {
        fld dword ptr [esp+4]
        fcomp dword ptr [ecx+18h]
        fnstsw ax
        test ah,44h
        jnp done
        mov eax,dword ptr [esp+4]
        mov dword ptr [ecx+18h],eax
        mov edx,dword ptr ds:[00B7CB84h]
        mov dword ptr [ecx+28h],edx
        mov dword ptr [ecx+2Ch],0
        mov byte ptr [ecx+30h],1
        done: ret 4
    }
}
