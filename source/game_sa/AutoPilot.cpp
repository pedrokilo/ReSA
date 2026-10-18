/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAutoPilot
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AutoPilot.h"
#include "Curves.h"
#include "Timer.h"

void CAutoPilot::InjectHooks() {
    RH_ScopedClass(CAutoPilot);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RemoveOnePathNode, 0x41B950);
    RH_ScopedInstall(ModifySpeed, 0x41B980);
}

// 0x6D5E20
CAutoPilot::CAutoPilot() : m_aPathFindNodesInfo() {
    _smthNext = 1;
    _smthCurr = 1;

    m_nCarCtrlFlags = 0;

    field_C = 0;
    m_nSpeedScaleFactor = 1000;
    m_nNextLane = 0;
    m_nCurrentLane = 0;
    m_nCarDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS;
    m_nCarMission = eCarMission::MISSION_NONE;
    m_nTempAction = TEMPACT_NONE;
    SetCruiseSpeed(10);
    m_speed = 10.0F;
    m_nPathFindNodesCount = 0;
    m_TargetEntity = nullptr;

    m_nTimeToStartMission = CTimer::GetTimeInMS();
    m_nTimeSwitchedToRealPhysics = CTimer::GetTimeInMS();
    m_LastUpdateTimeMs = 0;

    m_nStraightLineDistance = 20;
    m_ucTempActionMode = 0;
    m_ucCarMissionModeCounter = 0;
    field_41 = 0;
    m_SpeedMult = 1.0f;
    m_ucHeliSpeedMult = 0;
    movementFlags.bIsStopped = false;
    movementFlags.bIsParked = false;
    field_4A = 0;
    m_ucCarFollowDist = 10;
    m_ucHeliTargetDist2 = 10;
    field_50 = CGeneral::GetRandomNumber() % 8 + 2;
    m_vehicleRecordingId = -1;
    m_bPlaneDogfightSomething = false;
    m_ObstructingEntity = nullptr;
    m_fMaxTrafficSpeed = 0.0F;
}

// (float)(a - b) con un unico redondeo (el x87 a 24 bits redondea la resta una sola vez). En double la resta ya redondea y un empate de float se resolveria "a par"
// aunque el resto exacto lo incline hacia un lado: se recupera el error de la resta (two-sum) y, solo si la suma esta en un empate de float, se empuja hacia el error.
static float SubtractRoundedOnce(double a, double b) {
    volatile double s = a - b;
    const double sum = s;
    if (!std::isfinite(sum)) {
        return (float)sum;
    }
    volatile double bb = sum - a;
    const double err = (a - (sum - (double)bb)) + (-b - (double)bb);
    if (err != 0.0) {
        const float  lo   = (float)sum;
        const float  nb   = nextafterf(lo, err > 0.0 ? INFINITY : -INFINITY);
        if (sum == ((double)lo + (double)nb) / 2.0) {
            return (float)nextafter(sum, err > 0.0 ? INFINITY : -INFINITY);
        }
    }
    return (float)sum;
}

