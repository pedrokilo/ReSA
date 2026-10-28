/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CHeli
 * Reconstrucción: COMPLETA — 18/18 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 18/18 entradas.
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
#include "CullZones.h"
#include "Streaming.h"
#include "Camera.h"
#include "PlayerInfo.h"
#include "Shadows.h"
#include "WindModifiers.h"
#include "Clock.h"
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
    RH_ScopedInstall(UpdateHelis, 0x6C79A0);
    RH_ScopedInstall(SendDownSwat, 0x6C69C0);
    RH_ScopedNamedGlobalInstall(FindSwatPositionRelativeToHeliExeAbi, "FindSwatPositionRelativeToHeli", 0x6C4760);
    RH_ScopedVMTInstall(Render, 0x6C4400);
    RH_ScopedVMTInstall(BlowUpCar, 0x6C6D30);
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6C4E60);
    RH_ScopedVMTInstall(PreRender, 0x6C5420);
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

// 0x6C7B5D..0x6C7B95: distancia jugador - helicoptero > 170 (0x858F98) como el original: d = jugador - helicoptero por componentes, suma (z, y, x), raiz y comparacion ordenada (un NaN no cuenta como lejos).
// La raiz se redondea a float como la x87 a 24 bits (la raiz doble de un float redondeada a float da el mismo resultado: 53 >= 2 * 24 + 2).
static bool HeliIsFarFromPlayer(const CVector& player, const CVector& heli) {
    const float dx = player.x - heli.x;
    const float dy = player.y - heli.y;
    const float dz = player.z - heli.z;
    const float zz = dz * dz;
    const float yy = dy * dy;
    const float xx = dx * dx;
    const float sumZY = zz + yy;
    const float sum = sumZY + xx;
    const float distance = static_cast<float>(std::sqrt(static_cast<double>(sum)));
    return distance > 170.0f;
}

