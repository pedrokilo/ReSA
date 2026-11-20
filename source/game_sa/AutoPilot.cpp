/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAutoPilot
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

#include "AutoPilot.h"
#include "Curves.h"
#include "Timer.h"

namespace {
// 0x463490 (lote 222A, sin validar; el listado decia CEntity; thiscall sobre el CAutoPilot (+0x390 del CVehicle), un argumento de pila leido como BYTE, `ret 4`, 21 bytes: `mov al,[ecx+2Ah]; cmp al,39h; je ..;
// cmp al,3Ah; je ..; mov al,[esp+4]; mov [ecx+2Ah],al; ret 4`): cambia la mision (m_nCarMission, +0x2A) SALVO que la actual sea MISSION_PLANE_CRASH_AND_BURN (0x39) o MISSION_HELI_CRASH_AND_BURN (0x3A): en
// esos casos no escribe nada. Del argumento solo cuenta el byte bajo. Llamador del EXE: 0x467EEC (comando de script sobre el piloto automatico de un vehiculo del pool); no lee EAX/ECX/EDX despues.
static_assert(offsetof(CAutoPilot, m_nCarMission) == 0x2A);
// 0x463490
__declspec(noinline) void __fastcall AutoPilotSetCarMissionUnlessCrashAndBurn(CAutoPilot* self, void* /*edx*/, uint32 mission) {
    uint8* const current = reinterpret_cast<uint8*>(self) + 0x2A;
    if (*current == 0x39u || *current == 0x3Au) {
        return;
    }
    *current = uint8(mission);
}
} // namespace

void CAutoPilot::InjectHooks() {
    RH_ScopedClass(CAutoPilot);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x6D5E20); // lote 195A (sin validar)
    RH_ScopedInstall(RemoveOnePathNode, 0x41B950);
    RH_ScopedInstall(ModifySpeed, 0x41B980);
    RH_ScopedNamedGlobalInstall(AutoPilotSetCarMissionUnlessCrashAndBurn, "SetCarMissionUnlessCrashAndBurn", 0x463490); // lote 222A (sin validar)
}

// 0x6D5E20 (lote 195A, sin validar): constructor de CAutoPilot del EXE (thiscall, sin argumentos de pila, `ret`, 227 bytes; lo llama el constructor de CVehicle 0x6D5F10). Escribe SOLO estos campos, en el
// ancho que indica el EXE (los demas conservan lo que hubiera en la memoria):
//  - el area (WORD +0, +4 y +8) de los tres CNodeAddress y los tres WORD de CCarPathLinkAddress (+0x14, +0x16, +0x18) a 0xFFFF y el area de los ocho nodos de la ruta (WORD +0x68 + 4*i); el identificador de nodo NO se escribe;
//  - +0x25 y +0x26 a 1; +0x4B (carCtrlFlags) = valor & 0xFC & 7, es decir solo conserva el bit 2; +0x4C (movementFlags) &= 0xFC;
//  - +0xC y +0x30 y +0x3C y +0x8C y +0x90 a 0 (dwords), +0x10 = 1000, +0x38 = 10.0f, +0x44 = 1.0f; +0x1C y +0x20 = CTimer::m_snTimeInMilliseconds (una sola lectura);
//  - bytes: +0x27, +0x28, +0x29, +0x2A, +0x2B, +0x34, +0x35, +0x41, +0x49, +0x4A, +0x95 a 0; +0x40 = 10; +0x4D = 20; +0x4E y +0x4F = 10; +0x94 = 0xFF; WORD +0x88 = 0;
//  - +0x50 = (rand() & 7) + 2 con el rand() del CRT DEL EXE (0x821B1E; avanza la semilla del juego, el de la DLL no), llamado despues de +0x4C y antes de +0x90.
// No se tocan +0x24, +0x2C, +0x36, +0x42, +0x48, +0x51..+0x5B, el vector de destino (+0x5C), +0x8A ni +0x96.
static_assert(offsetof(CAutoPilot, m_nCurrentPathNodeInfo) == 0x14 && offsetof(CAutoPilot, m_nPreviousPathNodeInfo) == 0x18 && offsetof(CAutoPilot, m_nTimeToStartMission) == 0x1C);
static_assert(offsetof(CAutoPilot, _smthCurr) == 0x25 && offsetof(CAutoPilot, m_nCurrentLane) == 0x27 && offsetof(CAutoPilot, m_nTempAction) == 0x2B && offsetof(CAutoPilot, m_LastUpdateTimeMs) == 0x30);
static_assert(offsetof(CAutoPilot, m_speed) == 0x38 && offsetof(CAutoPilot, m_nCruiseSpeed) == 0x40 && offsetof(CAutoPilot, m_SpeedMult) == 0x44 && offsetof(CAutoPilot, m_nCarCtrlFlags) == 0x4B);
static_assert(offsetof(CAutoPilot, m_nMovementFlags) == 0x4C && offsetof(CAutoPilot, m_ucHeliTargetDist2) == 0x4F && offsetof(CAutoPilot, field_50) == 0x50 && offsetof(CAutoPilot, m_aPathFindNodesInfo) == 0x68);
static_assert(offsetof(CAutoPilot, m_nPathFindNodesCount) == 0x88 && offsetof(CAutoPilot, m_TargetEntity) == 0x8C && offsetof(CAutoPilot, m_ObstructingEntity) == 0x90 && offsetof(CAutoPilot, m_vehicleRecordingId) == 0x94);

