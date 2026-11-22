#include "StdInc.h"

#include <bit>

#include "RunningScript.h"
#include "TheScripts.h"
#include "MissionCleanup.h"
#include "Pools/Pools.h"
#include "World.h"
#include "Weapon.h"
#include "Pickups.h"
#include "Pickup.h"
#include "Radar.h"
#include "Explosion.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "ModelIndices.h"
#include "Text/Text.h"
#include "Audio/AudioEngine.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Object/Object.h"

// Lote SCRB de codigo vivo (sin validar): 0x48CDD0 (7376 bytes), CRunningScript::ProcessCommands1300To1399 (thiscall, un argumento de pila, `ret 4`; resultado en AL), el despachador original de los
// comandos de script 1300..1399 (ranura 13 de la tabla 0x8A6168). Salta con un indice de bytes (0x48DDD4; indice = comando - 1301) a una tabla de 97 saltos (0x48DD34). Siguiendo el criterio de las ranuras 3, 4,
// 15 y 20, solo llevan caso los comandos del estado `original` de docs/ComandosScript.csv (sin manejador propio en C++): 1303 1304 1305 1308 1309 1310 1342 1343 1345 1359 1360 1365 1380 1381 1382 1386 1390
// 1392 1396. Los comandos con manejador propio (reescrito, nop) los atiende ProcessOneCommand antes de llegar aqui; los que el EXE manda al final de la funcion sin caso (no implementados: 0x48DD27) devuelven
// OR_INTERRUPT (AL = 0xFF), que es tambien el resultado de los comandos fuera de la tabla (1300 y 1398 en adelante).
// Cada caso sigue el desensamblado: misma secuencia de CollectParameters / lecturas de ScriptParams / llamadas y las comparaciones x87 con -100.0 (0x859014) conservan su sentido con NaN.
namespace {
inline tScriptParam& P(size_t index) {
    return ScriptParams[index];
}
// 0x404910 (CPool<CPed>::GetAtRef), 0x4048E0 (CPool<CVehicle>::GetAtRef) y 0x465040 (CPool<CObject>::GetAtRef): sin comprobar el resultado, como el EXE.
inline CPed* PedOf(int32 handle) {
    return CPools::GetPed(handle);
}
inline CVehicle* VehicleOf(int32 handle) {
    return CPools::GetVehicle(handle);
}
inline CObject* ObjectOf(int32 handle) {
    return CPools::GetObject(handle);
}
// 0x859014: -100.0f. Con z <= -100.0 (ordenado; NaN no) el EXE pide la altura del suelo (CWorld::FindGroundZForCoord 0x569660).
constexpr float EXE_MIN_GROUND_Z = -100.0f;
// 0x858B8C: 0.5f, elevacion que se suma a la altura del suelo en los pickups de propiedad
constexpr float EXE_HALF = std::bit_cast<float>(0x3F000000u);
// physical flags que tocan FREEZE_CAR_POSITION (0x200C), FREEZE_OBJECT_POSITION y FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION (0x2004)
constexpr uint32 EXE_PHYS_FREEZE_CAR = 0x200C;
constexpr uint32 EXE_PHYS_FREEZE     = 0x2004;
} // namespace

static_assert(offsetof(CPed, m_pLastEntityDamage) == 0x764 && offsetof(CPed, m_pVehicle) == 0x58C);
static_assert(offsetof(CVehicle, m_pLastDamageEntity) == 0x50C && offsetof(CVehicle, m_nGunFiringTime) == 0x4D4);
static_assert(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission) == 0x3BA);
static_assert(offsetof(CPhysical, m_nPhysicalFlags) == 0x40 && offsetof(CPhysical, m_vecMoveSpeed) == 0x44);
static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);

