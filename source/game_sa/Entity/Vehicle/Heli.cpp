/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CHeli
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
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
#include "X87Math.h"
#include "Ropes.h"
#include "Population.h"
#include "WaterLevel.h"
#include "Stats.h"
#include "Explosion.h"
#include "FireManager.h"
#include "Darkel.h"
#include "VisibilityPlugins.h"
#include "CarCtrl.h"
#include "Wanted.h"
#include "Streaming.h"
#include "Camera.h"
#include "PlayerInfo.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskComplexUseSwatRope.h"
#include "Tasks/TaskTypes/TaskComplexWanderCop.h"

extern "C" double __cdecl GeneralRandomInRangeDblCpp(int random, float min, float max); // X87Math.cpp (0x41BD90 sin el redondeo a float final)

namespace {
// rand() del CRT del EXE (0x821B1E: avanza la semilla del juego; la rand() de la DLL no).
int32 HeliExeRand() {
    return reinterpret_cast<int32(__cdecl*)()>(0x821B1E)();
}

// Acceso crudo a un campo del EXE cuando el ancho de la escritura es parte del comportamiento (bytes y palabras sueltas).
template<class T>
T& HeliField(void* object, size_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(object) + offset);
}
} // namespace

// Envoltorio con la ABI del EXE de CHeli::FindSwatPositionRelativeToHeli (0x6C4760: thiscall con puntero oculto, ret 8). El EXE no toca EDX (el llamador de 0x6C7844 lo
// conserva a traves de la llamada), deja EAX = puntero oculto y ECX = this en los casos 0..3 o 0 en el defecto. Termina en ResidueRegsCpp, que no toca registros.
#pragma runtime_checks("", off)
static uint32 __declspec(safebuffers) __fastcall FindSwatPositionRelativeToHeliExeAbi(CHeli* self, uint32 entryEdx, CVector* result, int32 swatNumber) {
    *result = self->FindSwatPositionRelativeToHeli(swatNumber);
    return ResidueRegsCpp(static_cast<uint32>(swatNumber) <= 3u ? static_cast<uint32>(reinterpret_cast<uintptr_t>(self)) : 0u, entryEdx, static_cast<uint32>(reinterpret_cast<uintptr_t>(result)));
}
#pragma runtime_checks("", restore)

void CHeli::InjectHooks() {
    RH_ScopedVirtualClass(CHeli, 0x871680, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6C4190);
    RH_ScopedInstall(InitHelis, 0x6C4560);
    RH_ScopedInstall(AddHeliSearchLight, 0x6C45B0);
    RH_ScopedInstall(Pre_SearchLightCone, 0x6C4650);
    RH_ScopedInstall(Post_SearchLightCone, 0x6C46E0);
    RH_ScopedInstall(SwitchPoliceHelis, 0x6C4800);
    RH_ScopedInstall(RenderAllHeliSearchLights, 0x6C7C50);
    RH_ScopedInstall(TestSniperCollision, 0x6C6890);
    RH_ScopedInstall(GenerateHeli, 0x6C6520);
    RH_ScopedInstall(SendDownSwat, 0x6C69C0);
    RH_ScopedNamedGlobalInstall(FindSwatPositionRelativeToHeliExeAbi, "FindSwatPositionRelativeToHeli", 0x6C4760);
    RH_ScopedVMTInstall(Render, 0x6C4400);
    RH_ScopedVMTInstall(BlowUpCar, 0x6C6D30);
    RH_ScopedVMTInstall(Fix, 0x6C4530);
    RH_ScopedVMTInstall(BurstTyre, 0x6C4330);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6C4320);
}

