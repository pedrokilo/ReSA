/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxEmitter_c
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxEmitter.h"
#include "EmissionInfo.h"
#include "FxEmitterPrt.h"
#include "FxPrimBP.h"
#include "FxEmitterBP.h"
#include "FxEmitterPrt.h"
#include "Weather.h"
#include "X87Math.h"

void FxEmitter_c::InjectHooks() {
    RH_ScopedVirtualClass(FxEmitter_c, 0x85A7A4, 6);
    RH_ScopedCategory("Fx");

    // RH_ScopedInstall(Init_Reversed, 0x4A2550);
    // RH_ScopedInstall(Update_Reversed, 0x4A4460); // bad
    // RH_ScopedInstall(Reset_Reversed, 0x4A2570);
    // La entrada 0x4A3EA0 es la sobrecarga con vector de posicion (vtable +0x14); la de matriz es 0x4A4050 (+0x10).
    RH_ScopedVMTOverloadedInstall(AddParticle, "vector", 0x4A3EA0, void(FxEmitter_c::*)(const CVector&, const CVector&, float, const FxPrtMult_c&, float, float, bool));
    RH_ScopedVMTOverloadedInstall(AddParticle, "matrix", 0x4A4050, void(FxEmitter_c::*)(const RwMatrix&, const CVector&, float, const FxPrtMult_c&, float, float, bool));
    RH_ScopedInstall(CreateParticles, 0x4A41E0);
    RH_ScopedInstall(CreateParticle, 0x4A2580);
}


// 0x4A2550
bool FxEmitter_c::Init(FxPrimBP_c* primBP, FxSystem_c* system) {
    m_PrimBP             = primBP;
    m_System             = system;
    m_fEmissionIntensity = 0.0f;
    return true;
}

// 0x4A4460
void FxEmitter_c::Update(float currentTime, float deltaTime) {
    if (m_bEnabled && !m_System->m_stopParticleCreation) {
        CreateParticles(currentTime, deltaTime);
    }
}

// 0x4A2570
void FxEmitter_c::Reset() {
    m_fEmissionIntensity = 0.0f;
}

