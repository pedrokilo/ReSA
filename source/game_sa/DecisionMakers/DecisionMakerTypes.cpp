#include "StdInc.h"

#include "DecisionMakerTypes.h"

void CDecisionMakerTypes::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypes);
    RH_ScopedCategory("DecisionMakers");
    RH_ScopedOverloadedInstall(MakeDecision, "Ped", 0x606E70, void(CDecisionMakerTypes::*)(CPed*, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType, bool, int16&, int16&));
    RH_ScopedOverloadedInstall(MakeDecision, "Group", 0x606F80, eTaskType(CDecisionMakerTypes::*)(CPedGroup*, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType));

    RH_ScopedInstall(GetInstance, 0x4684F0);
    RH_ScopedInstall(AddDecisionMaker, 0x607050);
    RH_ScopedInstall(RemoveDecisionMaker, 0x6043A0);
    RH_ScopedInstall(LoadEventIndices, 0x600840);
}

static __declspec(naked) void Decision6SetPed() {
    __asm {
        push 00600B50h
        ret
    }
}
static __declspec(naked) void Decision6SetGroup() {
    __asm {
        push 005F7340h
        ret
    }
}
static __declspec(naked) void Decision6Assign() {
    __asm {
        push 006006B0h
        ret
    }
}
static __declspec(naked) void Decision6ReadEvents() {
    __asm {
        push 005BB9F0h
        ret
    }
}

// 0x607050
__declspec(naked) int32 CDecisionMakerTypes::AddDecisionMaker(CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission) {
    __asm {
        push        ecx
        mov         dl,byte ptr [esp+10h]
        push        ebx
        push        esi
        or          eax,0FFFFFFFFh
        test        dl,dl
        push        edi
        mov         dword ptr [esp+0Ch],ecx
        je decision6_60706F
        mov         edx,0Fh
        mov         esi,14h
        jmp decision6_607076
decision6_60706F:
        xor         edx,edx
        mov         esi,0Fh
decision6_607076:
        movzx       edi,dx
        movzx       edx,si
        cmp         edi,edx
        jge decision6_6070DA
decision6_607080:
        mov         bl,byte ptr [edi+00C0B01Ch]
        test        bl,bl
        je decision6_607096
        inc         edi
        cmp         edi,edx
        jl decision6_607080
        pop         edi
        pop         esi
        pop         ebx
        pop         ecx
        ret         0Ch
decision6_607096:
        mov         al,byte ptr [esp+18h]
        mov         ebx,dword ptr [esp+14h]
        mov         edx,edi
        imul        edx,edx,99Ch
        lea         esi,[edx+ecx+4]
        push        ebp
        mov         byte ptr [edi+00C0B01Ch],1
        mov         byte ptr [edi+00C0AFE0h],al
        sub         ebx,esi
        mov         ebp,29h
        nop
decision6_6070C0:
        lea         eax,[ebx+esi]
        push        eax
        mov         ecx,esi
        call Decision6Assign
        add         esi,3Ch
        dec         ebp
        jne decision6_6070C0
        mov         eax,dword ptr [esp+10h]
        inc         dword ptr [eax]
        mov         eax,edi
        pop         ebp
decision6_6070DA:
        pop         edi
        pop         esi
        pop         ebx
        pop         ecx
        ret         0Ch
    }
}

// 0x4684F0
namespace {
__declspec(naked) void Task11AllocateDecisionTypes() {
    __asm {
        push 0082119Ah
        ret
    }
}
__declspec(naked) void Task11ConstructDecisionTypes() {
    __asm {
        push 004650F0h
        ret
    }
}
}
// The original EXE SEH handler is retained. Normal-path restoration is tested;
// exception unwinding through this cross-module registration is not certified.
#pragma warning(push)
#pragma warning(disable: 4733)
__declspec(naked) CDecisionMakerTypes* CDecisionMakerTypes::GetInstance() {
    __asm {
        push 0FFFFFFFFh
        push 837D9Bh
        mov eax,dword ptr fs:[00000000h]
        push eax
        mov dword ptr fs:[00000000h],esp
        push ecx
        mov eax,dword ptr ds:[00C0B030h]
        test eax,eax
        jne task11_46854E
        push 0F1C0h
        call Task11AllocateDecisionTypes
        add esp,4
        mov dword ptr [esp],eax
        test eax,eax
        mov dword ptr [esp+0Ch],0
        je task11_468547
        mov ecx,eax
        call Task11ConstructDecisionTypes
        mov dword ptr ds:[00C0B030h],eax
        mov ecx,dword ptr [esp+4]
        mov dword ptr fs:[00000000h],ecx
        add esp,10h
        ret
task11_468547:
        xor eax,eax
        mov dword ptr ds:[00C0B030h],eax
task11_46854E:
        mov ecx,dword ptr [esp+4]
        mov dword ptr fs:[00000000h],ecx
        add esp,10h
        ret
    }
}
#pragma warning(pop)