// 0x6C4190: thiscall, ECX = this, (modelIndex, createdBy), `ret 8`; llama a CAutomobile::CAutomobile(modelIndex, createdBy, true) (0x6B0A90), fija el tipo y los campos del helicoptero y deja
// EAX = this (lo leen los llamadores del EXE; ECX/EDX no). Hook `Constructor` como el de CProjectile; Plane.cpp ya no lo llama por direccion.
CHeli::CHeli(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, true) {
    m_nVehicleSubType = VEHICLE_TYPE_HELI;

    m_fLeftRightSkid           = 0.0f;
    m_fSteeringUpDown          = 0.0f;
    m_fSteeringLeftRight       = 0.0f;
    m_fAccelerationBreakStatus = 0.0f;

    field_99C = 0;
    m_fRotorZ = 0;
    m_fSecondRotorZ = 0;

    m_fMinAltitude = 10.0f;
    m_fMaxAltitude = 10.0f;

    field_9AC = 10.0f;
    field_9B4 = 0;

    m_nHeliFlags = m_nHeliFlags & 0xFC;
    m_fSearchLightIntensity = 0.0f;
    physicalFlags.bDontCollideWithFlyers = true;

    if (modelIndex == MODEL_HUNTER) {
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        m_doors[DOOR_LEFT_FRONT].Init(std::bit_cast<float>(0x3F71463Bu) /* 0x6C4254: no es (3*PI)/10 en float */, 0.0f, DOOR_AXIS_NEG_X, DOOR_AXIS_Y, DOOR_EXTRA_BASED);
    }

    m_nNumSwatOccupants = 4;
    m_aSwatState.fill(0);

    m_nSearchLightTimer = CTimer::GetTimeInMS();

    // el EXE pone a cero 0x9C0..0x9EF (field_9C0, f9CC, f9D0, la lista de particulas y los dos historiales)
    field_9C0 = CVector{ 0.0f, 0.0f, 0.0f };
    f9CC = 0;
    f9D0 = 0;
    m_pParticlesList = nullptr;
    m_aSearchLightHistoryX.fill(0.0f);
    m_aSearchLightHistoryY.fill(0.0f);

    m_nShootTimer = 0;
    m_nPoliceShoutTimer = CTimer::GetTimeInMS();

    vehicleFlags.bNeverUseSmallerRemovalRange = true; // 0x6C42BD
    m_autoPilot.m_ucHeliTargetDist2 = 10;

    m_ppGunflashFx = nullptr;
    m_nFiringMultiplier = 16;

    field_9B8 = 0;
    m_bSearchLightEnabled = false;
    // rand() del CRT del EXE (0x821B1E: avanza la semilla del juego; la rand() de la DLL no) y el lerp x87 del EXE (rnd * (1/32767) * 6.0 + 2.0, sin redondear antes del fstp)
    field_A14 = static_cast<float>(GeneralRandomInRangeDblCpp(reinterpret_cast<int32(__cdecl*)()>(0x821B1E)(), 2.0f, 8.0f));
}

// 0x6C4340
CHeli::~CHeli() {
    if (m_ppGunflashFx) {
        for (auto i = 0; i < CVehicle::GetPlaneNumGuns(); i++) {
            if (auto& fx = m_ppGunflashFx[i]) {
                fx->Kill();
                g_fxMan.DestroyFxSystem(fx);
            }
        }
        delete[] m_ppGunflashFx;
        m_ppGunflashFx = nullptr;
    }

    m_vehicleAudio.Terminate();
}

// 0x6C4560
void CHeli::InitHelis() {
    std::ranges::fill(pHelis, nullptr);
    // Como el original: el bucle del EXE termina en 0xC1CA30, así que solo inicializa los dos primeros focos
    for (auto i = 0u; i < 2u; i++) {
        HeliSearchLights[i].Init();
    }
    NumberOfSearchLights = 0;
    bPoliceHelisAllowed = true;
}

// 0x6C45B0
void CHeli::AddHeliSearchLight(const CVector& origin, const CVector& target, float targetRadius, float power, uint32 coronaIndex, uint8 unknownFlag, uint8 drawShadow) {
    auto& light = HeliSearchLights.data()[NumberOfSearchLights]; // Como el original: sin comprobar el límite

    light.m_vecOrigin     = origin;
    light.m_vecTarget     = target;
    light.m_fTargetRadius = targetRadius;
    light.m_fPower        = power;
    light.m_nCoronaIndex  = coronaIndex;
    light.field_24        = unknownFlag;
    light.m_bDrawShadow   = drawShadow;

    NumberOfSearchLights += 1;
}

