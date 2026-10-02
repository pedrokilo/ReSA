/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: BoneNode_c
 * Reconstrucción: COMPLETA — 17/17 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 17/17 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "BoneNode.h"
#include "BoneNodeManager.h"

#include "rtslerp.h"

/**
 * Registra las operaciones de nodos óseos usadas por IK/ragdoll.
 */
void BoneNode_c::InjectHooks() {
    RH_ScopedClass(BoneNode_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x616B30);
    RH_ScopedInstall(Destructor, 0x616B80);

    RH_ScopedInstall(Init, 0x6177B0);
    RH_ScopedInstall(InitLimits, 0x617490);
    RH_ScopedGlobalInstall(EulerToQuat, 0x6171F0);
    RH_ScopedGlobalInstall(QuatToEuler, 0x617080);
    RH_ScopedGlobalInstall(GetIdFromBoneTag, 0x617050);
    RH_ScopedInstall(ClampLimitsCurrent, 0x6175D0);
    RH_ScopedInstall(ClampLimitsDefault, 0x617530);
    RH_ScopedInstall(Limit, 0x617650);
    RH_ScopedInstall(BlendKeyframe, 0x616E30);
    RH_ScopedInstall(GetSpeed, 0x616CB0);
    RH_ScopedInstall(SetSpeed, 0x616CC0);
    RH_ScopedInstall(SetLimits, 0x616C50);
    RH_ScopedInstall(GetLimits, 0x616BF0);
    RH_ScopedInstall(AddChild, 0x616BD0);
    RH_ScopedInstall(CalcWldMat, 0x616CD0);
}

// 0x6177B0
/**
 * Vincula el nodo con un frame de animación y captura su pose local inicial.
 *
 * También reinicia jerarquía, límites y velocidad; m_InterpFrame permanece como
 * referencia externa al estado de animación del ped.
 */
bool BoneNode_c::Init(eBoneTag32 boneTag, RpHAnimBlendInterpFrame* interpFrame) {
    m_BoneTag     = boneTag;
    m_InterpFrame = interpFrame;
    m_Orientation = interpFrame->q;
    m_Pos         = interpFrame->t;
    m_Parent      = nullptr;

    m_Childs.RemoveAll();
    InitLimits();

    m_Speed = 1.0f;

    return true;
}

// 0x617490
// Deriva los límites Euler absolutos sumando/restando tolerancias a la pose base del bone.
void BoneNode_c::InitLimits() {
    const auto& bone = BoneNodeManager_c::ms_boneInfos[GetIdFromBoneTag(GetBoneTag())];
    m_LimitMin = bone.PoseRots - CVector{
        bone.MinX,
        bone.MinY,
        bone.MinZ
    };
    m_LimitMax = bone.PoseRots + CVector{
        bone.MaxX,
        bone.MaxY,
        bone.MaxZ
    };
}

