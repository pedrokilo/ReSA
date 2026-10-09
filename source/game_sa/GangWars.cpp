/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGangWars
 * Reconstrucción: COMPLETA — 35/35 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 35/35 entradas.
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

#include "Garages.h"
#include "GangWars.h"
#include "GangWarsSaveStructure.h"
#include "ModelIndices.h"
#include "TaskComplexWander.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexDriveWander.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexWanderGang.h"
#include "Hud.h"
#include "EventScriptCommand.h"
#include "EventGroup.h"
#include "Pools/PedPool.h"

void CGangWars::InjectHooks() {
    RH_ScopedClass(CGangWars);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitAtStartOfGame, 0x443920);
    RH_ScopedInstall(AddKillToProvocation, 0x443950);                  // ?
    RH_ScopedInstall(AttackWaveOvercome, 0x445B30);
    RH_ScopedInstall(CalculateTimeTillNextAttack, 0x443DB0);
    RH_ScopedInstall(CanPlayerStartAGangWarHere, 0x443F80);
    RH_ScopedInstall(CheerVictory, 0x444040);
    RH_ScopedInstall(ClearSpecificZonesToTriggerGangWar, 0x443FF0);
    RH_ScopedInstall(ClearTheStreets, 0x4444B0);
    RH_ScopedInstall(CreateAttackWave, 0x444810);                   //
    RH_ScopedInstall(CreateDefendingGroup, 0x4453D0);
    RH_ScopedInstall(DoesPlayerControlThisZone, 0x443AE0);
    RH_ScopedInstall(DoStuffWhenPlayerVictorious, 0x446400);
    RH_ScopedInstall(DontCreateCivilians, 0x4439C0);
    RH_ScopedInstall(EndGangWar, 0x4464C0);
    RH_ScopedInstall(GangWarFightingGoingOn, 0x443AC0);
    RH_ScopedInstall(GangWarGoingOn, 0x443AA0);
    RH_ScopedInstall(MakeEnemyGainInfluenceInZone, 0x445FD0);          //
    RH_ScopedInstall(MakePlayerGainInfluenceInZone, 0x445E80);
    RH_ScopedInstall(PedStreamedInForThisGang, 0x4439D0);
    RH_ScopedInstall(PickStreamedInPedForThisGang, 0x443A20);          // +-
    RH_ScopedInstall(PickZoneToAttack, 0x443B00);                      //
    RH_ScopedInstall(ReleaseCarsInAttackWave, 0x445E20);
    RH_ScopedInstall(ReleasePedsInAttackWave, 0x445C30);
    RH_ScopedInstall(SetGangWarsActive, 0x446570);
    RH_ScopedInstall(SetSpecificZoneToTriggerGangWar, 0x444010);
    RH_ScopedInstall(StartDefensiveGangWar, 0x444300);
    RH_ScopedInstall(StartOffensiveGangWar, 0x446050);                 //
    RH_ScopedInstall(StrengthenPlayerInfluenceInZone, 0x445F50);
    RH_ScopedInstall(SwitchGangWarsActive, 0x4465F0);
    RH_ScopedInstall(TellGangMembersTo, 0x444530);                     // ?
    RH_ScopedInstall(TellStreamingWhichGangsAreNeeded, 0x443D50);
    RH_ScopedInstall(Update, 0x446610);                             //
    RH_ScopedInstall(UpdateTerritoryUnderControlPercentage, 0x443DE0); //
    RH_ScopedInstall(Load, 0x5D3EB0);
    RH_ScopedInstall(Save, 0x5D5530);
}

// 0x5D3EB0
bool CGangWars::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // Discard
    auto data = CGenericGameStorage::LoadDataFromWorkBuffer<CGangWarsSaveStructure>();
    data.Extract();
    return true;
}

// 0x5D5530
bool CGangWars::Save() {
    CGangWarsSaveStructure data; // Como el original (0x5D5530): sin inicializar (el relleno y lo que Construct no escribe se guardan tal cual)
    data.Construct();
    CGenericGameStorage::SaveDataToWorkBuffer(sizeof(CGangWarsSaveStructure));
    CGenericGameStorage::SaveDataToWorkBuffer(data);
    return true;
}

// 0x443920
void CGangWars::InitAtStartOfGame() {
    ZoneScoped;

    State               = NOT_IN_WAR;
    State2              = NO_ATTACK;
    NumSpecificZones    = 0;
    Provocation         = 0.0f;
    bGangWarsActive     = false;
    bIsPlayerOnAMission = false;
}

// 0x443950
void CGangWars::AddKillToProvocation(ePedType pedType) {
    if (!bCanTriggerGangWarWhenOnAMission && CTheScripts::IsPlayerOnAMission()) return;
    if (pedType != PED_TYPE_GANG1 && pedType != PED_TYPE_GANG3) return;
    if (NumSpecificZones == 0) {
        Provocation += 1.0f;
        return;
    }
    for (int32 i = 0; i < NumSpecificZones; ++i) {
        const auto& zone = CTheZones::NavigationZoneArray[aSpecificZones[i]];
        const auto& info = CTheZones::ZoneInfoArray[static_cast<uint16>(zone.m_ZoneInfoIndex)];
        if (info.GangStrength[pedType - PED_TYPE_GANG1] != 0) {
            Provocation += 1.0f;
            return;
        }
    }
}

// 0x445B30
bool CGangWars::AttackWaveOvercome() {
    auto pedsNearPlayer = 0u, pedsLiving = 0u;

    // Como el original (0x445B60..0x445C01): recorre el pool de peatones del ultimo al primero
    auto* const pool = GetPedPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const ped = pool->GetAt(i);
        if (!ped || !ped->bPartOfAttackWave)
            continue;

        if (ped->IsStateDying()) {
            // Como el original (0x445BED): solo se quita bPartOfAttackWave (and eax,0FFFFEFFFh); bDonePositionOutOfCollision no se toca
            ped->bPartOfAttackWave = false;
            ped->SetCharCreatedBy(PED_GAME);
            continue;
        }

        pedsLiving++;
        if (DistanceBetweenPoints2D(ped->GetPosition2D(), FindPlayerCoors()) < 45.0f) {
            pedsNearPlayer++;
        }
    }

    return pedsLiving <= 1 && pedsNearPlayer == 0;
}

// 0x443DB0
uint32 CGangWars::CalculateTimeTillNextAttack() {
    const int32 random = CGeneral::GetRandomNumber();
    const float unit = std::bit_cast<float>(0x38000100u), variation = 0.9f, span = 1080000.0f, base = 648000.0f;
    int64 result;
    uint16 oldControl, truncateControl;
    __asm {
        fnstcw oldControl
        mov ax, oldControl
        or ax, 0C00h
        mov truncateControl, ax
        fild random
        fmul unit
        fmul variation
        fmul span
        fadd base
        fldcw truncateControl
        fistp result
        fldcw oldControl
    }
    return static_cast<uint32>(result);
}

// 0x443F80
bool CGangWars::CanPlayerStartAGangWarHere(CZoneInfo* zoneInfo) {
    // Como el original (0x443F8A..0x443F9D): ZoneInfoArray + indice*0x11 sin comprobar el rango del indice
    if (bTrainingMission)
        return zoneInfo == CTheZones::ZoneInfoArray.data() + ZoneInfoForTraining;

    if (NumSpecificZones == 0)
        return true;

    // Como el original (0x443FAC..0x443FD9): compara con ZoneInfoArray[zona.m_ZoneInfoIndex] (incluido el indice 0, que
    // GetZoneInfo(CZone*) convertiria en nullptr) mientras i < NumSpecificZones (con signo)
    for (int32 i = 0; i < NumSpecificZones; ++i) {
        const auto& zone = CTheZones::NavigationZoneArray.data()[aSpecificZones.data()[i]];
        if (zoneInfo == CTheZones::ZoneInfoArray.data() + static_cast<uint16>(zone.m_ZoneInfoIndex)) {
            return true;
        }
    }

    return false;
}

