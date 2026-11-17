/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RtQuat
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rtquat.h"
#include "X87Math.h"

// Lote R5A (sin validar). RtQuatConvertFromMatrix (0x7EB5C0) y sus tres ramas (0x7EB6A0, 0x7EB700, 0x7EB760), RtQuatRotate (0x7EB7C0) y RtQuatTransformVectors (0x7EBBB0) en C++.
// Las matematicas (x87 a 24 bits) estan en los ayudantes RtQuat*Cpp de X87Math.cpp; aqui van los saltos, las llamadas y los guardados que el EXE hace entre operaciones.
// RtQuatQueryRotate (0x7EBA80) y RtQuatModulus (0x7EBD10) siguen siendo envolturas (fuera del lote).

static_assert(sizeof(RtQuat) == 0x10 && offsetof(RtQuat, real) == 0xC);
static_assert(offsetof(RwMatrix, right) == 0x00 && offsetof(RwMatrix, up) == 0x10 && offsetof(RwMatrix, at) == 0x20);

// Ramas internas de RtQuatConvertFromMatrix (funciones propias del EXE, sin declaracion en el SDK).
void RtQuatConvertFromMatrixAxisX(RtQuat* qpQuat, const RwMatrix* mpMatrix);
void RtQuatConvertFromMatrixAxisY(RtQuat* qpQuat, const RwMatrix* mpMatrix);
void RtQuatConvertFromMatrixAxisZ(RtQuat* qpQuat, const RwMatrix* mpMatrix);

void RtQuatInjectHooks() {
    RH_ScopedNamespaceName("RtQuat");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RtQuatConvertFromMatrix, 0x7EB5C0);
    RH_ScopedGlobalInstall(RtQuatConvertFromMatrixAxisX, 0x7EB6A0);
    RH_ScopedGlobalInstall(RtQuatConvertFromMatrixAxisY, 0x7EB700);
    RH_ScopedGlobalInstall(RtQuatConvertFromMatrixAxisZ, 0x7EB760);
    RH_ScopedGlobalInstall(RtQuatRotate, 0x7EB7C0);
    RH_ScopedGlobalInstall(RtQuatTransformVectors, 0x7EBBB0);
}

namespace {
// RWERROR((codigo [, argumento])) del SDK con el pluginID literal 0x195 del EXE: RwError { 0x195, _rwerror(codigo [, argumento]) } + RwErrorSet (0x808820).
// _rwerror (0x8088D0) se llama por direccion para conservar la secuencia de colaboradores.
constexpr RwInt32 kRtQuatPluginID = 0x195;

void RaiseRtQuatError(RwInt32 code) {
    RwError error;
    error.pluginID  = kRtQuatPluginID;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code);
    RwErrorSet(&error);
}

void RaiseRtQuatError(RwInt32 code, const char* argument) {
    RwError error;
    error.pluginID  = kRtQuatPluginID;
    error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(code, argument);
    RwErrorSet(&error);
}

constexpr RwInt32 kRwNullPointerError   = static_cast<RwInt32>(0x80000016); // E_RW_NULLP
constexpr RwInt32 kRwInvalidParamError  = static_cast<RwInt32>(0x80000003); // E_RW_INVALIDPARAM
// "Invalid combination type" (.rdata 0x8E21FC)
const char* const kInvalidCombinationMessage = reinterpret_cast<const char*>(0x8E21FC);
} // namespace

// 0x7EB5C0: matriz -> cuaternion. Sin cuaternion o sin matriz devuelve FALSE sin error. Con traza > 0: real = 0.5 * sqrt(traza + 1) y los tres ejes por la diferencia de los
// elementos simetricos; si no, rama del eje de mayor diagonal (x si right.x > up.y y right.x > at.z; y si up.y >= right.x y up.y > at.z; z en los demas casos, con las
// comparaciones ordenadas del EXE: con NaN cuenta como "no mayor"). Devuelve TRUE.
RwBool RtQuatConvertFromMatrix(RtQuat* qpQuat, const RwMatrix* const mpMatrix) {
    if (qpQuat == nullptr || mpMatrix == nullptr) {
        return FALSE;
    }
    const float* const m = reinterpret_cast<const float*>(mpMatrix);
    float trPlusOne;
    if (RtQuatMatrixTraceCpp(m, &trPlusOne)) {
        RtQuatMatrixTraceFinishCpp(reinterpret_cast<float*>(qpQuat), m, trPlusOne);
        return TRUE;
    }
    if (m[0] > m[5]) {
        if (m[0] > m[10]) {
            RtQuatConvertFromMatrixAxisX(qpQuat, mpMatrix);
        } else {
            RtQuatConvertFromMatrixAxisZ(qpQuat, mpMatrix);
        }
    } else if (m[5] > m[10]) {
        RtQuatConvertFromMatrixAxisY(qpQuat, mpMatrix);
    } else {
        RtQuatConvertFromMatrixAxisZ(qpQuat, mpMatrix);
    }
    return TRUE;
}