static __declspec(naked) void Decision7One() {
    __asm {
        push 006040D0h
        ret
    }
}
static __declspec(naked) void Decision7Many() {
    __asm {
        push 00606060h
        ret
    }
}

// 0x606E70
__declspec(naked) void CDecisionMakerTypes::MakeDecision(CPed* ped, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskTypeToAvoid1, eTaskType taskTypeToAvoid2, eTaskType taskTypeToAvoid3, eTaskType taskTypeToSeek, bool bUseInGroupDecisionMaker, int16& taskType, int16& facialTaskType) {
    __asm {
        mov         al,byte ptr [esp+24h]
        test        al,al
        push        ebx
        mov         ebx,dword ptr [esp+8]
        push        esi
        push        edi
        je decision7_606E8D
        mov         eax,dword ptr [ebx+0000047Ch]
        mov         eax,dword ptr [eax+000000B8h]
        jmp decision7_606E99
decision7_606E8D:
        mov         edx,dword ptr [ebx+0000047Ch]
        mov         eax,dword ptr [edx+000000B4h]
decision7_606E99:
        cmp         eax,0FFFFFFFEh
        mov         edi,dword ptr [esp+38h]
        mov         edx,dword ptr [esp+14h]
        mov         esi,dword ptr [esp+34h]
        mov         edx,dword ptr [ecx+edx*4+0000C034h]
        push        edi
        mov         word ptr [esi],0C8h
        mov         word ptr [edi],0FFFFh
        push        esi
        jne decision7_606EF0
        mov         eax,dword ptr [esp+34h]
        imul        edx,edx,3Ch
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        lea         ecx,[edx+ecx+0000D4ECh]
        call Decision7One
        pop         edi
        pop         esi
        pop         ebx
        ret         2Ch
decision7_606EF0:
        cmp         eax,0FFFFFFFFh
        jne decision7_606F3F
        cmp         byte ptr [ebx+00000484h],2
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        mov         eax,dword ptr [esp+34h]
        push        eax
        push        edx
        jne decision7_606F2E
        add         ecx,0CB50h
        call Decision7Many
        pop         edi
        pop         esi
        pop         ebx
        ret         2Ch
decision7_606F2E:
        add         ecx,0C1B4h
        call Decision7Many
        pop         edi
        pop         esi
        pop         ebx
        ret         2Ch
decision7_606F3F:
        mov         esi,dword ptr [esp+34h]
        imul        eax,eax,29h
        push        esi
        mov         esi,dword ptr [esp+34h]
        push        esi
        mov         esi,dword ptr [esp+34h]
        push        esi
        mov         esi,dword ptr [esp+34h]
        add         eax,edx
        imul        eax,eax,3Ch
        push        esi
        mov         esi,dword ptr [esp+34h]
        push        esi
        mov         esi,dword ptr [esp+34h]
        push        esi
        lea         ecx,[eax+ecx+4]
        call Decision7One
        pop         edi
        pop         esi
        pop         ebx
        ret         2Ch
    }
}

// 0x6043A0
__declspec(naked) void CDecisionMakerTypes::RemoveDecisionMaker(eDecisionTypes dm) {
    __asm {
        push        ecx
        push        ebx
        mov         ebx,dword ptr [esp+0Ch]
        mov         al,byte ptr [ebx+00C0B01Ch]
        test        al,al
        mov         dword ptr [esp+4],ecx
        je decision6_604484
        mov         al,byte ptr [ebx+00C0AFE0h]
        test        al,al
        push        esi
        push        edi
        jne decision6_60443A
        push        ebp
        mov         ebp,dword ptr ds:[00B74490h]
        mov         esi,dword ptr [ebp+8]
        test        esi,esi
        je decision6_60441F
        mov         edi,esi
        imul        edi,edi,7C4h
        lea         ebx,[ebx+00000000h]
decision6_6043E0:
        mov         eax,dword ptr [ebp+4]
        mov         cl,byte ptr [esi+eax-1]
        dec         esi
        sub         edi,7C4h
        test        cl,cl
        js decision6_604417
        mov         eax,dword ptr [ebp]
        add         eax,edi
        je decision6_604417
        mov         ecx,dword ptr [eax+0000047Ch]
        cmp         dword ptr [ecx+000000B4h],ebx
        jne decision6_604417
        mov         edx,dword ptr [eax+0000059Ch]
        movsx       eax,byte ptr [edx+32h]
        push        eax
        call Decision6SetPed
decision6_604417:
        test        esi,esi
        jne decision6_6043E0
        mov         ecx,dword ptr [esp+10h]
decision6_60441F:
        pop         ebp
        mov         byte ptr [ebx+00C0B01Ch],0
        mov         byte ptr [ebx+00C0AFE0h],0
        mov         eax,dword ptr [ecx]
        pop         edi
        dec         eax
        pop         esi
        mov         dword ptr [ecx],eax
        pop         ebx
        pop         ecx
        ret         4
decision6_60443A:
        xor         edi,edi
        mov         esi,0C09950h
decision6_604441:
        mov         al,byte ptr [edi+00C098E0h]
        test        al,al
        je decision6_60445C
        cmp         ebx,dword ptr [esi+00000298h]
        jne decision6_60445C
        push        0FFFFFFFFh
        mov         ecx,esi
        call Decision6SetGroup
decision6_60445C:
        add         esi,2D4h
        inc         edi
        cmp         esi,0C0AFF0h
        jl decision6_604441
        mov         eax,dword ptr [esp+0Ch]
        mov         byte ptr [ebx+00C0B01Ch],0
        mov         byte ptr [ebx+00C0AFE0h],0
        mov         ecx,dword ptr [eax]
        dec         ecx
        pop         edi
        mov         dword ptr [eax],ecx
        pop         esi
decision6_604484:
        pop         ebx
        pop         ecx
        ret         4
    }
}

