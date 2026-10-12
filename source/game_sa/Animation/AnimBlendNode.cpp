/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimBlendNode
 * Reconstrucción: COMPLETA — 16/16 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 16/16 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AnimBlendNode.h"
#include "X87Math.h"

/**
 * Registra las operaciones de interpolación temporal de un nodo de animación.
 */
void CAnimBlendNode::InjectHooks() {
    RH_ScopedClass(CAnimBlendNode);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Init, 0x4CFB70);
    RH_ScopedInstall(NextKeyFrameNoCalc, 0x4CFB90);
    RH_ScopedInstall(GetCurrentTranslation, 0x4CFC50);
    RH_ScopedInstall(GetCurrentTranslationCompressed, 0x4CFE60);
    RH_ScopedInstall(GetEndTranslation, 0x4CFD90);
    RH_ScopedInstall(GetEndTranslationCompressed, 0x4D0000);
    RH_ScopedInstall(CalcTheta, 0x4D00E0);
    RH_ScopedInstall(UpdateTime, 0x4D0160);
    RH_ScopedInstall(CalcDeltas, 0x4D0190);
    RH_ScopedInstall(FindKeyFrame, 0x4D0240);
    RH_ScopedInstall(CalcDeltasCompressed, 0x4D0350);
    RH_ScopedInstall(NextKeyFrame, 0x4D04A0);
    RH_ScopedInstall(NextKeyFrameCompressed, 0x4D0570);
    RH_ScopedInstall(SetupKeyFrameCompressed, 0x4D0650);
    RH_ScopedInstall(Update, 0x4D06C0);
    RH_ScopedInstall(UpdateCompressed, 0x4D08D0);
}

// 0x4CFB70
// Restablece índices, tiempos y referencias al estado inicial por defecto.
void CAnimBlendNode::Init() {
    // Como el original: no toca m_Theta ni m_InvSinTheta (antes se reasignaba el nodo entero a cero).
    m_BlendAssoc      = nullptr;
    m_Seq             = nullptr;
    m_KFRemainingTime = 0.0f;
    m_KFPrev          = -1;
    m_KFCurr          = -1;
}

// 0x4D0240
/**
 * Sitúa el nodo en el par de keyframes que contiene @p time.
 *
 * Consume DeltaTime sucesivos, respeta el flag looped de la asociación y calcula
 * después los parámetros de interpolación de rotación para el par seleccionado.
 */
bool CAnimBlendNode::FindKeyFrame(float time) {
    // Como el original: contador con signo (movsx) y el keyframe se vuelve a leer en cada vuelta (antes se
    // comparaba siempre con el DeltaTime del keyframe 1).
    const int32 numFrames = static_cast<int16>(m_Seq->m_FramesNum);
    if (numFrames < 1) {
        return false;
    }

    m_KFCurr = 0;
    m_KFPrev = 0;

    if (numFrames == 1) {
        m_KFRemainingTime = 0.0f;
        CalcDeltas();
        return true;
    }

    // Find kf that spans over the specified `time`
    m_KFCurr = 1;
    while (time > KeyFrameDeltaTime<false>(KeyFrameAt<false>(m_Seq, m_KFCurr))) {
        time -= KeyFrameDeltaTime<false>(KeyFrameAt<false>(m_Seq, m_KFCurr));

        if (m_KFCurr + 1 >= numFrames) {
            // reached end of animation
            if (!m_BlendAssoc->IsLooped()) {
                CalcDeltas();
                m_KFRemainingTime = 0.0f;
                return false;
            }
            m_KFCurr = 0;
        }
        m_KFPrev = m_KFCurr;
        m_KFCurr = static_cast<int16>(m_KFCurr + 1);
    }

    // Now calculate how much we have remaining from this frame
    m_KFRemainingTime = KeyFrameDeltaTime<false>(KeyFrameAt<false>(m_Seq, m_KFCurr)) - time;

    CalcDeltas();
    return true;
}

// 0x4D00E0
// Precalcula el ángulo y 1/sin(theta) usados por Slerp entre los dos keyframes.
void CAnimBlendNode::CalcTheta(const CQuaternion& prev, const CQuaternion& curr, float& theta, float& invSinTheta) {
    // Como el original (0x4D00E0): producto escalar, recorte a 1, acos del CRT del juego y 1/sin sin redondear; en X87Math.cpp.
    AnimCalcThetaCpp(&prev.x, &curr.x, &theta, &invSinTheta);
}

// 0x4D0190
// Prepara la interpolación de rotación para keyframes sin comprimir.
void CAnimBlendNode::CalcDeltas() {
    if (!m_Seq->m_bHasRotation) {
        return;
    }

    // Como el original: copia los dos quaternions (anterior y actual) y llama a CalcTheta(anterior, actual).
    const CQuaternion rotPrev = KeyFrameRot<false>(KeyFrameAt<false>(m_Seq, m_KFPrev));
    const CQuaternion rotCurr = KeyFrameRot<false>(KeyFrameAt<false>(m_Seq, m_KFCurr));
    CalcTheta(rotPrev, rotCurr, m_Theta, m_InvSinTheta);
}

