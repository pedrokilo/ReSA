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

#include "X87Math.h"

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

// 0x6171F0
// Grados Euler (XYZ) a quaternion: semiángulos h = grados·π/180/2, productos de cos/sin
// (cc=cx·cz, cs=cx·sz, sc=sx·cz, ss=sx·sz) y
//   x = sc·cy − cs·sy,  y = cc·sy + ss·cy,  z = cs·cy − sc·sy,  w = cc·cy + ss·sy.
// OJO: el original "normaliza" multiplicando por 1/(x²+y²+z²+w²), sin raíz cuadrada.
// El cálculo vive en X87Math (BoneNodeEulerToQuatCpp): fsin/fcos sin redondear y un redondeo a 24 bits por operación, como el EXE.
void BoneNode_c::EulerToQuat(const CVector& angles, RtQuat& outQuat) {
    BoneNodeEulerToQuatCpp(&angles.x, &outQuat.imag.x);
}

// 0x617080
// Quaternion a grados Euler, equivalente a:
//   s = 2·(x·z − y·w);  c = sqrt(1 − s²)
//   out.y = deg(atan2(−s, c))
//   si s == ±1 (bloqueo cardán): out.x = deg(atan2(−2·(y·z − x·w), 1 − 2·(x² + z²))), out.z = deg(atan2(0, 1))
//   si no: out.x = deg(atan2(2·(x·w + y·z)/c, (1 − 2·(x² + y²))/c)),  out.z = deg(atan2(2·(z·w + x·y)/c, (1 − 2·(y² + z²))/c))
// El cálculo vive en X87Math (BoneNodeQuatToEulerCpp): fpatan sin redondear y raíz/divisiones con un redondeo a 24 bits, como el EXE.
void BoneNode_c::QuatToEuler(const RtQuat& quat, CVector& outAngles) {
    BoneNodeQuatToEulerCpp(&quat.imag.x, &outAngles.x);
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
// sin redondear a float (registro x87); la sujeción vive en X87Math (BoneNodeLimitClampCpp) y conserva ese orden.
void BoneNode_c::Limit(float blend) {
    CVector angles;
    QuatToEuler(m_Orientation, angles);

    // El EXE compara m_BoneTag en bruto con 5 (BONE_HEAD) y busca la entrada de la cabeza en ms_boneInfos (0x8D26D0).
    const bool narrowZ = m_BoneTag == BONE_HEAD;
    const float poseZ  = narrowZ ? BoneNodeManager_c::ms_boneInfos[GetIdFromBoneTag(eBoneTag32{ BONE_HEAD })].PoseRots.z : 0.0f;
    BoneNodeLimitClampCpp(&angles.x, &m_LimitMin.x, &m_LimitMax.x, narrowZ, poseZ);

    EulerToQuat(angles, m_Orientation);
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
// Las nueve entradas de rotación se calculan en X87Math (BoneNodeLocalMatrixCpp); la posición se copia bit a bit (mov) y los
// campos de relleno de la matriz local quedan sin inicializar, como la pila del EXE.
void BoneNode_c::CalcWldMat(const RwMatrix* boneMatrix) {
    RwMatrix local;
    BoneNodeLocalMatrixCpp(&m_Orientation.imag.x, &local.right.x, &local.up.x, &local.at.x);
    local.flags = rwMATRIXTYPEORTHONORMAL; // 3
    std::memcpy(&local.pos, &m_Pos, sizeof(local.pos));
    RwMatrixMultiply(&m_WorldMat, &local, boneMatrix); // 0x7F18B0

    for (auto* child = m_Childs.GetHead(); child; child = m_Childs.GetNext(child)) { // el siguiente se lee tras volver, como el EXE
        child->CalcWldMat(&m_WorldMat);
    }
}