// 0x6C79A0..0x6C7C45 (cdecl, sin argumentos, resultado ninguno): lo llama CGame::Process.
void CHeli::UpdateHelis() {
    ZoneScoped;

    NumberOfSearchLights = 0;
    // Como el original: FindPlayerWanted(-1) y NumOfHelisRequired se llaman sin comprobar el nulo
    int32 maxHelis = FindPlayerWanted(-1)->NumOfHelisRequired();

    // Cuenta de helicopteros vivos y bandera "hay un helicoptero de policia (0x1F1) que no se retira": se pueden anadir las noticias
    int32 numHelis = 0;
    bool newsAllowed = false;
    for (size_t i = 0; i < 2; i++) {
        CHeli* const heli = pHelis.data()[i];
        if (!heli) {
            continue;
        }
        numHelis++;
        if (static_cast<uint16>(heli->m_nModelIndex) == 0x1F1 && !(HeliField<uint32>(heli, 0x40) & 0x20000000u) && !(HeliField<uint8>(heli, 0x42B) & 0x40u)) {
            newsAllowed = true;
        }
    }

    // Como el original: PlayerNoRain se llama siempre; sin helicopteros permitidos (zona, interior, tormenta de arena en la region actual o la anterior) maxHelis = 0
    if (CCullZones::PlayerNoRain() || StaticRef<int32>(0xB72914) != 0) { // CGame::currArea
        maxHelis = 0;
    }
    if (!bPoliceHelisAllowed) {
        maxHelis = 0;
    }
    if (StaticRef<int16>(0xC81320) == 0x13 || StaticRef<int16>(0xC8131C) == 0x13) { // CWeather::OldWeatherType / NewWeatherType: 19
        maxHelis = 0;
    }

    // Con un helicoptero de noticias (0x1E8) vivo no se anade otro
    for (size_t i = 0; i < 2; i++) {
        CHeli* const heli = pHelis.data()[i];
        if (heli && static_cast<uint16>(heli->m_nModelIndex) == 0x1E8) {
            newsAllowed = false;
        }
    }

    // 0xB7CB8C (CWanted::UseNewsHeliInAdditionToPolice) y la bandera: con ambas se mira el byte 0x8E72F0, si no la bandera se anula y se mira 0x8E73A4
    bool allowed;
    if (CWanted::UseNewsHeliInAdditionToPolice && newsAllowed) {
        allowed = StaticRef<uint8>(0x8E72F0) == 1;
    } else {
        newsAllowed = false;
        allowed = StaticRef<uint8>(0x8E73A4) == 1;
    }

    // Generacion: el temporizador (sin signo) se rearma aunque no falte ningun helicoptero
    if (allowed && CTimer::GetTimeInMS() > TestForNewRandomHelisTimer) {
        TestForNewRandomHelisTimer = CTimer::GetTimeInMS() + 15000u;
        if (numHelis < maxHelis) {
            CPed* const target = FindPlayerPed(-1);
            CHeli* const heli = GenerateHeli(target, newsAllowed); // 0x6C6520
            // Como el original: el helicoptero va a la primera ranura libre; si las dos estan ocupadas no se registra (y no se libera)
            if (!pHelis.data()[0]) {
                pHelis.data()[0] = heli;
                heli->RegisterReference(reinterpret_cast<CEntity**>(&pHelis.data()[0])); // 0x571B70
            } else if (!pHelis.data()[1]) {
                pHelis.data()[1] = heli;
                heli->RegisterReference(reinterpret_cast<CEntity**>(&pHelis.data()[1]));
            }
        }
    }

    // Retirada: marcado para borrar (+0x40 bit 29 o +0x42B bit 6) -> huye (mision 42) y se suelta; en mision 42 y a mas de 170 del jugador se saca del mundo y se destruye
    for (size_t i = 0; i < 2; i++) {
        CHeli* heli = pHelis.data()[i];
        if (!heli) {
            continue;
        }
        if ((HeliField<uint32>(heli, 0x40) & 0x20000000u) || (HeliField<uint8>(heli, 0x42B) & 0x40u)) {
            heli->m_autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
            pHelis.data()[i] = nullptr;
            continue;
        }
        if (heli->m_autoPilot.m_nCarMission != MISSION_HELI_FLY_AWAY_FROM_PLAYER) {
            continue;
        }
        const CVector heliPos = heli->GetPosition(); // matriz + 0x30 o posicion compacta +4
        const CVector playerPos = FindPlayerCoors(-1);
        if (!HeliIsFarFromPlayer(playerPos, heliPos)) {
            continue;
        }
        CWorld::Remove(heli);
        heli = pHelis.data()[i]; // el original relee la ranura
        if (heli) {
            delete heli; // destructor virtual con flag 1
        }
        pHelis.data()[i] = nullptr;
    }

    // Si sobra algun helicoptero (mas vivos que maxHelis) el primero que no huye se manda a huir con alturas de 100; el segundo solo si ya no queda cupo
    int32 remaining = maxHelis;
    CHeli* heli = pHelis.data()[0];
    if (heli && heli->m_autoPilot.m_nCarMission != MISSION_HELI_FLY_AWAY_FROM_PLAYER) {
        if (maxHelis > 0) {
            remaining = maxHelis - 1;
        } else {
            heli->m_autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
            pHelis.data()[0]->m_fMinAltitude = 100.0f; // +0x9B0
            pHelis.data()[0]->m_fMaxAltitude = 100.0f; // +0x9A8
        }
    }
    heli = pHelis.data()[1];
    if (heli && heli->m_autoPilot.m_nCarMission != MISSION_HELI_FLY_AWAY_FROM_PLAYER && remaining <= 0) {
        heli->m_autoPilot.m_nCarMission = MISSION_HELI_FLY_AWAY_FROM_PLAYER;
        pHelis.data()[1]->m_fMinAltitude = 100.0f;
        pHelis.data()[1]->m_fMaxAltitude = 100.0f;
    }
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

// Disposicion que usan ProcessFlyingCarStuff y PreRender (comprobada en compilacion)
VALIDATE_OFFSET(CHeli, m_fLeftRightSkid, 0x98C);
VALIDATE_OFFSET(CHeli, m_fSteeringUpDown, 0x990);
VALIDATE_OFFSET(CHeli, m_fSteeringLeftRight, 0x994);
VALIDATE_OFFSET(CHeli, m_fAccelerationBreakStatus, 0x998);
VALIDATE_OFFSET(CHeli, m_fRotorZ, 0x9A0);
VALIDATE_OFFSET(CHeli, m_fSecondRotorZ, 0x9A4);
VALIDATE_OFFSET(CHeli, m_vecSearchLightTarget, 0x9F4);
VALIDATE_OFFSET(CHeli, m_fSearchLightIntensity, 0xA00);
VALIDATE_OFFSET(CHeli, m_bSearchLightEnabled, 0xA11);
VALIDATE_OFFSET(CAutomobile, m_wheelRotation, 0x828);
VALIDATE_OFFSET(CAutomobile, m_wheelPosition, 0x838);
VALIDATE_OFFSET(CAutomobile, m_fPropRotate, 0x958);
VALIDATE_OFFSET(CAutomobile, m_aSuspensionSpringLength, 0x878);
VALIDATE_OFFSET(CAutomobile, m_aSuspensionLineLength, 0x888);
VALIDATE_OFFSET(CAutomobile, m_fWheelsSuspensionCompression, 0x7D4);
VALIDATE_OFFSET(CAutomobile, m_nNumContactWheels, 0x960);
VALIDATE_OFFSET(CVehicle, m_vehicleAudio, 0x138);
VALIDATE_OFFSET(CVehicle, m_pHandlingData, 0x384);
VALIDATE_OFFSET(CVehicle, m_nHandlingFlagsIntValue, 0x38C);
VALIDATE_OFFSET(RpAtomic, boundingSphere, 0x1C);
VALIDATE_OFFSET(RpAtomic, interpolator, 0x4C);
VALIDATE_OFFSET(RwFrame, modelling, 0x10);

namespace {
// 0x6C549B..0x6C54B6: el EXE copia dos de las tres componentes de un vector con fld/fstp, que silencia los NaN de señal; la tercera con mov (sin cambios)
float HeliQuietNan(float value) {
    uint32 bits = std::bit_cast<uint32>(value);
    if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x7FFFFFu) != 0) {
        bits |= 0x400000u;
    }
    return std::bit_cast<float>(bits);
}
} // namespace

