#include "StdInc.h"

#include "EventSource.h"

static __declspec(naked) void EventSource5IsPlayer() {
    __asm {
        push 005DF8F0h
        ret
    }
}
static __declspec(naked) void EventSource5Friendly() {
    __asm {
        push 00601BC0h
        ret
    }
}
static __declspec(naked) void EventSource5Threatened() {
    __asm {
        push 00601C30h
        ret
    }
}
__declspec(naked) int32 CEventSource::ComputeEventSourceType(const CEvent&, const CPed&) {
    __asm {
        mov         ecx,dword ptr [esp+4]
        mov         eax,dword ptr [ecx]
        push        ebx
        push        esi
        xor         ebx,ebx
        call        dword ptr [eax+28h]
        mov         esi,eax
        test        esi,esi
        je          eventSource5_4ABB27
        mov         cl,byte ptr [esi+36h]
        and         cl,7
        cmp         cl,3
        jne         eventSource5_4ABB27
        mov         edx,dword ptr [esp+10h]
        push        edi
        mov         edi,dword ptr [edx+0000047Ch]
        push        esi
        mov         ecx,edi
        call EventSource5Threatened
        test        al,al
        je          eventSource5_4ABAFE
        pop         edi
        pop         esi
        mov         eax,3
        pop         ebx
        ret
eventSource5_4ABAFE:
        push        esi
        mov         ecx,edi
        call EventSource5Friendly
        test        al,al
        je          eventSource5_4ABB13
        pop         edi
        pop         esi
        mov         eax,2
        pop         ebx
        ret
eventSource5_4ABB13:
        mov         ecx,esi
        call EventSource5IsPlayer
        test        al,al
        je          eventSource5_4ABB2C
        pop         edi
        pop         esi
        mov         eax,1
        pop         ebx
        ret
eventSource5_4ABB27:
        pop         esi
        xor         eax,eax
        pop         ebx
        ret
eventSource5_4ABB2C:
        pop         edi
        pop         esi
        mov         eax,ebx
        pop         ebx
        ret
    }
}

void CEventSource::InjectHooks() {
    RH_ScopedClass(CEventSource);
    RH_ScopedCategory("Events");
    RH_ScopedInstall(ComputeEventSourceType, 0x4ABAC0);
}