// 0x444040
void CGangWars::CheerVictory() {
    auto& playerGroup = FindPlayerPed()->GetPlayerGroup();

    CPed* nearestMember = nullptr;
    playerGroup.FindDistanceToNearestMember(&nearestMember);
    if (!nearestMember) {
        return;
    }

    // Como el original (0x444080..0x4442C1): _stricmp (0x8229B6) del INICIO de la zona (CZone::m_InfoLabel, +0; el puntero
    // que se pasa es la propia zona), en este orden, y sin coincidencia no dice nada. La etiqueta de East LS es "ELS" (0x8598E4).
    static constexpr std::pair<const char*, eGlobalSpeechContext> mapping[]{
        {"CHC",  CTX_GLOBAL_TAKE_TURF_LAS_COLINAS      },
        {"LFL",  CTX_GLOBAL_TAKE_TURF_LOS_FLORES       },
        {"EBE",  CTX_GLOBAL_TAKE_TURF_EAST_BEACH       },
        {"ELS",  CTX_GLOBAL_TAKE_TURF_EAST_LS          },
        {"JEF",  CTX_GLOBAL_TAKE_TURF_JEFFERSON        },
        {"GLN",  CTX_GLOBAL_TAKE_TURF_GLEN_PARK        },
        {"IWD",  CTX_GLOBAL_TAKE_TURF_IDLEWOOD         },
        {"GAN",  CTX_GLOBAL_TAKE_TURF_GANTON           },
        {"LMEX", CTX_GLOBAL_TAKE_TURF_LITTLE_MEXICO    },
        {"LIND", CTX_GLOBAL_TAKE_TURF_WILLOWFIELD      },
        {"PLS",  CTX_GLOBAL_TAKE_TURF_PLAYA_DEL_SEVILLE},
        {"SUN",  CTX_GLOBAL_TAKE_TURF_TEMPLE           },
    };
    for (const auto& [label, context] : mapping) {
        if (_stricmp(pZoneToFightOver->m_InfoLabel, label) == 0) {
            nearestMember->Say(context);
            return;
        }
    }
}

// 0x443FF0
void CGangWars::ClearSpecificZonesToTriggerGangWar() {
    NumSpecificZones = 0;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x4444B0
void CGangWars::ClearTheStreets() {
    // Como el original (0x4444B8..0x444522): pool de peatones del ultimo al primero
    auto* const pool = GetPedPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const ped = pool->GetAt(i);
        if (!ped || ped->IsPlayer() || !ped->IsCivilian())
            continue;

        if (auto task = ped->GetTaskManager().Find<CTaskComplexWander>()) {
            task->m_nMoveState = PEDMOVE_SPRINT;
        }
    }
}

// 0x444810
namespace {
float GangWaveRandomOffset(float base, float scale, float subtract) {
    const int32 random = CGeneral::GetRandomNumber();
    const float unit = std::bit_cast<float>(0x38000100u);
    float result;
    __asm {
        fild random
        fmul unit
        fmul scale
        fadd base
        fsub subtract
        fstp result
    }
    return result;
}
int32 GangWaveInteger(float value, float scale, float base, int32 multiple) {
    int64 result;
    uint16 oldControl, truncateControl;
    __asm {
        fnstcw oldControl
        mov ax, oldControl
        or ax, 0C00h
        mov truncateControl, ax
        fld value
        fmul scale
        fadd base
        fimul multiple
        fldcw truncateControl
        fistp result
        fldcw oldControl
    }
    return static_cast<int32>(result);
}
void GangWaveCommand(CPed* ped, CTask* task) {
    alignas(4) std::array<uint8, 0x18> event;
    plugin::CallMethod<0x4B0A00>(reinterpret_cast<CEventScriptCommand*>(event.data()), 3, task, false);
    auto* group = reinterpret_cast<CEventGroup*>(reinterpret_cast<uint8*>(ped->GetIntelligence()) + 0x68);
    plugin::CallMethod<0x4AB420>(group, event.data(), false);
    plugin::CallMethod<0x4B0A50>(reinterpret_cast<CEventScriptCommand*>(event.data()));
}
void GangWaveBlip(CPed* ped, eGangID gang, int32 scale) {
    const uint32 color = (uint32(*reinterpret_cast<uint8*>(0x8D1344 + gang)) << 24) |
                         (uint32(*reinterpret_cast<uint8*>(0x8D1350 + gang)) << 16) |
                         (uint32(*reinterpret_cast<uint8*>(0x8D135C + gang)) << 8) | 255u;
    auto* pool = *reinterpret_cast<CPedPool**>(0xB74490);
    const int32 ref = plugin::CallMethodAndReturn<int32, 0x4442D0>(pool, ped);
    const auto blip = CRadar::SetEntityBlip(BLIP_CHAR, ref, color, BLIP_DISPLAY_BLIPONLY);
    CRadar::ChangeBlipScale(blip, scale);
    CRadar::ChangeBlipColour(blip, static_cast<eBlipColour>(color));
}
}

