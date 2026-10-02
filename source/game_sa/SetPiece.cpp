/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSetPiece
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Automobile.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskComplexBeCop.h"

void CSetPiece::InjectHooks() {
    RH_ScopedClass(CSetPiece);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(TryToGenerateCopPed, 0x499690);
    RH_ScopedInstall(TryToGenerateCopCar, 0x4998A0);
    RH_ScopedInstall(Update, 0x499A80);
}

// 0x499690
CCopPed* CSetPiece::TryToGenerateCopPed(CVector2D posn) {
    CVector   point{ posn, 1000.0f };
    CColPoint cp{};
    CEntity*  entity{};
    if (CWorld::ProcessVerticalLine(
        point,
        -1000.0f,
        cp,
        entity,
        true,
        false,
        false,
        false,
        true
    )) {
        point.z = cp.m_vecPoint.z + 0.9f;
    }

    int16 numCollidingObjects{};
    CWorld::FindObjectsKindaColliding(
        point,
        0.6f,
        false,
        &numCollidingObjects,
        16,
        nullptr,
        false,
        true,
        true,
        false,
        false
    );

    // Como el original (0x49971C `jne`): si hay algo en el sitio no se genera el poli.
    if (numCollidingObjects) {
        return nullptr;
    }

    auto* cop = new CCopPed(0);
    cop->SetPosn(point);

    NOTSA_LOG_DEBUG("generated cop at ({}, {}, {}) dist to player {:.2f}", cop->GetPosition().x, cop->GetPosition().y, cop->GetPosition().z, DistanceBetweenPoints(FindPlayerCoors(), cop->GetPosition()));
    CWorld::Add(cop);
    return cop;
}

// 0x4998A0
CVehicle* CSetPiece::TryToGenerateCopCar(CVector2D posn, CVector2D target) {
    const auto carModel = CStreaming::GetDefaultCopCarModel();
    if (carModel < 0) {
        return nullptr;
    }

    auto* car = new CAutomobile(carModel, RANDOM_VEHICLE, true);

    CVector point{ posn, 1000.0f };
    CColPoint cp{};
    CEntity*  entity{};
    if (CWorld::ProcessVerticalLine(
        point,
        -1000.0f,
        cp,
        entity,
        true,
        false,
        false,
        false,
        true
    )) {
        point.z = car->GetHeightAboveRoad() + cp.m_vecPoint.z;
    }

    // Como el original: CVector{dx, dy, 0} normalizado con CVector::Normalise (0x59C910).
    CVector dir{ target.x - posn.x, target.y - posn.y, 0.0f };
    dir.Normalise();
    car->GetForward()  = CVector{ dir.x, dir.y, 0.0f };
    car->GetRight()    = CVector{ dir.y, -dir.x, 0.0f };
    car->GetUp()       = CVector::ZAxisVector();
    car->GetPosition() = point;

    // Como el original: radio de la esfera del modelo de colision del model info (sin modelos especiales).
    int16 numCollidingObjects{};
    CWorld::FindObjectsKindaColliding(
        point,
        CModelInfo::GetModelInfo(car->m_nModelIndex)->GetColModel()->GetBoundRadius(),
        false,
        &numCollidingObjects,
        16,
        nullptr,
        false,
        true,
        true,
        false,
        false
    );

    if (!numCollidingObjects) {
        car->ChangeLawEnforcerState(true);
        NOTSA_LOG_DEBUG("generated cop car at ({}, {}, {}) dist to player {:.2f}", car->GetPosition().x, car->GetPosition().y, car->GetPosition().z, DistanceBetweenPoints(FindPlayerCoors(), car->GetPosition()));
        CWorld::Add(car);
    } else {
        delete std::exchange(car, nullptr);
    }

    return car;
}

