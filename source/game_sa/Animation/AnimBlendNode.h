/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
// by Github.com / jte
#pragma once

#include "Quaternion.h"
#include "AnimBlendSequence.h"
#include "AnimBlendAssociation.h"

/*!
 * @brief Estado de reproducción de una secuencia para un único nodo/bone.
 *
 * Conserva el par de keyframes activo, el tiempo restante y los valores auxiliares
 * necesarios para interpolar traslación y quaternion. m_Seq y m_BlendAssoc son
 * referencias externas; el nodo no es propietario de ninguno de los dos.
 */
class CAnimBlendNode {
public:
    static void InjectHooks();

    void Init();

    //! @addr 0x4D00E0
    //! Como el original: función libre (cdecl) que recibe los dos quaternions y devuelve theta y 1/sin(theta)
    //! por referencia; no es un miembro con el coseno como argumento.
    static void CalcTheta(const CQuaternion& prev, const CQuaternion& curr, float& theta, float& invSinTheta);

    //! @addr 0x4D0190
    void CalcDeltas();

    //! @addr 0x4D0350
    void CalcDeltasCompressed();


    //! @addr 0x4CFC50
    void GetCurrentTranslation(CVector& trans, float weight) { return I_GetCurrentTranslation<false>(trans, weight); }

    //! @addr 0x4CFE60
    void GetCurrentTranslationCompressed(CVector& trans, float weight) { return I_GetCurrentTranslation<true>(trans, weight); }


    //! @addr 0x4CFD90
    void GetEndTranslation(CVector& trans, float weight) { return I_GetEndTranslation<false>(trans, weight); }

    //! @addr 0x4D0000
    void GetEndTranslationCompressed(CVector& trans, float weight) { return I_GetEndTranslation<true>(trans, weight); }


    //! @addr 0x4D04A0
    bool NextKeyFrame() { return I_NextKeyFrame<false>(); }

    //! @addr 0x4D0570
    bool NextKeyFrameCompressed() { return I_NextKeyFrame<true>(); }

    //! @addr 0x4CFB90
    bool NextKeyFrameNoCalc();


    //! @addr 0x4D06C0
    bool Update(CVector& trans, CQuaternion& rot, float weight) { return I_Update<false>(trans, rot, weight); }

    //! @addr 0x4D08D0
    bool UpdateCompressed(CVector& trans, CQuaternion& rot, float weight) { return I_Update<true>(trans, rot, weight); }

    bool UpdateTime(); // Unused
    bool SetupKeyFrameCompressed();
    bool FindKeyFrame(float time);
    auto GetAnimAssoc() const { return m_BlendAssoc; }
    bool IsValid() const { return !!m_Seq; }
    auto GetSeq() const { return m_Seq; }
    auto GetRootKF() const { return &m_Seq[0]; }

    // Como el original: acceso crudo a los keyframes. El EXE no mira el flag de compresión (el formato lo decide
    // la función llamada), calcula el paso solo con el bit de traslación y usa índices con signo (movsx).
    // Los comprimidos se expanden con las constantes del EXE: 1/4096 (rotación), 1/1024 (traslación) y
    // 0x3C888889 (tiempo: multiplica por 1/60 en vez de dividir por 60).
    template<bool IsCompressed>
    static uint8* KeyFrameAt(const CAnimBlendSequence* seq, int32 n) {
        const int32 stride = IsCompressed
            ? (seq->m_bHasTranslation ? 16 : 10)
            : (seq->m_bHasTranslation ? 32 : 20);
        return static_cast<uint8*>(seq->m_Frames) + n * stride;
    }

    template<bool IsCompressed>
    static float KeyFrameDeltaTime(const uint8* kf) {
        if constexpr (IsCompressed) {
            return static_cast<float>(*reinterpret_cast<const int16*>(kf + 8)) * std::bit_cast<float>(0x3C888889u);
        } else {
            return *reinterpret_cast<const float*>(kf + 16);
        }
    }

