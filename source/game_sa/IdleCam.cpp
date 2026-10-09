/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CIdleCam
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "IdleCam.h"
#include "InterestingEvents.h"
#include "HandShaker.h"

auto& gIdleCam = StaticRef<CIdleCam>(0xB6FDA0);
auto& gbCineyCamProcessedOnFrame = StaticRef<uint32>(0xB6EC40);

namespace {
// Como el original: constantes exactas del EXE (no las de common.h)
const float kIdlePi       = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
const float kIdleTwoPi    = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
const float kIdleMinusPi  = std::bit_cast<float>(0xC0490FDBu); // 0x858CC0
const float kIdleOne      = 1.0f;                              // 0x858624
const float kIdleHalf     = 0.5f;                              // 0x858B8C
const float kIdle180      = 180.0f;                            // 0x85A994
const float kIdle270      = 270.0f;                            // 0x859070
const float kIdleDegToRad = std::bit_cast<float>(0x3C8EFA35u); // 0x8595EC

// Como el original (0x59C810): vector por matriz (producto escalar con las filas), sin posicion y con el orden de sumas del EXE
CVector IdleMultiply3x3_VM(const CVector& v, const CMatrix& m) {
    return {
        m.GetRight().y * v.y + m.GetRight().z * v.z + v.x * m.GetRight().x,
        m.GetForward().y * v.y + m.GetForward().x * v.x + m.GetForward().z * v.z,
        m.GetUp().y * v.y + m.GetUp().x * v.x + m.GetUp().z * v.z,
    };
}
} // namespace

void CIdleCam::InjectHooks() {
    RH_ScopedClass(CIdleCam);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetLookAtPositionOnTarget, 0x50EAE0);
    RH_ScopedInstall(Init, 0x50E6D0);
    RH_ScopedInstall(Reset, 0x50A160);
    RH_ScopedInstall(ProcessIdleCamTicker, 0x50A200);
    RH_ScopedInstall(SetTarget, 0x50A280);
    RH_ScopedInstall(FinaliseIdleCamera, 0x50E760);
    RH_ScopedInstall(SetTargetPlayer, 0x50EB50);
    RH_ScopedInstall(IsTargetValid, 0x517770);
    RH_ScopedInstall(ProcessTargetSelection, 0x517870);
    RH_ScopedInstall(ProcessSlerp, 0x5179E0);
    RH_ScopedInstall(ProcessFOVZoom, 0x517BF0);
    RH_ScopedInstall(Run, 0x51D3E0);
    RH_ScopedInstall(Process, 0x522C80);
    RH_ScopedInstall(IdleCamGeneralProcess, 0x50E690);
}

// 0x517760
CIdleCam::CIdleCam() {
    Init();
}

// 0x50E6D0
void CIdleCam::Init() {
    m_DistStartFOVZoom                         = 15.0f;
    m_ZoomNearest                              = 15.0f;
    m_SlerpDuration                            = 2000.0f;
    m_TimeControlsIdleForIdleToKickIn          = 90000.0f;
    m_TimeToConsiderNonVisibleEntityAsOccluded = 3000.0f;
    m_DistTooClose                             = 4.0f;
    m_DistTooFar                               = 80.0f;
    m_DegreeShakeIdleCam                       = 1.0f;
    m_ShakeBuildUpTime                         = 3000.0f;
    m_ZoomFarthest                             = 70.0f;
    m_DurationFOVZoom                          = 1000.0f;
    m_TargetLOSFramestoReject                  = 14;
    m_TimeBeforeNewZoomIn                      = 12000.0f;
    m_TimeMinimumToLookAtSomething             = 5000.0f;
    m_IncreaseMinimumTimeFactorForZoomedIn     = 2.0f;
    m_LastTimePadTouched                       = 0;
    m_IdleTickerFrames                         = 0;
    Reset(false);
}