// 0x4A3EA0 (vtable +0x14)
// ProcessEmissionInfo(0, 0, longitud, useConstTime), tres matrices del pool (A: resultado, B: mundo, C: identidad en `pos`), RwMatrixUpdate(C), B = C o C * padre,
// una cuarta del pool = GetRWMatrix del prim (se destruye tras A = D * B), CreateParticle(emision, A, &vel, ...) y, si rotZ >= 0 (ordenado), m_RotZ = byte bajo de _ftol2(rotZ * 0.5f).
// Destruye C, B, A en ese orden. Ningun llamador del EXE lee EAX/ECX/EDX.
void FxEmitter_c::AddParticle(const CVector& pos, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) {
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(0.0f, 0.0f, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    RwMatrix* const mat0 = g_fxMan.FxRwMatrixCreate();
    RwMatrix* const mat1 = g_fxMan.FxRwMatrixCreate();
    RwMatrix* const mat  = g_fxMan.FxRwMatrixCreate();

    // 0x4A3EF7..0x4A3F44: identidad (right.x = up.y = at.z = 1, el resto de las columnas a 0), flags |= 0x20003 (identidad + ortonormal) y posicion = pos. No se tocan los rellenos.
    const uint32 flags = mat->flags;
    mat->right = { 1.0f, 0.0f, 0.0f };
    mat->up    = { 0.0f, 1.0f, 0.0f };
    mat->at    = { 0.0f, 0.0f, 1.0f };
    mat->pos   = { 0.0f, 0.0f, 0.0f };
    mat->flags = flags | 0x20003;
    mat->pos   = pos;
    RwMatrixUpdate(mat);

    if (m_System->m_ParentMatrix) {
        RwMatrixMultiply(mat1, mat, m_System->m_ParentMatrix);
    } else {
        std::memcpy(mat1, mat, sizeof(RwMatrix));
    }

    RwMatrix* const primMat = g_fxMan.FxRwMatrixCreate();
    m_PrimBP->GetRWMatrix(*primMat);
    RwMatrixMultiply(mat0, primMat, mat1);
    g_fxMan.FxRwMatrixDestroy(primMat);

    if (auto* const particle = CreateParticle(emission, *mat0, &vel, timeSince, fxMults, brightness, createLocal)) {
        if (rotZ >= 0.0f) {
            particle->m_RotZ = static_cast<int8>(FxAddParticleRotZByteCpp(rotZ));
        }
    }

    g_fxMan.FxRwMatrixDestroy(mat);
    g_fxMan.FxRwMatrixDestroy(mat1);
    g_fxMan.FxRwMatrixDestroy(mat0);
}

// 0x4A4050 (vtable +0x10)
// Igual que la sobrecarga de vector (0x4A3EA0) salvo que la matriz de mundo C no se construye con identidad + posicion: el EXE escribe la identidad (dead stores) y a continuacion copia las 16
// palabras de `mat` encima con rep movs (flags incluidos), asi que C = mat. Despues: RwMatrixUpdate(C), B = C o C * padre, D = GetRWMatrix del prim, A = D * B, CreateParticle(emision, A, &vel, ...)
// y, si rotZ >= 0 (ordenado), m_RotZ = byte bajo de _ftol2(rotZ * 0.5f). Destruye C, B, A en ese orden. Ningun llamador del EXE lee EAX/ECX/EDX.
void FxEmitter_c::AddParticle(const RwMatrix& mat, const CVector& vel, float timeSince, const FxPrtMult_c& fxMults, float rotZ, float brightness, bool createLocal) {
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(0.0f, 0.0f, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    RwMatrix* const mat0 = g_fxMan.FxRwMatrixCreate();
    RwMatrix* const mat1 = g_fxMan.FxRwMatrixCreate();
    RwMatrix* const mat2 = g_fxMan.FxRwMatrixCreate();

    std::memcpy(mat2, &mat, sizeof(RwMatrix));
    RwMatrixUpdate(mat2);

    if (m_System->m_ParentMatrix) {
        RwMatrixMultiply(mat1, mat2, m_System->m_ParentMatrix);
    } else {
        std::memcpy(mat1, mat2, sizeof(RwMatrix));
    }

    RwMatrix* const primMat = g_fxMan.FxRwMatrixCreate();
    m_PrimBP->GetRWMatrix(*primMat);
    RwMatrixMultiply(mat0, primMat, mat1);
    g_fxMan.FxRwMatrixDestroy(primMat);

    if (auto* const particle = CreateParticle(emission, *mat0, &vel, timeSince, fxMults, brightness, createLocal)) {
        if (rotZ >= 0.0f) {
            particle->m_RotZ = static_cast<int8>(FxAddParticleRotZByteCpp(rotZ));
        }
    }

    g_fxMan.FxRwMatrixDestroy(mat2);
    g_fxMan.FxRwMatrixDestroy(mat1);
    g_fxMan.FxRwMatrixDestroy(mat0);
}

// 0x4A41E0
void FxEmitter_c::CreateParticles(float currentTime, float deltaTime) {
    EmissionInfo_t emission;
    m_PrimBP->m_FxInfoManager.ProcessEmissionInfo(currentTime, deltaTime, m_System->m_SystemBP->m_fLength, m_System->m_UseConstTime, &emission);

    // 0x4A421C..0x4A4331: visibilidad por distancia, acumulacion de la intensidad (se guarda siempre) y comprobaciones de viento, lluvia y m_fEmissionIntensity >= 1.
    if (!FxEmitterAccumulateCpp(&m_fEmissionIntensity, &m_System->m_fCameraDistance, m_System->m_nRateMult, m_PrimBP->m_FxInfoManager.m_nLodStart, m_PrimBP->m_FxInfoManager.m_nLodEnd, emission.m_fCount, &emission.m_fMinWind, &emission.m_fMaxWind, &emission.m_fMinRain, &emission.m_fMaxRain, &CWeather::Wind, &CWeather::Rain)) {
        return;
    }

    auto* const mat  = g_fxMan.FxRwMatrixCreate();
    auto* const mat1 = g_fxMan.FxRwMatrixCreate();
    RwMatrixUpdate(&m_System->m_LocalMatrix);
    if (m_System->m_ParentMatrix) {
        RwMatrixMultiply(mat1, &m_System->m_LocalMatrix, m_System->m_ParentMatrix);
    } else {
        *mat1 = m_System->m_LocalMatrix;
    }

    auto* const mat2 = g_fxMan.FxRwMatrixCreate();
    m_PrimBP->GetRWMatrix(*mat2);
    RwMatrixMultiply(mat, mat2, mat1);
    g_fxMan.FxRwMatrixDestroy(mat2);

    // 0x4A43B0..0x4A4416: el EXE vuelve a convertir la intensidad (_ftol2) tras cada particula. CreateParticle (0x4A2580) ya esta en C++ (lote 30).
    for (int32 step = 0; step < FxEmitterTruncateCpp(&m_fEmissionIntensity); ++step) {
        FxPrtMult_c prtMult;
        const float timeSince = FxEmitterTimeSinceCpp(step, &m_fEmissionIntensity, deltaTime);
        CreateParticle(emission, *mat, nullptr, timeSince, prtMult, 1.2f, m_System->m_createLocal);
    }
    FxEmitterTakeWholeCpp(&m_fEmissionIntensity);

    g_fxMan.FxRwMatrixDestroy(mat1);
    g_fxMan.FxRwMatrixDestroy(mat);
}

// rand() del CRT del EXE (0x821B1E): el del juego avanza la misma semilla que el resto del EXE, no el de la DLL. `% 10000` es el idiv del EXE (el resultado de rand es >= 0).
static int FxEmitterExeRemainder() {
    return reinterpret_cast<int(__cdecl*)()>(0x821B1E)() % 10000;
}

// 0x4A2580 (thiscall, ret 1Ch). Orden de los rand() del EXE (0x821B1E, todos `% 10000`): vida, tres bytes aleatorios, rotacion, [GetRWMatrix si createLocal], posicion (3; esfera con radio > 0: un cuarto),
// y, sin velocidad impuesta, dos angulos y un factor de velocidad (este ultimo tras RotateVecIntoVec). Las tiradas no tocan la x87, asi que se piden aqui y los ayudantes FxPrt* (X87Math.cpp)
// reproducen las secuencias x87 de 24 bits con los restos. Ningun llamador del EXE lee ECX/EDX; EAX = la particula (o 0) lo leen 0x4A3FDF y 0x4A417D.
FxEmitterPrt_c* FxEmitter_c::CreateParticle(const EmissionInfo_t& emissionInfo, RwMatrix& wldMat, const CVector* velOverride, float timeSince, const FxPrtMult_c& fxMults, float brightness, bool createLocal) {
    // 0x4A2589..0x4A25BF: GetParticle(0); sin hueco, solo si el sistema exige crear (m_MustCreateParticles) se libera una de otro sistema y se vuelve a pedir.
    Particle_c* particle = g_fxMan.GetParticle(0);
    if (!particle) {
        if (!m_System->m_MustCreateParticles) {
            return nullptr;
        }
        g_fxMan.FreeUpParticle();
        particle = g_fxMan.GetParticle(0);
        if (!particle) {
            return nullptr;
        }
    }
    auto* const prt = static_cast<FxEmitterPrt_c*>(particle);
    auto* const bytes = reinterpret_cast<uint8*>(prt);

    // 0x4A25C7..0x4A2600
    const int32 lifeRem = FxEmitterExeRemainder();
    prt->m_fCurrentLife = 0.0f;
    FxPrtLifeCpp(lifeRem, emissionInfo.m_fLifeBias, emissionInfo.m_fLife, fxMults.m_fLife, &prt->m_fTotalLife);
    prt->m_System = m_System;

    // 0x4A2609..0x4A266B: rojo, verde, azul, alfa, tamano y rotacion (byte bajo de _ftol2(x * 255.0f)).
    bytes[0x2C] = static_cast<uint8>(FxPrtColorByteCpp(fxMults.m_Color.red));
    bytes[0x2D] = static_cast<uint8>(FxPrtColorByteCpp(fxMults.m_Color.green));
    bytes[0x2E] = static_cast<uint8>(FxPrtColorByteCpp(fxMults.m_Color.blue));
    bytes[0x2F] = static_cast<uint8>(FxPrtColorByteCpp(fxMults.m_Color.alpha));
    bytes[0x30] = static_cast<uint8>(FxPrtColorByteCpp(fxMults.m_fSize));
    bytes[0x31] = static_cast<uint8>(FxPrtColorByteCpp(fxMults.m_Rot));
    bytes[0x37] = createLocal;
    // 0x4A2675..0x4A26FF
    bytes[0x32] = static_cast<uint8>(FxPrtRandByteCpp(FxEmitterExeRemainder()));
    bytes[0x33] = static_cast<uint8>(FxPrtRandByteCpp(FxEmitterExeRemainder()));
    bytes[0x34] = static_cast<uint8>(FxPrtRandByteCpp(FxEmitterExeRemainder()));
    bytes[0x35] = static_cast<uint8>(FxPrtBrightnessByteCpp(brightness));
    // 0x4A2702..0x4A2735
    const int32 rotationRem = FxEmitterExeRemainder();
    bytes[0x36] = 0xFF;
    FxPrtRotationCpp(rotationRem, emissionInfo.m_fRotationMinAngle, emissionInfo.m_fRotationMaxAngle, &prt->m_CurrentRotation);

    // 0x4A2738..0x4A2740: con la bandera local la matriz de trabajo es la del prim.
    if (bytes[0x37]) {
        m_PrimBP->GetRWMatrix(wldMat);
    }

    // 0x4A2743..0x4A28B6: posicion. |radio| < 0.001 (o NaN): caja; si no, esfera. x87 de 24 bits en FxPrtBoxPositionCpp / FxPrtSpherePositionCpp.
    const float* const matrix = reinterpret_cast<const float*>(&wldMat);
    float* const position = &prt->m_Pos.x;
    if (!(std::fabs(static_cast<double>(emissionInfo.m_fRadius)) >= static_cast<double>(0.001f))) { // fld; fabs; fcomp qword [0x85A7C8]; test ah, 1
        const int32 remX = FxEmitterExeRemainder();
        const int32 remY = FxEmitterExeRemainder();
        const int32 remZ = FxEmitterExeRemainder();
        FxPrtBoxPositionCpp(remX, remY, remZ, &emissionInfo.m_SizeMin.x, &emissionInfo.m_SizeMax.x, &emissionInfo.m_Pos.x, matrix, position);
    } else {
        const int32 remX = FxEmitterExeRemainder();
        const int32 remY = FxEmitterExeRemainder();
        const int32 remZ = FxEmitterExeRemainder();
        const bool useExtra = !(emissionInfo.m_fRadius < 0.0f); // radio > 0: un cuarto rand()
        const int32 remExtra = useExtra ? FxEmitterExeRemainder() : 0;
        FxPrtSpherePositionCpp(remX, remY, remZ, useExtra, remExtra, emissionInfo.m_fRadius, &emissionInfo.m_Pos.x, matrix, position);
    }

    // 0x4A2915..0x4A2AD0: velocidad.
    if (velOverride) {
        std::memcpy(&prt->m_Velocity, velOverride, sizeof(CVector));
    } else {
        const int32 angleRem = FxEmitterExeRemainder();
        const int32 betweenRem = FxEmitterExeRemainder();
        CVector randomizedAngleVec;
        FxPrtDirectionCpp(angleRem, betweenRem, emissionInfo.m_fAngleMin, emissionInfo.m_fAngleMax, reinterpret_cast<const float*>(0xBB3E00), &randomizedAngleVec.x); // tabla de senos (0xBB3E00)
        CVector vectorsIn;
        if (!(emissionInfo.m_Direction.x > 10.0f)) { // fcomp [10.0f]; test ah, 41h; jne: direccion (menor, igual o NaN), normalizada con RwV3dNormalize (0x7ED9B0)
            std::memcpy(&vectorsIn, &emissionInfo.m_Direction, sizeof(CVector));
            RwV3dNormalize(&vectorsIn, &vectorsIn);
        } else {
            std::memcpy(&vectorsIn, &prt->m_Pos, sizeof(CVector));
        }
        CVector vectorsOut;
        RwV3dTransformVectors(&vectorsOut, &vectorsIn, 1, &wldMat);
        CVector rotated;
        RotateVecIntoVec(rotated, randomizedAngleVec, vectorsOut);
        const int32 speedRem = FxEmitterExeRemainder();
        FxPrtSpeedCpp(speedRem, emissionInfo.m_fSpeedBias, emissionInfo.m_Speed, &rotated.x, &prt->m_Velocity.x);
    }

    // 0x4A2AD2..0x4A2B09
    FxPrtVelAddCpp(&m_System->m_VelAdd.x, &prt->m_Velocity.x);
    static_cast<FxEmitterBP_c*>(m_PrimBP)->UpdateParticle(timeSince, prt);
    m_PrimBP->m_Particles.AddItem(prt);

    return prt;
}
