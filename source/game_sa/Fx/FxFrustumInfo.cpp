#include "StdInc.h"
#include "FxFrustumInfo.h"

void FxFrustumInfo_c::InjectHooks() {
    RH_ScopedClass(FxFrustumInfo_c);
    RH_ScopedCategory("Fx");
    RH_ScopedInstall(IsCollision, 0x4AA030);
}

// Circular order starts at the previously rejected plane; success does not change it.
// Exact x87 comparisons retain unordered and equality branches from the EXE.
__declspec(naked) bool FxFrustumInfo_c::IsCollision(FxSphere_c&) {
    __asm {
    mov edx,dword ptr [esp+4] // EXE 0x004AA030
    fld dword ptr [edx] // EXE 0x004AA034
    fsub dword ptr [ecx] // EXE 0x004AA036
    fld dword ptr [ecx+4] // EXE 0x004AA038
    fsubr dword ptr [edx+4] // EXE 0x004AA03B
    fld dword ptr [ecx+8] // EXE 0x004AA03E
    fsubr dword ptr [edx+8] // EXE 0x004AA041
    fld dword ptr [ecx+0Ch] // EXE 0x004AA044
    fadd dword ptr [edx+0Ch] // EXE 0x004AA047
    fld st(1) // EXE 0x004AA04A
    fmul st,st(2) // EXE 0x004AA04C
    fld st(3) // EXE 0x004AA04E
    fmul st,st(4) // EXE 0x004AA050
    faddp st(1),st // EXE 0x004AA052
    fld st(4) // EXE 0x004AA054
    fmul st,st(5) // EXE 0x004AA056
    faddp st(1),st // EXE 0x004AA058
    fld st(1) // EXE 0x004AA05A
    fmul st,st(2) // EXE 0x004AA05C
    fcompp // EXE 0x004AA05E
    fstp st(0) // EXE 0x004AA060
    fstp st(0) // EXE 0x004AA062
    fnstsw ax // EXE 0x004AA064
    fstp st(0) // EXE 0x004AA066
    test ah,41h // EXE 0x004AA068
    fstp st(0) // EXE 0x004AA06B
    je L004AA074 // EXE 0x004AA06D
    xor eax,eax // EXE 0x004AA06F
    ret 4 // EXE 0x004AA071
    L004AA074: push ebx
    push esi // EXE 0x004AA075
    push edi // EXE 0x004AA076
    mov edi,dword ptr [edx+10h] // EXE 0x004AA077
    xor ebx,ebx // EXE 0x004AA07A
    lea esp,[esp] // EXE 0x004AA07C
    L004AA080: mov esi,edi
    and esi,3 // EXE 0x004AA082
    mov eax,esi // EXE 0x004AA085
    shl eax,4 // EXE 0x004AA087
    fld dword ptr [eax+ecx+18h] // EXE 0x004AA08A
    lea eax,[eax+ecx+14h] // EXE 0x004AA08E
    fmul dword ptr [edx+4] // EXE 0x004AA092
    fld dword ptr [eax] // EXE 0x004AA095
    fmul dword ptr [edx] // EXE 0x004AA097
    faddp st(1),st // EXE 0x004AA099
    fld dword ptr [eax+8] // EXE 0x004AA09B
    fmul dword ptr [edx+8] // EXE 0x004AA09E
    faddp st(1),st // EXE 0x004AA0A1
    fsub dword ptr [eax+0Ch] // EXE 0x004AA0A3
    fcomp dword ptr [edx+0Ch] // EXE 0x004AA0A6
    fnstsw ax // EXE 0x004AA0A9
    test ah,41h // EXE 0x004AA0AB
    je L004AA0C2 // EXE 0x004AA0AE
    inc ebx // EXE 0x004AA0B0
    inc edi // EXE 0x004AA0B1
    cmp ebx,4 // EXE 0x004AA0B2
    jl L004AA080 // EXE 0x004AA0B5
    pop edi // EXE 0x004AA0B7
    pop esi // EXE 0x004AA0B8
    mov eax,1 // EXE 0x004AA0B9
    pop ebx // EXE 0x004AA0BE
    ret 4 // EXE 0x004AA0BF
    L004AA0C2: pop edi
    mov dword ptr [edx+10h],esi // EXE 0x004AA0C3
    pop esi // EXE 0x004AA0C6
    xor eax,eax // EXE 0x004AA0C7
    pop ebx // EXE 0x004AA0C9
    ret 4 // EXE 0x004AA0CA
    }
}
