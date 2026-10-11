#include "StdInc.h"

#include "TaskSimpleFight.h"

// 0x61C470
CTaskSimpleFight::CTaskSimpleFight(CEntity* entity, int32 nCommand, uint32 nIdlePeriod) : CTaskSimple() {
    plugin::CallMethod<0x61C470, CTaskSimpleFight*, CEntity*, int32, uint32>(this, entity, nCommand, nIdlePeriod);
}

// 0x61C530
CTaskSimpleFight::~CTaskSimpleFight() {
    plugin::CallMethod<0x61C530, CTaskSimpleFight*>(this);
}

// 0x61C5E0
// Colaboradores de referencia reales; saltos sin cambiar registros adicionales.
static __declspec(naked) void TaskLeafCleanupReference() { __asm {
    push 00571A00h
    ret
} }
static __declspec(naked) void TaskLeafRegisterReference() { __asm {
    push 00571B70h
    ret
} }

__declspec(naked) bool CTaskSimpleFight::ControlFight(CEntity*, uint8) {
    __asm {
        push        ebx
        mov         ebx,dword ptr [esp+8]
        push        esi
        push        edi
        mov         edi,ecx
        mov         ecx,dword ptr [edi+18h]
        cmp         ebx,ecx
        lea         esi,[edi+18h]
        mov         byte ptr [edi+9],1
        je          taskControl_61C60F
        test        ecx,ecx
        je          taskControl_61C601
        push        esi
        call TaskLeafCleanupReference
taskControl_61C601:
        test        ebx,ebx
        mov         dword ptr [esi],ebx
        je          taskControl_61C60F
        push        esi
        mov         ecx,ebx
        call TaskLeafRegisterReference
taskControl_61C60F:
        mov         al,byte ptr [esp+14h]
        cmp         al,byte ptr [edi+26h]
        jle         taskControl_61C61B
        mov         byte ptr [edi+26h],al
taskControl_61C61B:
        pop         edi
        pop         esi
        mov         al,1
        pop         ebx
        ret 8
    }
}

// 0x4ABDC0
__declspec(naked) bool CTaskSimpleFight::IsComboSet() {
    __asm {
        movsx eax,byte ptr [ecx+24h]
        mov cl,byte ptr [ecx+25h]
        add eax,0FFFFFFFCh
        xor edx,edx
        test eax,eax
        setl dl
        dec edx
        and eax,edx
        imul eax,eax,88h
        movzx eax,word ptr [eax+0C17154h]
        mov edx,10h
        shl edx,cl
        test eax,edx
        setne al
        ret
    }
}

// 0x4ABDF0
__declspec(naked) bool CTaskSimpleFight::IsHitComboSet() {
    __asm {
        mov eax,ecx
        mov cl,byte ptr [eax+25h]
        cmp cl,2
        jg fightHitFalse
        movsx eax,byte ptr [eax+24h]
        add eax,0FFFFFFFCh
        xor edx,edx
        test eax,eax
        setl dl
        dec edx
        and eax,edx
        imul eax,eax,88h
        movzx eax,word ptr [eax+0C17154h]
        mov edx,1000h
        shl edx,cl
        test edx,eax
        je fightHitFalse
        mov eax,1
        ret
    fightHitFalse:
        xor eax,eax
        ret
    }
}

// 0x4ABDA0
__declspec(naked) AssocGroupId CTaskSimpleFight::GetComboAnimGroupID() {
    __asm {
        movsx eax,byte ptr [ecx+24h]
        add eax,0FFFFFFFCh
        xor ecx,ecx
        test eax,eax
        setl cl
        dec ecx
        and eax,ecx
        imul eax,eax,88h
        mov eax,dword ptr [eax+0C170D0h]
        ret
    }
}

// 0x5BEDC0
void CTaskSimpleFight::LoadMeleeData() {
    ZoneScoped;

    plugin::Call<0x5BEDC0>();
}

// 0x6239F0
bool CTaskSimpleFight::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x6239F0, CTaskSimpleFight*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x629920
bool CTaskSimpleFight::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x629920, CTaskSimpleFight*, CPed*>(this, ped);
}

void CTaskSimpleFight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFight, 0x86D684, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61C470);
    RH_ScopedInstall(Destructor, 0x61C530);
    RH_ScopedInstall(ControlFight, 0x61C5E0);
    RH_ScopedInstall(GetComboAnimGroupID, 0x4ABDA0);
    RH_ScopedInstall(IsComboSet, 0x4ABDC0);
    RH_ScopedInstall(IsHitComboSet, 0x4ABDF0);

}

