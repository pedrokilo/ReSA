/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRoadBlocks
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "RoadBlocks.h"
#include "PedPlacement.h"
#include "TaskComplexWanderCop.h"
#include "TaskSimpleStandStill.h"
#include <extensions/File.hpp>

void CRoadBlocks::InjectHooks() {
    RH_ScopedClass(CRoadBlocks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x461100);
    RH_ScopedInstall(ClearScriptRoadBlocks, 0x460EC0);
    RH_ScopedInstall(ClearSpaceForRoadBlockObject, 0x461020);
    RH_ScopedInstall(CreateRoadBlockBetween2Points, 0x4619C0);
    RH_ScopedInstall(GenerateRoadBlockPedsForCar, 0x461170);
    RH_ScopedInstall(GenerateRoadBlocks, 0x4629E0);
    RH_ScopedInstall(GetRoadBlockNodeInfo, 0x460EE0);
    RH_ScopedInstall(RegisterScriptRoadBlock, 0x460DF0);
}

// 0x461100
void CRoadBlocks::Init() {
    rng::fill(InOrOut, true);
    GenerateDynamicRoadBlocks = false;

    if (notsa::File rbx("data\\paths\\roadblox.dat", "rb"); rbx) {
        rbx.Read(&NumRoadBlocks, sizeof(int32));
        assert(NumRoadBlocks <= MAX_ROADBLOCKS);
        rbx.Read(RoadBlockNodes.data(), RoadBlockNodes.size() * sizeof(CNodeAddress));
    } else {
        NOTSA_UNREACHABLE("roadblox.dat couldn't be opened!");
    }
    ClearScriptRoadBlocks();
}

// 0x460EC0
void CRoadBlocks::ClearScriptRoadBlocks() {
    for (auto& srb : aScriptRoadBlocks) {
        srb.IsActive = false;
    }
}

// 0x461020
// Returns true if cleared successfully.
bool CRoadBlocks::ClearSpaceForRoadBlockObject(CVector cornerA, CVector cornerB){
    int16 numEntities{};
    CEntity* entities[2]{};
    CWorld::FindObjectsIntersectingCube(
        cornerA,
        cornerB,
        &numEntities,
        std::size(entities),
        entities,
        false,
        true,
        true,
        true,
        false
    );

    if (numEntities > std::size(entities) || numEntities <= 0) {
        return numEntities <= 0;
    }

    const auto Remove = [](CEntity* e) {
        CWorld::Remove(e);
        delete e;
    };

    for (auto* entity : entities | rngv::take(numEntities)) {
        switch (entity->GetType()) {
        case ENTITY_TYPE_VEHICLE:
            if (auto* v = entity->AsVehicle(); !v->CanBeDeleted()) {
                return false;
            } else if (!v->vehicleFlags.bCreateRoadBlockPeds) {
                Remove(v);
            }
            break;
        case ENTITY_TYPE_PED:
            if (auto* p = entity->AsPed(); p->CanBeDeleted()) {
                Remove(p);
            } else {
                return false;
            }
            break;
        case ENTITY_TYPE_OBJECT:
            // Como el original (0x4610B7): primero m_nObjectType (cmp [esi+13Ch],1), luego CanBeDeleted (0x59F120).
            if (auto* o = entity->AsObject(); o->m_nObjectType != OBJECT_GAME && o->CanBeDeleted()) {
                Remove(o);
            } else {
                return false;
            }
            break;
        default:
            return false; // Como el original (0x4610EC): cualquier otro tipo devuelve false
        }
    }

    return true;
}