void CAutoPilot::InitialiseAsExe() {
    m_currentAddress.m_wAreaId = 0xFFFF;
    m_startingRouteNode.m_wAreaId = 0xFFFF;
    m_endingRouteNode.m_wAreaId = 0xFFFF;
    *reinterpret_cast<uint16*>(&m_nCurrentPathNodeInfo)  = 0xFFFF;
    *reinterpret_cast<uint16*>(&m_nNextPathNodeInfo)     = 0xFFFF;
    *reinterpret_cast<uint16*>(&m_nPreviousPathNodeInfo) = 0xFFFF;
    for (auto& node : m_aPathFindNodesInfo) {
        node.m_wAreaId = 0xFFFF;
    }

    _smthNext = 1;
    _smthCurr = 1;
    m_nCarCtrlFlags = static_cast<uint8>(m_nCarCtrlFlags & 0xFC & 7); // Como el original: and al,0FCh ... and al,7 (solo sobrevive el bit 2)
    field_C = 0;
    m_nSpeedScaleFactor = 1000;
    m_nNextLane = 0;
    m_nCurrentLane = 0;
    m_nCarDrivingStyle = DRIVING_STYLE_STOP_FOR_CARS;
    m_nCarMission = MISSION_NONE;
    m_nTempAction = TEMPACT_NONE;
    m_nCruiseSpeed = 10;
    m_speed = 10.0F;
    m_nPathFindNodesCount = 0;
    m_TargetEntity = nullptr;

    const uint32 timeMs = CTimer::GetTimeInMS();
    m_nTimeToStartMission = timeMs;
    m_nTimeSwitchedToRealPhysics = timeMs;
    field_41 = 0;
    m_SpeedMult = 1.0f;
    m_vehicleRecordingId = -1;
    m_ucCarMissionModeCounter = 0;
    m_nStraightLineDistance = 20;
    m_LastUpdateTimeMs = 0;
    m_ucTempActionMode = 0;
    m_ucHeliSpeedMult = 0;
    field_4A = 0;
    m_ucCarFollowDist = 10;
    m_ucHeliTargetDist2 = 10;
    m_bPlaneDogfightSomething = false;
    m_nMovementFlags = static_cast<uint8>(m_nMovementFlags & 0xFC);
    const auto exeRand = reinterpret_cast<int32(__cdecl*)()>(0x821B1E);
    field_50 = static_cast<char>((exeRand() & 7) + 2);
    m_ObstructingEntity = nullptr;
    m_fMaxTrafficSpeed = 0.0F;
}

// 0x6D5E20 (hook): ver InitialiseAsExe; devuelve this (`mov eax,esi`).
CAutoPilot* CAutoPilot::Constructor() {
    InitialiseAsExe();
    return this;
}

// Constructor de C++ (lo usan los miembros de CVehicle): los miembros CNodeAddress/CCarPathLinkAddress ya se construyen con identificador -1 antes del cuerpo (el EXE solo escribe el area); el resto es el cuerpo del EXE.
CAutoPilot::CAutoPilot() : m_aPathFindNodesInfo() {
    InitialiseAsExe();
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