// 0x50A160
void CIdleCam::Reset(bool resetControls) {
    m_PositionToSlerpFrom.Reset();
    m_LastIdlePos.Reset();
    m_TimeSlerpStarted               = -1.0f;
    m_TimeIdleCamStarted             = -1.0f;
    m_TimeLastTargetSelected         = -1.0f;
    m_ZoomFrom                       = -1.0f;
    m_ZoomTo                         = -1.0f;
    m_TimeZoomStarted                = -1.0f;
    m_TimeTargetEntityWasLastVisible = -1.0f;
    m_TimeLastZoomIn                 = -1.0f;
    m_Target                         = 0;
    m_ZoomState                      = eIdleCamZoomState::UNK_3;
    m_nForceAZoomOut                 = 0;
    m_CurFOV                         = 70.0f;
    m_TargetLOSCounter               = 0;
    m_bHasZoomedIn                   = 0;
    m_SlerpTime                      = 1.0f;

    if (resetControls) {
        CPad::GetPad()->LastTimeTouched = CTimer::GetTimeInMS();
    }
}

// 0x50A200
void CIdleCam::ProcessIdleCamTicker() {
    if (m_LastTimePadTouched == CPad::GetPad(0)->LastTimeTouched) {
        // Como el original (0x50A21D..0x50A234): (ms_fTimeStep * 0.02f) * 1000.0f, dos multiplicaciones a float, y _ftol2 (truncado a 64 bits)
        const float scaledStep = CTimer::ms_fTimeStep * 0.02f;
        const float ticks      = scaledStep * 1000.0f;
        m_IdleTickerFrames += static_cast<int32>(static_cast<int64>(ticks));
    } else {
        m_LastTimePadTouched = CPad::GetPad(0)->LastTimeTouched;
        m_IdleTickerFrames   = 0;
    }
}

// inlined
bool CIdleCam::IsItTimeForIdleCam() {
    // Always false in Android
    return static_cast<float>(m_IdleTickerFrames) > m_TimeControlsIdleForIdleToKickIn;
}

// wrong name?
// 0x50E690
void CIdleCam::IdleCamGeneralProcess() {
    if (!IsItTimeForIdleCam()) {
        g_InterestingEvents.m_b1 = false;
    }

    if (TheCamera.GetActiveCam().m_nMode != MODE_FOLLOWPED) {
        g_InterestingEvents.m_b1    = false;
        gIdleCam.m_IdleTickerFrames = 0;
    }
}

// 0x50EAE0
void CIdleCam::GetLookAtPositionOnTarget(const CEntity* target, CVector& outPos) {
    outPos = target->GetPosition();
    if (target->GetIsTypePed()) {
        switch (target->AsPed()->m_nPedType) {
        case PED_TYPE_CIVFEMALE:
        case PED_TYPE_PROSTITUTE:
            outPos.z += 0.1f;
            break;
        default:
            outPos.z += 0.5f;
            break;
        }
    }
}