bool CGangWars::CreateAttackWave(int32 warFerocity, int32 waveID) {
    const auto playerPosition = FindPlayerCoors();
    auto* player = FindPlayerPed();
    if (CGame::currArea || !(playerPosition.z <= 950.0f) ||
        !ThePaths.AreNodesLoadedForArea(playerPosition.x - 20.0f, playerPosition.x + 20.0f, playerPosition.y - 20.0f, playerPosition.y + 20.0f)) return false;
    if (!PedStreamedInForThisGang(Gang1)) {
        const int32 group = *reinterpret_cast<int32*>(0x8D2618 + int32(Gang1) * 12);
        const uint16 model = *reinterpret_cast<uint16*>(0xC0F358 + group * 42);
        CStreaming::RequestModel(model, 8);
    }
    if (int32(Gang1) != Gang2 && !PedStreamedInForThisGang(static_cast<eGangID>(Gang2))) {
        const int32 model = plugin::CallAndReturn<int32, 0x406E80>(Gang2 + 0x12, 0, 0);
        CStreaming::RequestModel(model, 8);
    }
    int32 model1 = static_cast<int32>(reinterpret_cast<uintptr_t>(player)), model2 = model1;
    int32 weapon1 = 5, weapon2 = 22;
    switch (warFerocity) {
    case 0: model1=336; model2=346; weapon1=5; weapon2=22; break;
    case 1: model1=346; model2=352; weapon1=22; weapon2=28; break;
    case 2: model1=352; model2=353; weapon1=28; weapon2=29; break;
    case 3: model1=353; model2=355; weapon1=29; weapon2=30; break;
    case 4: model1=355; model2=355; weapon1=30; weapon2=30; break;
    case 5: model1=348; model2=355; weapon1=24; weapon2=30; break;
    default: break;
    }
    if (bTrainingMission) {
        model1 = model2 = warFerocity == 0 ? 336 : warFerocity == 1 ? 346 : 372;
        weapon1 = weapon2 = warFerocity == 0 ? 5 : warFerocity == 1 ? 22 : 32;
    }
    const auto loaded = [](int32 model) { return *reinterpret_cast<uint8*>(0x8E4CD0 + model * 20) == 1; };
    if (!loaded(model1) || !loaded(model2)) {
        CStreaming::RequestModel(model1, 0);
        CStreaming::RequestModel(model2, 0);
        return false;
    }
    if (!PedStreamedInForThisGang(Gang1) || !PedStreamedInForThisGang(static_cast<eGangID>(Gang2))) return false;
    CVector previous{999999.9f, 999999.9f, 999999.9f};
    bool madePed = false;
    eGangID lastGang = Gang1;
    for (int32 pass = 0; pass < 2; ++pass) {
        const auto gang = pass == 0 ? Gang1 : static_cast<eGangID>(Gang2);
        lastGang = gang;
        bool found = false;
        CVector position;
        for (int32 attempt = 0; attempt < 20; ++attempt) {
            CVector candidate;
            CNodeAddress first, second;
            first.m_wAreaId = second.m_wAreaId = 0xFFFF;
            float fraction;
            if (!CCarCtrl::GenerateCarCreationCoors2(playerPosition, 1.0f, 0.0f, -1.0f, true, 50.0f, 50.0f, &candidate, &first, &second, &fraction, false, true)) continue;
            if (!TheCamera.IsSphereVisible(candidate, 7.0f)) {
                position = candidate;
                found = true;
                const auto delta = previous - candidate;
                if (std::sqrt(delta.z * delta.z + delta.y * delta.y + delta.x * delta.x) > 15.0f) break;
            } else if (!found) {
                position = candidate;
                found = true;
            }
        }
        if (!found) continue;
        previous = position;
        CVector direction{playerPosition.y - position.y, position.x - playerPosition.x, 0.0f};
        direction.Normalise();
        direction *= 1.2f;
        int32 count = GangWaveInteger(Difficulty, 0.3f, 0.7f, static_cast<int32>(static_cast<uint32>(warFerocity) + 3));
        if (bTrainingMission) count = 2;
        if (warFerocity == 5) count = 10;
        if (count > 0) {
            int32 row = -(count / 2) * 2;
            for (int32 remaining = count; remaining != 0; --remaining, row += 2) {
                CVector pedPosition{direction.x * float(row) + position.x, direction.y * float(row) + position.y, direction.z * float(row) + position.z};
                pedPosition.x = GangWaveRandomOffset(pedPosition.x, 3.0f, 1.5f);
                pedPosition.y = GangWaveRandomOffset(pedPosition.y, 3.0f, 1.5f);
                int32 model;
                if (!PickStreamedInPedForThisGang(gang, model)) continue;
                pedPosition.z = CWorld::FindGroundZFor3DCoord({pedPosition.x,pedPosition.y,pedPosition.z+2.0f}, nullptr, nullptr) + 1.3f;
                auto* memory = CPed::operator new(0x79C);
                auto* ped = memory ? plugin::CallMethodAndReturn<CPed*,0x5DDB70>(static_cast<CPed*>(memory), int32(Gang1)+7, model) : nullptr;
                ped->SetPosn(pedPosition);
                ped->SetCharCreatedBy(PED_MISSION);
                CWorld::Add(ped);
                auto* taskMemory = CTask::operator new(0x38);
                auto* task = taskMemory ? plugin::CallMethodAndReturn<CTask*,0x620E30>(static_cast<CTask*>(taskMemory), player, -1, 0, 0, 0, 2) : nullptr;
                alignas(4) std::array<uint8,0x18> event;
                plugin::CallMethod<0x4B0A00>(reinterpret_cast<CEventScriptCommand*>(event.data()), 3, task, false);
                plugin::CallMethod<0x4AB420>(reinterpret_cast<CEventGroup*>(reinterpret_cast<uint8*>(ped->GetIntelligence())+0x68), event.data(), false);
                const auto weapon = static_cast<eWeaponType>(CGeneral::GetRandomNumberInRange(-0.3f,1.3f) <= Difficulty ? weapon2 : weapon1);
                ped->GiveWeapon(weapon, 5000, false);
                ped->SetCurrentWeapon(weapon);
                *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(ped)+0x71A) = static_cast<uint8>(GangWaveInteger(Difficulty, 25.0f, 70.0f, 1));
                *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped)+0x474) |= 0x3000;
                ped->m_fHealth = ped->m_fMaxHealth = 120.0f;
                GangWaveBlip(ped, gang, 2);
                madePed = true;
                plugin::CallMethod<0x4B0A50>(reinterpret_cast<CEventScriptCommand*>(event.data()));
            }
        }
        CVector pickup{GangWaveRandomOffset(position.x,4.0f,2.0f),GangWaveRandomOffset(position.y,4.0f,2.0f),position.z};
        pickup.z=CWorld::FindGroundZFor3DCoord({pickup.x,pickup.y,pickup.z+1.0f},nullptr,nullptr)+0.75f;
        if (((waveID == 0 || waveID == 1) && pass == 0) || waveID == 2) {
            const uint16 pickupModel = waveID == 1 || (waveID == 2 && pass != 0) ? ModelIndices::MI_PICKUP_BODYARMOUR : ModelIndices::MI_PICKUP_HEALTH;
            CPickups::GenerateNewOne(pickup,pickupModel,PICKUP_ONCE_TIMEOUT_SLOW,0,0,false,nullptr);
        }
    }
    if (!madePed) return false;
    if (!bTrainingMission && !pDriveByCar) {
        const auto model=CPopulation::PickGangCar(Gang1);
        if (int32(model) >= 0 && loaded(model)) {
            pDriveByCar=CCarCtrl::CreateCarForScript(model,FindPlayerCoors(),false);
            if (pDriveByCar) {
                pDriveByCar->RegisterReference(reinterpret_cast<CEntity**>(&pDriveByCar));
                CCarCtrl::JoinCarWithRoadSystemGotoCoors(pDriveByCar,FindPlayerCoors(),false,false);
                auto* raw=reinterpret_cast<uint8*>(pDriveByCar);
                raw[0x42D]|=8;raw[0x3BA]=0x3D;raw[0x3D0]=10;
                *reinterpret_cast<CPed**>(raw+0x41C)=FindPlayerPed();
                raw[0x3B9]=2;raw[0x36]=(raw[0x36]&7)|0x18;raw[0x42C]|=0x40;raw[0x3DD]=30;
                CCarCtrl::SetUpDriverAndPassengersForVehicle(pDriveByCar,int32(Gang1)+14,1,false,false,1);
                if (pDriveByCar->m_pDriver) pDriveByCar->m_pDriver->GetIntelligence()->SetPedDecisionMakerType(-1);
                for (int32 seat=0;seat<8;++seat) {
                    auto* ped=pDriveByCar->m_apPassengers[seat];
                    if (!ped) continue;
                    ped->GiveDelayedWeapon(WEAPON_MICRO_UZI,1500);
                    ped->SetCurrentWeapon(WEAPON_MICRO_UZI);
                    auto* memory=CTask::operator new(0x44);
                    auto* task=memory?plugin::CallMethodAndReturn<CTask*,0x6217D0>(static_cast<CTask*>(memory),FindPlayerPed(),nullptr,100.0f,50,8,~seat&1):nullptr;
                    GangWaveCommand(ped,task);
                }
                GangWaveBlip(pDriveByCar->m_pDriver,lastGang,3);
                *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(pDriveByCar->m_pDriver)+0x474)|=0x2000;
            }
        }
    }
    return true;
}

// 0x4453D0
namespace {
// Como el original (0x44553B..0x4455E8): posicion de cada defensor. Se usa el x87 a precision completa para fsin/fcos
// y el orden exacto de operaciones: sin(a) * (r1 * unidad * 3.0 + 2.0) + x (el 2.0 va DENTRO del factor, no al final).
void GangDefenderOffset(int32 pedIndex, int32 pedTotal, int32 random1, int32 random2, float baseX, float baseY, float& outAngle, float& outX, float& outY) {
    const float twoPiValue = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
    const float unitValue = std::bit_cast<float>(0x38000100u);  // 0x858C7C
    const float radiusScale = 3.0f;                              // 0x858B3C
    const float radiusBias = 2.0f;                               // 0x858CA0
    float angleValue, xValue, yValue;
    __asm {
        fild random1
        fmul unitValue
        fild pedIndex
        fmul twoPiValue
        fidiv pedTotal
        fst angleValue
        fsin
        fxch st(1)
        fmul radiusScale
        fadd radiusBias
        fmulp st(1), st
        fadd baseX
        fstp xValue
        fild random2
        fmul unitValue
        fld angleValue
        fcos
        fxch st(1)
        fmul radiusScale
        fadd radiusBias
        fmulp st(1), st
        fadd baseY
        fstp yValue
    }
    outAngle = angleValue;
    outX = xValue;
    outY = yValue;
}
float GangPickupOffset(int32 random, float base) {
    // Como el original (0x445A4D..0x445A70): ((r * unidad) * 4.0 + base) - 2.0
    const float unitValue = std::bit_cast<float>(0x38000100u); // 0x858C7C
    return ((static_cast<float>(random) * unitValue) * 4.0f + base) - 2.0f;
}
}