// 0x6C4E60: thiscall virtual (ranura 68 de la vtabla 0x871680), sin argumentos, void. Con estado de jugador, roto o a distancia (ver abajo) el rotor se acelera o se frena; con rotor rapido llama a FlyingControl;
// con rotor sobre 0.015 coloca la paleta del rotor (nodo 12) y, con jugador/roto/abandonado, registra viento y colision de aspas; por ultimo, el sonido de las aspas al cruzar la camara.
// La aritmetica con rotor y dist2 va por Heli75*Cpp (x87 a 24 bits con exponente extendido, X87Math.cpp).
void CHeli::ProcessFlyingCarStuff() {
    const auto C = [](uint32 bits) { return std::bit_cast<float>(bits); };
    const uint32 status = HeliField<uint8>(this, 0x36) >> 3;
    const uint16 model = m_nModelIndex;
    float& rotor = m_fHeliRotorSpeed; // +0x84C

    if (status == 0 || status == 8 || status == 3) { // 0x6C4EF8: jugador, jugador remoto o destruido
        if (rotor < C(0x3E6147AEu)) { // 0x8717AC
            if (!(HeliField<uint32>(this, 0x40) & 0x100u)) {
                const bool rc = model == 0x1D1 || model == 0x1F5;
                rotor = rotor + C(rc ? 0x3B449BA6u : 0x3A83126Fu); // 0x859CD8 / 0x858CDC
            }
        }
        if (rotor > C(0x3E19999Au)) { // 0x6C4F43: 0x858FCC
            const bool special = (HeliField<uint32>(this, 0x40) & 0x8000000u) && (model == 0x1BF || model == 0x1A1);
            if (HeliField<uint8>(this, 0x42C) & 0x80u) {
                FlyingControl(static_cast<eFlightModel>(2), m_fLeftRightSkid, m_fSteeringUpDown, m_fSteeringLeftRight, m_fAccelerationBreakStatus);
            } else {
                const float limit = C(0x3CA3D70Au); // 0x858B38
                if ((m_nNumContactWheels < 4 && !special) || m_fAccelerationBreakStatus > 0.0f
                    || std::fabs(m_vecMoveSpeed.x) > limit || std::fabs(m_vecMoveSpeed.y) > limit || std::fabs(m_vecMoveSpeed.z) > limit) {
                    FlyingControl(static_cast<eFlightModel>(6), m_fLeftRightSkid, m_fSteeringUpDown, m_fSteeringLeftRight, m_fAccelerationBreakStatus);
                }
            }
        }
    } else { // 0x6C4E93: el resto de estados
        if (!(*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(m_pHandlingData) + 0xCC) & 0x2000000u)) {
            return;
        }
        HeliField<uint8>(this, 0x428) &= 0xEF; // bEngineOn
        if (Heli75DecayCpp(CTimer::GetTimeStep(), C(0x3A102DE0u), &rotor)) { // 0x8717B0
            HeliField<uint8>(this, 0xB8) = 0;
        }
    }

    // 0x6C501D: paleta del rotor (solo jugador, jugador remoto y destruido; los demas estados saltan a 0x6C5200)
    if ((status == 0 || status == 8 || status == 3) && rotor > C(0x3C75C290u)) { // 0x8717A8
        if (RwFrame* const node = m_aCarNodes[HELI_STATIC_ROTOR]) { // +0x678
            CMatrix mat;
            mat.Attach(&node->modelling, false); // 0x59BD10
            RpAtomic* atomic = nullptr;
            RwFrameForAllObjects(node, GetCurrentAtomicObjectCB, &atomic); // 0x7F1200
            if (atomic) {
                const float radius = RpAtomicGetBoundingSphere(atomic)->radius; // 0x7491F0 si el bit 1 de las banderas del interpolador esta puesto
                if (radius > C(0x3DCCCCCDu)) { // 0.1
                    float scale = 1.0f;
                    switch (model) {
                    case 0x1D1:
                    case 0x1F5:
                        scale = C(0x3F666666u);
                        break;
                    case 0x1D5:
                    case 0x1BF:
                        scale = C(0x3F4CCCCDu);
                        break;
                    case 0x1A9:
                        scale = 0.5f;
                        break;
                    default:
                        break;
                    }
                    if (status == 0 || status == 8) {
                        DoBladeCollision(mat.GetPosition(), *m_matrix, -3, radius, scale); // 0x6E2E50; el EXE no usa el resultado
                    }
                }
            }
            const CVector& windPos = m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn;
            if ((status == 0 || status == 3) && rotor > C(0x3BF5C290u)) { // 0x8717A4
                const float t = rotor * C(0x40D55555u); // 0x866FBC
                const float power = (1.0f < t) ? 1.0f : t;
                CWindModifiers::RegisterOne(windPos, 1, power); // 0x72C8B0
            }
            // 0x6C51B4: el EXE compara aqui el estado con 2 (viento a potencia 1.0), pero a este punto solo llegan los estados 0, 8 y 3: no se reproduce
        }
    }

    // 0x6C5200: sonido de las aspas al pasar por la camara (no en los RC)
    if (model == 0x1D1 || model == 0x1F5) {
        return;
    }
    if (!(rotor < C(0x3E1DB22Du)) || !(rotor > C(0x3B902DE0u))) { // 0x8717A0 / 0x87179C
        return;
    }
    RwFrame* const node = m_aCarNodes[HELI_STATIC_ROTOR];
    if (!node) {
        return;
    }
    const float* const heliPos = m_matrix ? &m_matrix->GetPosition().x : &m_placement.m_vPosn.x;
    const float* const camPos = TheCamera.m_matrix ? &TheCamera.m_matrix->GetPosition().x : &TheCamera.m_placement.m_vPosn.x;
    float d[4]; // dx, dy, dz, dist2
    if (!Heli75NearCpp(heliPos, camPos, d)) {
        return;
    }
    if (!(std::fabs(m_fPropRotate - m_wheelRotation[1]) > C(0x3F060A92u))) { // 0x858F20
        return;
    }
    CMatrix mat;
    mat.Attach(&node->modelling, false); // 0x59BD10
    const CVector axis = mat.GetRight();
    if (Heli75DotCpp(&m_matrix->GetRight().x, &axis.x, d, d[3])) {
        m_vehicleAudio.AddAudioEvent(static_cast<eAudioEvents>(0x5E), 0.0f); // 0x4F6420
        HeliField<uint32>(this, 0x958) = HeliField<uint32>(this, 0x82C);
    }
}