// 0x48CDD0
OpcodeResult CRunningScript::ProcessCommands1300To1399(int32 command) {
    switch (command) {
    // 0x48CDF9: CREATE_LOCKED_PROPERTY_PICKUP (3 parametros; modelo MI_PICKUP_PROPERTY 0x8CD5E8, tipo 0x11; el texto se lee y se traduce pero se pasa la etiqueta)
    case 1303: {
        CollectParameters(3);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y) + EXE_HALF;
        }
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        (void)TheText.Get(label); // el resultado se descarta
        static_cast<void>(CPickups::GetActualPickupIndex(tPickupReference{ CollectNextParameterWithoutIncreasingPC() })); // 0x464250 + 0x4552A0 (el resultado no se usa)
        P(0).iParam = CPickups::GenerateNewOne_ExeAbi(CVector{ x, y, z }, static_cast<uint32>(static_cast<uint16>(ModelIndices::MI_PICKUP_PROPERTY)), PICKUP_PROPERTY_LOCKED, 0, 0, 0, label); // 0x456F20
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48CEDB: CREATE_FORSALE_PROPERTY_PICKUP (4 parametros; modelo ModelIndices::MI_PICKUP_PROPERTY_FORSALE 0x8CD5EC, tipo 0x12; la municion es el cuarto parametro)
    case 1304: {
        CollectParameters(4);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y) + EXE_HALF;
        }
        char label[16]{};
        ReadTextLabelFromScript(label, 8);
        (void)TheText.Get(label); // el resultado se descarta
        static_cast<void>(CPickups::GetActualPickupIndex(tPickupReference{ CollectNextParameterWithoutIncreasingPC() }));
        P(0).iParam = CPickups::GenerateNewOne_ExeAbi(CVector{ x, y, z }, static_cast<uint32>(static_cast<uint16>(ModelIndices::MI_PICKUP_PROPERTY_FORSALE)), PICKUP_PROPERTY_FORSALE, P(3).uParam, 0, 0, label); // 0x456F20
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48CFA5: FREEZE_CAR_POSITION
    case 1305: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->m_nPhysicalFlags |= EXE_PHYS_FREEZE_CAR;
            veh->SkipPhysics(); // 0x5433B0
            veh->m_vecMoveSpeed = CVector{ 0.0f, 0.0f, 0.0f }; // 0x441130
            veh->m_vecTurnSpeed = CVector{ 0.0f, 0.0f, 0.0f }; // 0x45AFB0
        } else {
            veh->m_nPhysicalFlags &= ~EXE_PHYS_FREEZE_CAR;
        }
        return OR_CONTINUE;
    }
    // 0x48D0D2: HAS_CAR_BEEN_DAMAGED_BY_CHAR (el segundo parametro -1 significa cualquier ped)
    case 1308: {
        CollectParameters(2);
        CVehicle* const veh    = VehicleOf(P(0).iParam);
        const int32     pedRef = P(1).iParam;
        CPed* const     ped    = (pedRef != -1) ? PedOf(pedRef) : nullptr;
        bool            result = false;
        if (veh) {
            if (CEntity* const dmg = veh->m_pLastDamageEntity) {
                if (pedRef != -1) {
                    if (dmg == ped) {
                        result = true;
                    }
                    if (ped->bInVehicle && dmg == ped->m_pVehicle) {
                        result = true;
                    }
                } else {
                    result = (dmg->GetType() == ENTITY_TYPE_PED); // byte +0x36 & 7 == 3
                }
            }
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    // 0x48D15B: HAS_CAR_BEEN_DAMAGED_BY_CAR (el segundo parametro -1 significa cualquier vehiculo)
    case 1309: {
        CollectParameters(2);
        CVehicle* const veh1   = VehicleOf(P(0).iParam);
        const int32     veh2Id = P(1).iParam;
        CVehicle* const veh2   = (veh2Id != -1) ? VehicleOf(veh2Id) : nullptr;
        bool            result = false;
        if (veh1) {
            if (CEntity* const dmg = veh1->m_pLastDamageEntity) {
                if (veh2Id != -1) {
                    result = (dmg == veh2);
                } else {
                    result = (dmg->GetType() == ENTITY_TYPE_VEHICLE); // byte +0x36 & 7 == 2
                }
            }
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    // 0x48D1C6: GET_RADIO_CHANNEL (no lee parametros; movsx de la emisora - 1)
    case 1310: {
        P(0).iParam = static_cast<int32>(static_cast<int8>(AudioEngine.GetCurrentRadioStationID())) - 1;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48D278: GET_RANDOM_CAR_OF_TYPE_IN_AREA_NO_SAVE (como 0x48150A, sin la prueba de bIsLawEnforcer ni la lista de limpieza; solo marca bHasBeenOwnedByPlayer, +0x42A bit 1)
    case 1342: {
        CollectParameters(5);
        int32 result = -1;
        auto* const pool = GetVehiclePool();
        int32 i = pool->GetSize();
        if (i != 0) {
            do {
                i--;
                if (result != -1) {
                    break;
                }
                CVehicle* const vehicle = pool->GetAt(i);
                if (!vehicle) {
                    continue;
                }
                if (vehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE && vehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_BIKE) { // 0x6D1080 (la segunda llamada solo si la primera no da 1)
                    continue;
                }
                const int32 model = static_cast<int16>(vehicle->m_nModelIndex);
                if (model != P(4).iParam && P(4).iParam >= 0) {
                    continue;
                }
                if (!vehicle->CanBeDeleted()) { // 0x6D1180
                    continue;
                }
                if (!vehicle->IsWithinArea(P(0).fParam, P(1).fParam, P(2).fParam, P(3).fParam)) { // 0x54F200
                    continue;
                }
                result = pool->GetRef(vehicle); // 0x424160
                vehicle->vehicleFlags.bHasBeenOwnedByPlayer = true;
            } while (i != 0);
        }
        P(0).iParam = result;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48D356: SET_CAN_BURST_CAR_TYRES (+0x42B bit 7 = bTyresDontBurst: parametro 0 lo activa)
    case 1343: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        veh->vehicleFlags.bTyresDontBurst = (P(1).iParam == 0);
        return OR_CONTINUE;
    }
    // 0x48D3AA: FIRE_HUNTER_GUN
    case 1345: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (CTimer::GetTimeInMS() > veh->m_nGunFiringTime + 0x96u) {
            CWeapon       minigun(WEAPON_MINIGUN, 0x1388);
            const CVector gunPos = CAutomobile::vecHunterGunPos; // 0x8D3394
            // 0x40FEC0 (velocidad * timestep), 0x59C890 (matriz * vector, orden de sumas del EXE) y 0x40FE30 (suma)
            const CVector step{ veh->m_vecMoveSpeed.x * CTimer::GetTimeStep(), veh->m_vecMoveSpeed.y * CTimer::GetTimeStep(), veh->m_vecMoveSpeed.z * CTimer::GetTimeStep() };
            const CMatrix& m = *veh->m_matrix;
            const CVector transformed{
                ((m.GetUp().x * gunPos.z + m.GetForward().x * gunPos.y) + m.GetRight().x * gunPos.x) + m.GetPosition().x,
                ((m.GetUp().y * gunPos.z + m.GetRight().y * gunPos.x) + m.GetForward().y * gunPos.y) + m.GetPosition().y,
                ((m.GetUp().z * gunPos.z + m.GetRight().z * gunPos.x) + m.GetForward().z * gunPos.y) + m.GetPosition().z,
            };
            CVector point{ transformed.x + step.x, transformed.y + step.y, transformed.z + step.z };
            minigun.FireInstantHit(veh, &point, &point, nullptr, nullptr, nullptr, false, true); // 0x73FB10
            CVector2D direction(0.0f, 0.1f);
            minigun.AddGunshell(veh, point, direction, std::bit_cast<float>(0x3CCCCCCDu)); // 0x73A3E0 (0.025)
            AudioEngine.ReportWeaponEvent(0x95, WEAPON_MINIGUN, veh); // 0x506F40
            veh->m_nGunFiringTime = CTimer::GetTimeInMS();
        }
        return OR_CONTINUE;
    }
    // 0x48D5E8: CLEAR_CAR_LAST_DAMAGE_ENTITY
    case 1359: {
        CollectParameters(1);
        if (CVehicle* const veh = VehicleOf(P(0).iParam)) {
            veh->m_pLastDamageEntity = nullptr;
        }
        return OR_CONTINUE;
    }
    // 0x48D621: FREEZE_OBJECT_POSITION
    case 1360: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        if (P(1).iParam != 0) {
            obj->m_nPhysicalFlags |= EXE_PHYS_FREEZE;
        } else {
            obj->m_nPhysicalFlags &= ~EXE_PHYS_FREEZE;
        }
        return OR_CONTINUE;
    }
    // 0x48D673: REMOVE_WEAPON_FROM_CHAR (el ped se busca con el handle y el tipo de arma queda en la pila: ClearWeapon 0x5E62B0)
    case 1365: {
        CollectParameters(2);
        CPed* const ped = PedOf(P(0).iParam);
        ped->ClearWeapon(static_cast<eWeaponType>(P(1).iParam));
        return OR_CONTINUE;
    }
    // 0x48D903: MAKE_HELI_COME_CRASHING_DOWN (misiones 0x39 y 0x3A se dejan; el resto pasa a 0x3A)
    case 1380: {
        CollectParameters(1);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        const auto mission = static_cast<uint8>(veh->m_autoPilot.m_nCarMission);
        if (mission != 0x39 && mission != 0x3A) {
            veh->m_autoPilot.m_nCarMission = static_cast<eCarMission>(0x3A);
        }
        return OR_CONTINUE;
    }
    // 0x48D94A: ADD_EXPLOSION_NO_SOUND (victima y creador nulos, vida 0, sin sonido, temblor de camara -1.0, no visible)
    case 1381: {
        CollectParameters(4);
        const CVector pos{ P(0).fParam, P(1).fParam, P(2).fParam };
        CExplosion::AddExplosion(nullptr, nullptr, static_cast<eExplosionType>(P(3).iParam), pos, 0, 0, -1.0f, 0); // 0x736A50
        return OR_CONTINUE;
    }
    // 0x48D9B2: SET_OBJECT_AREA_VISIBLE
    case 1382: {
        CollectParameters(2);
        CObject* const obj = ObjectOf(P(0).iParam);
        obj->SetAreaCode(static_cast<eAreaCodes>(P(1).u8Param)); // byte +0x2F
        return OR_CONTINUE;
    }
    // 0x48DA3C: WAS_CUTSCENE_SKIPPED (no lee parametros; byte 0xB5F854)
    case 1386: {
        UpdateCompareFlag(CCutsceneMgr::ms_wasCutsceneSkipped != 0);
        return OR_CONTINUE;
    }
    // 0x48DAF9: DOES_VEHICLE_EXIST
    case 1390: {
        CollectParameters(1);
        UpdateCompareFlag(VehicleOf(P(0).iParam) != nullptr);
        return OR_CONTINUE;
    }
    // 0x48DB3D: ADD_SHORT_RANGE_SPRITE_BLIP_FOR_CONTACT_POINT (tipo 5, color 2, mostrar 3; el nombre del blip es el del script)
    case 1392: {
        CollectParameters(4);
        const float x = P(0).fParam;
        const float y = P(1).fParam;
        float       z = P(2).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y);
        }
        (void)CRadar::GetActualBlipArrayIndex(CollectNextParameterWithoutIncreasingPC()); // el EXE descarta el resultado (0x582870)
        const auto blip = CRadar::SetShortRangeCoordBlip(BLIP_CONTACT_POINT, CVector{ x, y, z }, (eBlipColour)2, (eBlipDisplay)3, m_szName); // 0x583920
        CRadar::SetBlipSprite(blip, (eRadarSprite)P(3).iParam); // 0x583D70
        P(0).iParam = blip;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48DC3F: FREEZE_CAR_POSITION_AND_DONT_LOAD_COLLISION
    case 1396: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->m_nPhysicalFlags |= EXE_PHYS_FREEZE;
            if (m_UsesMissionCleanup) {
                CWorld::Remove(veh);                        // 0x563280
                veh->m_bIsStaticWaitingForCollision = true; // +0x1C |= 0x40000
                CWorld::Add(veh);                           // 0x563220
            }
        } else {
            veh->m_nPhysicalFlags &= ~EXE_PHYS_FREEZE;
            *(reinterpret_cast<uint8*>(veh) + 0xB8) = 0; // byte +0xB8
        }
        return OR_CONTINUE;
    }
    case COMMAND_IS_CHAR_DROWNING_IN_WATER: // 1313: el EXE (0x48DD27: xor al,al) no lee parametros y devuelve OR_CONTINUE
        return OR_CONTINUE;
    default:
        return OR_INTERRUPT;
    }
}