bool CGangWars::CreateDefendingGroup(int32 unused) {
    if (!PedStreamedInForThisGang(static_cast<eGangID>(Gang1))) {
        // Como el original (0x4453FF..0x445416): primer modelo del grupo (movzx word [grupo*0x2A + 0xC0F358]), sin comprobar el numero de peatones
        const int32 group = *reinterpret_cast<int32*>(0x8D2618 + int32(Gang1) * 12);
        const uint16 model = *reinterpret_cast<uint16*>(0xC0F358 + group * 42);
        CStreaming::RequestModel(model, STREAMING_KEEP_IN_MEMORY);

        return false;
    }

    if (!ThePaths.AreNodesLoadedForArea(PointOfAttack.x, PointOfAttack.x, PointOfAttack.y, PointOfAttack.y))
        return false;

    auto node = ThePaths.FindNodeClosestToCoors(PointOfAttack, PATH_TYPE_VEH, 400.0f, 0, 0, 0, 0, 1);
    if (!node.IsValid())
        return false;

    auto nodePos = ThePaths.GetPathNode(node)->GetPosition();
    auto playerPos = FindPlayerCoors();
    // Como el original (0x4454F8..0x445503): sale si dist <= 40.0f o NaN (test ah,41h + jne)
    if (!(DistanceBetweenPoints2D(playerPos, nodePos) > 40.0f))
        return false;

    // Como el original (0x445509..0x445534): ((Difficulty * 0.4 + 0.6) * 10) truncado; el bucle usa el valor CON signo
    const int32 pedCount = static_cast<int32>(10.0f * (0.4f * Difficulty + 0.6f));

    int32 outPedId;
    for (int32 i = 0; i < pedCount; i++) {
        if (!PickStreamedInPedForThisGang(static_cast<eGangID>(Gang1), outPedId))
            continue;

        const int32 random1 = CGeneral::GetRandomNumber();
        const int32 random2 = CGeneral::GetRandomNumber();
        float angle, pedX, pedY;
        GangDefenderOffset(i, pedCount, random1, random2, nodePos.x, nodePos.y, angle, pedX, pedY);

        auto pedPos = CVector{ pedX, pedY, nodePos.z + 2.0f };
        pedPos.z = CWorld::FindGroundZFor3DCoord(pedPos, nullptr, nullptr) + 1.0f; // Como el original (0x445604): fadd 1.0f

        // Como el original (0x44560A..0x44563D) y como CreateAttackWave: CPed::operator new(0x79C) (0x5E4720) y el constructor del
        // EXE 0x5DDB70 (CCivilianPed::CCivilianPed(tipo, modelo)); si no hay memoria el EXE sigue usando el puntero nulo
        auto* const pedMemory = CPed::operator new(0x79C);
        auto* const ped = pedMemory ? plugin::CallMethodAndReturn<CCivilianPed*, 0x5DDB70>(static_cast<CCivilianPed*>(pedMemory), Gang1 + 7, outPedId) : nullptr;
        ped->SetPosn(pedPos);
        ped->m_fAimingRotation = angle;
        ped->m_fCurrentRotation = angle;
        ped->SetHeading(angle);
        ped->SetCharCreatedBy(PED_MISSION);
        CWorld::Add(ped);

        auto task = new CTaskComplexKillPedOnFoot(FindPlayerPed(), -1, 0, 0, 0, 2);
        CEventScriptCommand event(TASK_PRIMARY_PRIMARY, task, false);
        ped->GetEventGroup().Add(&event);

        ped->SetWeaponAccuracy(static_cast<uint8>(static_cast<int32>(90.0f - Difficulty * 60.0f)));
        ped->GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_HATE, CPedType::GetPedFlag(PED_TYPE_PLAYER1));

        eWeaponType weaponToGive = [&i]{
            switch (i % 4) {
            case 0: return WEAPON_PISTOL;
            case 1: return WEAPON_MICRO_UZI;
            case 2: return WEAPON_SHOTGUN;
            case 3: return WEAPON_MP5;
            default: NOTSA_UNREACHABLE("suppress warning"); return WEAPON_DILDO1;
            }
        }();

        ped->GiveDelayedWeapon(weaponToGive, 5000);
        ped->SetCurrentWeapon(weaponToGive);
        ped->bPartOfAttackWave = true;
        ped->bClearRadarBlipOnDeath = true;

        auto color = GetGangColor(Gang1); // 0x44582F
        auto blip = CRadar::SetEntityBlip(BLIP_CHAR, CPools::GetPedRef(ped), color, BLIP_DISPLAY_BLIPONLY);
        CRadar::ChangeBlipScale(blip, 2);
        CRadar::ChangeBlipColour(blip, color);
    }

    // Como el original (0x445866..0x445A2F): el indice del n-esimo nodo es 1, 2, 3 (inc edi antes de empujarlo) y la posicion
    // del jugador se vuelve a pedir en cada vuelta
    for (int32 i = 1; i <= 3; i++) { // todo: magic number
        auto carNode = ThePaths.FindNthNodeClosestToCoors(PointOfAttack, 0, 100.0f, false, false, i, false, true, nullptr);
        auto carNodePos = ThePaths.GetPathNode(carNode)->GetPosition();

        if (!(DistanceBetweenPoints2D(carNodePos, FindPlayerCoors()) > 25.0f)) // sale si <= 25.0f o NaN
            continue;

        auto gangCarModel = CPopulation::PickGangCar(Gang1);
        if (static_cast<int32>(gangCarModel) >= 0 && CStreaming::GetInfo(gangCarModel).IsLoaded()) {
            // Como el original (0x44596F..0x445A25): el vector es perpendicular a (carNode - node): (dy, -dx, 0) normalizado
            // es el "forward" y (v.y, -v.x, 0) el "right"
            auto fwd = CVector{ carNodePos.y - nodePos.y, nodePos.x - carNodePos.x, 0.0f };
            fwd.Normalise();

            auto car = CCarCtrl::CreateCarForScript(gangCarModel, carNodePos, false);
            car->m_matrix->GetForward() = fwd;
            car->m_matrix->GetRight() = CVector{fwd.y, -fwd.x, 0.0f};

            car->AsAutomobile()->PlaceOnRoadProperly();
            car->vehicleFlags.bIsLocked = false;
            car->SetVehicleCreatedBy(MISSION_VEHICLE);
            car->vehicleFlags.bPartOfAttackWave = true;
        }
    }

    // Como el original (0x445A35..0x445AFD): rand directos (r * unidad * 4.0 + base - 2.0), z = suelo(x, y, nodo.z + 1.0) + 0.75
    // y el modelo sale de rand() & 1 (1 = salud, 0 = chaleco)
    const int32 pickupRandomX = CGeneral::GetRandomNumber();
    const float pickupX = GangPickupOffset(pickupRandomX, nodePos.x);
    const int32 pickupRandomY = CGeneral::GetRandomNumber();
    const float pickupY = GangPickupOffset(pickupRandomY, nodePos.y);
    auto pickupCoors = CVector{ pickupX, pickupY, nodePos.z + 1.0f };
    pickupCoors.z = CWorld::FindGroundZFor3DCoord(pickupCoors, nullptr, nullptr) + 0.75f;

    const ModelIndex pickupModel = (CGeneral::GetRandomNumber() & 1) ? ModelIndices::MI_PICKUP_HEALTH : ModelIndices::MI_PICKUP_BODYARMOUR;

    CPickups::GenerateNewOne(pickupCoors, pickupModel.get_underlying(), PICKUP_ONCE_TIMEOUT_SLOW, 0, 0, false, nullptr);
    return true;
}

// 0x446400
void CGangWars::DoStuffWhenPlayerVictorious() {
    ReleasePedsInAttackWave(true, false);
    ReleaseCarsInAttackWave();
    CheerVictory();
    State = NOT_IN_WAR;
    CMessages::AddMessageQ(TheText.Get("GW_YRS"), 4500, 1, true);
    CMessages::AddToPreviousBriefArray(TheText.Get("GW_YRS"), -1, -1, -1, -1, -1, -1, nullptr);
    Provocation = 0.0f;
    TellGangMembersTo(true);
    CStats::IncrementStat(STAT_RESPECT, 45.0f);
    CTheZones::FillZonesWithGangColours(false);

    // Como el original (0x446488..0x4464B2): (T - 240000) y si NO es mayor que 30000 (fcom + test ah,41h + jne, tambien con NaN)
    // se guarda 30000.0f (0x46EA6000); es decir, un MAXIMO, no un minimo
    const float remaining = TimeTillNextAttack - 240'000.0f;
    TimeTillNextAttack = remaining > 30'000.0f ? remaining : 30'000.0f;
}