// Transcripción literal (x87) de 0x6171F0 y 0x617080. Como en CGeneral::GetRandomNumberInRange<float>,
// el x87 es intencionado: fsin/fcos/fpatan/fsqrt y los intermedios de 80 bits con el control de
// precisión del juego dan resultados que ninguna expresión SSE/double reproduce bit a bit.
namespace {
const float  s_X87Pi       = 3.14159274f;
const float  s_X87Inv180   = 1.0f / 180.0f;
const float  s_X87Half     = 0.5f;
const float  s_X87One      = 1.0f;
const float  s_X87180      = 180.0f;
const float  s_X87InvPi    = 0.318309873f;
const float  s_X87MinusOne = -1.0f;
const double s_X87ZeroD    = 0.0;
const double s_X87OneD     = 1.0;
const float  s_X87Inv45    = 1.0f / 45.0f;
const float  s_X87Zero     = 0.0f;
const float  s_X87Two      = 2.0f;

// void(const CVector* angles, RtQuat* out)
__declspec(naked) void EulerToQuatX87(const CVector*, RtQuat*) {
    __asm {
        sub esp,10h
        mov eax,dword ptr [esp+14h]
        fld dword ptr [eax]
        fmul dword ptr [s_X87Pi]
        fmul dword ptr [s_X87Inv180]
        fmul dword ptr [s_X87Half]
        fld dword ptr [eax+4]
        fmul dword ptr [s_X87Pi]
        fmul dword ptr [s_X87Inv180]
        fmul dword ptr [s_X87Half]
        fld dword ptr [eax+8]
        fmul dword ptr [s_X87Pi]
        fmul dword ptr [s_X87Inv180]
        fmul dword ptr [s_X87Half]
        fstp dword ptr [esp+8]
        fld st(1)
        fcos
        fstp dword ptr [esp+4]
        fld st(0)
        fcos
        fstp dword ptr [esp+14h]
        fld dword ptr [esp+8]
        fcos
        fstp dword ptr [esp]
        fxch st(1)
        fsin
        fstp dword ptr [esp+0Ch]
        fsin
        fld dword ptr [esp+8]
        fsin
        fld dword ptr [esp]
        fmul dword ptr [esp+4]
        fstp dword ptr [esp+8]
        fld dword ptr [esp+4]
        fmul st,st(1)
        fld dword ptr [esp+0Ch]
        fmul dword ptr [esp]
        fxch st(2)
        fmul dword ptr [esp+0Ch]
        fstp dword ptr [esp+0Ch]
        fld dword ptr [esp+14h]
        fmul st,st(2)
        fld st(1)
        fmul st,st(4)
        fsubp st(1),st
        fstp dword ptr [esp]
        fld dword ptr [esp+8]
        fmul st,st(3)
        fld dword ptr [esp+0Ch]
        fmul dword ptr [esp+14h]
        faddp st(1),st
        fstp dword ptr [esp+4]
        fmul dword ptr [esp+14h]
        fxch st(1)
        fmul st,st(2)
        fsubp st(1),st
        fld dword ptr [esp+0Ch]
        fmul st,st(2)
        fld dword ptr [esp+8]
        fmul dword ptr [esp+14h]
        faddp st(1),st
        fst dword ptr [esp+8]
        fmul dword ptr [esp+8]
        mov eax,dword ptr [esp+18h]
        fld st(1)
        fmul st,st(2)
        faddp st(1),st
        fld dword ptr [esp+4]
        fmul dword ptr [esp+4]
        faddp st(1),st
        fld dword ptr [esp]
        fmul dword ptr [esp]
        faddp st(1),st
        fld dword ptr [s_X87One]
        fdiv st,st(1)
        fstp dword ptr [esp+14h]
        fstp st(0)
        fld dword ptr [esp+14h]
        fmul dword ptr [esp]
        fstp dword ptr [eax]
        fld dword ptr [esp+14h]
        fmul dword ptr [esp+4]
        fstp dword ptr [eax+4]
        fld dword ptr [esp+14h]
        fmul st,st(1)
        fstp dword ptr [eax+8]
        fstp st(0)
        fstp st(0)
        fld dword ptr [esp+14h]
        fmul dword ptr [esp+8]
        fstp dword ptr [eax+0Ch]
        add esp,10h
        ret
    }
}

// void(const RtQuat* quat, CVector* out)
__declspec(naked) void QuatToEulerX87(const RtQuat*, CVector*) {
    __asm {
        sub esp,24h
        mov ecx,dword ptr [esp+28h]
        fld dword ptr [ecx+4]
        mov edx,dword ptr [esp+2Ch]
        fld st(0)
        fmulp st(1),st
        fadd st(0),st
        fld dword ptr [ecx+8]
        fld dword ptr [s_X87One]
        fsub st,st(2)
        fld st(1)
        fmul st,st(2)
        fadd st(0),st
        fsubp st(1),st
        fstp dword ptr [esp]
        fstp st(0)
        fld dword ptr [ecx+8]
        fmul dword ptr [ecx+0Ch]
        fadd st(0),st
        fld dword ptr [ecx]
        fmul dword ptr [ecx+4]
        fadd st(0),st
        faddp st(1),st
        fstp dword ptr [esp+0Ch]
        fld dword ptr [ecx+8]
        fmul dword ptr [ecx]
        fadd st(0),st
        fld dword ptr [ecx+0Ch]
        fmul dword ptr [ecx+4]
        fadd st(0),st
        fsubp st(1),st
        fld dword ptr [ecx+8]
        fmul dword ptr [ecx+4]
        fadd st(0),st
        fld dword ptr [ecx+0Ch]
        fmul dword ptr [ecx]
        fadd st(0),st
        faddp st(1),st
        fstp dword ptr [esp+1Ch]
        fld dword ptr [ecx]
        fld st(0)
        fmul st,st(1)
        fadd st(0),st
        fsubr dword ptr [s_X87One]
        fsub st,st(3)
        fstp dword ptr [esp+20h]
        fstp st(0)
        fchs
        fstp st(1)
        fld st(0)
        fmul st,st(1)
        fsubr dword ptr [s_X87One]
        fsqrt
        fstp dword ptr [esp+28h]
        fld st(0)
        fld dword ptr [esp+28h]
        fpatan
        fmul dword ptr [s_X87180]
        fmul dword ptr [s_X87InvPi]
        fstp dword ptr [edx+4]
        fcom dword ptr [s_X87One]
        fnstsw ax
        test ah,44h
        jnp L0061718A
        fcomp dword ptr [s_X87MinusOne]
        fnstsw ax
        test ah,44h
        jnp L0061718C
        fld dword ptr [s_X87One]
        fdiv dword ptr [esp+28h]
        fld dword ptr [esp+1Ch]
        fmul st,st(1)
        fld st(1)
        fmul dword ptr [esp+20h]
        fpatan
        fmul dword ptr [s_X87180]
        fmul dword ptr [s_X87InvPi]
        fstp dword ptr [edx]
        fld dword ptr [esp+0Ch]
        fmul st,st(1)
        fxch st(1)
        fmul dword ptr [esp]
        fpatan
        fmul dword ptr [s_X87180]
        fmul dword ptr [s_X87InvPi]
        fstp dword ptr [edx+8]
        add esp,24h
        ret
    L0061718A:
        fstp st(0)
    L0061718C:
        fld dword ptr [ecx]
        fld dword ptr [ecx+8]
        fld st(1)
        fmulp st(2),st
        fxch st(1)
        fadd st(0),st
        fsubr dword ptr [s_X87One]
        fld st(1)
        fmul st,st(2)
        fadd st(0),st
        fsubp st(1),st
        fstp st(1)
        fld dword ptr [ecx+8]
        fmul dword ptr [ecx+4]
        fadd st(0),st
        fld dword ptr [ecx+0Ch]
        fmul dword ptr [ecx]
        fadd st(0),st
        fsubp st(1),st
        fchs
        fxch st(1)
        fpatan
        fmul dword ptr [s_X87180]
        fmul dword ptr [s_X87InvPi]
        fstp dword ptr [edx]
        fld qword ptr [s_X87ZeroD]
        fld qword ptr [s_X87OneD]
        fpatan
        fmul dword ptr [s_X87180]
        fmul dword ptr [s_X87InvPi]
        fstp dword ptr [edx+8]
        add esp,24h
        ret
    }
}

// Cuerpo de BoneNode_c::Limit (thiscall, ret 4): __fastcall con edx ignorado tiene la misma convención.
__declspec(naked) void __fastcall LimitX87(BoneNode_c*, void*, float) {
    __asm {
        sub esp,38h
        push ebx
        push esi
        mov esi,ecx
        lea eax,[esp+0Ch]
        push eax
        lea ebx,[esi+10h]
        push ebx
        call QuatToEulerX87
        fld dword ptr [esp+14h]
        fcomp dword ptr [esi+7Ch]
        add esp,8
        fnstsw ax
        test ah,5
        jp L0061767D
        mov ecx,dword ptr [esi+7Ch]
        mov dword ptr [esp+0Ch],ecx
    L0061767D:
        fld dword ptr [esp+0Ch]
        fcomp dword ptr [esi+00000088h]
        fnstsw ax
        test ah,41h
        jne L00617698
        mov edx,dword ptr [esi+00000088h]
        mov dword ptr [esp+0Ch],edx
    L00617698:
        fld dword ptr [esi+00000080h]
        mov eax,dword ptr [esi+0000008Ch]
        fld dword ptr [esp+10h]
        mov dword ptr [esp+8],eax
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp L006176BB
        fstp dword ptr [esp+10h]
        jmp L006176BD
    L006176BB:
        fstp st(0)
    L006176BD:
        fld dword ptr [esp+10h]
        fcomp dword ptr [esp+8]
        fnstsw ax
        test ah,41h
        jne L006176D4
        mov ecx,dword ptr [esp+8]
        mov dword ptr [esp+10h],ecx
    L006176D4:
        mov eax,dword ptr [esi+8]
        fld dword ptr [esi+00000084h]
        cmp eax,5
        mov edx,dword ptr [esi+00000090h]
        mov dword ptr [esp+8],edx
        jne L00617769
        xor ecx,ecx
        mov eax,8D26D0h
    L006176F3:
        cmp word ptr [eax],5
        je L00617707
        add eax,28h
        inc ecx
        cmp eax,8D2BD0h
        jl L006176F3
        or ecx,0FFFFFFFFh
    L00617707:
        fld dword ptr [esp+0Ch]
        lea esi,[ecx+ecx*4]
        fabs
        push edi
        fmul dword ptr [s_X87Inv45]
        lea esi,[esi*8+8D26D0h]
        mov ecx,0Ah
        fsubr dword ptr [s_X87One]
        lea edi,[esp+1Ch]
        fld dword ptr [s_X87Zero]
        rep movsd
        fcomp st(1)
        fnstsw ax
        test ah,41h
        pop edi
        jne L00617747
        fstp st(0)
        fld dword ptr [s_X87Zero]
    L00617747:
        fxch st(1)
        fsub dword ptr [esp+24h]
        fmul st,st(1)
        fadd dword ptr [esp+24h]
        fxch st(1)
        fld dword ptr [esp+8]
        fsub dword ptr [esp+24h]
        fmul st,st(1)
        fadd dword ptr [esp+24h]
        fstp dword ptr [esp+8]
        fstp st(0)
    L00617769:
        fld dword ptr [esp+14h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp L0061777C
        fstp dword ptr [esp+14h]
        jmp L0061777E
    L0061777C:
        fstp st(0)
    L0061777E:
        fld dword ptr [esp+14h]
        fcomp dword ptr [esp+8]
        fnstsw ax
        test ah,41h
        jne L00617795
        mov eax,dword ptr [esp+8]
        mov dword ptr [esp+14h],eax
    L00617795:
        push ebx
        lea ecx,[esp+10h]
        push ecx
        call EulerToQuatX87
        add esp,8
        pop esi
        pop ebx
        add esp,38h
        ret 4
        nop
    }
}

// Cuerpo de BoneNode_c::CalcWldMat (thiscall, ret 4). Se recurre a sí misma para los hijos, como el
// original a 0x616CD0, y multiplica con RwMatrixMultiply (0x7F18B0).
__declspec(naked) void __fastcall CalcWldMatX87(BoneNode_c*, void*, const RwMatrix*) {
    __asm {
        sub esp,70h
        push esi
        mov esi,ecx
        fld dword ptr [esi+14h]
        push edi
        fld dword ptr [esi+10h]
        fld dword ptr [esi+18h]
        fld dword ptr [esi+1Ch]
        fld st(2)
        fmul st,st(3)
        fld st(4)
        fmul st,st(5)
        faddp st(1),st
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(1)
        fmul st,st(2)
        faddp st(1),st
        fdivr dword ptr [s_X87Two]
        fstp st(4)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fld st(0)
        fmul dword ptr [esi+10h]
        fld st(1)
        fmul dword ptr [esi+14h]
        fstp dword ptr [esp+24h]
        fxch st(1)
        fmul dword ptr [esi+18h]
        fld st(1)
        fmul dword ptr [esi+1Ch]
        fstp dword ptr [esp+8]
        fld dword ptr [esp+24h]
        fmul dword ptr [esi+1Ch]
        fstp dword ptr [esp+0Ch]
        fld st(0)
        fmul dword ptr [esi+1Ch]
        fstp dword ptr [esp+10h]
        fld st(1)
        fmul dword ptr [esi+10h]
        fstp dword ptr [esp+14h]
        fld dword ptr [esp+24h]
        fmul dword ptr [esi+14h]
        fstp dword ptr [esp+18h]
        fld st(0)
        fmul dword ptr [esi+18h]
        fstp dword ptr [esp+1Ch]
        fmul dword ptr [esi+14h]
        fstp dword ptr [esp+2Ch]
        fmul dword ptr [esi+18h]
        fld dword ptr [esp+24h]
        fmul dword ptr [esi+10h]
        fld dword ptr [esp+1Ch]
        fadd dword ptr [esp+18h]
        fsubr dword ptr [s_X87One]
        fstp dword ptr [esp+38h]
        fld dword ptr [esp+10h]
        fadd st,st(1)
        fstp dword ptr [esp+3Ch]
        fld st(1)
        fsub dword ptr [esp+0Ch]
        fstp dword ptr [esp+40h]
        fsub dword ptr [esp+10h]
        lea eax,[esi+20h]
        mov ecx,dword ptr [eax]
        mov edx,dword ptr [eax+4]
        fstp dword ptr [esp+48h]
        mov eax,dword ptr [eax+8]
        fld dword ptr [esp+1Ch]
        mov dword ptr [esp+68h],ecx
        fadd dword ptr [esp+14h]
        mov ecx,dword ptr [esp+7Ch]
        mov dword ptr [esp+6Ch],edx
        push ecx
        fsubr dword ptr [s_X87One]
        lea edx,[esp+3Ch]
        push edx
        fstp dword ptr [esp+54h]
        lea edi,[esi+3Ch]
        fld dword ptr [esp+34h]
        push edi
        fadd dword ptr [esp+14h]
        mov dword ptr [esp+50h],3
        mov dword ptr [esp+7Ch],eax
        fstp dword ptr [esp+5Ch]
        fadd dword ptr [esp+18h]
        fstp dword ptr [esp+64h]
        fld dword ptr [esp+38h]
        fsub dword ptr [esp+14h]
        fstp dword ptr [esp+68h]
        fld dword ptr [esp+24h]
        fadd dword ptr [esp+20h]
        fsubr dword ptr [s_X87One]
        fstp dword ptr [esp+6Ch]
        mov eax,7F18B0h
        call eax
        mov esi,dword ptr [esi+30h]
        add esp,0Ch
        test esi,esi
        je L00616E22
    L00616E13:
        push edi
        mov ecx,esi
        call CalcWldMatX87
        mov esi,dword ptr [esi+4]
        test esi,esi
        jne L00616E13
    L00616E22:
        pop edi
        pop esi
        add esp,70h
        ret 4
        nop
    }
}
} // namespace

// 0x6171F0
// Grados Euler (XYZ) a quaternion: semiángulos h = grados·π/180/2, productos de cos/sin
// (cc=cx·cz, cs=cx·sz, sc=sx·cz, ss=sx·sz) y
//   x = sc·cy − cs·sy,  y = cc·sy + ss·cy,  z = cs·cy − sc·sy,  w = cc·cy + ss·sy.
// OJO: el original "normaliza" multiplicando por 1/(x²+y²+z²+w²), sin raíz cuadrada.
void BoneNode_c::EulerToQuat(const CVector& angles, RtQuat& outQuat) {
    EulerToQuatX87(&angles, &outQuat);
}

// 0x617080
// Quaternion a grados Euler, equivalente a:
//   s = 2·(x·z − y·w);  c = sqrt(1 − s²)
//   out.y = deg(atan2(−s, c))
//   si s == ±1 (bloqueo cardán): out.x = deg(atan2(−2·(y·z − x·w), 1 − 2·(x² + z²))), out.z = deg(atan2(0, 1))
//   si no: out.x = deg(atan2(2·(x·w + y·z)/c, (1 − 2·(x² + y²))/c)),  out.z = deg(atan2(2·(z·w + x·y)/c, (1 − 2·(y² + z²))/c))
void BoneNode_c::QuatToEuler(const RtQuat& quat, CVector& outAngles) {
    QuatToEulerX87(&quat, &outAngles);
}

// inline
// 0x617050
inline int32 BoneNode_c::GetIdFromBoneTag(eBoneTag32 tag) {
    for (auto&& [i, info] : rngv::enumerate(BoneNodeManager_c::ms_boneInfos)) {
        if (info.BoneTag == tag) {
            return (eBoneTag)(i);
        }
    }
    return BONE_UNKNOWN;
}

// Empty in Android
// 0x6175D0
void BoneNode_c::ClampLimitsCurrent(bool limitX, bool limitY, bool limitZ) {
    static auto& s_TestSkipClampCurrent = StaticRef<bool>(0x8D2BD1); // true

    if (!s_TestSkipClampCurrent) {
        CVector current;
        QuatToEuler(m_Orientation, current);
        if (limitX) {
            m_LimitMin.x = m_LimitMax.x = current.x;
        }
        if (limitY) {
            m_LimitMin.y = m_LimitMax.y = current.y;
        }
        if (limitZ) {
            m_LimitMin.z = m_LimitMax.z = current.z;
        }
    }
}

// Empty in Android
// 0x617530
void BoneNode_c::ClampLimitsDefault(bool limitX, bool limitY, bool limitZ) {
    static auto& s_TestSkipClampDefault = StaticRef<bool>(0x8D2BD0); // true

    if (!s_TestSkipClampDefault) {
        if (const auto* info = GetBoneInfo()) { // BUGFIX: Check if info is available
            if (limitX) {
                m_LimitMin.x = m_LimitMax.x = info->PoseRots.x;
            }
            if (limitY) {
                m_LimitMin.y = m_LimitMax.y = info->PoseRots.y;
            }
            if (limitZ) {
                m_LimitMin.z = m_LimitMax.z = info->PoseRots.z;
            }
        }
    }
}

// argument (float blend) - ignored
// 0x617650
// Pasa la orientación a Euler, la sujeta a [m_LimitMin, m_LimitMax] por eje (primero el mínimo y
// después el máximo, como el original) y vuelve a quaternion. Para BONE_HEAD el rango Z se estrecha
// hacia la pose de ms_boneInfos con el factor max(1 − |x|/45, 0); el mínimo resultante se compara
// sin redondear a float (registro x87), por eso se conserva la transcripción literal.
void BoneNode_c::Limit(float blend) {
    LimitX87(this, nullptr, blend);
}

// 0x616E30
// Mezcla mediante Slerp la orientación original del frame con la orientación IK calculada.
void BoneNode_c::BlendKeyframe(float blend) {
    auto src = m_InterpFrame->q;
    auto dst = m_Orientation;
    RtQuatSlerpCache cache;
    RtQuatSetupSlerpCache(&src, &dst, &cache);
    RtQuatSlerp(&m_InterpFrame->q, &src, &dst, blend, &cache);
}

// notsa
const BoneInfo_t* BoneNode_c::GetBoneInfo() {
    return BoneNodeManager_c::GetBoneInfoFromTag(GetBoneTag());
}

// 0x616CB0
float BoneNode_c::GetSpeed() const {
    return m_Speed;
}

// 0x616CC0
void BoneNode_c::SetSpeed(float speed) {
    m_Speed = speed;
}

// 0x616C50
void BoneNode_c::SetLimits(eRotationAxis axis, float min, float max) {
    m_LimitMin[+axis] = min;
    m_LimitMax[+axis] = max;
}

// 0x616BF0
void BoneNode_c::GetLimits(eRotationAxis axis, float& outMin, float& outMax) const {
    outMin = m_LimitMin[+axis];
    outMax = m_LimitMax[+axis];
}

// 0x616BD0
// Establece la relación padre-hijo y enlaza el hijo en la lista intrusiva local.
void BoneNode_c::AddChild(BoneNode_c* children) {
    children->m_Parent = this;
    m_Childs.AddItem(children);
}

// 0x616CD0
/**
 * Compone la transformación local con la matriz del padre y propaga recursivamente
 * el resultado a todos los hijos, formando la cadena de matrices world de IK.
 */
// R se construye en línea a partir del quaternion con s = 2/(x²+y²+z²+w²) (1−s(y²+z²), s(xy+zw),
// s(xz−yw)...), marcada ortonormal (flags 3); la transcripción literal conserva el redondeo exacto.
void BoneNode_c::CalcWldMat(const RwMatrix* boneMatrix) {
    CalcWldMatX87(this, nullptr, boneMatrix);
}