// 0x6C5420: thiscall virtual (ranura 17 de la vtabla 0x871680), sin argumentos, void. CVehicle::PreRender, foco de busqueda (de noche), relaciones de la suspension de las cuatro ruedas, matrices de rueda, polvo del
// rotor, giro de los dos rotores y colocacion de sus cuatro paletas, y sombra. Aritmetica x87 con las Heli75*Cpp de X87Math.cpp.
void CHeli::PreRender() {
    const auto C = [](uint32 bits) { return std::bit_cast<float>(bits); };
    CVehicle::PreRender(); // 0x6D6480
    CVehicleModelInfo* const modelInfo = reinterpret_cast<CVehicleModelInfo* const*>(0xA9B0C8)[static_cast<int16>(m_nModelIndex)];

    if (HeliField<uint8>(this, 0xA11) && m_fSearchLightIntensity > 0.0f && CClock::GetIsTimeInRange(19, 6)) { // 0x858B50
        CVector target;
        target.x = m_vecSearchLightTarget.x;                      // mov (sin cambios)
        target.y = HeliQuietNan(m_vecSearchLightTarget.y);       // fld/fstp
        target.z = HeliQuietNan(m_vecSearchLightTarget.z);
        const CVector local{ 0.0f, C(0x40600000u), C(0xBE99999Au) };
#pragma warning(push)
#pragma warning(disable: 4996)
        const CVector origin = *m_matrix * local; // 0x59C890
#pragma warning(pop)
        AddHeliSearchLight(origin, target, 20.0f, m_fSearchLightIntensity, static_cast<uint32>(reinterpret_cast<uintptr_t>(this)) + 0xBu, 1, 1);
    }

    (void)GetColModel(); // 0x535300 (el EXE no usa el resultado)
    if (vehicleFlags.bVehicleColProcessed) { // byte 0x42B bit 0
        DoBurstAndSoftGroundRatios(); // virtual +0xD0 (ranura 52)
        for (int32 i = 0; i < 4; i++) {
            const float ratio = Heli75RatioCpp(m_fWheelsSuspensionCompression[i], m_aSuspensionSpringLength[i], m_aSuspensionLineLength[i]);
            CVector wheel;
            modelInfo->GetWheelPosn(i, wheel, true); // 0x4C7D20
            const float handlingValue = *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(m_pHandlingData) + 0xB8);
            const bool direct = (HeliField<uint32>(this, 0x40) & 0x4000000u) && (HeliField<uint32>(this, 0x38C) & 0x20000u);
            Heli75WheelCpp(ratio, wheel.z, handlingValue, m_aSuspensionSpringLength[i], &m_wheelPosition[i], direct);
        }
    }

    UpdateWheelMatrix(4, 1); // 0x6AA290
    UpdateWheelMatrix(7, 1);
    UpdateWheelMatrix(2, 1);
    UpdateWheelMatrix(5, 1);
    const uint16 model = m_nModelIndex;
    if (model != 0x1D1 && model != 0x1F5) {
        DoHeliDustEffect(1.0f, 1.0f); // 0x6B0690
    }

    const bool heavy = model == 0x1D5 || model == 0x1BF || model == 0x1E7 || model == 0x1E8 || model == 0x1F1;
    Heli75AngleACpp(CTimer::GetTimeStep(), m_fHeliRotorSpeed, heavy, StaticRef<float>(0x8D33A0), &m_fRotorZ);
    Heli75AngleBCpp(CTimer::GetTimeStep(), m_fHeliRotorSpeed, model == 0x1A1, &m_fSecondRotorZ);

    // 0x6C56E9..0x6C589D: la misma CMatrix local unida al modelling de cada paleta (nodos 12 y 13 con SetRotateZ(angulo 1), 14 y 15 con SetRotateX(angulo 2))
    CMatrix mat;
    for (int32 k = 0; k < 4; k++) {
        RwFrame* const node = m_aCarNodes[HELI_STATIC_ROTOR + k]; // +0x678, +0x67C, +0x680, +0x684
        if (!node) {
            continue;
        }
        mat.Attach(&node->modelling, false); // 0x59BD10
        const CVector saved = mat.GetPosition();
        if (k < 2) {
            mat.SetRotateZ(m_fRotorZ); // 0x59B0E0
        } else {
            mat.SetRotateX(m_fSecondRotorZ); // 0x59B060
        }
        Heli75AddPosCpp(&mat.GetPosition().x, &saved.x);
        mat.UpdateRW(); // 0x59BBB0
    }
    CShadows::StoreShadowForVehicle(this, VEH_SHD_HELI); // 0x70BDA0
}

// 0x6C7050
// El control del helicóptero continúa delegado al ejecutable original.
void CHeli::ProcessControl() {
    plugin::CallMethod<0x6C7050, CHeli*>(this);
}