// 0x443AE0
bool CGangWars::DoesPlayerControlThisZone(CZoneInfo* zoneInfo) {
    auto enemyDensity = zoneInfo->GangStrength[GANG_BALLAS] + zoneInfo->GangStrength[GANG_VAGOS];
    auto groveDensity = zoneInfo->GangStrength[GANG_GROVE];
    return groveDensity != 0 && groveDensity >= enemyDensity;
}

// 0x4439C0
bool CGangWars::DontCreateCivilians() {
    return State != NOT_IN_WAR;
}

// 0x4464C0
void CGangWars::EndGangWar(bool end) {
    State = NOT_IN_WAR;

    if (State2 != NO_ATTACK) { // Como el original (0x4464C6): test eax,eax / je; cualquier valor distinto de 0, no solo WAR_NOTIFIED
        State2 = NO_ATTACK;
        TimeTillNextAttack = static_cast<float>(CalculateTimeTillNextAttack());

        auto releasedPeds = ReleasePedsInAttackWave(true, false);
        MakeEnemyGainInfluenceInZone(Gang1, 3 * releasedPeds);
    }

    Provocation = 0.0f;
    CTheZones::FillZonesWithGangColours(false);
    ReleasePedsInAttackWave(true, end);
    ReleaseCarsInAttackWave();
}

// 0x443AC0
bool CGangWars::GangWarFightingGoingOn() {
    return State != NOT_IN_WAR || State2 == PLAYER_CAME_TO_WAR;
}

// 0x443AA0
bool CGangWars::GangWarGoingOn() {
    return State != NOT_IN_WAR || State2 != NO_ATTACK; // Como el original (0x443AA9): State2 distinto de 0, no solo WAR_NOTIFIED
}

// 0x445FD0
void CGangWars::MakeEnemyGainInfluenceInZone(int32 gangId, int32 density) {
    auto* const info = pZoneInfoToFightOver;
    // Como el original (0x445FD5..0x446001): el control previo se calcula antes de sumar y gana ESE valor para decidir el contador
    const bool controlledBefore = info->GangStrength[GANG_GROVE] != 0 && info->GangStrength[GANG_GROVE] >= info->GangStrength[GANG_VAGOS] + info->GangStrength[GANG_BALLAS];

    auto totalGangDensity = info->GangStrength[GANG_BALLAS]
        + info->GangStrength[GANG_GROVE]
        + info->GangStrength[GANG_VAGOS];

    if (!totalGangDensity)
        return;

    info->GangStrength[gangId] += density;

    // Como el original (0x44600D..0x446048): solo cuenta como territorio perdido si ANTES lo controlaba y ahora no
    if (controlledBefore && !DoesPlayerControlThisZone(info)) {
        CStats::IncrementStat(STAT_TERRITORIES_LOST, 1.0f);
    }
}

// 0x445E80
bool CGangWars::MakePlayerGainInfluenceInZone(float removeMult) {
    bool controlledBefore = DoesPlayerControlThisZone(pZoneInfoToFightOver);
    uint32 totalEnemyDensity = 0u; // Como el original (ebp): se acumula en 32 bits, no en un uint8 que daria la vuelta

    // for all gangs except grove (esi = 0..9, salta el 1)
    static constexpr eGangID gangs[] = {
        GANG_BALLAS, GANG_VAGOS, GANG_RIFA, GANG_DANANGBOYS, GANG_MAFIA, GANG_TRIAD, GANG_AZTECAS,
        GANG_UNUSED1, GANG_UNUSED2
    };
    for (auto gang : gangs) {
        auto& density = pZoneInfoToFightOver->GangStrength[gang];
        auto densityInitial = density;

        // Como el original (0x445EB9..0x445ED0): (1.0f - removeMult) * densidad truncado con _ftol2; se guarda el byte bajo
        density = static_cast<uint8>(static_cast<int32>((1.0f - removeMult) * static_cast<float>(density)));
        if (density < 4u) {
            density = 0u;
        }
        pZoneInfoToFightOver->GangStrength[GANG_GROVE] += densityInitial - density;
        totalEnemyDensity += density;
    }

    if (!controlledBefore && DoesPlayerControlThisZone(pZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_TAKEN_OVER, 1.0f);
    }

    return totalEnemyDensity == 0u;
}

// 0x4439D0
bool CGangWars::PedStreamedInForThisGang(eGangID gangId) {
    for (auto midx : CPopulation::GetModelsInPedGroup(CPopulation::GetGangGroupId(gangId))) {
        if (CStreaming::IsModelLoaded(midx)) {
            return true;
        }
    }
    return false;
}

// 0x443A20
bool CGangWars::PickStreamedInPedForThisGang(eGangID gangId, int32& outPedId) {
    // Como el original (0x443A20..0x443A9F): sin comprobar el grupo (solo el numero de peatones con signo, movsx word),
    // el cursor es la variable del EXE 0x96ABD4 (no CGarages::aCarsInSafeHouse, que esta en 0x96ABD8), el modelo es movzx word
    // y el cursor se guarda tambien al fallar.
    const auto groupId = *reinterpret_cast<int32*>(0x8D2618 + int32(gangId) * 12);
    auto numPeds = static_cast<int32>(*reinterpret_cast<int16*>(0xC0ECC0 + groupId * 2));
    if (numPeds <= 0)
        return false;

    int32 cursor = PedPickCursor;
    for (auto i = 0; i < numPeds; i++) {
        cursor = (cursor + 1) % numPeds;
        outPedId = *reinterpret_cast<uint16*>(0xC0F358 + (groupId * 21 + cursor) * 2);

        if (*reinterpret_cast<uint8*>(0x8E4CD0 + outPedId * 20) == 1) { // CStreaming::ms_aInfoForModel[model].m_LoadState == LOADED
            PedPickCursor = cursor;
            return true;
        }
        numPeds = static_cast<int32>(*reinterpret_cast<int16*>(0xC0ECC0 + *reinterpret_cast<int32*>(0x8D2618 + int32(gangId) * 12) * 2));
    }

    PedPickCursor = cursor;
    return false;
}

// 0x443B00
bool CGangWars::PickZoneToAttack() {
    const auto totalZones = static_cast<int32>(static_cast<uint16>(CTheZones::TotalNumberOfNavigationZones)); // movzx word
    CCarCtrl::InitSequence(totalZones);

    // Como el original (0x443B30..0x443D38): los dos bucles estan ANIDADOS. Por cada zona enemiga (en orden de secuencia) se recorre
    // TODA la lista de zonas EN ORDEN DE ARRAY buscando una zona del jugador cercana; si no hay ninguna se sigue con la siguiente
    // zona enemiga (antes se tomaba solo la primera y el segundo bucle recorria tambien en orden de secuencia).
    for (int32 i = 0; i < totalZones; i++) {
        auto enemyGangZone = &CTheZones::NavigationZoneArray.data()[CCarCtrl::FindSequenceElement(i)];
        const auto enemyInfoIndex = static_cast<uint16>(enemyGangZone->m_ZoneInfoIndex);
        if (!enemyInfoIndex)
            continue;

        auto enemyGangZoneInfo = &CTheZones::ZoneInfoArray.data()[enemyInfoIndex];
        if (enemyGangZoneInfo->GangStrength[GANG_VAGOS] + enemyGangZoneInfo->GangStrength[GANG_BALLAS] < 20)
            continue;

        // find a close territory that is controlled by the gsf
        for (int32 j = 0; j < totalZones; j++) {
            auto zone = &CTheZones::NavigationZoneArray.data()[j];
            const auto zoneInfoIndex = static_cast<uint16>(zone->m_ZoneInfoIndex);
            if (!zoneInfoIndex)
                continue;

            if ((float)zone->m_fX1 <= 2500.0f && (float)zone->m_fX2 >= 2500.0f && (float)zone->m_fY1 <= -1666.0f && (float)zone->m_fY2 >= -1666.0f) // todo:
                continue;

            auto zoneInfo = &CTheZones::ZoneInfoArray.data()[zoneInfoIndex];
            if (zoneInfo->GangStrength[GANG_GROVE] <= 15)
                continue;

            if (CTheZones::Calc2DDistanceBetween2Zones(enemyGangZone, zone) < 10.0f) {
                pZoneToFightOver = zone;
                pZoneInfoToFightOver = zoneInfo;
                // Como el original: Ballas solo si Vagos < Ballas (cmp + sbb), si no Vagos
                Gang1 = (enemyGangZoneInfo->GangStrength[GANG_VAGOS] < enemyGangZoneInfo->GangStrength[GANG_BALLAS]) ? GANG_BALLAS : GANG_VAGOS;
                PointOfAttack = CVector{
                    (float)(zone->m_fX1 + zone->m_fX2) / 2.0f,
                    (float)(zone->m_fY1 + zone->m_fY2) / 2.0f,
                    10.0f
                };

                if (DistanceBetweenPoints2D(FindPlayerCoors(), PointOfAttack) > 60.0f)
                    return true;
            }
        }
    }

    return false;
}