// 0x7EB6A0 / 0x7EB700 / 0x7EB760: ramas de RtQuatConvertFromMatrix (cdecl, dos argumentos; el EXE las llama con `call eax` y las dos direcciones quedan en pila).
void RtQuatConvertFromMatrixAxisX(RtQuat* qpQuat, const RwMatrix* mpMatrix) {
    const float* const m = reinterpret_cast<const float*>(mpMatrix);
    RtQuatMatrixAxisFinishCpp(reinterpret_cast<float*>(qpQuat), m, 0, RtQuatMatrixAxisArgCpp(m, 0));
}

void RtQuatConvertFromMatrixAxisY(RtQuat* qpQuat, const RwMatrix* mpMatrix) {
    const float* const m = reinterpret_cast<const float*>(mpMatrix);
    RtQuatMatrixAxisFinishCpp(reinterpret_cast<float*>(qpQuat), m, 1, RtQuatMatrixAxisArgCpp(m, 1));
}

void RtQuatConvertFromMatrixAxisZ(RtQuat* qpQuat, const RwMatrix* mpMatrix) {
    const float* const m = reinterpret_cast<const float*>(mpMatrix);
    RtQuatMatrixAxisFinishCpp(reinterpret_cast<float*>(qpQuat), m, 2, RtQuatMatrixAxisArgCpp(m, 2));
}

// 0x7EB7C0: gira el cuaternion `quat` por `angle` (grados) alrededor de `axis`. Sin cuaternion o sin eje: RWERROR(E_RW_NULLP) y NULL. Segun combineOp:
//   rwCOMBINEREPLACE (0): quat = (sin(h) * normalizado(eje), cos(h)), con h = angle * pi/360; el coseno se guarda en quat->real antes de normalizar el eje dentro de quat->imag.
//   rwCOMBINEPRECONCAT (1) / rwCOMBINEPOSTCONCAT (2): el eje se normaliza en un temporal y se multiplica por el cuaternion (copia previa) en el orden del EXE.
//   otro valor: RWERROR(E_RW_INVALIDPARAM, "Invalid combination type") y NULL.
RtQuat* RtQuatRotate(RtQuat* quat, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp) {
    if (quat == nullptr || axis == nullptr) {
        RaiseRtQuatError(kRwNullPointerError);
        return nullptr;
    }
    float* const quatF = reinterpret_cast<float*>(quat);
    switch (static_cast<int>(combineOp)) {
    case 0: { // rwCOMBINEREPLACE
        float sinValue;
        RtQuatRotateSinCosCpp(angle, &sinValue, &quatF[3]);
        RwV3dNormalize(&quat->imag, axis);
        RtQuatRotateReplaceScaleCpp(quatF, sinValue);
        return quat;
    }
    case 1:   // rwCOMBINEPRECONCAT
    case 2: { // rwCOMBINEPOSTCONCAT
        float sinValue, cosValue;
        RwV3d unitAxis;
        RtQuatRotateSinCosCpp(angle, &sinValue, &cosValue);
        RwV3dNormalize(&unitAxis, axis);
        RtQuatRotateConcatCpp(quatF, reinterpret_cast<const float*>(&unitAxis), sinValue, cosValue, combineOp == 2);
        return quat;
    }
    default:
        RaiseRtQuatError(kRwInvalidParamError, kInvalidCombinationMessage);
        return nullptr;
    }
}

const RtQuat* RtQuatQueryRotate(const RtQuat* quat, RwV3d* unitAxis, RwReal* angle) {
    return ((const RtQuat*(__cdecl *)(const RtQuat*, RwV3d*, RwReal*))0x7EBA80)(quat, unitAxis, angle);
}

// 0x7EBBB0: rota `numPoints` vectores de `vectorsIn` por el cuaternion `quat` y los escribe en `vectorsOut` (pueden coincidir). No comprueba punteros; con numPoints <= 0 no escribe nada. Devuelve vectorsOut.
RwV3d* RtQuatTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, const RwInt32 numPoints, const RtQuat* quat) {
    RtQuatTransformVectorsCpp(reinterpret_cast<float*>(vectorsOut), reinterpret_cast<const float*>(vectorsIn), numPoints, reinterpret_cast<const float*>(quat));
    return vectorsOut;
}

RwReal RtQuatModulus(RtQuat* q) {
    return ((RwReal(__cdecl *)(RtQuat*))0x7EBD10)(q);
}
