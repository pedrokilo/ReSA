/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CQuaternion
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Quaternion.h"
#include "X87Math.h"

void CQuaternion::InjectHooks() {
    RH_ScopedClass(CQuaternion);
    RH_ScopedCategory("Core");

    RH_ScopedOverloadedInstall(Get, "matrix", 0x59C080, void (CQuaternion::*)(RwMatrix*) const);
    RH_ScopedOverloadedInstall(Get, "axis-angle", 0x59C230, void (CQuaternion::*)(RwV3d*, float*));
    RH_ScopedOverloadedInstall(Set, "matrix", 0x59C3E0, void (CQuaternion::*)(const RwMatrix&));
    RH_ScopedOverloadedInstall(Set, "axis-angle", 0x59C600, void (CQuaternion::*)(RwV3d*, float));
    RH_ScopedOverloadedInstall(Get, "euler", 0x59C160, void (CQuaternion::*)(float*, float*, float*));
    RH_ScopedInstall(Multiply, 0x59C270);
    RH_ScopedOverloadedInstall(Set, "euler", 0x59C530, void (CQuaternion::*)(float, float, float));
    RH_ScopedInstall(Conjugate, 0x4D37D0);
    RH_ScopedOverloadedInstall(Slerp, "halftheta", 0x59C300, void (CQuaternion::*)(const CQuaternion&, const CQuaternion&, float, float, float));
    RH_ScopedOverloadedInstall(Slerp, "t", 0x59C630, void (CQuaternion::*)(const CQuaternion&, const CQuaternion&, float));
}

// 0x59C080: quat to matrix. Escribe sólo right (+0x00), up (+0x10) y at (+0x20) del RwMatrix; la aritmética x87 exacta del EXE (2z guardado como float, 2x y 2y en registros,
// 2y·w y 2z·w sin redondear a float) vive en el módulo compartido X87Math (RpNanQuatToMatrixCpp), compilado con /arch:IA32.
void CQuaternion::Get(RwMatrix* out) const {
    RpNanQuatToMatrixCpp(&x, reinterpret_cast<float*>(out));
}

// Quat to euler angles (0x59C160): Get(RwMatrix*) sobre una matriz local y tres atan2 x87 de 24 bits (QuatEulerGetCpp, X87Math); escribe z, x, y en este orden y suma 2*pi a los negativos.
void CQuaternion::Get(float* x, float* y, float* z) {
    RwMatrix matrix;
    Get(&matrix);
    QuatEulerGetCpp(reinterpret_cast<const float*>(&matrix), x, y, z);
}

// Quat to axis & angle (0x59C230): *angle = float(acos(2w)) con el _CIacos del EXE (0x822380; dominio |2w| <= 1, fuera de el NaN indefinido y errno = 33) y
// axis = {x, y, z} / sin(acos(2w)), con 1/sin sin redondear a float; secuencia x87 de 24 bits en la maquina FlcX87 (QuaternionGetAxisAngleCpp, X87Math).
void CQuaternion::Get(RwV3d* axis, float* angle) {
    QuaternionGetAxisAngleCpp(&x, &axis->x, angle);
}

// Stores result of quat multiplication (0x59C270): producto con la x87 de 24 bits del EXE; out puede solaparse con a o b (QuatMultiplyCpp, X87Math).
void CQuaternion::Multiply(const CQuaternion& a, const CQuaternion& b) {
    QuatMultiplyCpp(&x, &a.x, &b.x);
}

// Spherical linear interpolation (0x59C300): contrato thiscall propio de GTA (no es RtQuatSetupSlerpCache de RenderWare) con el angulo y 1/sin(angulo) ya precalculados
// (CAnimBlendNode::CalcTheta, 0x4D00E0; lo llama CAnimBlendNode::Update, 0x4D06C0). theta == 0 copia `to`; si theta > pi/2 usa pi - theta y el segundo peso cambia de signo; el resto es
// {sin((1 - t)*theta)*inv}*from + {+/-sin(theta*t)*inv}*to con la aritmetica x87 de 24 bits y los senos sin redondear (QuaternionSlerpCpp, X87Math).
void CQuaternion::Slerp(const CQuaternion& from, const CQuaternion& to, float halftheta, float sintheta_inv, float t) {
    QuaternionSlerpCpp(&x, &from.x, &to.x, halftheta, sintheta_inv, t);
}