// 0x445E20
void CGangWars::ReleaseCarsInAttackWave() {
    // Como el original (0x445E21..0x445E6D): pool de vehiculos del ultimo al primero
    auto* const pool = GetVehiclePool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const vehicle = pool->GetAt(i);
        if (vehicle && vehicle->vehicleFlags.bPartOfAttackWave) {
            vehicle->vehicleFlags.bPartOfAttackWave = false;
            vehicle->SetVehicleCreatedBy(eVehicleCreatedBy::RANDOM_VEHICLE);
        }
    }
}

// Returns num of released peds
// 0x445C30
uint32 CGangWars::ReleasePedsInAttackWave(bool isEndOfWar, bool restoreGangPedsAcquaintance) {
    auto numReleasedPeds = 0u;

    // Como el original (0x445C45..0x445E0B): pool de peatones del ultimo al primero y los blips se limpian con la REFERENCIA
    // del peaton ((indice << 8) + estado), no con el indice del pool
    auto* const pool = GetPedPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const ped = pool->GetAt(i);
        if (!ped)
            continue;

        if (ped->bPartOfAttackWave) {
            ped->bPartOfAttackWave = false;
            ped->SetCharCreatedBy(PED_GAME);
            numReleasedPeds++;
            CRadar::ClearBlipForEntity(BLIP_CHAR, CPools::GetPedRef(ped));
            ped->bClearRadarBlipOnDeath = false;

            if (restoreGangPedsAcquaintance) {
                auto taskWander = CTaskComplexWander::GetWanderTaskByPedType(ped);
                CEventScriptCommand event(TASK_PRIMARY_PRIMARY, taskWander, false);
                ped->GetEventGroup().Add(&event);
                ped->m_acquaintance = CPedType::GetPedTypeAcquaintances(ped->m_nPedType);
            }
        }

        if (isEndOfWar && ped->bClearRadarBlipOnDeath) {
            CRadar::ClearBlipForEntity(BLIP_CHAR, CPools::GetPedRef(ped));
            ped->bClearRadarBlipOnDeath = false;
        }
    }

    return numReleasedPeds;
}

// 0x446570
void CGangWars::SetGangWarsActive(bool active) {
    if (active != bGangWarsActive) {
        CTheZones::FillZonesWithGangColours(!active);
        TimeTillNextAttack = static_cast<float>(CalculateTimeTillNextAttack());

        if (!active)
            EndGangWar(false);
    }
    bGangWarsActive = active;
}

// 0x444010
void CGangWars::SetSpecificZoneToTriggerGangWar(int32 zoneId) {
    aSpecificZones.data()[NumSpecificZones] = zoneId; // Como el original: sin comprobar el rango del indice
    NumSpecificZones++;
    CTheZones::FillZonesWithGangColours(false);
}

// 0x444300
void CGangWars::StartDefensiveGangWar() {
    if (PickZoneToAttack()) {
        Difficulty = TerritoryUnderControlPercentage;
        CHud::SetHelpMessage(TheText.Get("GW_ATK"), true, false, true);
        State2 = WAR_NOTIFIED;
        FightTimer = static_cast<uint32>(DistanceBetweenPoints2D(PointOfAttack, FindPlayerCoors()) * 200.0f + 240'000.0f);
        RadarBlip = CRadar::SetCoordBlip(BLIP_COORD, PointOfAttack, GetGangColor(Gang1), BLIP_DISPLAY_BLIPONLY, nullptr);

        CRadar::SetBlipSprite(RadarBlip, [] {
            switch (Gang1) {
            case GANG_BALLAS:
                return RADAR_SPRITE_GANGP;
            case GANG_VAGOS:
            case GANG_DANANGBOYS:
                return RADAR_SPRITE_GANGY;
            case GANG_RIFA:
                return RADAR_SPRITE_GANGB;
            default:
                return RADAR_SPRITE_ENEMYATTACK;
            }
        }());

        bPlayerIsCloseby = false;
        pZoneInfoToFightOver->RadarMode = 2;
        pZoneInfoToFightOver->ZoneColor = CRGBA{ 255, 0, 0, 160 };
    } else {
        TimeTillNextAttack = static_cast<float>(CalculateTimeTillNextAttack());
    }
}

// 0x446050
void CGangWars::StartOffensiveGangWar() {
    CoorsOfPlayerAtStartOfWar = FindPlayerCoors();
    auto zoneInfo = CTheZones::GetZoneInfo(CoorsOfPlayerAtStartOfWar, &pZoneToFightOver);
    pZoneInfoToFightOver = zoneInfo;
    Difficulty = TerritoryUnderControlPercentage;

    if (!CanPlayerStartAGangWarHere(pZoneInfoToFightOver)) {
        Provocation = 0.0f;
        return;
    }

    // Como el original (0x4460B0..0x446260): los dos mayores se buscan en los indices 0 y 2..9 (se salta Grove, indice 1) con
    // los mismos registros que el EXE: Gang1 solo se escribe cuando aparece un nuevo maximo y Gang2 recibe el valor ANTERIOR del
    // registro de Gang1 (que, si el indice 0 no tiene fuerza, es el Gang1 que ya hubiera en memoria). La suma tambien salta Grove.
    int32 gang1Reg = static_cast<int32>(Gang1); // eax
    int32 gang1Density = 0;                      // esi
    int32 gang2Density = 0;                      // ebp
    int32 densitySum = zoneInfo->GangStrength[0]; // ebx
    if (densitySum > 0) {
        Gang2 = Gang1;
        Gang1 = static_cast<eGangID>(0);
        gang1Reg = 0;
        gang1Density = zoneInfo->GangStrength[0];
    }
    for (int32 gang = 2; gang < 10; ++gang) {
        const int32 density = zoneInfo->GangStrength[gang];
        densitySum += density;
        if (density > gang1Density) {
            Gang2 = gang1Reg;
            gang1Reg = gang;
            gang2Density = gang1Density;
            Gang1 = static_cast<eGangID>(gang1Reg);
            gang1Density = density;
        } else if (density > gang2Density) {
            gang2Density = density;
            Gang2 = gang;
        }
    }

    Provocation = 0.0f;
    if (densitySum > 0 && (gang1Reg == GANG_BALLAS || gang1Reg == GANG_VAGOS)) {
        if (State2 != NO_ATTACK)
            return;

        // Como el original (0x44628C..0x4462D1): TheText.Get("GW_PROV") se pide dos veces (una por mensaje)
        CMessages::AddMessageQ(TheText.Get("GW_PROV"), 4500, 1, true);
        CMessages::AddToPreviousBriefArray(TheText.Get("GW_PROV"));
        TimeStarted = CTimer::GetTimeInMS();
        State = PRE_FIRST_WAVE;
        ClearTheStreets();
        WarFerocity = std::min(densitySum / 15, 2);

        if (CStats::GetStatValue(STAT_CITY_UNLOCKED) < 0.0f) {
            // Como el original (0x44632F..0x446367): <= 0.4f -> 0; >= 0.5f -> 5; entre medias no cambia
            if (TerritoryUnderControlPercentage <= 0.4f) {
                WarFerocity = 0;
            } else if (TerritoryUnderControlPercentage >= 0.5f) {
                WarFerocity = 5;
            }
        }
        if (bTrainingMission) // Como el original (0x446367): byte 0x96AB91, no bIsPlayerOnAMission
            WarFerocity = 0;

        if (gang1Density > gang2Density / 3)
            Gang2 = Gang1;

        TellGangMembersTo(false);
        pZoneInfoToFightOver->RadarMode = 2;
        zoneInfo->ZoneColor = CRGBA{ 255, 0, 0, 160 };
        pDriveByCar = nullptr;
    }
}

// 0x445F50
void CGangWars::StrengthenPlayerInfluenceInZone(int32 density) {
    bool controlledBefore = DoesPlayerControlThisZone(pZoneInfoToFightOver);

    auto& groveDensity = pZoneInfoToFightOver->GangStrength[GANG_GROVE];
    if (groveDensity < 55u) { // todo: magic number
        groveDensity = std::min(groveDensity + density, 55);
    }

    if (!controlledBefore && DoesPlayerControlThisZone(pZoneInfoToFightOver)) {
        CStats::IncrementStat(STAT_TERRITORIES_TAKEN_OVER, 1.0f);
    }
}

// 0x4465F0
void CGangWars::SwitchGangWarsActive() {
    SetGangWarsActive(!bGangWarsActive);
}

// 0x444530
void CGangWars::TellGangMembersTo(bool isGangWarEnding) {
    // return plugin::Call<0x444530, bool>(isGangWarEnding);

    // Como el original (0x444548..0x4447F7): pool de peatones del ultimo al primero
    auto* const pool = GetPedPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const pedPtr = pool->GetAt(i);
        if (!pedPtr)
            continue;
        auto& ped = *pedPtr;
        if (ped.IsPlayer())
            continue;

        if (!ped.IsGangster() || ped.m_nPedType == PED_TYPE_GANG2)
            continue;

        if (!isGangWarEnding) {
            auto player = FindPlayerPed();
            auto task = new CTaskComplexKillPedOnFoot(player, -1, 0, 0, 0, 2);
            CEventScriptCommand esc(TASK_PRIMARY_PRIMARY, task, false);
            ped.GetEventGroup().Add(&esc);

            continue;
        }

        // Como el original (0x4445E1..0x4446EC): un ocupante de vehiculo recibe SOLO la tarea del vehiculo; el paseo de pandilla
        // (0x4446F1) es la rama alternativa, no un paso posterior (antes se daban las dos)
        if (ped.IsInVehicle()) {
            CTask* task;
            if (!ped.IsInVehicleAsPassenger()) {
                task = new CTaskComplexCarDriveWander(ped.GetVehicleIfInOne(), DRIVING_STYLE_STOP_FOR_CARS, 10.0f);
            } else {
                task = new CTaskSimpleCarDrive(ped.GetVehicleIfInOne());
            }
            CEventScriptCommand event(TASK_PRIMARY_PRIMARY, task, false);
            ped.GetEventGroup().Add(&event);
            continue;
        }

        auto task = new CTaskComplexWanderGang(PEDMOVE_WALK, CGeneral::GetRandomIntInRange(0, 8), 5000, true, 0.5f); // 0x407180
        CEventScriptCommand event(TASK_PRIMARY_PRIMARY, task, false);
        ped.GetEventGroup().Add(&event);
    }
}