// 0x6C4640
void CHeli::PreRenderAlways() {
    // NOP
}

// 0x6C4650
void CHeli::Pre_SearchLightCone() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,        RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,            RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATEREQUAL));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0));
}

// 0x6C46E0
void CHeli::Post_SearchLightCone() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLBACK));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(2u));
}

// 0x6C4750
void CHeli::SpecialHeliPreRender() {
    // NOP
}

// 0x6C4760
CVector CHeli::FindSwatPositionRelativeToHeli(int32 swatNumber) {
    // Offsets of the swat ropes: the EXE compares the index as unsigned (`cmp eax,3; ja`) and jumps through the table 0x6C47F0, which is in reverse order.
    switch (static_cast<uint32>(swatNumber)) {
    case 0:
        return { -1.2f, -1.0f, -0.5f };
    case 1:
        return { 1.2f,  -1.0f, -0.5f };
    case 2:
        return { -1.2f, 1.0f,  -0.5f };
    case 3:
        return { 1.2f,  1.0f,  -0.5f };
    default:
        return { 0.0f,  0.0f,  0.0f  };
    }
}

// 0x6C4800
void CHeli::SwitchPoliceHelis(bool enable) {
    bPoliceHelisAllowed = enable;
}

// 0x6C58E0
void CHeli::SearchLightCone(int32 coronaIndex,
                            CVector origin,
                            CVector target,
                            float targetRadius,
                            float power,
                            uint8 unknownFlag,
                            uint8 drawShadow,
                            CVector& useless0,
                            CVector& useless1,
                            CVector& useless2,
                            bool a11,
                            float baseRadius,
                            float a13,
                            float a14,
                            float a15
) {
    ((void(__cdecl*)(int32, CVector, CVector, float, float, uint8, uint8, CVector&, CVector&, CVector&, bool, float, float, float, float))0x6C58E0)(coronaIndex, origin, target, targetRadius, power, unknownFlag, drawShadow, useless0, useless1, useless2, a11, baseRadius, a13, a14, a15);
}

// Disposicion que usan GenerateHeli, SendDownSwat y BlowUpCar (comprobada en compilacion)
VALIDATE_OFFSET(CVehicle, m_autoPilot, 0x390);
VALIDATE_OFFSET(CAutoPilot, m_nCarMission, 0x2A);
VALIDATE_OFFSET(CAutoPilot, m_nCruiseSpeed, 0x40);
VALIDATE_OFFSET(CAutoPilot, m_TargetEntity, 0x8C);
VALIDATE_OFFSET(CHeli, m_fMaxAltitude, 0x9A8);
VALIDATE_OFFSET(CHeli, m_fMinAltitude, 0x9B0);
VALIDATE_OFFSET(CHeli, m_nNumSwatOccupants, 0x9B9);
VALIDATE_OFFSET(CHeli, m_aSwatState, 0x9BA);
VALIDATE_OFFSET(CAutomobile, m_fHeliRotorSpeed, 0x84C);
VALIDATE_OFFSET(CAutomobile, m_aCarNodes, 0x648);
VALIDATE_OFFSET(CVehicle, m_fHealth, 0x4C0);
VALIDATE_OFFSET(CVehicle, m_nTimeWhenBlowedUp, 0x4D8);
VALIDATE_OFFSET(CVehicle, m_wBombTimer, 0x4DE);
VALIDATE_OFFSET(CPhysical, m_vecMoveSpeed, 0x44);
VALIDATE_OFFSET(CPhysical, m_vecTurnSpeed, 0x50);
VALIDATE_OFFSET(CPlayerInfo, m_nHavocCaused, 0x140);
VALIDATE_OFFSET(CPlayerInfo, m_fCurrentChaseValue, 0x148);