// 0x4619C0
void CRoadBlocks::CreateRoadBlockBetween2Points(CVector a, CVector b, bool isGangRoadBlock) {
    // Como el original: reparto de hasta cinco coches y ocho barreras.
    CMatrix matrix;
    CVector direction = b - a;
    const float length = std::sqrt((direction.x*direction.x + direction.y*direction.y) + direction.z*direction.z);
    const CVector center = (a + b) * 0.5f;
    direction.Normalise();
    CVector facing{direction.y, -direction.x, 0.f};
    facing.Normalise();
    const auto playerDelta = FindPlayerCoors() - center;
    if ((playerDelta.z*facing.z + playerDelta.x*facing.x) + playerDelta.y*facing.y < 0.f) facing *= -1.f;
    int32 model;
    if (isGangRoadBlock) {
        model = CPopulation::PickRiotRoadBlockCar();
        if (model == -1) return;
    } else {
        if (FindPlayerWanted()->AreArmyRequired()) model = MODEL_BARRACKS;
        else if (FindPlayerWanted()->AreFbiRequired()) model = MODEL_FBIRANCH;
        else if (FindPlayerWanted()->AreSwatRequired()) model = MODEL_ENFORCER;
        else model = CStreaming::GetDefaultCopCarModel(false);
        if (!CStreaming::IsModelLoaded(model)) model = CStreaming::GetDefaultCopCarModel(false);
        if (model == MODEL_COPBIKE) return;
    }
    auto box = CModelInfo::GetModelInfo(model)->GetColModel()->GetBoundingBox();
    const float breadth = (box.m_vecMax.x - box.m_vecMin.x) + 2.f;
    float depth = (box.m_vecMax.y - box.m_vecMin.y) + 0.2f;
    if (isGangRoadBlock) depth += 0.5f;
    std::array<uint8,5> sideways;
    float remaining = length;
    int32 count = 0;
    while (count < 5) {
        sideways[count] = model == MODEL_BARRACKS || isGangRoadBlock ? 0 : CGeneral::GetRandomNumber() & 1;
        const float span = sideways[count] ? breadth : depth;
        if (!(remaining >= span)) break;
        remaining -= span;
        ++count;
    }
    float distance = remaining / float(count + 1);
    const float advance = depth*0.5f + distance;
    // Multiply3x3_VM suma y+z+x en la derecha y y+x+z en los otros ejes.
    const auto transform = [&](CVector v) {
        const auto& r = matrix.GetRight(); const auto& f = matrix.GetForward(); const auto& u = matrix.GetUp();
        return CVector{(r.y*v.y+r.z*v.z)+r.x*v.x, (f.y*v.y+f.x*v.x)+f.z*v.z, (u.y*v.y+u.x*v.x)+u.z*v.z} + matrix.GetPosition();
    };
    for (int32 i = 0; i < count; ++i) {
        bool onSide = false;
        float jitter;
        if (sideways[i]) {
            distance += breadth*0.5f;
            matrix.GetForward() = facing;
            matrix.GetRight() = CVector{facing.y,-facing.x,0.f};
            matrix.GetUp() = CVector{0.f,0.f,1.f};
            jitter = (float(CGeneral::GetRandomNumber() & 255)-128.f)*(isGangRoadBlock ? 0.004f : 0.002f);
        } else {
            distance += depth*0.5f;
            onSide = isGangRoadBlock && (CGeneral::GetRandomNumber() & 255) < 64;
            matrix.GetForward() = direction;
            matrix.GetRight() = onSide ? CVector{0.f,0.f,1.f} : CVector{direction.y,-direction.x,0.f};
            matrix.GetUp() = onSide ? CVector{-direction.y,direction.x,0.f} : CVector{0.f,0.f,1.f};
            if (CGeneral::GetRandomNumber() & 1) {
                if (onSide) matrix.RotateY(std::bit_cast<float>(0x40490FDBu));
                else matrix.RotateZ(std::bit_cast<float>(0x40490FDBu));
            }
            jitter = (float(CGeneral::GetRandomNumber() & 255)-128.f)*(isGangRoadBlock ? 0.006f : 0.003f);
        }
        matrix.RotateZ(jitter);
        matrix.SetTranslateOnly(a + direction*distance);
        matrix.GetPosition().z = (matrix.GetPosition().z - (onSide ? box.m_vecMin.x : box.m_vecMin.z)) + 0.3f;
        distance += advance;
        const auto low = transform(box.m_vecMin), high = transform(box.m_vecMax);
        if (!ClearSpaceForRoadBlockObject(low,high)) continue;
        if (isGangRoadBlock && (onSide || (CGeneral::GetRandomNumber() & 255) < 64)) {
            auto* object = new CObject(ModelIndices::MI_ROADBLOCKFUCKEDCAR1,true);
            object->GetMatrix() = matrix;
            object->SetPosn(matrix.GetPosition());
            object->SetIsStatic(false);
            ++CObject::nNoTempObjects;
            object->m_nObjectType = OBJECT_TEMPORARY;
            object->m_nRemovalTime = CTimer::GetTimeInMS() + 600'000;
            CWorld::Add(object);
            gFireManager.StartFire(object,nullptr,2.8f,1,60'000,2);
            if (object->m_pFire) reinterpret_cast<uint8*>(object->m_pFire)[0x21] = 0x5C;
            continue;
        }
        auto* car = new CAutomobile(model,RANDOM_VEHICLE,true);
        CVehicle* vehicle = car; // Como el original: llamadas por la tabla virtual.
        vehicle->SetStatus(STATUS_ABANDONED);
        matrix.GetPosition().z = (vehicle->GetHeightAboveRoad() - 0.6f) + matrix.GetPosition().z;
        car->GetMatrix() = matrix;
        car->PlaceOnRoadProperly();
        vehicle->SetIsStatic(false);
        car->UpdateRwMatrix();
        car->m_nDoorLock = CARLOCK_UNLOCKED;
        CCarCtrl::SwitchVehicleToRealPhysics(car);
        // CAutoPilot: escrituras en línea con las anchuras originales.
        auto* raw = reinterpret_cast<uint8*>(car);
        raw[0x3BA] = raw[0x3BB] = raw[0x3B7] = raw[0x3B8] = raw[0x3D0] = 0;
        *reinterpret_cast<uint32*>(raw+0x3C8) = 0;
        raw[0x42C] |= 0x40;
        raw[0x428] &= 0xE7;
        bool burning = false;
        if (isGangRoadBlock) {
            const auto damage = int32((float(CGeneral::GetRandomNumber())*std::bit_cast<float>(0x38000000u))*4.f);
            if (!damage) car->SetTotalDamage(true);
            else {
                vehicle->BlowUpCarCutSceneNoExtras(true,true,true,true);
                car->m_nTimeWhenBlowedUp += 1'000'000;
                burning = true;
            }
        } else if (car->IsLawEnforcementVehicle() && (CGeneral::GetRandomNumber() & 1)) {
            raw[0x42D] |= 0x80;
        }
        if (!(car->GetMatrix().GetUp().z > 0.94f)) { delete vehicle; continue; }
        CVisibilityPlugins::SetClumpAlpha(car->GetRpClump(),0);
        CWorld::Add(car);
        raw[0x42A] |= 0x10;
        car->m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 7'000;
        car->m_nNumPedsForRoadBlock = count < 4 ? 2 : 1;
        if (sideways[i]) {
            car->m_nPedsPositionForRoadBlock = 2;
            const auto open = [&](eDoors door) {
                const auto node = CDamageManager::GetCarNodeIndexFromDoor(door);
                if (car->m_aCarNodes[node]) vehicle->OpenDoor(nullptr,node,door,1.f,true);
            };
            open(DOOR_LEFT_FRONT);
            if (car->m_nNumPedsForRoadBlock > 1) open(DOOR_RIGHT_FRONT);
        } else {
            const auto delta = car->GetPosition() - FindPlayerCoors();
            const auto& right = car->GetMatrix().GetRight();
            car->m_nPedsPositionForRoadBlock = ((delta.y*right.y+delta.z*right.z)+delta.x*right.x) < 0.f ? 0 : 1;
        }
        if (burning) {
            gFireManager.StartFire(car,nullptr,2.8f,1,60'000,2);
            if (car->m_pFire) reinterpret_cast<uint8*>(car->m_pFire)[0x21] = 0x5C;
        }
    }
    if (isGangRoadBlock) return;
    const int32 barrierModel = uint16(ModelIndices::MI_ROADWORKBARRIER1);
    box = CModelInfo::GetModelInfo(barrierModel)->GetColModel()->GetBoundingBox();
    const float barrierWidth = (box.m_vecMax.x - box.m_vecMin.x) + 0.5f;
    const int32 barriers = std::min(int32(length/barrierWidth),8);
    const float gap = (length - float(barriers)*barrierWidth)/float(barriers+1);
    CObject::DeleteAllTempObjectsInArea(center,length*0.5f);
    for (int32 i = 0; i < barriers; ++i) {
        const float along = float(i+1)*gap + (float(i)+0.5f)*barrierWidth;
        matrix.SetUnity();
        matrix.SetTranslate(CVector{0.f,0.f,0.f});
        matrix.GetRight() = direction;
        matrix.GetForward() = CVector{direction.y,-direction.x,0.f};
        matrix.GetUp() = CVector{0.f,0.f,1.f};
        matrix.RotateZ((float(CGeneral::GetRandomNumber() & 255)-128.f)*0.003f);
        matrix.SetTranslateOnly((a + direction*along) + facing*5.f);
        matrix.GetPosition().x += float(CGeneral::GetRandomNumber() & 15)*0.05f;
        matrix.GetPosition().y += float(CGeneral::GetRandomNumber() & 15)*0.05f;
        bool found;
        matrix.GetPosition().z = CWorld::FindGroundZFor3DCoord(matrix.GetPosition() + CVector{0.f,0.f,2.f},&found,nullptr);
        if (!found) continue;
        matrix.GetPosition().z -= box.m_vecMin.z;
        if (!ClearSpaceForRoadBlockObject(transform(box.m_vecMin),transform(box.m_vecMax))) continue;
        auto* object = new CObject(barrierModel,true);
        object->GetMatrix() = matrix;
        object->SetPosn(matrix.GetPosition());
        ++CObject::nNoTempObjects;
        object->m_nObjectType = OBJECT_TEMPORARY;
        object->m_nRemovalTime = CTimer::GetTimeInMS() + 600'000;
        CWorld::Add(object);
    }
}

// 0x461170
void CRoadBlocks::GenerateRoadBlockPedsForCar(CVehicle* vehicle, int32 pedsPositionsType, ePedType pedType) {
    // Como el original (0x461190): el modelo de ped inicial es 0, no -1 (el EXE solo lo cambia en las ramas de SWAT/ejercito/FBI/pandillas).
    const auto Generate = [&](eModelID pedModel = MODEL_PLAYER, eCopType copType = COP_TYPE_CITYCOP, bool isSpecialCop = false) {
        static constexpr auto PLACEMENTS = std::to_array<CVector>({
            { -1.5f, +1.9f, 0.0f },
            { -1.5f, -2.6f, 0.0f },
            { +1.5f, +1.9f, 0.0f },
            { +1.5f, -2.6f, 0.0f },
            { -1.5f,  0.0f, 0.0f },
            { +1.5f,  0.0f, 0.0f },
        });

        static constexpr auto SPECIAL_COP_PLACEMENTS = std::to_array<CVector>({
            {  0.0f, +3.2f, 0.0f },
            { +1.5f, -1.8f, 0.0f },
            {  0.0f, +3.2f, 0.0f },
            { -1.5f, -1.8f, 0.0f },
            { -1.5f,  0.0f, 0.0f },
            { +1.5f,  0.0f, 0.0f },
        });

        const auto placementIdx = 2 * pedsPositionsType;
        // Como el original (0x4613E9..0x419): primero GetColModel del vehiculo (0x535300) y luego GetDefaultCopCarModel (0x407C50);
        // C++ no fija el orden de evaluacion de los dos operandos de la division.
        const float vehicleRadius = vehicle->GetColModel()->GetBoundingSphere().m_fRadius;
        const float copCarRadius  = CModelInfo::GetModelInfo(CStreaming::GetDefaultCopCarModel(false))->GetColModel()->GetBoundingSphere().m_fRadius;
        const auto  radiusRatio   = vehicleRadius / copCarRadius;

        for (auto i = 0u; i < vehicle->m_nNumPedsForRoadBlock; i++) {
            const auto offset = (isSpecialCop ? SPECIAL_COP_PLACEMENTS : PLACEMENTS)[placementIdx + i] * radiusRatio;
            // Como el original (0x59C890): suma z, luego y, luego x y al final la posicion; TransformPoint de la DLL suma x, y, z.
            const auto& vm = vehicle->GetMatrix();
            const CVector pos{
                ((vm.GetUp().x * offset.z + vm.GetForward().x * offset.y) + vm.GetRight().x * offset.x) + vm.GetPosition().x,
                ((vm.GetUp().y * offset.z + vm.GetRight().y * offset.x) + vm.GetForward().y * offset.y) + vm.GetPosition().y,
                ((vm.GetUp().z * offset.z + vm.GetRight().z * offset.x) + vm.GetForward().z * offset.y) + vm.GetPosition().z,
            };

            auto* ped = [&]() -> CPed* {
                if (pedType != PED_TYPE_COP) { // 0x461560
                    // Como el original (0x461580..0x461596): el camino de civiles/pandillas cae en 0x461592 y tambien llama a
                    // CPed::SetCurrentWeapon(WEAPON_PISTOL = 0x16) (0x5E6280) antes de seguir.
                    auto* p = new CCivilianPed(pedType, pedModel);
                    p->SetCurrentWeapon(WEAPON_PISTOL);
                    return p;
                } else {
                    // Como el original (0x4614E1..0x461596): solo mira si el modelo esta cargado cuando copType != CITYCOP, y la pistola
                    // se da siempre que el tipo realmente creado sea CITYCOP (tambien cuando se cae a CITYCOP por modelo no cargado).
                    const auto actualType = (copType != COP_TYPE_CITYCOP && CStreaming::IsModelLoaded(pedModel)) ? copType : COP_TYPE_CITYCOP;
                    auto* p = new CCopPed(actualType);
                    if (actualType == COP_TYPE_CITYCOP) {
                        p->SetCurrentWeapon(WEAPON_PISTOL);
                    }
                    return p;
                }
            }();

            ped->SetPosn(std::get<CVector>(CPedPlacement::FindZCoorForPed(pos)));
            // Como el original (0x4615D5..0x461627): -pi/2 exacto (0xBFC90FDB); SetRotate (0x59B120) y luego suma la posicion
            // guardada; sin matriz solo escribe el heading en m_placement (la DLL no debe reservar una matriz).
            constexpr float NEG_HALF_PI = std::bit_cast<float>(0xBFC90FDBu);
            if (ped->m_matrix) {
                auto& m = ped->GetMatrix();
                const CVector saved = m.GetPosition();
                m.SetRotate(0.0f, 0.0f, NEG_HALF_PI);
                m.GetPosition().x += saved.x;
                m.GetPosition().y += saved.y;
                m.GetPosition().z += saved.z;
            } else {
                ped->m_placement.m_fHeading = NEG_HALF_PI;
            }

            if (pedType == PED_TYPE_COP) {
                // Como el original (0x461637): mov byte ptr [esi+79Ch],1 (CCopPed::m_bDontPursuit) antes de reservar la tarea.
                static_cast<CCopPed*>(ped)->m_bDontPursuit = true;
                // Como el original (0x46168D..0x4616B4): el EXE solo pone a cero los campos de la tarea que el constructor de la DLL ya
                // inicializa (NSDMI); las asignaciones `= {}` de los temporizadores copiaban ademas el relleno, que el EXE no escribe.
                auto* t = new CTaskComplexWanderCop(PEDMOVE_STILL, uint8(CGeneral::GetRandomIntInRange(0, 8)));
                ped->GetTaskManager().SetTask(t, TASK_PRIMARY_PRIMARY);
            }
            ped->GetTaskManager().SetTask(new CTaskSimpleStandStill(0, true), TASK_PRIMARY_DEFAULT);

            ped->bStayInSamePlace         = true;
            ped->bNotAllowedToDuck        = true;
            ped->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS() + 10'000;
            ped->bCrouchWhenShooting      = !isSpecialCop || pedsPositionsType != 2;
            ped->bCullExtraFarAway        = true;
            CEntity::RegisterReference(ped->m_pVehicle = vehicle);
            CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);

            if (pedType != PED_TYPE_COP) {
                // Como el original (0x4617A6..0x4617F8): un solo rand()*(1/32768)*100 truncado; el reparto depende de si hay 2 o 3 armas.
                const auto& gang = CGangs::Gang.data()[pedType - PED_TYPE_GANG1]; // Como el original: sin comprobar el rango (lee [ecx*16+0xC091F4])
                const auto roll = CGeneral::GetRandomIntInRange(0, 100);
                auto weapon = gang.m_nGangWeapons[2];
                if (weapon != WEAPON_UNARMED) {
                    if (roll < 33) weapon = gang.m_nGangWeapons[0];
                    else if (roll < 66) weapon = gang.m_nGangWeapons[1];
                } else {
                    weapon = gang.m_nGangWeapons[1];
                    if (weapon == WEAPON_UNARMED || roll < 50) weapon = gang.m_nGangWeapons[0];
                }
                if (weapon != WEAPON_UNARMED) {
                    ped->GiveDelayedWeapon(weapon, 25'001);
                    // Como el original (0x46180B..0x46181A): GetWeaponInfo(arma, 1)->m_nSlot (+0x14) y SetCurrentWeapon(ranura) (0x5E61F0).
                    ped->SetCurrentWeapon(int32(CWeaponInfo::GetWeaponInfo(weapon, eWeaponSkill::STD)->m_nSlot));
                }
            }
            CWorld::Add(ped);
            ped->GetEventGroup().Add<CEventScriptCommand>({ TASK_PRIMARY_PRIMARY, new CTaskComplexKillPedOnFoot(FindPlayerPed()) });
        }
    };

    if (pedType == PED_TYPE_COP) {
        switch (vehicle->GetModelId()) {
        case MODEL_ENFORCER: Generate(MODEL_SWAT,    COP_TYPE_SWAT1,   true); break;
        case MODEL_BARRACKS: Generate(MODEL_ARMY,    COP_TYPE_ARMY,    true); break;
        case MODEL_FBIRANCH: Generate(MODEL_FBI,     COP_TYPE_FBI,     true); break;
        case MODEL_COPCARRU: Generate(MODEL_INVALID, COP_TYPE_CITYCOP, true); break;
        default:             Generate(MODEL_INVALID, COP_TYPE_CITYCOP, false); break;
        }
    } else if (IsPedTypeGang(pedType)) {
        for (auto i = 0; i < TOTAL_GANGS; i++) {
            if (!CPopCycle::m_pCurrZoneInfo->GangStrength[i]) {
                continue;
            }
            const auto pedModel = CGangs::ChooseGangPedModel((eGangID)i);
            if (pedModel == MODEL_INVALID) {
                continue;
            }
            Generate(pedModel);
            return;
        }
    } else {
        Generate();
    }
}

// 0x4629E0
void CRoadBlocks::GenerateRoadBlocks() {
    ZoneScoped;

    if (FindPlayerWanted()->m_ChanceOnRoadBlock && FindPlayerVehicle()) {
        if (!GenerateDynamicRoadBlocks) {
            rng::fill(InOrOut, true);
            GenerateDynamicRoadBlocks = true;
        }

        // Como el original (0x462A6E..0x462AB7): primero = (325*(frame&15))/16 y ultimo = (325*(frame&15)+325)/16 (sar con signo),
        // el limite es min(NumRoadBlocks, ultimo) con comparacion con signo y se reevalua en cada vuelta.
        const int32 firstBase = int32(MAX_ROADBLOCKS) * int32(CTimer::GetFrameCounter() & 0xF);
        const int32 firstIdx  = firstBase >> 4;
        const int32 lastIdx   = (firstBase + int32(MAX_ROADBLOCKS)) >> 4;
        const int32 rbsToGenerate = std::min<int32>(NumRoadBlocks, lastIdx);

        for (int32 counter2 = firstIdx; counter2 < rbsToGenerate; counter2++) {
            const auto& mrbNode = RoadBlockNodes[counter2];
            if (!ThePaths.IsAreaLoaded(mrbNode.m_wAreaId)) { // Como el original (0x462AC4): area = palabra baja; pasar el CNodeAddress lo convertia a bool (area 0 o 1)
                continue;
            }
            const auto& mainNode = ThePaths.GetPathNode(mrbNode);
            const auto  playerPos = FindPlayerCoors();
            if (std::abs(playerPos.x - mainNode->GetPosition().x) >= 90.0f ||
                std::abs(playerPos.y - mainNode->GetPosition().y) >= 90.0f ||
                DistanceBetweenPoints2D(playerPos, mainNode->GetPosition()) >= 90.0f)
            {
                InOrOut[counter2] = false;
                continue;
            }

            if (InOrOut[counter2]) {
                continue;
            }
            InOrOut[counter2] = true;

            // Como el original (0x462BE4): rand() & 0x7F (no la plantilla de rango) y comparacion con signo contra la palabra +0x1C
            if (int32(CGeneral::GetRandomNumber() & 0x7F) >= int32(FindPlayerWanted()->m_ChanceOnRoadBlock)) {
                continue;
            }

            float mrbWidth{};
            CVector mrbDir{};
            if (!GetRoadBlockNodeInfo(mrbNode, mrbWidth, mrbDir)) {
                continue;
            }

            if (mainNode->m_nPathWidth) {
                const auto width = mainNode->m_nPathWidth / 16.0f;
                CreateRoadBlockBetween2Points(
                    mainNode->GetPosition() + mrbDir * (mrbWidth / 2.f + width),
                    mainNode->GetPosition() + mrbDir * width,
                    false
                );
                CreateRoadBlockBetween2Points(
                    mainNode->GetPosition() - mrbDir * width,
                    mainNode->GetPosition() - mrbDir * (mrbWidth / 2.f + width),
                    false
                );
                continue;
            }

            // Como el original (0x462EBC): solo hasta NumRoadBlocks (la DLL recorria las 325 entradas del array)
            bool createdWithOther = false;
            for (int32 i = 0; i < NumRoadBlocks; i++) {
                const auto nodeAddr = RoadBlockNodes[i];
                if (counter2 == i || InOrOut[i] || !ThePaths.IsAreaLoaded(nodeAddr.m_wAreaId)) {
                    continue;
                }
                const auto& node = ThePaths.GetPathNode(nodeAddr);

                if (std::abs(mainNode->GetPosition().x - node->GetPosition().x) >= 30.0f ||
                    std::abs(mainNode->GetPosition().y - node->GetPosition().y) >= 30.0f)
                {
                    continue;
                }

                float   width{};
                CVector dir{}; 
                if (!GetRoadBlockNodeInfo(nodeAddr, width, dir)) {
                    continue;
                }

                // Como el original (0x462FD0..0x462FF9): fabs(producto escalar) <= 0.7 (orden x, z, y; z es 0)
                const float dirDot = (dir.x * mrbDir.x + dir.z * mrbDir.z) + dir.y * mrbDir.y;
                if (mrbWidth != width || !(std::fabs(dirDot) > 0.7f)) { // incluye NaN, como fcomp + test ah,41h
                    continue;
                }

                [[maybe_unused]] CColPoint col{};
                [[maybe_unused]] CEntity*  colEntity{};
                if (CWorld::ProcessLineOfSight(
                    mainNode->GetPosition() + CVector{0.0f, 0.0f, 1.0f},
                    node->GetPosition() + CVector{0.0f, 0.0f, 1.0f},
                    col,
                    colEntity,
                    true,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false,
                    false
                )) {
                    continue;
                }

                const auto dirFromMain = (node->GetPosition() - mainNode->GetPosition()).Normalized();
                CreateRoadBlockBetween2Points(
                    node->GetPosition()     + dirFromMain * (mrbWidth / 2.0f),
                    mainNode->GetPosition() - dirFromMain * (mrbWidth / 2.0f),
                    false
                );
                InOrOut[i] = true;
                // Como el original (0x4631EC -> 0x4631F7): tras crear una barricada se pasa al siguiente nodo principal
                createdWithOther = true;
                break;
            }

            // Como el original (0x4631F7..0x4632AE): si el bucle termino sin encontrar pareja (indice == NumRoadBlocks) se crea la
            // barricada del propio nodo con la direccion mrbDir (el hueco de pila de la direccion solo se pisa al encontrar pareja).
            if (!createdWithOther) {
                CreateRoadBlockBetween2Points(
                    mainNode->GetPosition() + mrbDir * (mrbWidth * 0.5f),
                    mainNode->GetPosition() - mrbDir * (mrbWidth * 0.5f),
                    false
                );
            }
        }
    } else {
        GenerateDynamicRoadBlocks = false;
    }

    if (auto& srb = aScriptRoadBlocks[CTimer::GetFrameCounter() % MAX_SCRIPT_ROADBLOCKS]; srb.IsActive) {
        const auto c = CVector::Centroid({ srb.CornerA, srb.CornerB });

        if (DistanceBetweenPoints(FindPlayerCoors(), c) >= 90.0f) {
            srb.IsSafeToCreate = true;
        } else if (srb.IsSafeToCreate) {
            CreateRoadBlockBetween2Points(srb.CornerA, srb.CornerB, srb.IsGangRoadBlock);
            srb.IsSafeToCreate = false;
        }
    }
}

// 0x460EE0
bool CRoadBlocks::GetRoadBlockNodeInfo(CNodeAddress nodeAddress, float& outWidth, CVector& outDir) {
    auto* const node = ThePaths.GetPathNode(nodeAddress);
    assert(node);

    assert(node->m_nNumLinks >= 2);
    const auto naviLinkAddrA = ThePaths.GetNaviLink(nodeAddress.m_wAreaId, node->m_wBaseLinkId + 0),
               naviLinkAddrB = ThePaths.GetNaviLink(nodeAddress.m_wAreaId, node->m_wBaseLinkId + 1);
    if (!ThePaths.IsAreaLoaded(naviLinkAddrA.m_wAreaId) || !ThePaths.IsAreaLoaded(naviLinkAddrB.m_wAreaId)) {
        return false;
    }

    const auto &naviLinkA = ThePaths.GetCarPathLink(naviLinkAddrA),
               &naviLinkB = ThePaths.GetCarPathLink(naviLinkAddrB);

    const auto maxNumLanes = std::max(
        naviLinkA.m_numOppositeDirLanes + naviLinkA.m_numSameDirLanes,
        naviLinkB.m_numOppositeDirLanes + naviLinkB.m_numSameDirLanes
    );

    outWidth = ((float)maxNumLanes + 1.f) * 5.f;
    outDir   = CVector{ (naviLinkB.GetNodeCoors() - naviLinkA.GetNodeCoors()).GetPerpRight(), 0.f }.Normalized();

    return true;
}

// 0x460DF0
void CRoadBlocks::RegisterScriptRoadBlock(CVector cornerA, CVector cornerB, bool isGangRoadBlock) {
    auto free = rng::find_if(aScriptRoadBlocks, [](const auto& srb) { return !srb.IsActive; });
    if (free == aScriptRoadBlocks.end()) {
        // No free script roadblock found
        return;
    }

    free->CornerA         = cornerA;
    free->CornerB         = cornerB;
    free->IsActive        = true;
    free->IsSafeToCreate  = true;
    free->IsGangRoadBlock = isGangRoadBlock;
}