// Colaborador real de referencias, con ruta RH actual.
static __declspec(naked) void FightCtor5RegisterRef() {
    __asm {
        push 00571B70h
        ret
    }
}
#pragma warning(push)
#pragma warning(disable: 4733) // Como el original: handler SEH en el EXE.
__declspec(naked) CTaskSimpleFight* CTaskSimpleFight::Constructor(CEntity*, int32, uint32) {
    __asm {
        push        0FFFFFFFFh
        push        83F318h
        mov         eax,dword ptr fs:[00000000h]
        push        eax
        mov         dword ptr fs:[00000000h],esp
        push        ecx
        push        esi
        mov         esi,ecx
        push        edi
        mov         dword ptr [esp+8],esi
        mov dword ptr [ecx+4],0
        mov         edi,dword ptr [esp+1Ch]
        xor         eax,eax
        or          cl,0FFh
        cmp         edi,eax
        mov         byte ptr [esi+24h],cl
        mov         byte ptr [esi+25h],cl
        mov         cl,byte ptr [esp+20h]
        lea         edx,[esi+18h]
        mov         dword ptr [esp+14h],eax
        mov         dword ptr [esi],86D684h
        mov         byte ptr [esi+8],al
        mov         byte ptr [esi+9],1
        mov         byte ptr [esi+0Ah],al
        mov         dword ptr [esi+0Ch],21h
        mov         word ptr [esi+12h],ax
        mov         byte ptr [esi+14h],al
        mov         byte ptr [esi+15h],al
        mov         dword ptr [edx],edi
        mov         dword ptr [esi+1Ch],eax
        mov         dword ptr [esi+20h],eax
        mov         byte ptr [esi+26h],cl
        mov         byte ptr [esi+27h],al
        je          fightCtor5_61C4E8
        push        edx
        mov         ecx,edi
        call FightCtor5RegisterRef
fightCtor5_61C4E8:
        mov         eax,dword ptr [esp+24h]
        cmp         eax,0EA60h
        jbe         fightCtor5_61C4F8
        mov         eax,0EA60h
fightCtor5_61C4F8:
        mov         ecx,dword ptr [esp+0Ch]
        mov         word ptr [esi+10h],ax
        pop         edi
        mov         eax,esi
        pop         esi
        mov         dword ptr fs:[00000000h],ecx
        add         esp,10h
        ret         0Ch
    }
}
#pragma warning(pop)

// Normal-path SEH uses the original EXE handler, matching 0x61C530.
static __declspec(naked) void FightDtor12Cleanup() { __asm {
    push 00571A00h
    ret
} }
static __declspec(naked) void FightDtor12Callback() { __asm {
    push 004CEBC0h
    ret
} }
static __declspec(naked) void FightDtor12ReleaseBlock() { __asm {
    push 004D3FD0h
    ret
} }
static __declspec(naked) void FightDtor12Base() { __asm {
    push 0061A3A0h
    ret
} }
#pragma warning(push)
#pragma warning(disable: 4733)
__declspec(naked) CTaskSimpleFight* CTaskSimpleFight::Destructor() {
    __asm {
        push 0FFFFFFFFh
        push 0083F338h
        mov eax,fs:[0]
        push eax
        mov fs:[0],esp
        push ecx
        push esi
        mov esi,ecx
        mov dword ptr [esp+4],esi
        mov dword ptr [esi],0086D684h
        mov ecx,dword ptr [esi+18h]
        test ecx,ecx
        lea eax,[esi+18h]
        mov dword ptr [esp+10h],0
        je fightDtor12_61C56B
        push eax
        call FightDtor12Cleanup
fightDtor12_61C56B:
        mov ecx,dword ptr [esi+1Ch]
        test ecx,ecx
        je fightDtor12_61C57E
        push 0
        push 004D1390h
        call FightDtor12Callback
fightDtor12_61C57E:
        mov ecx,dword ptr [esi+20h]
        test ecx,ecx
        je fightDtor12_61C591
        push 0
        push 004D1390h
        call FightDtor12Callback
fightDtor12_61C591:
        mov al,byte ptr [esi+0Ah]
        test al,al
        je fightDtor12_61C5C1
        mov eax,dword ptr [esi+0Ch]
        cmp eax,21h
        je fightDtor12_61C5C1
        mov ecx,dword ptr ds:[00B4EA34h]
        lea eax,[eax+eax*4]
        mov eax,dword ptr [ecx+eax*4]
        sub eax,00B5D4A0h
        sar eax,5
        push eax
        call FightDtor12ReleaseBlock
        add esp,4
        mov byte ptr [esi+0Ah],0
fightDtor12_61C5C1:
        mov ecx,esi
        mov dword ptr [esp+10h],0FFFFFFFFh
        call FightDtor12Base
        mov ecx,dword ptr [esp+8]
        pop esi
        mov fs:[0],ecx
        add esp,10h
        ret
    }
}
#pragma warning(pop)