// Quat from matrix (0x59C3E0). Cuatro ramas segun la traza; cada operacion x87 del EXE (24 bits) es una operacion float, en su
// orden, y las lecturas de la matriz y escrituras del cuaternion se intercalan como en el EXE (la salida puede solaparse con la matriz).
#pragma float_control(precise, on, push)
void CQuaternion::Set(const RwMatrix& m) {
    const float zero = 0.0f, half = 0.5f, one = 1.0f; // 0x858B50, 0x858B8C, 0x858624
    // fcom + "test ah,1": un NaN (desordenado) o negativo sigue a la rama siguiente.
    const float trace = (m.up.y + m.right.x) + m.at.z;
    if (trace >= zero) {
        const float root = std::sqrt(trace + one);
        w = half * root;
        const float inv = half / root;
        x = (m.up.z - m.at.y) * inv;
        y = (m.at.x - m.right.z) * inv;
        z = (m.right.y - m.up.x) * inv;
        return;
    }
    const float dx = (m.right.x - m.up.y) - m.at.z;
    if (dx >= zero) {
        const float root = std::sqrt(dx + one);
        x = half * root;
        const float inv = half / root;
        y = (m.up.x + m.right.y) * inv;
        z = (m.at.x + m.right.z) * inv;
        w = (m.up.z - m.at.y) * inv;
        return;
    }
    const float dy = (m.up.y - m.right.x) - m.at.z;
    if (dy >= zero) {
        const float root = std::sqrt(dy + one);
        y = half * root;
        const float inv = half / root;
        w = (m.at.x - m.right.z) * inv;
        x = (m.up.x - m.right.y) * inv;
        z = (m.at.y + m.up.z) * inv;
        return;
    }
    // Ultima rama sin comparacion (fsubr): m22 - (m11 + m00); la raiz de un negativo da el NaN indefinido igual que fsqrt.
    const float dz = m.at.z - (m.up.y + m.right.x);
    const float root = std::sqrt(dz + one);
    z = half * root;
    const float inv = half / root;
    w = (m.right.y - m.up.x) * inv;
    x = (m.at.x + m.right.z) * inv;
    y = (m.at.y + m.up.z) * inv;
}
#pragma float_control(pop)

// Quat from euler angles (0x59C530): medios angulos {x, y, z} * 0.5 (0x858B8C); cos de cada uno sin redondear (el de z se guarda como float), sin de cada uno guardado como float,
// productos y sumas a 24 bits con la pila x87 del EXE (QuaternionFromEulerCpp, X87Math). Escribe w, x, y, z en ese orden.
void CQuaternion::Set(float x, float y, float z) {
    QuaternionFromEulerCpp(&this->x, x, y, z);
}

// Quat from axis & angle (0x59C600): {sin(a/2) * eje, cos(a/2)} con el seno sin redondear (QuaternionFromAxisAngleCpp, X87Math).
void CQuaternion::Set(RwV3d* axis, float angle) {
    QuaternionFromAxisAngleCpp(&x, &axis->x, angle);
}

// Spherical linear interpolation (0x59C630): CalcTheta (0x4D00E0, con el _CIacos del EXE) y luego la version de cinco argumentos (0x59C300) con (theta, 1/sin(theta), t)
// (QuatSlerpTCpp, X87Math).
void CQuaternion::Slerp(const CQuaternion& from, const CQuaternion& to, float t) {
    QuatSlerpTCpp(&x, &from.x, &to.x, t);
}

// Conjugate of a quat (0x4D37D0): x, y y z cambian de signo y w no se toca. El EXE lo hace con fld dword / fchs / fstp dword por componente, de modo que un sNaN sale como qNaN
// (bit 22 puesto, con el signo invertido igual que cualquier otro NaN); el resto de valores (cero, subnormal, inf, qNaN) solo cambia de signo. Se reproduce con enteros, sin tocar el x87.
void CQuaternion::Conjugate() {
    const auto negateLikeX87 = [](float& v) {
        auto bits = std::bit_cast<uint32>(v);
        if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0u) {
            bits |= 0x00400000u; // fld convierte el sNaN en qNaN
        }
        v = std::bit_cast<float>(bits ^ 0x80000000u); // fchs
    };
    negateLikeX87(x);
    negateLikeX87(y);
    negateLikeX87(z);
}

// Squared length of a quat
float CQuaternion::GetLengthSquared() const {
    // Originally NOP.
    return sq(x) + sq(y) + sq(z) + sq(w);
}

// Multiplies quat by a floating point value
void CQuaternion::Scale(float multiplier) {
    ((void(__thiscall*)(CQuaternion*, float))0x4CF9B0)(this, multiplier);
}

// Copies value from other quat
void CQuaternion::Copy(const CQuaternion& from) {
    ((void(__thiscall*)(CQuaternion*, const CQuaternion&))0x4CF9E0)(this, from);
}

// Gets a dot product for quats
float CQuaternion::Dot(const CQuaternion& rhs) {
    return this->w * rhs.w + this->z * rhs.z + this->y * rhs.y + this->x * rhs.x;
}

// Normalises a quat
void CQuaternion::Normalise() {
    const auto sqMag = GetLengthSquared();
    if (sqMag == 0.f) {
        w = 1.0;
    } else {
        *this = *this / std::sqrt(sqMag);
    }
}
