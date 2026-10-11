#include "StdInc.h"

#include "TaskComplexWaitForPed.h"

void CTaskComplexWaitForPed::InjectHooks() {
    RH_ScopedClass(CTaskComplexWaitForPed);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x683340);
}

static __declspec(naked) void TaskCtorRegisterReference() {
    __asm {
        push 00571B70h
        ret
    }
}
// Como el original: entrada completa, enlace SEH y padding intacto.
#pragma warning(push)
// The original handler belongs to the host EXE, not this DLL's SafeSEH table.
#pragma warning(disable: 4733)
__declspec(naked) CTaskComplexWaitForPed* CTaskComplexWaitForPed::Constructor(CPed*, float, uint32, bool) {
    __asm {
        push        0FFFFFFFFh
        push        846798h
        mov         eax,dword ptr fs:[00000000h]
        push        eax
        mov         dword ptr fs:[00000000h],esp
        push        ecx
        push        esi
        mov         esi,ecx
        mov         dword ptr [esp+4],esi
        mov dword ptr [ecx+4],0
        mov dword ptr [ecx+8],0
        mov         edx,dword ptr [esp+1Ch]
        mov         ecx,dword ptr [esp+18h]
        xor         eax,eax
        mov         dword ptr [esi],8706F8h
        mov         dword ptr [esi+1Ch],eax
        mov         dword ptr [esi+20h],eax
        mov         byte ptr [esi+24h],al
        mov         byte ptr [esi+25h],al
        mov         dword ptr [esi+10h],edx
        mov         edx,dword ptr [esp+20h]
        mov         dword ptr [esp+10h],eax
        lea         eax,[esi+0Ch]
        mov         dword ptr [esi+14h],edx
        mov         dl,byte ptr [esp+24h]
        push        eax
        mov         dword ptr [eax],ecx
        mov         byte ptr [esi+18h],dl
        call TaskCtorRegisterReference
        mov         ecx,dword ptr [esp+8]
        mov         eax,esi
        pop         esi
        mov         dword ptr fs:[00000000h],ecx
        add         esp,10h
        ret         10h
    }
}
#pragma warning(pop)
