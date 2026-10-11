#include "StdInc.h"

#include "PlayerRelationshipRecorder.h"
#include "TaskCategories.h"

// 0x61A130
CPlayerRelationshipRecorder::CPlayerRelationshipRecorder() {
    Flush();
}

// 0x61A2C0
CPlayerRelationshipRecorder::~CPlayerRelationshipRecorder() {
    Flush();
}

// 0x61A2A0
void CPlayerRelationshipRecorder::Flush() {
    for (auto& relationship : m_Relationships) {
        relationship.Flush();
    }
}

// 0x61A180
void CPlayerRelationshipRecorder::AddRelationship(const CPed* ped, int32 value) {
    auto& rel = m_Relationships[0];
    if (rel.Ped) {
        rel.Ped = ped;
        rel.Relationship = value;
    }
}

// 0x61A1D0
static __declspec(naked) void Relationship6GetActive() {
    __asm {
        push 00681720h
        ret
    }
}
static __declspec(naked) void Relationship6Kill() {
    __asm {
        push 006985E0h
        ret
    }
}
static __declspec(naked) void Relationship6Follow() {
    __asm {
        push 00698610h
        ret
    }
}
__declspec(naked) void CPlayerRelationshipRecorder::RecordRelationshipWithPlayer(const CPed*) {
    __asm {
        sub         esp,8
        push        ebx
        push        ebp
        mov         ebp,dword ptr [esp+14h]
        push        esi
        xor         ebx,ebx
        push        edi
        mov         esi,ecx
        xor         eax,eax
relationship6_61A1E1:
        cmp         dword ptr [esi+eax*8],ebp
        je relationship6_61A1EE
        inc         eax
        cmp         eax,10h
        jl relationship6_61A1E1
        jmp relationship6_61A1F5
relationship6_61A1EE:
        mov         dword ptr [esi+eax*8],ebx
        mov         byte ptr [esi+eax*8+4],bl
relationship6_61A1F5:
        mov         eax,dword ptr [ebp+0000047Ch]
        lea         ecx,[eax+4]
        call Relationship6GetActive
        mov         edi,eax
        cmp         edi,ebx
        je relationship6_61A294
        lea         eax,[esp+1Ch]
        push        eax
        lea         ecx,[esp+19h]
        push        ecx
        push        edi
        mov         byte ptr [esp+21h],bl
        mov         byte ptr [esp+28h],bl
        call Relationship6Kill
        mov         al,byte ptr [esp+28h]
        add         esp,0Ch
        cmp         al,bl
        jne relationship6_61A253
        lea         edx,[esp+13h]
        push        edx
        lea         eax,[esp+1Ah]
        push        eax
        push        edi
        mov         byte ptr [esp+22h],bl
        mov         byte ptr [esp+1Fh],bl
        call Relationship6Follow
        mov         al,byte ptr [esp+1Fh]
        add         esp,0Ch
        cmp         al,bl
        je relationship6_61A267
relationship6_61A253:
        cmp         dword ptr [esi],ebx
        je relationship6_61A294
        pop         edi
        mov         byte ptr [esi+4],3
        mov         dword ptr [esi],ebp
        pop         esi
        pop         ebp
        pop         ebx
        add         esp,8
        ret         4
relationship6_61A267:
        lea         ecx,[esp+14h]
        push        ecx
        lea         edx,[esp+1Bh]
        push        edx
        push        edi
        mov         byte ptr [esp+23h],bl
        mov         byte ptr [esp+20h],bl
        call Relationship6Kill
        mov         al,byte ptr [esp+20h]
        add         esp,0Ch
        cmp         al,bl
        je relationship6_61A294
        cmp         dword ptr [esi],ebx
        je relationship6_61A294
        mov         byte ptr [esi+4],7
        mov         dword ptr [esi],ebp
relationship6_61A294:
        pop         edi
        pop         esi
        pop         ebp
        pop         ebx
        add         esp,8
        ret         4
    }
}

// 0x61A1A0
int32 CPlayerRelationshipRecorder::GetRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

        return relationship.Relationship;
    }
    return 0;
}

// 0x61A150
void CPlayerRelationshipRecorder::ClearRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

        return relationship.Flush();
    }
}

// 0x61A2E0
CPlayerRelationshipRecorder& GetPlayerRelationshipRecorder() {
    static auto& g_sPlayerRelationshipRecorder = StaticRef<CPlayerRelationshipRecorder*>(0xC17084);

    if (!g_sPlayerRelationshipRecorder) {
        g_sPlayerRelationshipRecorder = new CPlayerRelationshipRecorder();
    }
    return *g_sPlayerRelationshipRecorder;
}

void CPlayerRelationshipRecorder::InjectHooks() {
    RH_ScopedClass(CPlayerRelationshipRecorder);
    RH_ScopedCategory("Ped");
    RH_ScopedInstall(RecordRelationshipWithPlayer, 0x61A1D0);
}