// 0x41B980: fija la velocidad (minimo 0,01) y, si las dos areas de nodos estan cargadas, recalcula m_nSpeedScaleFactor (+0x10) y el tiempo de inicio (field_C, +0xC)
// para que el tramo ya recorrido (q = tiempo transcurrido / escala anterior) se conserve. Como el original (x87 a 24 bits): cada operacion se redondea a float,
// los enteros entran por fild (exactos) y los "negativos" de 32 bits se corrigen sumando 2^32; las conversiones a entero son _ftol2 (0x821B40, int64 truncado, se usa la parte baja).
// Las sumas/productos con enteros de 32 bits se hacen en double y se redondean una vez a float.
void CAutoPilot::ModifySpeed(float target) {
    // fcom 0,01; test ah,41h: tambien sustituye NaN
    if (!(target > 0.01f)) {
        target = 0.01f;
    }

    // 0x41B9A5..0x41B9C7: (now - field_C) como entero con signo; si es negativo se le suma 2^32 (float); luego / m_nSpeedScaleFactor (fidiv, entero con signo)
    const int32 elapsed = (int32)(CTimer::GetTimeInMS() - (uint32)field_C);
    const double elapsedD = elapsed < 0 ? (double)(float)((double)elapsed + 4294967296.0) : (double)elapsed;
    const float  q = (float)(elapsedD / (double)(int32)m_nSpeedScaleFactor);
    m_speed = target;

    const uint16 curAddr  = *(uint16*)&m_nCurrentPathNodeInfo;
    if (!(&ThePaths.m_pPathNodes[0])[curAddr >> 10]) {
        return;
    }
    const uint16 nextAddr = *(uint16*)&m_nNextPathNodeInfo;
    if (!(&ThePaths.m_pPathNodes[0])[nextAddr >> 10]) {
        return;
    }

    CCarPathLink* const linkA = &(&ThePaths.m_pNaviNodes[0])[curAddr >> 10][curAddr & 0x3FF];
    CCarPathLink* const linkB = &(&ThePaths.m_pNaviNodes[0])[nextAddr >> 10][nextAddr & 0x3FF];
    const uint8* const  a = (const uint8*)linkA;
    const uint8* const  b = (const uint8*)linkB;

    const float laneA = (float)_smthCurr;  // +0x25
    const float laneB = (float)_smthNext;  // +0x26
    const float adx = (float)(int8)a[8], ady = (float)(int8)a[9];
    const float bdx = (float)(int8)b[8], bdy = (float)(int8)b[9];

    // direcciones (0x41BA3B..0x41BAB9): (dir · 0,01) · carril
    const float aDirX = (adx * 0.01f) * laneA;
    const float aDirY = (ady * 0.01f) * laneA;
    const float bDirX = (bdx * 0.01f) * laneB;
    const float bDirY = (bdy * 0.01f) * laneB;

    // 0x44DB00 (desplazamiento del carril, CCarPathLink): primero el tramo actual, luego el siguiente
    const float offA = linkA->OneWayLaneOffset();
    const float widthA = (offA + (float)m_nCurrentLane) * 5.4f;
    const float offB = linkB->OneWayLaneOffset();
    const float widthB = (offB + (float)m_nNextLane) * 5.4f;

    CVector start, end;
    start.x = (float)((const int16*)a)[0] * 0.125f + widthA * aDirY;
    start.y = (float)((const int16*)a)[1] * 0.125f - widthA * aDirX;
    start.z = 0.0f;
    end.x   = (float)((const int16*)b)[0] * 0.125f + widthB * bDirY;
    end.y   = (float)((const int16*)b)[1] * 0.125f - widthB * bDirX;
    end.z   = 0.0f;

    const float scale = CCurves::CalcSpeedScaleFactor(start, end, aDirX, aDirY, bDirX, bDirY);
    // fld 1000; fdiv speed; fmulp
    const int32 newScale = (int32)(int64)(float)((double)scale * (double)(float)(1000.0f / m_speed));
    m_nSpeedScaleFactor = (uint32)newScale;

    // 0x41BC3B..0x41BC5F: (now [+2^32 si < 0]) - newScale · q, truncado
    const int32  nowS   = (int32)CTimer::GetTimeInMS();
    const double nowD   = nowS < 0 ? (double)(float)((double)nowS + 4294967296.0) : (double)nowS;
    const float  prod   = (float)((double)newScale * (double)q);
    field_C = (int32)(int64)SubtractRoundedOnce(nowD, (double)prod);
}

// 0x41B950: quita el primer nodo de la ruta (m_aPathFindNodesInfo, +0x68): decrementa el contador de 16 bits (+0x88) y, si queda positivo (con signo), desplaza
// cada nodo una posicion hacia el principio. Como el original, el indice es un int16 y el contador se vuelve a leer en cada vuelta: con un contador mayor que 8 la
// copia llega a escribir sobre el propio contador (+0x88), asi que la lectura es volatile y los accesos son por direccion (sin comprobar limites de std::array).
// Ninguno de sus 9 llamadores del EXE lee EAX/ECX/EDX a la salida.
void CAutoPilot::RemoveOnePathNode() {
    auto* const base = reinterpret_cast<uint8*>(this);
    volatile int16* const count = reinterpret_cast<volatile int16*>(base + 0x88);

    --m_nPathFindNodesCount;
    int16 i = 0;
    if (*count > 0) {
        do {
            const int32 slot = i;
            const uint32 next = *reinterpret_cast<const uint32*>(base + 0x6C + 4 * slot);
            ++i;
            *reinterpret_cast<uint32*>(base + 0x68 + 4 * slot) = next;
        } while (i < *count);
    }
}

void CAutoPilot::SetCarMission(eCarMission carMission, uint32 timeOffsetMs) {
    m_nCarMission = carMission;
    m_nTimeToStartMission = CTimer::GetTimeInMS() + timeOffsetMs;
}

// notsa
void CAutoPilot::SetTempAction(eAutoPilotTempAction action, uint32 durMs) {
    m_nTempAction = action;
    m_nTempActionTime = CTimer::GetTimeInMS() + durMs;
}