// FIX_BUGS: originally has int32* type, but changed to unsigned due possible UB
// 0x443D50
void CGangWars::TellStreamingWhichGangsAreNeeded(uint32& gangsBitFlags) {
    if (State2 == NO_ATTACK)
        return;

    if (DistanceBetweenPoints2D(PointOfAttack, FindPlayerCoors()) < 150.0f) {
        gangsBitFlags |= 1 << Gang1;
    }
}

namespace {
uint32 GangWarTimeDelta(bool negative) {
        const float step = CTimer::GetTimeStep(), seconds = 0.02f, milliseconds = negative ? -1000.0f : 1000.0f;
        int64 result;
        uint16 oldControl, truncateControl;
        __asm {
            fnstcw oldControl
            mov ax, oldControl
            or ax, 0C00h
            mov truncateControl, ax
            fld step
            fmul seconds
            fmul milliseconds
            fldcw truncateControl
            fistp result
            fldcw oldControl
        }
        return static_cast<uint32>(result);
}
}

// 0x446610
void CGangWars::Update() {
    ZoneScoped;
    if (CTheScripts::IsPlayerOnAMission() && !bIsPlayerOnAMission && NumSpecificZones == 0) EndGangWar(true);
    bIsPlayerOnAMission = CTheScripts::IsPlayerOnAMission();
    if (CCutsceneMgr::IsCutsceneProcessing()) return;
    if (static_cast<uint8>(CTimer::m_FrameCounter) == 56) UpdateTerritoryUnderControlPercentage();
    if (!bGangWarsActive || CGameLogic::IsCoopGameGoingOn()) return;
    Provocation -= CTimer::GetTimeStep() * 0.0005f;
    if (Provocation < 0.0f) Provocation = 0.0f;
    const auto message = [](const char* key, const char* brief) {
        CMessages::AddMessageQ(TheText.Get(key), 4500, 1, true);
        CMessages::AddToPreviousBriefArray(TheText.Get(brief), -1, -1, -1, -1, -1, -1, nullptr);
    };
    switch (State) {
    case NOT_IN_WAR:
        if (Provocation > 2.1f) StartOffensiveGangWar();
        break;
    case PRE_FIRST_WAVE:
        if (CTimer::GetTimeInMS() > TimeStarted + 10000 && CreateAttackWave(std::max(WarFerocity, 0), 0)) {
            State = FIRST_WAVE;
            TimeStarted = CTimer::GetTimeInMS();
        }
        break;
    case PRE_SECOND_WAVE:
        if (CTimer::GetTimeInMS() > TimeStarted + 10000 && CreateAttackWave(std::max(static_cast<int32>(static_cast<uint32>(WarFerocity) + 1), 0), 1)) {
            State = SECOND_WAVE;
            TimeStarted = CTimer::GetTimeInMS();
        }
        break;
    case PRE_THIRD_WAVE:
        if (CTimer::GetTimeInMS() > TimeStarted + 10000 && CreateAttackWave(std::max(static_cast<int32>(static_cast<uint32>(WarFerocity) + 2), 0), 2)) {
            State = THIRD_WAVE;
            TimeStarted = CTimer::GetTimeInMS();
        }
        break;
    case FIRST_WAVE:
    case SECOND_WAVE:
        if (AttackWaveOvercome()) {
            ReleasePedsInAttackWave(false, false);
            ReleaseCarsInAttackWave();
            if (MakePlayerGainInfluenceInZone(0.3f)) {
                DoStuffWhenPlayerVictorious();
            } else {
                const bool first = State == FIRST_WAVE;
                message(first ? "GW_CLR1" : "GW_CLR2", first ? "GW_CLR1" : "GW_CLR2");
                State = first ? PRE_SECOND_WAVE : PRE_THIRD_WAVE;
                TimeStarted = CTimer::GetTimeInMS();
            }
        }
        break;
    case THIRD_WAVE:
        if (AttackWaveOvercome()) {
            MakePlayerGainInfluenceInZone(1.0f);
            DoStuffWhenPlayerVictorious();
        }
        break;
    default: break;
    }
    if (State != NOT_IN_WAR) {
        const auto* zone = pZoneToFightOver;
        const bool inArea = FindPlayerCoors().x >= float(zone->m_fX1) - 50.0f &&
                            FindPlayerCoors().x <= float(zone->m_fX2) + 50.0f &&
                            FindPlayerCoors().y >= float(zone->m_fY1) - 50.0f &&
                            FindPlayerCoors().y <= float(zone->m_fY2) + 50.0f;
        if (CGame::currArea || inArea) {
            LastTimeInArea = CTimer::GetTimeInMS();
        } else if (CTimer::GetTimeInMS() - LastTimeInArea > 30000) {
            State = NOT_IN_WAR;
            Provocation = 0.0f;
            ReleasePedsInAttackWave(true, false);
            ReleaseCarsInAttackWave();
            CTheZones::FillZonesWithGangColours(false);
            message("GW_FLEE", "GW_WARN");
        } else if (CTimer::GetTimeInMS() - LastTimeInArea > 10000 && CTimer::GetPreviousTimeInMS() - LastTimeInArea <= 10000) {
            message("GW_WARN", "GW_WARN");
        }
    }
    if (State != NOT_IN_WAR || CTheScripts::IsPlayerOnAMission() || !(FindPlayerCoors().z <= 950.0f)) {
        if (TimeTillNextAttack <= 30000.0f) TimeTillNextAttack = 30000.0f;
    }
    const auto expired = [&] {
        FightTimer += GangWarTimeDelta(true);
        return static_cast<int32>(FightTimer) < 0;
    };
    if (State2 == NO_ATTACK) {
        if (!CTheScripts::IsPlayerOnAMission() && !bTrainingMission &&
            (!FindPlayerVehicle() || (FindPlayerVehicle()->m_nVehicleSubType != VEHICLE_TYPE_HELI && FindPlayerVehicle()->m_nVehicleSubType != VEHICLE_TYPE_PLANE))) {
            if (State == NOT_IN_WAR) {
                const float delta = static_cast<float>(GangWarTimeDelta(false));
                TimeTillNextAttack -= CWeather::WeatherRegion == WEATHER_REGION_LA ? delta : delta * 0.06f;
            }
            if (TimeTillNextAttack < 0.0f) StartDefensiveGangWar();
        }
    } else if (State2 == WAR_NOTIFIED) {
        const auto delta = FindPlayerCoors() - PointOfAttack;
        if (std::sqrt(delta.y * delta.y + delta.x * delta.x) >= 70.0f) {
            if (expired()) {
                message("GW_NOSH", "GW_NOSH");
                State2 = NO_ATTACK;
                MakeEnemyGainInfluenceInZone(Gang1, 30);
                CTheZones::FillZonesWithGangColours(false);
                TimeTillNextAttack = static_cast<float>(CalculateTimeTillNextAttack());
                CStats::DecrementStat(STAT_RESPECT, 30.0f);
            }
        } else if (CreateDefendingGroup(0)) {
            FightTimer += 30000;
            State2 = PLAYER_CAME_TO_WAR;
        }
    } else if (State2 == PLAYER_CAME_TO_WAR) {
        if (AttackWaveOvercome()) {
            State2 = NO_ATTACK;
            TimeTillNextAttack = static_cast<float>(CalculateTimeTillNextAttack());
            message("GW_WON", "GW_WON");
            StrengthenPlayerInfluenceInZone(10);
            CTheZones::FillZonesWithGangColours(false);
            ReleasePedsInAttackWave(true, false);
        } else if (expired()) {
            State2 = NO_ATTACK;
            TimeTillNextAttack = static_cast<float>(CalculateTimeTillNextAttack());
            message("GW_SLOW", "GW_SLOW");
            MakeEnemyGainInfluenceInZone(Gang1, 3 * ReleasePedsInAttackWave(true, false));
            CTheZones::FillZonesWithGangColours(false);
            CStats::DecrementStat(STAT_RESPECT, 30.0f);
        }
    }
    if (RadarBlip) {
        if (State2 == NO_ATTACK || State2 == PLAYER_CAME_TO_WAR) {
            CRadar::ClearBlip(RadarBlip);
            RadarBlip = 0;
        } else {
            const int32 timer = static_cast<int32>(FightTimer);
            const int period = timer > 120000 ? 10 : timer > 60000 ? 9 : timer > 30000 ? 8 : 7;
            CRadar::ChangeBlipDisplay(RadarBlip, (CTimer::GetTimeInMS() >> period) & 1 ? BLIP_DISPLAY_NEITHER : BLIP_DISPLAY_BLIPONLY);
        }
    }
    if (State2 != NO_ATTACK) {
        const auto delta = FindPlayerCoors() - PointOfAttack;
        if (std::sqrt(delta.y * delta.y + delta.x * delta.x) >= 150.0f) {
            bPlayerIsCloseby = false;
        } else if (!bPlayerIsCloseby) {
            CStreaming::StreamZoneModels_Gangs(FindPlayerCoors());
            bPlayerIsCloseby = true;
        }
    }
}