// 0x6C6520: cdecl (objetivo, bool). Reserva con CVehicle::operator new (0x6E2D50) y construye con 0x6C4190 (modelo, 4). Solo se comprueba el nulo de la reserva (como el EXE, que se cae despues al usar el
// helicoptero si el pool esta agotado). El bucle sobre pHelis de 0x6C6790 no tiene efecto (su resultado no se usa) y no se reproduce; el llamador (0x6C7ACF) es quien registra el helicoptero.
CHeli* CHeli::GenerateHeli(CPed* target, bool newsHeli) {
    void* const storage = CVehicle::operator new(0xA18u); // 0x6E2D50: el tamano no se consulta
    CHeli* const heli = storage ? ::new (storage) CHeli(newsHeli ? 0x1E8 : 0x1F1, PERMANENT_VEHICLE) : nullptr; // sin `{}`: el EXE no escribe el relleno

    // Posicion base: matriz + 0x30 o posicion compacta (+4); copia cruda de tres palabras
    CVector base = target->GetPosition();
    float angle = static_cast<float>(HeliExeRand() & 0xFF) * std::bit_cast<float>(0x3CC8F5C3u); // 0x859C44
    float px = TrigCompoundCosMulAdd(angle, 250.0f, base.x); // fcos con precision completa, * 250.0 (0x859F80) + base, guardado como float
    float py = TrigCompoundSinMulAdd(angle, 250.0f, base.y);

    // Como el original: fuera de [-3000, 3000] (0x859A90/94) en x o y se repite con angulo + pi; un NaN cuenta como dentro
    if (px < -3000.0f || px > 3000.0f || py < -3000.0f || py > 3000.0f) {
        angle = angle + std::bit_cast<float>(0x40490FDBu); // 0x858CB8
        base = target->GetPosition();
        px = TrigCompoundCosMulAdd(angle, 250.0f, base.x);
        py = TrigCompoundSinMulAdd(angle, 250.0f, base.y);
    }

    float candidateZ = base.z + 50.0f; // 0x858B40
    const float toX = px - (-2322.0f); // 0x8717C0
    const float toY = py - (-1653.0f); // 0x8717BC
    float x = px;
    float y = py;
    if (std::sqrt(toY * toY + toX * toX) < 350.0f) { // 0x858A4C: cerca del Monte Chiliad se usa la posicion del objetivo
        x = base.x;
        y = base.y;
        const float lifted = base.z + 200.0f; // 0x858A48
        candidateZ = lifted > 560.0f ? lifted : 560.0f; // 0x8717B8; un NaN da 560
    }
    const float lowest = CWorld::FindGroundZForCoord(x, y) + 20.0f; // 0x858BA4
    const float z = candidateZ > lowest ? candidateZ : lowest;
    heli->m_matrix->SetTranslate(CVector{ x, y, z }); // 0x59AF40

    heli->vehicleFlags.bIsLocked = true;
    heli->SetStatus(STATUS_PHYSICS);
    if (newsHeli) {
        heli->m_autoPilot.m_nCarMission = MISSION_HELI_NEWS_BEHAVIOUR;
        heli->m_autoPilot.m_TargetEntity = reinterpret_cast<CVehicle*>(FindPlayerPed(-1)); // el EXE llama y despues lo sobrescribe
        heli->m_autoPilot.m_nCruiseSpeed = 0x23;
    } else {
        heli->m_autoPilot.m_nCarMission = MISSION_HELI_POLICE_BEHAVIOUR;
        heli->m_autoPilot.m_nCruiseSpeed = 0x46;
    }
    heli->m_autoPilot.m_TargetEntity = reinterpret_cast<CVehicle*>(target);
    heli->m_fMaxAltitude = newsHeli ? 30.0f : 20.0f;
    heli->m_fMinAltitude = newsHeli ? 27.0f : 12.0f;
    heli->m_fHeliRotorSpeed = std::bit_cast<float>(0x3E28F5C3u);

    // Orientacion: rumbo = angulo + pi guardado como float; right = (sin, -cos, 0), forward = (cos, sin, 0), up = (0, 0, 1)
    const float heading = angle + std::bit_cast<float>(0x40490FDBu);
    CMatrix& matrix = *heli->m_matrix;
    const float sine = TrigSimpleSinCpp(heading);
    const float cosine = TrigSimpleCosCpp(heading);
    matrix.GetRight() = CVector{ sine, -cosine, 0.0f };
    matrix.GetForward() = CVector{ cosine, sine, 0.0f };
    matrix.GetUp() = CVector{ 0.0f, 0.0f, 1.0f };

    CWorld::Add(heli); // 0x563220
    heli->SetUpDriver(-1, false, false); // 0x6D1A50
    return heli;
}