// 0x606F80
__declspec(naked) eTaskType CDecisionMakerTypes::MakeDecision(CPedGroup* pedGroup, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskId1, eTaskType taskId2, eTaskType taskId3, eTaskType taskId4) {
    __asm {
        mov         edx,dword ptr [esp+8]
        mov         edx,dword ptr [ecx+edx*4+0000C034h]
        push        esi
        mov         esi,dword ptr [esp+8]
        mov         eax,dword ptr [esi+000002C8h]
        cmp         eax,0FFFFFFFFh
        mov         dword ptr [esp+8],0C8h
        jne decision7_607002
        mov         al,byte ptr [esi+000002D0h]
        imul        edx,edx,3Ch
        cmp         al,1
        lea         eax,[esp+24h]
        push        eax
        lea         eax,[esp+0Ch]
        push        eax
        mov         eax,dword ptr [esp+2Ch]
        push        eax
        mov         eax,dword ptr [esp+2Ch]
        push        eax
        mov         eax,dword ptr [esp+2Ch]
        push        eax
        mov         eax,dword ptr [esp+2Ch]
        push        eax
        mov         eax,dword ptr [esp+2Ch]
        push        eax
        mov         eax,dword ptr [esp+2Ch]
        push        eax
        jne decision7_606FED
        lea         ecx,[edx+ecx+0000E824h]
        call Decision7One
        movsx       eax,word ptr [esp+8]
        pop         esi
        ret         20h
decision7_606FED:
        lea         ecx,[edx+ecx+0000DE88h]
        call Decision7One
        movsx       eax,word ptr [esp+8]
        pop         esi
        ret         20h
decision7_607002:
        imul        eax,eax,29h
        lea         esi,[esp+24h]
        push        esi
        lea         esi,[esp+0Ch]
        push        esi
        mov         esi,dword ptr [esp+2Ch]
        push        esi
        mov         esi,dword ptr [esp+2Ch]
        push        esi
        mov         esi,dword ptr [esp+2Ch]
        push        esi
        mov         esi,dword ptr [esp+2Ch]
        add         eax,edx
        imul        eax,eax,3Ch
        push        esi
        mov         esi,dword ptr [esp+2Ch]
        push        esi
        mov         esi,dword ptr [esp+2Ch]
        push        esi
        lea         ecx,[eax+ecx+4]
        call Decision7One
        movsx       eax,word ptr [esp+8]
        pop         esi
        ret         20h
    }
}

// 0x6044C0
void CDecisionMakerTypes::AddEventResponse(int32 decisionMakerIndex, eEventType eventType, eTaskType taskId, float* responseChances, int32* flags) {
    plugin::CallMethod<0x6044C0, CDecisionMakerTypes*, int32, int32, int32, float*, int32*>(this, decisionMakerIndex, eventType, taskId, responseChances, flags);
}

// 0x604490
void CDecisionMakerTypes::FlushDecisionMakerEventResponse(int32 decisionMakerIndex, eEventType eventId) {
    plugin::CallMethod<0x604490>(this, decisionMakerIndex, eventId);
}

__declspec(naked) void CDecisionMakerTypes::LoadEventIndices() {
    __asm {
        push        86CD44h
        lea         eax,[ecx+0000C034h]
        push        eax
        call Decision6ReadEvents
        ret
    }
}