// 0x443DE0
void CGangWars::UpdateTerritoryUnderControlPercentage() {
    // Como el original (0x443DE0..0x443F7C): solo las zonas de navegacion activas (el contador, sin signo), cada zona con
    // ZoneInfoIndex != 0 cuenta para UNA sola pandilla: Grove si (Ballas + Vagos) <= Grove (se ignoran las zonas con las tres a 0),
    // y si no Vagos cuando Ballas <= Vagos, y Ballas en el resto de casos (el orden y los empates son los del EXE).
    int32 ballasZones = 0, groveZones = 0, vagosZones = 0;
    const auto totalZones = static_cast<int32>(static_cast<uint16>(CTheZones::TotalNumberOfNavigationZones));
    for (int32 i = 0; i < totalZones; i++) {
        const auto infoIndex = static_cast<uint16>(CTheZones::NavigationZoneArray.data()[i].m_ZoneInfoIndex);
        if (!infoIndex)
            continue;

        const auto& zoneInfo = CTheZones::ZoneInfoArray.data()[infoIndex];
        const uint8 ballasDensity = zoneInfo.GangStrength[GANG_BALLAS],
                    groveDensity  = zoneInfo.GangStrength[GANG_GROVE],
                    vagosDensity  = zoneInfo.GangStrength[GANG_VAGOS];
        const int32 enemyDensity = ballasDensity + vagosDensity;
        if (enemyDensity == 0 && groveDensity == 0)
            continue;

        if (enemyDensity <= groveDensity) {
            groveZones++;
        } else if (ballasDensity <= vagosDensity) {
            vagosZones++;
        } else {
            ballasZones++;
        }
    }

    // Como el original (0x443E6E..0x443EB5): SetStatValue(TERRITORIES_HELD) y el "record" en linea (no SetNewRecordStat):
    // si el record actual es MENOR que el nuevo se usa el nuevo; si no se vuelve a leer el record y se escribe igual.
    const float held = static_cast<float>(groveZones);
    CStats::SetStatValue(STAT_TERRITORIES_HELD, held);
    float record;
    if (CStats::GetStatValue(STAT_HIGHEST_NUMBER_OF_TERRITORIES_HELD) < held) {
        record = held;
    } else {
        record = CStats::GetStatValue(STAT_HIGHEST_NUMBER_OF_TERRITORIES_HELD);
    }
    CStats::SetStatValue(STAT_HIGHEST_NUMBER_OF_TERRITORIES_HELD, record);

    const int32 allGangZones = ballasZones + vagosZones + groveZones; // lea eax,[ebx+ebp]; add eax,esi
    if (!allGangZones) {
        TerritoryUnderControlPercentage = 0.0f;
        return;
    }

    TerritoryUnderControlPercentage = held / static_cast<float>(allGangZones);

    // Como el original (0x443EE4..0x443F7C): GangRatings[rango] = pandilla y GangRatingStrength[rango] = zonas, ordenado de mayor a
    // menor con las permutaciones del EXE; empieza en (Grove, Ballas, Vagos) y los empates no se intercambian
    // (a diferencia de GangRatings[pandilla] = rango del std::sort anterior).
    int32 ids[3]       = { GANG_GROVE, GANG_BALLAS, GANG_VAGOS };
    int32 strengths[3] = { groveZones, ballasZones, vagosZones };
    for (;;) {
        bool swapped = false;
        if (strengths[0] < strengths[1]) {
            std::swap(strengths[0], strengths[1]);
            std::swap(ids[0], ids[1]);
            swapped = true;
        }
        if (strengths[1] < strengths[2]) {
            std::swap(strengths[1], strengths[2]);
            std::swap(ids[1], ids[2]);
            continue;
        }
        if (!swapped)
            break;
    }
    for (int32 i = 0; i < 3; i++) {
        GangRatings.data()[i]        = ids[i];
        GangRatingStrength.data()[i] = strengths[i];
    }
}

// todo: Use macro for color conversion
// 0x44582F NOTSA
eBlipColour CGangWars::GetGangColor(int32 gang) {
    // 0x8D1344 0x8D1350 0x8D135C
    static constexpr uint8 gaGangColoursR[] = { 200,  70, 255,   0, 255, 200, 240,   0, 255, 255, 0, 0 };
    static constexpr uint8 gaGangColoursB[] = { 200,   0,   0, 200, 190, 200, 240, 255, 255, 255, 0, 0 };
    static constexpr uint8 gaGangColoursG[] = {   0, 200, 200,   0, 220, 200, 140, 200, 255, 255, 0, 0 };

    auto r = gaGangColoursR[gang], g = gaGangColoursG[gang], b = gaGangColoursB[gang];
    return (eBlipColour)(((b | (((r << 8) | g) << 8)) << 8) | 0xFF);
}