// 0x6C6890
bool CHeli::TestSniperCollision(CVector* origin, CVector* target) {
    const CVector point = *target - *origin;

    // Como el original: (z·z + x·x) + y·y y solo sigue si z < mitad estricto (NaN termina)
    if (!(point.z < std::sqrt(point.z * point.z + point.x * point.x + point.y * point.y) * 0.5f)) {
        return false;
    }

    bool hit = false;
    for (auto& heli : pHelis) {
        if (!heli || heli->physicalFlags.bBulletProof)
            continue;

        // Como el original: MultiplyMatrixWithVector (0x59C890) con su orden de sumas
        const CMatrix& m = *heli->m_matrix;
        const CVector v{ -0.43f, 1.49f, 1.5f };
        const CVector rotorPos{
            m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
            m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
            m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
        };
        if (CCollision::DistToLine(*origin, *target, rotorPos) < 0.8f) {
            heli->m_fRotationBalance = (float)(CGeneral::GetRandomNumber() < 16383) * 0.1f - 0.05f; // [-0.05, 0.05]
            heli->BlowUpCar(FindPlayerPed(), false);
            heli->m_nNumSwatOccupants = 0;
            hit = true;
        }
    }
    return hit;
}

// 0x6C69C0: thiscall sin argumentos; el unico llamador (0x6C7801) ignora el resultado. Baja una cuerda y un SWAT cuando el helicoptero esta cerca del objetivo, casi parado y sobre el suelo.
// El EXE copia la matriz del helicoptero (0x59BCF0) y la destruye al salir (0x59ACD0): no se reproduce porque nadie la modifica entre medias. Con la matriz nula el EXE se cae en esa copia.
bool CHeli::SendDownSwat() {
    CEntity* const target = reinterpret_cast<CEntity*>(m_autoPilot.m_TargetEntity); // +0x41C
    const CVector targetPos = target->GetPosition();

    if (m_nNumSwatOccupants == 0 || physicalFlags.bSubmergedInWater) { // byte +0x9B9; bit 8 de +0x40
        return false;
    }
    if (CStreaming::ms_aInfoForModel.data()[285].m_LoadState != LOADSTATE_LOADED) { // 0x8E6314: modelo SWAT (285) cargado
        return false;
    }
    if ((HeliExeRand() & 0x7F) != 0) { // test al,7Fh: solo 1 de cada 128
        return false;
    }

    const CVector heliPos = GetPosition();
    const float dx = heliPos.x - targetPos.x;
    const float dy = heliPos.y - targetPos.y;
    const float dz = heliPos.z - targetPos.z;
    if (std::sqrt((dx * dx + dy * dy) + dz * dz) > 50.0f) { // 0x858B40; un NaN pasa
        return false;
    }
    if (std::sqrt((m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z) > 0.1f) { // CVector::Magnitude (0x4082C0) contra 0x858B1C
        return false;
    }

    // Posicion de la cuerda: rotacion de la matriz (0x59C790) por la posicion relativa y suma de la posicion del helicoptero (0x411A00)
    const CVector relative = FindSwatPositionRelativeToHeli(static_cast<int32>(static_cast<uint8>(m_nNumSwatOccupants)) - 1);
    const CVector heliPosNow = GetPosition();
    float world[3];
    HeliSwat74WorldPosCpp(reinterpret_cast<const float*>(m_matrix), &relative.x, &heliPosNow.x, world);
    const CVector ropePos{ world[0], world[1], world[2] };

    const float groundZ = CWorld::FindGroundZFor3DCoord(ropePos, nullptr, nullptr); // 0x5696C0
    float difference = targetPos.z - groundZ;
    if (difference < 0.0f) { // 0x858B50
        difference = -difference;
    }
    if (!(difference < 2.5f)) { // 0x858FA0
        return false;
    }

    float waterLevel;
    if (CWaterLevel::GetWaterLevelNoWaves(ropePos, &waterLevel, nullptr, nullptr) && !(waterLevel < groundZ)) { // 0x6E8580: el agua debe quedar por debajo del suelo
        return false;
    }

    // Como el original: el resultado de RegisterRope (bool 0/1) se compara con signo y nunca es negativo
    if (static_cast<int32>(CRopes::RegisterRope(GetRopeId(), static_cast<uint32>(eRopeType::SWAT), ropePos, false, 0, 0, nullptr, 20000)) < 0) {
        return false;
    }

    CPed* const ped = CPopulation::AddPed(PED_TYPE_COP, static_cast<eModelID>(3), ropePos, true); // 0x612710

    // Secuencia: bajar por la cuerda y despues merodear como policia. Cada reserva se comprueba contra nulo y se construye sin `{}`.
    void* const sequenceMemory = CTask::operator new(sizeof(CTaskComplexSequence)); // 0x40
    CTaskComplexSequence* const sequence = sequenceMemory ? ::new (sequenceMemory) CTaskComplexSequence : nullptr;
    void* const ropeMemory = CTask::operator new(sizeof(CTaskComplexUseSwatRope)); // 0x1C
    CTaskComplexUseSwatRope* const ropeTask = ropeMemory ? ::new (ropeMemory) CTaskComplexUseSwatRope(GetRopeId(), this) : nullptr;
    sequence->AddTask(ropeTask); // 0x632D10
    void* const wanderMemory = CTask::operator new(sizeof(CTaskComplexWanderCop)); // 0x50
    CTaskComplexWanderCop* wanderTask = nullptr;
    if (wanderMemory) {
        const int32 direction = reinterpret_cast<int32(__cdecl*)(int32, int32)>(0x407180)(0, 8); // CGeneral::GetRandomNumberInRange_int32
        wanderTask = ::new (wanderMemory) CTaskComplexWanderCop(static_cast<eMoveState>(4), static_cast<uint8>(direction));
    }
    sequence->AddTask(wanderTask);
    // CTaskManager::SetTask (0x681AF0) esta definido en linea en la cabecera: se llama por la direccion del EXE, como el original
    reinterpret_cast<void(__thiscall*)(CTaskManager*, CTask*, ePrimaryTasks, bool)>(0x681AF0)(&ped->GetTaskManager(), sequence, TASK_PRIMARY_PRIMARY, false);

    ped->m_bUsesCollision = false;
    m_nNumSwatOccupants = static_cast<int8>(m_nNumSwatOccupants - 1);
    m_aSwatState.data()[static_cast<uint8>(m_nNumSwatOccupants)] = 0xAA; // indice sin signo del valor nuevo, sin comprobar el limite
    CAnimManager::BlendAnimation(reinterpret_cast<RpClump*>(ped->GetRwObject()), static_cast<AssocGroupId>(0), static_cast<AnimationId>(0x98), 4.0f); // 0x4D4610
    return true;
}

// 0x6C79A0
void CHeli::UpdateHelis() {
    ZoneScoped;

    ((void(__cdecl*)())0x6C79A0)();
}

// 0x6C7C50
void CHeli::RenderAllHeliSearchLights() {
    ZoneScoped;

    // Como el original: solo los focos añadidos este fotograma (contador de 8 bits)
    for (uint8 i = 0; i < NumberOfSearchLights; i++) {
        auto& light = HeliSearchLights.data()[i];
        SearchLightCone(
            light.m_nCoronaIndex,
            light.m_vecOrigin,
            light.m_vecTarget,
            light.m_fTargetRadius,
            light.m_fPower,
            light.field_24,
            light.m_bDrawShadow,
            light.m_vecUseless[0],
            light.m_vecUseless[1],
            light.m_vecUseless[2],
            false,
            0.05f,
            0.0f,
            0.0f,
            1.0f
        );
    }
}

// 0x6C6D30: thiscall virtual (ranura 41 de la vtabla 0x871680), `ret 8`. bHideExplosion no se lee: el EXE pasa 0 a AddExplosion.
void CHeli::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    if (!vehicleFlags.bCanBeDamaged) { // bit 5 del byte +0x42A
        return;
    }
    const bool rcModel = m_nModelIndex == 0x1D1 || m_nModelIndex == 0x1F5; // RC Raider / RC Goblin
    if (GetStatus() != STATUS_PLAYER) {
        if (m_autoPilot.m_nCarMission != MISSION_HELI_CRASH_AND_BURN && !rcModel) {
            m_autoPilot.m_nCarMission = MISSION_HELI_CRASH_AND_BURN; // byte +0x3BA = 0x3A
            m_fHealth = 0.0f;
            return;
        }
    }

    // Como el original: FindPlayerPed siempre se llama; FindPlayerVehicle solo si el causante no es el jugador
    if (damager == FindPlayerPed(-1) || damager == FindPlayerVehicle(-1, false)) {
        auto& info = CWorld::Players[CWorld::PlayerInFocus]; // 0xB7CD98 + 0x190 * indice, sin comprobar el limite
        info.m_nHavocCaused += 20;
        info.m_fCurrentChaseValue += 10.0f; // 0x85862C
        CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(HeliExeRand() % 6000 + 4000));
    }
    if (m_nModelIndex == 0x1E8) {
        CWanted::UseNewsHeliInAdditionToPolice = false;
    }

    if (GetStatus() == STATUS_PLAYER) {
        HeliField<uint32>(this, 0x1C) &= 0xFFFFFF7Eu; // m_bUsesCollision y el bit 7 de las banderas de la entidad
        m_vecMoveSpeed = CVector{ 0.0f, 0.0f, 0.0f };
        m_vecTurnSpeed = CVector{ 0.0f, 0.0f, 0.0f };
    }
    SetStatus(STATUS_WRECKED);
    physicalFlags.bRenderScorched = true; // bit 29 de +0x40
    m_nTimeWhenBlowedUp = CTimer::GetTimeInMS(); // +0x4D8
    CVisibilityPlugins::SetClumpForAllAtomicsFlag(reinterpret_cast<RpClump*>(GetRwObject()), 0x4000);
    m_damageManager.FuckCarCompletely(false);

    if (!(m_nModelIndex == 0x1D1 || m_nModelIndex == 0x1F5)) { // el EXE vuelve a leer el modelo
        SetBumperDamage(static_cast<ePanels>(5), false);
        SetBumperDamage(static_cast<ePanels>(6), false);
        SetDoorDamage(static_cast<eDoors>(0), false);
        SetDoorDamage(static_cast<eDoors>(1), false);
        SetDoorDamage(static_cast<eDoors>(2), false);
        SetDoorDamage(static_cast<eDoors>(3), false);
        SetDoorDamage(static_cast<eDoors>(4), false);
        SetDoorDamage(static_cast<eDoors>(5), false);
        SpawnFlyingComponent(static_cast<eCarNodes>(5), 1);
        RpAtomic* atomic = nullptr;
        RwFrameForAllObjects(m_aCarNodes.data()[5], GetCurrentAtomicObjectCB, &atomic); // el EXE lee el nodo despues de SpawnFlyingComponent
        if (atomic) {
            RpAtomicSetFlags(atomic, 0);
        }
    }

    m_nBombOnBoard = 0;
    m_fHealth = 0.0f;
    m_wBombTimer = 0;
    TheCamera.CamShake(std::bit_cast<float>(0x3ECCCCCDu), GetPosition()); // 0x50A9F0; 0.4f
    KillPedsInVehicle(); // 0x6D1C80
    m_nOverrideLights = 0;
    vehicleFlags.bEngineOn = false;
    vehicleFlags.bLightsOn = false;
    vehicleFlags.bSirenOrAlarm = false; // bit 7 del byte +0x42D
    autoFlags.bTaxiLight = false;
    if (vehicleFlags.bIsAmbulanceOnDuty) {
        vehicleFlags.bIsAmbulanceOnDuty = false;
        CCarCtrl::NumAmbulancesOnDuty--;
    }
    if (vehicleFlags.bIsFireTruckOnDuty) {
        vehicleFlags.bIsFireTruckOnDuty = false;
        CCarCtrl::NumFireTrucksOnDuty--;
    }
    ChangeLawEnforcerState(false); // 0x6D2330
    gFireManager.StartFire(this, damager, std::bit_cast<float>(0x3F4CCCCDu), 1, 7000, 0); // 0x53A050; tamano 0.8f
    CDarkel::RegisterCarBlownUpByPlayer(*this, 0); // 0x43DF20
    CExplosion::AddExplosion(this, damager, rcModel ? EXPLOSION_RC_VEHICLE : EXPLOSION_AIRCRAFT, GetPosition(), 0, 1, -1.0f, 0); // 0x736A50
}