    template<bool IsCompressed>
    static CQuaternion KeyFrameRot(const uint8* kf) {
        if constexpr (IsCompressed) {
            const auto* r = reinterpret_cast<const int16*>(kf);
            return CQuaternion{
                static_cast<float>(r[0]) * 0.000244140625f,
                static_cast<float>(r[1]) * 0.000244140625f,
                static_cast<float>(r[2]) * 0.000244140625f,
                static_cast<float>(r[3]) * 0.000244140625f
            };
        } else {
            const auto* r = reinterpret_cast<const float*>(kf);
            return CQuaternion{ r[0], r[1], r[2], r[3] };
        }
    }

    template<bool IsCompressed>
    static CVector KeyFrameTrans(const uint8* kf) {
        if constexpr (IsCompressed) {
            const auto* v = reinterpret_cast<const int16*>(kf + 10);
            return CVector{
                static_cast<float>(v[0]) * 0.0009765625f,
                static_cast<float>(v[1]) * 0.0009765625f,
                static_cast<float>(v[2]) * 0.0009765625f
            };
        } else {
            const auto* v = reinterpret_cast<const float*>(kf + 20);
            return CVector{ v[0], v[1], v[2] };
        }
    }

    //! @notsa
    //! @brief Devuelve el progreso normalizado dentro del intervalo actual.
    template<bool IsCompressed>
    NOTSA_FORCEINLINE float GetTimeRemainingProgress() const {
        // Como el original: t = (dt - restante) / dt, o 0 si dt == 0 (en comprimido equivale a comparar el int16).
        const float dt = KeyFrameDeltaTime<IsCompressed>(KeyFrameAt<IsCompressed>(m_Seq, m_KFCurr));
        return dt == 0.0f
            ? 0.0f
            : (dt - m_KFRemainingTime) / dt;
    }

    // Como el original: (actual - anterior) * t + anterior, por componente (no to*t + from*(1-t)).
    static CVector LerpTranslation(const CVector& prev, const CVector& curr, float t) {
        const float dx = curr.x - prev.x;
        const float dy = curr.y - prev.y;
        const float dz = curr.z - prev.z;
        return CVector{ dx * t + prev.x, dy * t + prev.y, dz * t + prev.z };
    }

    template<bool IsCompressed>
    NOTSA_FORCEINLINE void I_GetCurrentTranslation(CVector& trans, float weight) {
        trans = CVector{0.0f, 0.0f, 0.0f};

        const auto blend = m_BlendAssoc->GetBlendAmount(weight);
        if (blend <= 0.0f) {
            return;
        }

        const float t = GetTimeRemainingProgress<IsCompressed>();
        if (!m_Seq->m_bHasTranslation) {
            return;
        }

        const auto* kfCurr = KeyFrameAt<IsCompressed>(m_Seq, m_KFCurr);
        const auto* kfPrev = KeyFrameAt<IsCompressed>(m_Seq, m_KFPrev);
        trans = LerpTranslation(KeyFrameTrans<IsCompressed>(kfPrev), KeyFrameTrans<IsCompressed>(kfCurr), t);
        trans.x *= blend;
        trans.y *= blend;
        trans.z *= blend;
    }

    template<bool IsCompressed>
    NOTSA_FORCEINLINE void I_GetEndTranslation(CVector& trans, float weight) {
        trans = CVector{ 0.0f, 0.0f, 0.0f };

        const auto blend = m_BlendAssoc->GetBlendAmount(weight);
        if (blend <= 0.0f) {
            return;
        }

        if (!m_Seq->m_bHasTranslation) {
            return;
        }

        // Como el original: último frame con el contador con signo, sin comprobar que haya frames.
        const auto end = KeyFrameTrans<IsCompressed>(KeyFrameAt<IsCompressed>(m_Seq, static_cast<int16>(m_Seq->m_FramesNum) - 1));
        trans.x = end.x * blend;
        trans.y = end.y * blend;
        trans.z = end.z * blend;
    }

