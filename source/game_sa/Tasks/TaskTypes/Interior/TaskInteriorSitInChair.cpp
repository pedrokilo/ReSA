#include "StdInc.h"

#include "TaskInteriorSitInChair.h"

void CTaskInteriorSitInChair::InjectHooks() {
    RH_ScopedClass(CTaskInteriorSitInChair);
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x675C80);
    RH_ScopedInstall(Constructor, 0x675C30);
}

// Como el original: conserva la vtable EXE para los demás métodos.
__declspec(naked) CTaskInteriorSitInChair* CTaskInteriorSitInChair::Constructor(void*, uint32, uint8) {
    __asm {
        push        esi
        mov         esi,ecx
        mov dword ptr [ecx+4],0
        mov         ecx,dword ptr [esp+8]
        mov         edx,dword ptr [esp+0Ch]
        xor         eax,eax
        mov         dword ptr [esi],870314h
        mov         dword ptr [esi+1Ch],eax
        mov         dword ptr [esi+20h],eax
        mov         byte ptr [esi+24h],al
        mov         byte ptr [esi+25h],al
        mov         dword ptr [esi+8],ecx
        mov         cl,byte ptr [esp+10h]
        mov         dword ptr [esi+14h],eax
        mov         byte ptr [esi+29h],al
        mov         byte ptr [esi+28h],al
        mov         byte ptr [esi+2Ah],al
        mov         dword ptr [esi+0Ch],edx
        mov         byte ptr [esi+10h],cl
        mov         dword ptr [esi+18h],0FFFFFFFFh
        mov         eax,esi
        pop         esi
        ret         0Ch
    }
}