// 0x6C4530
void CHeli::Fix() {
    m_damageManager.ResetDamageStatus();
    CHeli::SetupDamageAfterLoad(); // Como el original: en línea, sin pasar por la vtable
}

// 0x6C4330
bool CHeli::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C4320
bool CHeli::SetUpWheelColModel(CColModel* wheelCol) {
    return false;
}

// 0x6C4830
// El procesamiento de controles del helicóptero continúa delegado al ejecutable original.
void CHeli::ProcessControlInputs(uint8 playerNum) {
    plugin::CallMethod<0x6C4830, CHeli*, uint8>(this, playerNum);
}

// 0x6C4400
void CHeli::Render() {
    auto* mi = GetVehicleModelInfo();
    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000;
    mi->SetVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor);

    auto staticRotor = m_aCarNodes[HELI_STATIC_ROTOR];
    RpAtomic* data = nullptr;
    if (staticRotor) {
        RwFrameForAllObjects(staticRotor, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 255);
    }

    auto staticRotor2 = m_aCarNodes[HELI_STATIC_ROTOR2];
    data = nullptr;
    if (staticRotor2) {
        RwFrameForAllObjects(staticRotor2, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 255);
    }

    auto movingRotor = m_aCarNodes[HELI_MOVING_ROTOR];
    data = nullptr;
    if (movingRotor) {
        RwFrameForAllObjects(movingRotor, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 0);
    }

    auto movingRotor2 = m_aCarNodes[HELI_MOVING_ROTOR2];
    data = nullptr;
    if (movingRotor2) {
        RwFrameForAllObjects(movingRotor2, GetCurrentAtomicObjectCB, &data);
        if (data)
            CVehicle::SetComponentAtomicAlpha(data, 0);
    }

    CEntity::Render(); // exactly CEntity
}

// 0x6C4550
void CHeli::SetupDamageAfterLoad() {
    vehicleFlags.bIsDamaged = false;
}

// 0x6C4E60
// El procesamiento del modo de vuelo continúa delegado al ejecutable original.
void CHeli::ProcessFlyingCarStuff() {
    plugin::CallMethod<0x6C4E60, CHeli*>(this);
}

// 0x6C5420
// La preparación del helicóptero para el dibujo continúa delegada al ejecutable original.
void CHeli::PreRender() {
    plugin::CallMethod<0x6C5420, CHeli*>(this);
}

// 0x6C7050
// El control del helicóptero continúa delegado al ejecutable original.
void CHeli::ProcessControl() {
    plugin::CallMethod<0x6C7050, CHeli*>(this);
}