// 0x499A80
// Como el original: el nivel de busqueda se compara con signo (`cmp eax,N` + `jl`).
void CSetPiece::Update() {
    // Como el original: 40 s (0x9C40) entre generaciones, comparacion sin signo.
    if (m_nLastGenerationTime != 0 && CTimer::GetTimeInMS() <= (uint32)m_nLastGenerationTime + 40'000u) {
        return;
    }

    // Como el original: comparaciones x87 en el orden x1, x2, y1, y2 (un NaN deja continuar).
    const auto playerPos = FindPlayerCoors(-1);
    if ((float)m_AreaRect.x1 > playerPos.x || (float)m_AreaRect.x2 < playerPos.x
        || (float)m_AreaRect.y1 > playerPos.y || (float)m_AreaRect.y2 < playerPos.y) {
        return;
    }

    // Como el original: (jugador - salida).x * vel.x + (jugador - salida).y * vel.y sin componente z, con las
    // mismas llamadas a FindPlayerCoors/FindPlayerSpeed; solo se genera si es negativo (un NaN no genera).
    const auto IsPlayerApproaching = [](CVector2D spawn) {
        const auto  coorsA = FindPlayerCoors(-1);
        const auto& speedA = FindPlayerSpeed(-1);
        const auto  coorsB = FindPlayerCoors(-1);
        const float dy     = coorsB.y - spawn.y;
        const auto& speedB = FindPlayerSpeed(-1);
        return dy * speedB.y + (coorsA.x - spawn.x) * speedA.x < 0.0f;
    };

    // Como el original: coches lentos (estado fisico, velocidad 4/24, destino con z = 0, 25 s).
    const auto SetupSlowCar = [this](CVehicle* car, CVector2D target) {
        car->SetStatus(STATUS_PHYSICS);
        car->m_autoPilot.m_nCruiseSpeed        = (uint8)(m_nType == SETPIECE_2CARS_MEDIUM_SPEED ? 24 : 4);
        car->m_autoPilot.m_nCarDrivingStyle    = DRIVING_STYLE_SLOW_DOWN_FOR_CARS;
        car->m_autoPilot.m_nCarMission         = MISSION_SLOWLY_DRIVE_TOWARDS_PLAYER_1;
        car->m_autoPilot.m_speed               = (float)car->m_autoPilot.m_nCruiseSpeed;
        car->m_autoPilot.m_vecDestinationCoors = CVector{ target.x, target.y, 0.0f };
        car->m_nTimeTillWeNeedThisCar          = CTimer::GetTimeInMS() + 25'000;
        CCarAI::AddPoliceCarOccupants(car, false);
    };

    // Como el original: coches rapidos (sin m_speed ni destino), accion temporal hacia delante 100 ms,
    // velocidad = adelante * 0.6666667f y nueva marca de generacion.
    const auto SetupQuickCar = [this](CVehicle* car, eCarDrivingStyle style, eCarMission mission) {
        car->SetStatus(STATUS_PHYSICS);
        car->m_autoPilot.m_nCruiseSpeed     = 16;
        car->m_autoPilot.m_nCarDrivingStyle = style;
        car->m_autoPilot.m_nCarMission      = mission;
        car->m_autoPilot.m_nTempAction      = TEMPACT_GOFORWARD;
        car->m_autoPilot.m_nTempActionTime  = CTimer::GetTimeInMS() + 100;
        car->m_nTimeTillWeNeedThisCar       = CTimer::GetTimeInMS() + 10'000;
        car->GetMoveSpeed()                 = car->GetForward() * (2.0f / 3.0f);
        CCarAI::AddPoliceCarOccupants(car, false);
        m_nLastGenerationTime = CTimer::GetTimeInMS();
    };

    // Como el original: la z del suelo se calcula antes de tocar el poli y de crear las tareas.
    const auto SetupCop = [](CCopPed* cop, CVector2D target, float groundZ) {
        cop->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS() + 10'000;
        cop->bCullExtraFarAway        = true;
        auto* const goTo  = new CTaskComplexGoToPointAndStandStill{ PEDMOVE_RUN, CVector{ target.x, target.y, groundZ }, 0.5f, 2.0f, false, false };
        auto* const beCop = new CTaskComplexBeCop{ PEDMOVE_WALK, false, goTo };
        cop->GetTaskManager().SetTask(beCop, TASK_PRIMARY_PRIMARY);
    };

    switch (m_nType) {
    case SETPIECE_2CARS_SLOW_SPEED:
    case SETPIECE_2CARS_MEDIUM_SPEED: {
        if ((int32)FindPlayerPed(-1)->GetWantedLevel() < 1 || FindPlayerVehicle(-1, false)) {
            return;
        }
        auto* const car1 = TryToGenerateCopCar(GetSpawnCoord1(), GetTargetCoord1());
        if (!car1) {
            return;
        }
        auto* const car2 = TryToGenerateCopCar(GetSpawnCoord2(), GetTargetCoord2());
        if (!car2) {
            CWorld::Remove(car1);
            delete car1;
            return;
        }
        // Como el original: los dos coches se configuran despues de generar ambos.
        SetupSlowCar(car1, GetTargetCoord1());
        SetupSlowCar(car2, GetTargetCoord2());
        m_nLastGenerationTime = CTimer::GetTimeInMS();
        return;
    }
    case SETPIECE_1CAR_QUICK_SPEED:
    case SETPIECE_1CAR_MEDIUM_SPEED: {
        if ((int32)FindPlayerPed(-1)->GetWantedLevel() < 2 || !FindPlayerVehicle(-1, false)) {
            return;
        }
        if (!IsPlayerApproaching(GetSpawnCoord1())) {
            return;
        }
        if (auto* const car = TryToGenerateCopCar(GetSpawnCoord1(), GetTargetCoord1())) {
            if (m_nType == SETPIECE_1CAR_QUICK_SPEED) {
                SetupQuickCar(car, DRIVING_STYLE_PLOUGH_THROUGH, MISSION_BLOCKPLAYER_FORWARDANDBACK);
            } else {
                SetupQuickCar(car, DRIVING_STYLE_AVOID_CARS, MISSION_RAMPLAYER_CLOSE);
            }
        }
        return;
    }
    case SETPIECE_1PED: {
        // Como el original: sale si no hay nivel de busqueda o si el jugador va en vehiculo.
        if ((int32)FindPlayerPed(-1)->GetWantedLevel() < 1 || FindPlayerVehicle(-1, false)) {
            return;
        }
        auto* const cop = TryToGenerateCopPed(GetSpawnCoord1());
        if (!cop) {
            return;
        }
        const auto target = GetTargetCoord1();
        SetupCop(cop, target, CWorld::FindGroundZForCoord(target.x, target.y));
        m_nLastGenerationTime = CTimer::GetTimeInMS();
        return;
    }
    case SETPIECE_2PEDS: {
        if ((int32)FindPlayerPed(-1)->GetWantedLevel() < 1 || FindPlayerVehicle(-1, false)) {
            return;
        }
        auto* const cop1 = TryToGenerateCopPed(GetSpawnCoord1());
        if (!cop1) {
            return;
        }
        const auto target1 = GetTargetCoord1();
        SetupCop(cop1, target1, CWorld::FindGroundZForCoord(target1.x, target1.y));
        auto* const cop2 = TryToGenerateCopPed(GetSpawnCoord2());
        if (!cop2) {
            CWorld::Remove(cop1);
            delete cop1;
            return;
        }
        // Como el original: el segundo poli va a la x/y del destino 1 con la z del suelo del destino 2.
        const auto  target2  = GetTargetCoord2();
        const float groundZ2 = CWorld::FindGroundZForCoord(target2.x, target2.y);
        SetupCop(cop2, GetTargetCoord1(), groundZ2);
        m_nLastGenerationTime = CTimer::GetTimeInMS();
        return;
    }
    case SETPIECE_2CARS_QUICK_SPEED_BEFORE_HIT:
    case SETPIECE_2CARS_QUICK_SPEED: {
        // Como el original: sale si el nivel de busqueda es menor que 2 o si el jugador no va en vehiculo.
        if ((int32)FindPlayerPed(-1)->GetWantedLevel() < 2 || !FindPlayerVehicle(-1, false)) {
            return;
        }
        const auto style   = m_nType == SETPIECE_2CARS_QUICK_SPEED ? DRIVING_STYLE_AVOID_CARS : DRIVING_STYLE_PLOUGH_THROUGH;
        const auto mission = m_nType == SETPIECE_2CARS_QUICK_SPEED ? MISSION_RAMPLAYER_CLOSE : MISSION_BLOCKPLAYER_FORWARDANDBACK;
        // NOTE: No check of the first car
        if (IsPlayerApproaching(GetSpawnCoord1())) {
            if (auto* const car = TryToGenerateCopCar(GetSpawnCoord1(), GetTargetCoord1())) {
                SetupQuickCar(car, style, mission);
            }
        }
        if (!IsPlayerApproaching(GetSpawnCoord2())) {
            return;
        }
        if (auto* const car = TryToGenerateCopCar(GetSpawnCoord2(), GetTargetCoord2())) {
            SetupQuickCar(car, style, mission);
        }
        return;
    }
    default:
        return;
    }
}