// 0x4D0350
/**
 * Prepara la interpolación de rotación comprimida.
 *
 * Si los quaternions caen en hemisferios opuestos invierte uno para que Slerp
 * recorra el arco corto y evita una rotación visualmente innecesaria.
 */
void CAnimBlendNode::CalcDeltasCompressed() {
    if (!m_Seq->m_bHasRotation) {
        return;
    }

    uint8* const kfPrev = KeyFrameAt<true>(m_Seq, m_KFPrev);
    uint8* const kfCurr = KeyFrameAt<true>(m_Seq, m_KFCurr);

    const CQuaternion rotPrev = KeyFrameRot<true>(kfPrev);
    CQuaternion       rotCurr = KeyFrameRot<true>(kfCurr);

    // Como el original: producto escalar ((pw·cw + pz·cz) + py·cy) + px·cx; si es negativo se invierte el keyframe
    // ACTUAL (no el anterior), se vuelve a comprimir (·4096 truncado con _ftol2) y CalcTheta recibe el quaternion ya
    // invertido (antes se le pasaba el coseno negativo previo a la inversión).
    const float angleCos = ((rotPrev.w * rotCurr.w + rotPrev.z * rotCurr.z) + rotPrev.y * rotCurr.y) + rotPrev.x * rotCurr.x;
    if (angleCos < 0.0f) {
        rotCurr.x = -rotCurr.x;
        rotCurr.y = -rotCurr.y;
        rotCurr.z = -rotCurr.z;
        rotCurr.w = -rotCurr.w;
        auto* const r = reinterpret_cast<int16*>(kfCurr);
        r[0] = static_cast<int16>(static_cast<int64>(rotCurr.x * 4096.0f));
        r[1] = static_cast<int16>(static_cast<int64>(rotCurr.y * 4096.0f));
        r[2] = static_cast<int16>(static_cast<int64>(rotCurr.z * 4096.0f));
        r[3] = static_cast<int16>(static_cast<int64>(rotCurr.w * 4096.0f));
    }
    CalcTheta(rotPrev, rotCurr, m_Theta, m_InvSinTheta);
}

// 0x4CFB90 - Unused
// Como el original: igual que NextKeyFrame pero sin recalcular los deltas de rotación (antes no estaba implementada).
bool CAnimBlendNode::NextKeyFrameNoCalc() {
    const int32 numFrames = static_cast<int16>(m_Seq->m_FramesNum);
    if (numFrames <= 1) {
        return false;
    }

    m_KFPrev = m_KFCurr;

    bool looped = false;
    while (m_KFRemainingTime <= 0.0f) {
        m_KFCurr++;

        if (m_KFCurr >= numFrames) {
            if (!m_BlendAssoc->IsLooped()) {
                m_KFCurr--;
                m_KFRemainingTime = 0.0f;
                return false;
            }
            looped   = true;
            m_KFCurr = 0;
        }

        m_KFRemainingTime = KeyFrameDeltaTime<false>(KeyFrameAt<false>(m_Seq, m_KFCurr)) + m_KFRemainingTime;
    }

    m_KFPrev = static_cast<int16>(m_KFCurr - 1);
    if (m_KFPrev < 0) {
        m_KFPrev = static_cast<int16>(m_KFPrev + numFrames);
    }
    return looped;
}

// 0x4D0650
// Inicializa el primer intervalo de una secuencia comprimida y sus deltas de rotación.
bool CAnimBlendNode::SetupKeyFrameCompressed() {
    // Como el original: contador con signo (movsx).
    if (static_cast<int16>(m_Seq->m_FramesNum) < 1) {
        return false;
    }

    m_KFCurr = 1;
    m_KFPrev = 0;

    if (m_Seq->m_FramesNum == 1) {
        m_KFCurr          = 0;
        m_KFRemainingTime = 0.0f;
    } else {
        // Como el original: tiempo del keyframe 1 ·0x3C888889 (1/60 del EXE).
        m_KFRemainingTime = KeyFrameDeltaTime<true>(KeyFrameAt<true>(m_Seq, 1));
    }

    CalcDeltasCompressed();
    return true;
}

// 0x4D0160 - Unused
bool CAnimBlendNode::UpdateTime() {
    if (m_BlendAssoc->IsPlaying()) {
        m_KFRemainingTime -= m_BlendAssoc->m_TimeStep;
        if (m_KFRemainingTime <= 0.0f) {
            return NextKeyFrameNoCalc();
        }
    }
    return false;
}