    template<bool IsCompressed>
    NOTSA_FORCEINLINE bool I_NextKeyFrame() {
        // Como el original: contador con signo (movsx).
        const int32 numFrames = static_cast<int16>(m_Seq->m_FramesNum);
        if (numFrames <= 1) {
            return false;
        }

        // Store old KF
        m_KFPrev = m_KFCurr;

        // Find next frame, this should usually be the next one
        // Unless `m_BlendAssoc->m_TimeStep > NextKF->DeltaTime` (In which case `m_KFRemainingTime` becomes negative)
        bool looped = false;
        while (m_KFRemainingTime <= 0.0f) {
            m_KFCurr++;

            if (m_KFCurr >= numFrames) {
                // reached end of animation
                if (!m_BlendAssoc->IsLooped()) {
                    m_KFCurr--;
                    m_KFRemainingTime = 0.0f;
                    return false;
                }
                looped   = true;
                m_KFCurr = 0;
            }

            m_KFRemainingTime = KeyFrameDeltaTime<IsCompressed>(KeyFrameAt<IsCompressed>(m_Seq, m_KFCurr)) + m_KFRemainingTime;
        }

        // Como el original: anterior = actual - 1 en 16 bits y, si queda negativo, se le suma el número de frames
        // (no solo cuando el actual es 0).
        m_KFPrev = static_cast<int16>(m_KFCurr - 1);
        if (m_KFPrev < 0) {
            m_KFPrev = static_cast<int16>(m_KFPrev + numFrames);
        }

        if constexpr (IsCompressed) {
            CalcDeltasCompressed();
        } else {
            CalcDeltas();
        }

        return looped;
    }

    template<bool IsCompressed>
    NOTSA_FORCEINLINE bool I_Update(CVector& trans, CQuaternion& rot, float weight) {
        bool looped = false;

        trans = CVector{0.0f, 0.0f, 0.0f};
        rot   = CQuaternion{0.0f, 0.0f, 0.0f, 0.0f};

        if (m_BlendAssoc->IsPlaying()) {
            m_KFRemainingTime -= m_BlendAssoc->m_TimeStep;
            if (m_KFRemainingTime <= 0.0f) {
                looped = I_NextKeyFrame<IsCompressed>();
            }
        }

        const auto blend = m_BlendAssoc->GetBlendAmount(weight);
        if (blend > 0.0f) {
            const auto* kfCurr = KeyFrameAt<IsCompressed>(m_Seq, m_KFCurr);
            const auto* kfPrev = KeyFrameAt<IsCompressed>(m_Seq, m_KFPrev);

            // Calculate interpolation t
            const float t = GetTimeRemainingProgress<IsCompressed>();

            // Como el original: no exige traslación ni rotación (antes había un assert).

            // Update translation
            if (m_Seq->m_bHasTranslation) {
                trans = LerpTranslation(KeyFrameTrans<IsCompressed>(kfPrev), KeyFrameTrans<IsCompressed>(kfCurr), t);
                trans.x *= blend;
                trans.y *= blend;
                trans.z *= blend;
            }

            // Update rotation (Slerp reconstruido en C++, 0x59C300)
            if (m_Seq->m_bHasRotation) {
                const CQuaternion rotPrev = KeyFrameRot<IsCompressed>(kfPrev);
                const CQuaternion rotCurr = KeyFrameRot<IsCompressed>(kfCurr);
                rot.Slerp(rotPrev, rotCurr, m_Theta, m_InvSinTheta, t);
                rot *= blend;
            }
        }

        return looped;
    }

public:
    float                  m_Theta{};           //!< Ángulo entre los keyframes actual y anterior.
    float                  m_InvSinTheta{};     //!< 1 / sin(m_Theta), precalculado para Slerp.
    int16                  m_KFCurr{ -1 };      //!< Índice del keyframe actual.
    int16                  m_KFPrev{ -1 };      //!< Índice del keyframe anterior.
    float                  m_KFRemainingTime{}; //!< Tiempo restante antes de avanzar al siguiente keyframe.
    CAnimBlendSequence*    m_Seq{};             //!< Secuencia externa de este bone; no owned.
    CAnimBlendAssociation* m_BlendAssoc{};      //!< Asociación padre que aporta tiempo/blend; no owned.
};
VALIDATE_SIZE(CAnimBlendNode, 0x18);