// 0x517BF0
void CIdleCam::ProcessFOVZoom(float time) {
    const auto ticks = static_cast<int32>(CTimer::GetTimeInMS());
    // Como el original: FILD con signo y corrección del rango sin signo.
    const float now = ticks < 0 ? static_cast<float>(ticks) + 4294967296.0f : static_cast<float>(ticks);
    float nearest = m_ZoomNearest;
    bool shouldZoom = false;
    if (m_Target) {
        CVector targetPos;
        GetLookAtPositionOnTarget(m_Target, targetPos);
        const auto delta = m_Cam->m_vecSource - targetPos;
        // Como el original: suma z, y, x en ese orden.
        const float distance = std::sqrt((delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x);
        if (m_Target->GetIsTypePed() && (m_Target->AsPed()->m_nPedType == PED_TYPE_PROSTITUTE || m_Target->AsPed()->m_nPedType == PED_TYPE_CIVFEMALE)) {
            shouldZoom = true;
            nearest *= 0.5f;
            if (distance < 8.0f) {
                m_nForceAZoomOut = true;
            }
        }
        if (distance > m_DistStartFOVZoom) {
            shouldZoom = true;
        }
    }
    if (time >= 1.0f) {
        const auto previousState = m_ZoomState;
        bool changeTarget = false;
        if (shouldZoom) {
            if (now - m_TimeLastZoomIn > m_TimeBeforeNewZoomIn) {
                bool clear = true;
                if (m_Target) {
                    const auto oldIgnore = CWorld::pIgnoreEntity;
                    CWorld::pIgnoreEntity = m_Target;
                    CVector targetPos;
                    GetLookAtPositionOnTarget(m_Target, targetPos);
                    clear = CWorld::GetIsLineOfSightClear(m_Cam->m_vecSource, targetPos, true, false, false, true, false, false, true);
                    CWorld::pIgnoreEntity = oldIgnore;
                }
                if (m_TargetLOSCounter > 10 && m_ZoomState == eIdleCamZoomState::UNK_2) {
                    m_ZoomState = static_cast<eIdleCamZoomState>(1);
                }
                if (m_ZoomState == eIdleCamZoomState::UNK_3 && !m_bHasZoomedIn && clear) {
                    m_ZoomState = static_cast<eIdleCamZoomState>(0);
                    m_ZoomTo = nearest;
                    changeTarget = static_cast<int>(previousState) != 0;
                }
            }
        } else if (previousState == eIdleCamZoomState::UNK_2) {
            m_ZoomState = static_cast<eIdleCamZoomState>(1);
            m_ZoomTo = m_ZoomFarthest;
            changeTarget = true;
        }
        if (changeTarget) {
            m_TimeZoomStarted = now;
            m_ZoomFrom = m_CurFOV;
        }
    }
    if (m_ZoomState == eIdleCamZoomState::UNK_2) {
        m_TimeLastZoomIn = now;
        if (m_nForceAZoomOut) {
            m_TimeZoomStarted = now;
            m_ZoomFrom = m_CurFOV;
            m_ZoomState = static_cast<eIdleCamZoomState>(1);
            m_ZoomTo = m_ZoomFarthest;
        }
    }
    m_nForceAZoomOut = false;
    switch (static_cast<int>(m_ZoomState)) {
    case 0:
        if (std::fabs(m_CurFOV - nearest) < 1.0f) {
            m_ZoomState = eIdleCamZoomState::UNK_2;
            m_bHasZoomedIn = true;
            m_CurFOV = nearest;
            break;
        }
        [[fallthrough]];
    case 1:
        if (static_cast<int>(m_ZoomState) == 1 && std::fabs(m_CurFOV - m_ZoomFarthest) < 1.0f) {
            m_ZoomState = eIdleCamZoomState::UNK_3;
            m_CurFOV = m_ZoomFarthest;
            break;
        }
        {
            // Como el original: FSIN y el orden x87 exacto de la interpolación.
            const float angle = (270.0f - ((now - m_TimeZoomStarted) / m_DurationFOVZoom) * 180.0f) * std::bit_cast<float>(0x3C8EFA35u);
            const float from = m_ZoomFrom, to = m_ZoomTo, one = 1.0f, half = 0.5f;
            float result;
            __asm {
                fld angle
                fsin
                fadd one
                fmul half
                fld to
                fsub from
                fmulp st(1), st
                fadd from
                fstp result
            }
            m_CurFOV = result;
        }
        break;
    case 2: m_CurFOV = nearest; break;
    case 3: m_CurFOV = m_ZoomFarthest; break;
    }
    m_Cam->m_fFOV = m_CurFOV;
}

// 0x517770
bool CIdleCam::IsTargetValid(CEntity* target) {
    if (!target) {
        return false;
    }

    if (target == FindPlayerPed()) {
        return true;
    }
    CVector lookAtPos{};
    GetLookAtPositionOnTarget(target, lookAtPos);

    // Como el original (0x5177A4..0x5177D1): diferencias source - lookAt y suma de cuadrados en orden z, y, x
    const auto dx = m_Cam->m_vecSource.x - lookAtPos.x, dy = m_Cam->m_vecSource.y - lookAtPos.y, dz = m_Cam->m_vecSource.z - lookAtPos.z;
    const float dist = std::sqrt((dz * dz + dy * dy) + dx * dx);
    if (dist < m_DistTooClose || dist > m_DistTooFar) {
        return false;
    }

    if (m_SlerpTime < 1.0f) {
        return true;
    }

    const auto oldIgnore  = CWorld::pIgnoreEntity;
    CWorld::pIgnoreEntity = target;
    notsa::ScopeGuard _([&]{ CWorld::pIgnoreEntity = oldIgnore; });

    if (CWorld::GetIsLineOfSightClear(
        m_Cam->m_vecSource,
        lookAtPos,
        true,
        false,
        false,
        true,
        false,
        false,
        true
    )) {
        return true;
    }

    // Como el original (0x517849..0x517855): compara el valor anterior con jg -> falso solo si anterior > limite, es decir <=
    return m_TargetLOSCounter++ <= m_TargetLOSFramestoReject;
}

// 0x50A280
void CIdleCam::SetTarget(CEntity* target) {
    const auto time = static_cast<float>(CTimer::GetTimeInMS());
    if (m_Target) {
        m_PositionToSlerpFrom = m_LastIdlePos;
    } else {
        m_PositionToSlerpFrom = m_Cam->m_vecSource + m_Cam->m_vecFront;
    }

    CEntity::ChangeEntityReference(m_Target, target);

    m_TimeSlerpStarted       = time;
    m_TimeLastTargetSelected = time;
    m_TargetLOSCounter       = 0;
    m_bHasZoomedIn           = false;
}

// 0x50EB50
void CIdleCam::SetTargetPlayer() {
    SetTarget(FindPlayerPed());
    m_nForceAZoomOut = true;
}

// 0x517870
void CIdleCam::ProcessTargetSelection() {
    // Como el original (0x517876): FindPlayerPed(-1) se llama una vez al empezar y el resultado (EDI) se reutiliza en las comparaciones
    auto* const player = FindPlayerPed();
    // Como el original (0x51787F..0x5178A1): FILD carga el entero exacto (80 bits) y FSUB redondea una sola vez a 24 bits;
    // solo si es negativo como int32 hay un FADD 2^32 intermedio que ya redondea a float
    const uint32 nowMs = CTimer::GetTimeInMS();
    float timeDelta = (nowMs < 0x80000000u)
        ? static_cast<float>(static_cast<double>(nowMs) - static_cast<double>(m_TimeLastTargetSelected))
        : static_cast<float>(nowMs) - m_TimeLastTargetSelected;
    if (m_ZoomState != eIdleCamZoomState::UNK_3 && m_TargetLOSCounter <= 0) {
        timeDelta /= m_IncreaseMinimumTimeFactorForZoomedIn;
    }

    if (timeDelta > m_TimeMinimumToLookAtSomething) {
        g_InterestingEvents.InvalidateNonVisibleEvents();
        auto* event = g_InterestingEvents.GetInterestingEvent();

        if (event) {
            auto* eventEntity = event->entity;
            if (m_Target != eventEntity) {
                if (IsTargetValid(eventEntity)) {
                    if (m_ZoomState == eIdleCamZoomState::UNK_3) {
                        SetTarget(eventEntity);
                    } else {
                        m_nForceAZoomOut = true;
                    }
                }
            } else if (!IsTargetValid(eventEntity)) {
                g_InterestingEvents.InvalidateEvent(event);
            }
        } else if (!(m_Target && IsTargetValid(m_Target)) && m_Target != player) {
            // Como el original (0x5178E2..0x517905): sin objetivo valido, solo si el objetivo actual no es ya el jugador (el cacheado)
            if (m_ZoomState == eIdleCamZoomState::UNK_3) {
                SetTargetPlayer();
            } else {
                m_nForceAZoomOut = true;
            }
        }
    }

    if (!m_Target) {
        // Como el original (0x517944..0x517964): guarda el jugador directamente en m_Target (sin registrar referencia) antes de SetTarget,
        // de modo que SetTarget ve un objetivo previo (copia m_LastIdlePos y limpia la referencia) en vez de partir de source + front
        m_nForceAZoomOut = true;
        m_Target         = player;
        SetTarget(FindPlayerPed());
        m_nForceAZoomOut = true;
    }

    if (!IsTargetValid(m_Target) && timeDelta > m_TimeMinimumToLookAtSomething) {
        // Como el original (0x517983..0x5179B7): SetTarget solo si ZoomState == 3; el contador se lee DESPUES (SetTarget lo pone a 0)
        if (m_ZoomState == eIdleCamZoomState::UNK_3) {
            SetTarget(FindPlayerPed());
        }
        m_nForceAZoomOut = true;
        if (m_TargetLOSCounter > 0) {
            SetTarget(FindPlayerPed());
            m_nForceAZoomOut = true;
        }
    }

    if (m_TargetLOSCounter > m_TargetLOSFramestoReject) {
        SetTarget(FindPlayerPed());
        m_nForceAZoomOut = true;
    }
}

// 0x5179E0
float CIdleCam::ProcessSlerp(float& outX, float& outZ) {
    // Como el original: ms como float (FILD con signo + 2^32 si es negativo; redondeo unico a float)
    const float beginTime = static_cast<float>(CTimer::GetTimeInMS());

    // Como el original (0x517A09): se llama SIEMPRE y luego, si el contador alcanza el limite, se sobrescribe con m_LastIdlePos
    CVector lookAtPos;
    GetLookAtPositionOnTarget(m_Target, lookAtPos);
    if (m_TargetLOSCounter >= m_TargetLOSFramestoReject) {
        lookAtPos = m_LastIdlePos;
    }

    // Como el original (0x517A2D..0x517A8A): diferencias respecto a la fuente de la camara
    const CCam* const cam = m_Cam;
    const float d1x = m_PositionToSlerpFrom.x - cam->m_vecSource.x;
    const float d1y = m_PositionToSlerpFrom.y - cam->m_vecSource.y;
    const float d1z = m_PositionToSlerpFrom.z - cam->m_vecSource.z;
    const float d2x = lookAtPos.x - cam->m_vecSource.x;
    const float d2y = lookAtPos.y - cam->m_vecSource.y;
    const float d2z = lookAtPos.z - cam->m_vecSource.z;
    // Como el original: raiz de x*x + y*y (float por operacion; el x87 va a 24 bits)
    const float mag1 = std::sqrt(d1x * d1x + d1y * d1y);
    const float mag2 = std::sqrt(d2y * d2y + d2x * d2x);

    const uintptr_t atanFn = 0x53CC70; // CGeneral::GetATanOfXY: devuelve ST0 con precision completa y el original la usa sin redondear
    const float timeLast = m_TimeLastTargetSelected, slerpDuration = m_SlerpDuration;
    float slerpAtan{}, slerpDist{}, lookAtAtan{}, slerpT{}, resX{}, resZ{};
    // Como el original: 0x517A8E..0x517BD5, transcripcion x87. lookAtDistAtan (4a llamada) se queda en la pila sin redondear a float.
    __asm {
        push d1y
        push d1x
        call atanFn
        add esp, 8
        fadd dword ptr [kIdlePi]
        fstp slerpAtan
        push d1z
        push mag1
        call atanFn
        add esp, 8
        fstp slerpDist
        push d2y
        push d2x
        call atanFn
        add esp, 8
        fadd dword ptr [kIdlePi]
        fstp lookAtAtan
        push d2z
        push mag2
        call atanFn
        add esp, 8
        // ajuste de lookAtDistAtan: si (look - slerp) > PI resta 2PI; si no, si < -PI suma 2PI (con NaN no cambia)
        fld slerpDist
        fsubr st, st(1)
        fcom dword ptr [kIdlePi]
        fnstsw ax
        test ah, 41h
        jne L_dist_else
        fstp st(0)
        fsub dword ptr [kIdleTwoPi]
        jmp L_dist_done
    L_dist_else:
        fcomp dword ptr [kIdleMinusPi]
        fnstsw ax
        test ah, 5
        jp L_dist_done
        fadd dword ptr [kIdleTwoPi]
    L_dist_done:
        // lo mismo para lookAtAtan
        fld lookAtAtan
        fld slerpAtan
        fsubr st, st(1)
        fcom dword ptr [kIdlePi]
        fnstsw ax
        test ah, 41h
        jne L_atan_else
        fstp st(0)
        fsub dword ptr [kIdleTwoPi]
        jmp L_atan_done
    L_atan_else:
        fcomp dword ptr [kIdleMinusPi]
        fnstsw ax
        test ah, 5
        jp L_atan_done
        fadd dword ptr [kIdleTwoPi]
    L_atan_done:
        // t = min((ahora - inicio) / duracion, 1.0): solo se limita si t > 1.0 (con NaN se queda)
        fld beginTime
        fsub timeLast
        fdiv slerpDuration
        fst slerpT
        fcomp dword ptr [kIdleOne]
        fnstsw ax
        test ah, 41h
        jne L_t_done
        mov dword ptr [slerpT], 3F800000h
    L_t_done:
        // lerpT = (sin((270 - t * 180) * (PI / 180)) + 1) * 0.5 con FSIN
        fld slerpT
        fmul dword ptr [kIdle180]
        fsubr dword ptr [kIdle270]
        fmul dword ptr [kIdleDegToRad]
        fsin
        fadd dword ptr [kIdleOne]
        fmul dword ptr [kIdleHalf]
        // out = desde + (hasta - desde) * lerpT
        fxch st(2)
        fsub slerpDist
        fmul st, st(2)
        fadd slerpDist
        fstp resX
        fsub slerpAtan
        fmul st, st(1)
        fadd slerpAtan
        fstp resZ
        fstp st(0)
    }
    outX = resX;
    outZ = resZ;
    return slerpT;
}

// 0x50E760
void CIdleCam::FinaliseIdleCamera(float curAngleX, float curAngleY, float shakeDegree) {
    // Como el original: vecUp es una referencia al vector arriba de la camara (antes era una copia: `auto&a=x, b=y` solo hace referencia a la primera)
    auto &vecFwd = m_Cam->m_vecFront, &vecUp = m_Cam->m_vecUp;

    // Como el original (0x50E763..0x50E7B1): FCOS/FSIN del x87 (precision completa), cada componente se guarda como float
    float frontX{}, frontY{}, frontZ{};
    __asm {
        fld curAngleX
        fcos
        fld curAngleY
        fcos
        fmul st, st(1)
        fchs
        fstp frontX
        fld curAngleY
        fsin
        fmul st, st(1)
        fchs
        fstp frontY
        fstp st(0)
        fld curAngleX
        fsin
        fstp frontZ
    }
    vecFwd = CVector{ frontX, frontY, frontZ };
    vecFwd.Normalise(); // 0x50E7C0
    m_LastIdlePos = vecFwd + m_Cam->m_vecSource;

    // Como el original (0x50E80B..0x50E825): el temblor se pasa ya multiplicado por m_DegreeShakeIdleCam (antes se pasaba solo shakeDegree)
    auto& hs = gHandShaker[0];
    hs.Process(shakeDegree * m_DegreeShakeIdleCam);
    const float angle = hs.m_ang.z * m_DegreeShakeIdleCam * shakeDegree;
    // Como el original (0x50E855): Multiply3x3 vector * matriz (0x59C810: producto con las filas, sin posicion); antes TransformPoint (columnas + posicion)
    vecFwd = IdleMultiply3x3_VM(vecFwd, hs.m_resultMat);

    // Como el original (0x50E87D..0x50E8B2): sin/cos del x87 con precision completa
    float angleSin{}, angleCos{};
    __asm {
        fld angle
        fsin
        fstp angleSin
        fld angle
        fcos
        fstp angleCos
    }
    vecUp.Set(angleSin, 0.0f, angleCos);
    auto rightDir = CrossProduct(vecFwd, vecUp);
    rightDir.Normalise();
    vecUp = CrossProduct(rightDir, vecFwd);
    if (vecFwd.x == 0 && vecFwd.y == 0.0f) {
        vecFwd.x = vecFwd.y = std::bit_cast<float>(0x38D1B717u); // 0.0001f: 0x50E949
    }
    rightDir = CrossProduct(vecFwd, vecUp);
    rightDir.Normalise();
    vecUp = CrossProduct(rightDir, vecFwd);
    m_Cam->GetVectorsReadyForRW(); // 0x50E9D0..0x50EAD7: cuerpo de CCam::GetVectorsReadyForRW (0x509CE0) en linea
}

// 0x51D3E0
void CIdleCam::Run() {
    const auto beginTime = CTimer::GetTimeInMS();
    ProcessTargetSelection();

    float angleX{}, angleZ{};
    m_SlerpTime = ProcessSlerp(angleX, angleZ);
    ProcessFOVZoom(m_SlerpTime);

    const auto delta = beginTime - m_TimeIdleCamStarted;
    FinaliseIdleCamera(angleX, angleZ, delta < m_ShakeBuildUpTime ? delta / m_ShakeBuildUpTime : 1.f);
}

// 0x522C80
bool CIdleCam::Process() {
    ProcessIdleCamTicker();
    if (!IsItTimeForIdleCam()) {
        return false;
    }

    m_Cam = &TheCamera.GetActiveCam();
    if (m_LastFrameProcessed < CTimer::GetFrameCounter() - 1) {
        g_InterestingEvents.m_b1 = true;
        Reset(false);
        m_TimeIdleCamStarted = static_cast<float>(CTimer::GetTimeInMS());
        SetTarget(FindPlayerPed());
        m_nForceAZoomOut = true;
    }
    m_LastFrameProcessed = CTimer::GetFrameCounter();
    Run();
    gbCineyCamProcessedOnFrame = m_LastFrameProcessed;
    return true;
}
