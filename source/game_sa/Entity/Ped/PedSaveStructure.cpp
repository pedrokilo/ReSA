#include "StdInc.h"
#include "PedSaveStructure.h"

void CPedSaveStructure::Extract(CPed* ped) {
    plugin::CallMethod<0x5D44B0, CPedSaveStructure*, CPed*>(this, ped);
}

__declspec(naked) void CPedSaveStructure::Construct(CPed*) {
    __asm {
        push        esi
        mov         esi,dword ptr [esp+8]
        mov         eax,dword ptr [esi+14h]
        test        eax,eax
        push        edi
        je          pedSaveConstruct_5D43E2
        add         eax,30h
        jmp         pedSaveConstruct_5D43E5
pedSaveConstruct_5D43E2:
        lea         eax,[esi+4]
pedSaveConstruct_5D43E5:
        mov         edi,dword ptr [eax]
        mov         edx,ecx
        mov         dword ptr [edx],edi
        mov         edi,dword ptr [eax+4]
        mov         dword ptr [edx+4],edi
        mov         eax,dword ptr [eax+8]
        mov         dword ptr [edx+8],eax
        mov         edx,dword ptr [esi+00000540h]
        mov         dword ptr [ecx+0Ch],edx
        mov         eax,dword ptr [esi+00000548h]
        mov         dword ptr [ecx+10h],eax
        mov         dl,byte ptr [esi+00000484h]
        mov         byte ptr [ecx+00000180h],dl
        mov         al,byte ptr [esi+00000718h]
        mov         byte ptr [ecx+00000181h],al
        mov         dl,byte ptr [esi+2Fh]
        mov         byte ptr [ecx+00000182h],dl
        mov         al,byte ptr [esi+0000072Dh]
        mov         byte ptr [ecx+00000188h],al
        mov         dl,byte ptr [esi+0000072Eh]
        mov         byte ptr [ecx+00000189h],dl
        mov         dword ptr [ecx+00000184h],0FFFFFFFFh
        mov         eax,dword ptr [esi+0000078Ch]
        test        eax,eax
        je          pedSaveConstruct_5D448D
        mov         edx,eax
        mov         edi,dword ptr [edx+38h]
        test        edi,edi
        je          pedSaveConstruct_5D4461
        mov         edx,edi
pedSaveConstruct_5D4461:
        push        ebx
        mov         bl,byte ptr [edx+32h]
        test        bl,bl
        pop         ebx
        je          pedSaveConstruct_5D448D
        mov         edx,dword ptr ds:[0096A7D8h]
        sub         eax,dword ptr [edx]
        mov         edi,eax
        mov         eax,88888889h
        imul        edi
        add         edx,edi
        sar         edx,5
        mov         eax,edx
        shr         eax,1Fh
        add         edx,eax
        mov         dword ptr [ecx+00000184h],edx
pedSaveConstruct_5D448D:
        lea         edi,[ecx+14h]
        add         esi,5A0h
        mov         ecx,5Bh
        rep movsd
        pop         edi
        pop         esi
        ret 4
    }
}

void CPedSaveStructure::InjectHooks() {
    RH_ScopedClass(CPedSaveStructure);
    RH_ScopedCategory("Entity/Ped");
    RH_ScopedInstall(Construct, 0x5D43D0);
}
