#include "StdInc.h"

#include <bit>
#include <cmath>

#include "RunningScript.h"
#include "TheScripts.h"
#include "UserDisplay.h"
#include "OnscreenTimer.h"
#include "Pools/Pools.h"
#include "Tasks/TaskSequences.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskManager.h"
#include "World.h"
#include "Stats.h"
#include "Timer.h"
#include "CarEnterExit.h"
#include "VisibilityPlugins.h"
#include "X87Math.h"
#include "Crt/CrtState.h"
#include "Ragdoll/IKChainManager.h"
#include "PedGeometryAnalyser.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Object/Object.h"
#include "Tasks/TaskTypes/TaskSimplePause.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskComplexFallAndGetUp.h"
#include "Tasks/TaskTypes/TaskComplexJump.h"
#include "Tasks/TaskTypes/TaskSimpleTired.h"
#include "Tasks/TaskTypes/TaskComplexDie.h"
#include "Tasks/TaskTypes/TaskSimpleTriggerLookAt.h"
#include "Tasks/TaskTypes/TaskSimpleSay.h"
#include "Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h"
#include "Tasks/TaskTypes/TaskSimpleShakeFist.h"
#include "Tasks/TaskTypes/TaskSimpleCower.h"
#include "Tasks/TaskTypes/TaskSimpleHandsUp.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleUseAtm.h"
#include "Tasks/TaskTypes/TaskSimpleScratchHead.h"
#include "Tasks/TaskTypes/TaskSimpleLookAbout.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassengerTimed.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsDriverTimed.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndFlee.h"
#include "Tasks/TaskTypes/TaskComplexDriveToPoint.h"
#include "Tasks/TaskTypes/TaskComplexDriveWander.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h"
#include "Tasks/TaskTypes/TaskComplexGoToPointAndStandStillTimed.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Tasks/TaskTypes/TaskComplexFollowPointRoute.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h"
#include "Tasks/TaskTypes/TaskComplexFleePoint.h"
#include "Tasks/TaskTypes/TaskComplexFleeEntity.h"

// Lote SCRB (sin validar): 0x48EAA0 (8976 bytes), CRunningScript::ProcessCommands1400To1499 (thiscall, un argumento de pila, `ret 4`; resultado en AL), despachador original de los comandos de script
// 1400..1499 (ranura 14 de la tabla 0x8A6168; lo usa ProcessOneCommand cuando el comando no tiene manejador propio). Salta con una tabla DIRECTA (0x490C2C) indexada por comando - 1403 (97 entradas:
// 1403..1499; 1400..1402 y todo lo mayor que 1499 van a 0x490C0F = OR_INTERRUPT, -1). Los comandos nop (1403 1405 1407 1413 1421 1422 1425 1426 1430 1435 1457-1459 1461-1464 1478 1484 1486 1488 1493 ...)
// devuelven 0 en 0x490C0B sin leer parametros. Solo llevan caso los comandos que el EXE resuelve aqui y que NO tienen manejador propio en el C++ (docs/ComandosScript.csv, estado original): ProcessOneCommand
// solo llega a este despachador con esos comandos; los demas (reescrito, nop, no_implementado) devuelven OR_INTERRUPT por el `default`. Cada caso sigue el desensamblado (misma secuencia de
// CollectParameters / lecturas de ScriptParams / colaboradores) y mantiene las operaciones x87 en su orden. Los casos con tarea acaban como los de 1500..1599: GivePedScriptedTask(ped, tarea, comando)
// (0x465C20). Prologo SEH: solo protege los `new` de tareas, que en C++ no devuelven nulo.
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
// 0x8595EC: 0.017453292 (grados a radianes); 0x859878: 57.29578 (radianes a grados)
constexpr float EXE_DEG_TO_RAD = std::bit_cast<float>(0x3C8EFA35u);
constexpr float EXE_RAD_TO_DEG = std::bit_cast<float>(0x42652EE0u);
// 0x858B14: 3.0517578125e-05 (1 / 32768); 0x858B38: 0.02; 0x858B40: 50.0
constexpr float EXE_ONE_OVER_32768 = std::bit_cast<float>(0x38000000u);
constexpr float EXE_ZERO_POINT_02  = std::bit_cast<float>(0x3CA3D70Au);
constexpr float EXE_FIFTY          = std::bit_cast<float>(0x42480000u);
// 0x858C18: 1e-6 y 0x358637BD (el mismo valor, inmediato) para evitar divisiones entre cero en GET_2D_LINES_INTERSECT_POINT
constexpr float EXE_EPSILON = std::bit_cast<float>(0x358637BDu);
// 0xC9742400: -1000000.0f (resultado de GET_2D_LINES_INTERSECT_POINT sin interseccion)
constexpr float EXE_NO_INTERSECTION = std::bit_cast<float>(0xC9742400u);
// 0x85A308: double 0.01 (tolerancia de GET_2D_LINES_INTERSECT_POINT)
constexpr double EXE_TOLERANCE = 0.01;
// 0x86DB24, 0x86E69C, 0x86E698, 0x86FC8C y 0x870A8C: 20000 ms (tiempo por defecto de las tareas)
constexpr int32 EXE_DEFAULT_TIME = 20000;
// 0x86FC84 y 0x86FC88: 0.5 y 2.0 (radio y radio de cambio de paso de GoToPointAndStandStill); 0x86FC98 y 0x86FC9C: 0.5 y 5.0 (CTaskComplexFollowPointRoute)
constexpr float EXE_GOTO_RADIUS      = 0.5f;
constexpr float EXE_GOTO_MOVE_RADIUS = 2.0f;
constexpr float EXE_ROUTE_RADIUS     = 0.5f;
constexpr float EXE_ROUTE_SLOWDOWN   = 5.0f;
// 0x86FC7C y 0x86FC80: 0.5 y 0.2 (velocidad de giro y tolerancia de CTaskSimpleAchieveHeading)
constexpr float EXE_TURN_RATE    = 0.5f;
constexpr float EXE_TURN_MAXHEAD = std::bit_cast<float>(0x3E4CCCCDu);
// 0x86F664 y 0x86F668: 1000 y 1.0f (periodo y tolerancia de CTaskComplexFleeEntity)
constexpr int32 EXE_FLEE_POS_PERIOD    = 1000;
constexpr float EXE_FLEE_POS_TOLERANCE = 1.0f;
// 0x8D237C: movimiento por defecto de las tareas de entrar al coche (vale 6); el EXE lo copia al task y lo vuelve a escribir con 6.
inline auto& EnterCarMoveStateGlobal = StaticRef<int32>(0x8D237C);
// 0xC18D50: ruta global del EXE (CTheScripts)
inline auto& ScriptPointRoute = StaticRef<CPointRoute>(0xC18D50);
} // namespace

static_assert(offsetof(CRunningScript, m_UsesMissionCleanup) == 0xC6);

// 0x48EAA0
OpcodeResult CRunningScript::ProcessCommands1400To1499(int32 command) {
    // Los casos con tarea acaban igual: CRunningScript::GivePedScriptedTask(ped, tarea, comando) (0x465C20).
    const auto GiveTask = [&](int32 pedHandle, CTask* task) {
        GivePedScriptedTask(pedHandle, task, command);
    };
    // 0x48FEFD: CTaskComplexSequence::AddTask sobre la secuencia activa (0xC178F0 + ms_iActiveSequence * 0x40)
    const auto AddToActiveSequence = [](CTask* task) {
        CTaskSequences::GetActiveSequence().AddTask(task);
    };

    switch (command) {
    // 0x48EBF4: SET_LOAD_COLLISION_FOR_CAR_FLAG
    case 1415: {
        CollectParameters(2);
        CVehicle* const veh = VehicleOf(P(0).iParam);
        if (P(1).iParam != 0) {
            veh->physicalFlags.bDontLoadCollision = false; // and [+0x40], 0xFFFFBFFF
            if (!m_UsesMissionCleanup) {
                return OR_CONTINUE;
            }
            CWorld::Remove(veh);
            veh->m_nFlags |= 0x40000; // m_bIsStaticWaitingForCollision
            CWorld::Add(veh);
            return OR_CONTINUE;
        }
        veh->physicalFlags.bDontLoadCollision = true; // or [+0x40], 0x4000
        if (veh->m_nFlags & 0x40000) {
            veh->m_nFlags &= 0xFFFBFFFFu;
            if (!veh->GetIsStatic()) {
                veh->AddToMovingList();
            }
        }
        return OR_CONTINUE;
    }
    // 0x48EDA0: GET_PROGRESS_PERCENTAGE
    case 1420: {
        P(0).fParam = CStats::GetPercentageProgress();
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48EDBB: SET_VEHICLE_TO_FADE_IN
    case 1428: {
        CollectParameters(2);
        const int32 alpha = P(1).iParam;
        CVehicle* const veh = VehicleOf(P(0).iParam);
        CVisibilityPlugins::SetClumpAlpha(reinterpret_cast<RpClump*>(veh->GetRwObject()), alpha);
        return OR_CONTINUE;
    }
    // 0x48EDEF: REGISTER_ODDJOB_MISSION_PASSED
    case 1429: {
        CStats::IncrementStat(static_cast<eStats>(0x93), 1.0f);
        const float value = CStats::GetStatValue(static_cast<eStats>(0xB0));
        CStats::IncrementStat(static_cast<eStats>(0xB1), value);
        CStats::SetStatValue(static_cast<eStats>(0xB0), 0.0f);
        CTheScripts::LastMissionPassedTime = CTimer::GetTimeInMS(); // [0xB7CB84]
        return OR_CONTINUE;
    }
    // 0x48EE42: IS_AUSTRALIAN_GAME (lee un parametro y la bandera queda en falso)
    case 1434: {
        CollectParameters(1);
        UpdateCompareFlag(false);
        return OR_CONTINUE;
    }
    // 0x48EE90: SET_ONSCREEN_COUNTER_FLASH_WHEN_FIRST_DISPLAYED (el indice de la variable global se lee ANTES de los parametros)
    case 1436: {
        const uint16 varIndex = GetIndexOfGlobalVariable();
        CollectParameters(1);
        CUserDisplay::OnscnTimer.SetCounterFlashWhenFirstDisplayed(varIndex, P(0).iParam != 0);
        return OR_CONTINUE;
    }
    // 0x48EED9: SHUFFLE_CARD_DECKS
    case 1437: {
        CollectParameters(1);
        auto& cardStack = CTheScripts::CardStack;
        std::fill(cardStack.begin(), cardStack.end(), int16(0)); // rep stosd de 0x9C dwords (312 int16)
        int16 deck[312]; // [esp+0x154], 0x138 entradas
        for (int32 i = 0; i < 0x138; i++) {
            deck[i] = static_cast<int16>(i);
        }
        int32 remaining = P(0).iParam * 0x34; // edi (se usa su palabra baja con signo)
        for (int32 card = 1; card < 0x35; card++) {
            for (int32 n = 0; n < P(0).iParam; n++) { // el numero de mazos se relee de ScriptParams en cada vuelta
                const int32 count = static_cast<int16>(remaining); // movsx esi, di
                const int32 randomValue = ExeCrt::Rand() & 0xFFFF;
                const float scaled = static_cast<float>(randomValue) * EXE_ONE_OVER_32768;
                int32 index = static_cast<int16>(static_cast<int32>(scaled * static_cast<float>(count))); // _ftol, movsx ax
                cardStack[static_cast<size_t>(static_cast<int16>(deck[index]))] = static_cast<int16>(card);
                for (int32 j = index; j < count; j++) {
                    if (j > 0x136) {
                        deck[j] = 0;
                    } else {
                        deck[j] = deck[j + 1];
                    }
                }
                remaining--;
            }
        }
        CTheScripts::CardStackPosition = 0;
        return OR_CONTINUE;
    }
    // 0x48EFAD: FETCH_NEXT_CARD
    case 1438: {
        int16 position = CTheScripts::CardStackPosition;
        if (CTheScripts::CardStack[static_cast<size_t>(position)] == 0) {
            position = 0;
        }
        P(0).iParam = CTheScripts::CardStack[static_cast<size_t>(position)];
        position++;
        CTheScripts::CardStackPosition = position;
        if (position == 0x138) {
            CTheScripts::CardStackPosition = 0;
        }
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48F07D: ADD_TO_OBJECT_ROTATION_VELOCITY
    case 1441: {
        CollectParameters(4);
        CObject* const object = ObjectOf(P(0).iParam);
        const float ax = P(1).fParam * EXE_ZERO_POINT_02;
        const float ay = P(2).fParam * EXE_ZERO_POINT_02;
        const float az = P(3).fParam * EXE_ZERO_POINT_02;
        CVector turn = object->m_vecTurnSpeed; // [+0x50]
        const CVector local{ ax, ay, az };
        CVector rotated;
        Multiply3x3Exe(&rotated, object->m_matrix, &local);
        turn.x = turn.x + rotated.x;
        turn.y = turn.y + rotated.y;
        turn.z = turn.z + rotated.z;
        if (object->m_nFlags & 4) {
            object->SetIsStatic(false); // vtable +0x10
            object->AddToMovingList();
        }
        object->m_vecTurnSpeed = turn;
        return OR_CONTINUE;
    }
    // 0x48F17E: SET_OBJECT_ROTATION_VELOCITY
    case 1442: {
        CollectParameters(4);
        CObject* const object = ObjectOf(P(0).iParam);
        const float timeStep = CTimer::GetTimeStep(); // [0xB7CB5C]
        const CVector local{ timeStep * P(1).fParam, timeStep * P(2).fParam, timeStep * P(3).fParam };
        CVector rotated;
        Multiply3x3Exe(&rotated, object->m_matrix, &local);
        if (object->m_nFlags & 4) {
            object->SetIsStatic(false);
            object->AddToMovingList();
        }
        object->m_vecTurnSpeed = CVector{ rotated.x * EXE_ZERO_POINT_02, rotated.y * EXE_ZERO_POINT_02, rotated.z * EXE_ZERO_POINT_02 }; // CPhysical::SetTurnSpeed (0x45AFB0)
        return OR_CONTINUE;
    }
    // 0x48F260: IS_OBJECT_STATIC
    case 1443: {
        CollectParameters(1);
        bool result = false;
        if (ObjectOf(P(0).iParam)->GetIsStatic()) {
            result = true;
        }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    // 0x48F2A2: GET_ANGLE_BETWEEN_2D_VECTORS
    case 1444: {
        CollectParameters(4);
        const float x1 = P(0).fParam, y1 = P(1).fParam, x2 = P(2).fParam, y2 = P(3).fParam;
        const float dot = (y2 * y1) + (x2 * x1);
        const float length2 = std::sqrt((y2 * y2) + (x2 * x2));
        const float length1 = std::sqrt((y1 * y1) + (x1 * x1));
        P(0).fParam = ExeAcosCpp(dot / (length2 * length1)) * EXE_RAD_TO_DEG; // acos del CRT del EXE (0x822380)
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48F339: DO_2D_RECTANGLES_COLLIDE
    case 1445: {
        CollectParameters(8);
        const float a = P(0).fParam, b = P(1).fParam, c = P(2).fParam, d = P(3).fParam;
        const float e = P(4).fParam, f = P(5).fParam, g = P(6).fParam, h = P(7).fParam;
        const float D = static_cast<float>(d * 0.5); // fmul qword [0x859060] = 0.5
        const float H = static_cast<float>(h * 0.5);
        const float C = static_cast<float>(c * 0.5);
        const float G = static_cast<float>(g * 0.5);
        bool result = true;
        // Cada comparacion falla solo con orden estricto (los NaN dejan el resultado como esta, como las pruebas de C0/C2/C3 del EXE)
        if ((f - H) > (b + D)) { result = false; }
        if ((b - D) > (f + H)) { result = false; }
        if ((a + C) < (e - G)) { result = false; }
        if ((a - C) > (e + G)) { result = false; }
        UpdateCompareFlag(result);
        return OR_CONTINUE;
    }
    // 0x48F461: GET_OBJECT_ROTATION_VELOCITY
    case 1446: {
        CollectParameters(1);
        CObject* const object = ObjectOf(P(0).iParam);
        const CVector turn = object->m_vecTurnSpeed;
        CVector rotated;
        Multiply3x3VMExe(&rotated, &turn, object->m_matrix);
        P(0).fParam = rotated.x * EXE_FIFTY;
        P(1).fParam = rotated.y * EXE_FIFTY;
        P(2).fParam = rotated.z * EXE_FIFTY;
        StoreParameters(3);
        return OR_CONTINUE;
    }
    // 0x48F4E9: ADD_VELOCITY_RELATIVE_TO_OBJECT_VELOCITY
    case 1447: {
        CollectParameters(4);
        CObject* const object = ObjectOf(P(0).iParam);
        CVector amount{ P(1).fParam, P(2).fParam, P(3).fParam };
        amount *= CTimer::GetTimeStep(); // CVector::operator*=(float) (0x40FEF0)
        {
            // CVector::operator/=(50.0f) del EXE (0x411A30): multiplica por el inverso 1.0 / 50.0
            const double inverse = 1.0 / double(EXE_FIFTY);
            amount.x = float(inverse * amount.x);
            amount.y = float(inverse * amount.y);
            amount.z = float(inverse * amount.z);
        }
        if (object->m_nFlags & 4) {
            return OR_CONTINUE;
        }
        CVector speed = object->m_vecMoveSpeed; // [+0x44]
        CVector forward = speed;
        forward.Normalise();
        if (forward.z == 1.0f) {
            return OR_CONTINUE;
        }
        const CVector up{ 0.0f, 0.0f, 1.0f };
        CVector right = CrossProduct(forward, up);
        right.Normalise();
        CVector top = CrossProduct(right, forward);
        top.Normalise();
        speed = object->m_vecMoveSpeed;
        speed += right * amount.x; // VectorTimesFloat (0x40FEC0) y operator+= (0x411A00)
        speed += forward * amount.y;
        speed += top * amount.z;
        if (object->m_nFlags & 4) {
            object->SetIsStatic(false);
            object->AddToMovingList();
        }
        object->m_vecMoveSpeed = speed;
        return OR_CONTINUE;
    }
    // 0x48F73F: GET_OBJECT_SPEED
    case 1448: {
        CollectParameters(1);
        CObject* const object = ObjectOf(P(0).iParam);
        P(0).fParam = object->m_vecMoveSpeed.Magnitude() * EXE_FIFTY;
        StoreParameters(1);
        return OR_CONTINUE;
    }
    // 0x48F830: GET_2D_LINES_INTERSECT_POINT
    case 1456: {
        CollectParameters(8);
        float a = P(0).fParam, b = P(1).fParam, c = P(2).fParam, d = P(3).fParam;
        float e = P(4).fParam, f = P(5).fParam, g = P(6).fParam, h = P(7).fParam;
        bool result = true;
        // Primera recta (a,b)-(c,d): pendiente m1 = (b - d) / (a - c) (1e-6 si a - c es exactamente cero)
        const float dx1 = a - c;
        const float divisor1 = (dx1 != 0.0f || std::isnan(dx1)) ? dx1 : EXE_EPSILON;
        // Segunda recta (e,f)-(g,h): m2 = (f - h) / (e - g)
        const float dx2 = e - g;
        const float divisor2 = (dx2 != 0.0f || std::isnan(dx2)) ? dx2 : EXE_EPSILON;
        const float dy1 = b - d;
        const float m1 = dy1 / divisor1;
        const float dy2 = f - h;
        const float m2 = dy2 / divisor2;
        const float i1 = b - m1 * a;
        const float i2 = f - m2 * e;
        const float negM1 = -m1;
        const float negM2 = -m2;
        const float den = negM1 - negM2;
        const float num = (negM1 * i2) - (negM2 * i1);
        float x, y;
        if (!(den != 0.0f || std::isnan(den))) {
            // rectas paralelas
            x = EXE_NO_INTERSECTION;
            y = EXE_NO_INTERSECTION;
            result = false;
        } else {
            if (dx1 != 0.0f || std::isnan(dx1)) {
                x = (i1 - i2) / den;
            } else {
                x = a; // recta vertical
            }
            if (dy1 != 0.0f || std::isnan(dy1)) {
                y = num / den;
            } else {
                y = b; // recta horizontal
            }
            // Ordena los extremos de cada segmento (el EXE intercambia con comparaciones estrictas)
            if (a > c) { std::swap(a, c); }
            if (b > d) { std::swap(b, d); }
            if (e > g) { std::swap(e, g); }
            if (f > h) { std::swap(f, h); }
            const auto Minus = [](float v) { return static_cast<float>(static_cast<double>(v) - EXE_TOLERANCE); };
            const auto Plus  = [](float v) { return static_cast<float>(static_cast<double>(v) + EXE_TOLERANCE); };
            // Cada prueba falla si NO se cumple la comparacion ordenada
            if (!(Minus(a) <= x) || !(Plus(c) >= x) || !(Minus(e) <= x) || !(Plus(g) >= x)
                || !(Minus(b) <= y) || !(Plus(d) >= y) || !(Minus(f) <= y) || !(Plus(h) >= y)) {
                x = EXE_NO_INTERSECTION;
                y = EXE_NO_INTERSECTION;
                result = false;
            }
        }
        UpdateCompareFlag(result);
        P(1).fParam = y;
        P(0).fParam = x;
        StoreParameters(2);
        return OR_CONTINUE;
    }
    // 0x48FB0E: TASK_PAUSE
    case 1465: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 time = P(1).iParam;
        GiveTask(pedHandle, new CTaskSimplePause(time));
        return OR_CONTINUE;
    }
    // 0x48FB82: TASK_STAND_STILL (-1: 20000 ms; -2: 999999 ms y repetida)
    case 1466: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 time = P(1).iParam;
        CTask* task;
        if (time == -1) {
            task = new CTaskSimpleStandStill(EXE_DEFAULT_TIME, false, false, 8.0f);
        } else if (time == -2) {
            task = new CTaskSimpleStandStill(0xF423F, true, false, 8.0f);
        } else {
            task = new CTaskSimpleStandStill(time, false, false, 8.0f);
        }
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x48FC9D: TASK_FALL_AND_GET_UP
    case 1467: {
        CollectParameters(3);
        const int32 pedHandle = P(0).iParam;
        const int32 direction = P(1).iParam;
        const int32 downTime = P(2).iParam;
        GiveTask(pedHandle, new CTaskComplexFallAndGetUp(direction, downTime));
        return OR_CONTINUE;
    }
    // 0x48FCE7: TASK_JUMP
    case 1468: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 flag = P(1).iParam;
        auto* const task = new CTaskComplexJump(CTaskComplexJump::eForceClimb::OK);
        task->m_UsePlayerLaunchForce = flag != 0; // byte [+0x10]
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x48FD4A: TASK_TIRED
    case 1469: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 time = P(1).iParam;
        GiveTask(pedHandle, new CTaskSimpleTired(time));
        return OR_CONTINUE;
    }
    // 0x48FD8D: TASK_DIE
    case 1470: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        auto* const task = new CTaskComplexDie(
            static_cast<eWeaponType>(0),
            static_cast<AssocGroupId>(0),
            static_cast<AnimationId>(0xF),
            4.0f,
            0.0f,
            false,
            false,
            static_cast<eDirection>(0),
            false
        );
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x48FE0F: TASK_LOOK_AT_CHAR (-1: 20000 ms; -2: sin limite, -1)
    case 1471: {
        CollectParameters(3);
        int32 time = P(2).iParam; // esi
        const int32 pedHandle = P(0).iParam;
        const int32 targetHandle = P(1).iParam;
        if (time == -1) {
            time = EXE_DEFAULT_TIME;
        } else if (time == -2) {
            time = -1;
        }
        CPed* const target = PedOf(targetHandle); // edi
        if (pedHandle != -1) {
            g_ikChainMan.LookAt("COMMAND_TASK_LOOK_AT_CHAR", PedOf(pedHandle), target, time, eBoneTag32{ static_cast<eBoneTag>(5) }, nullptr, false, 0.25f, 500, 6, true);
            return OR_CONTINUE;
        }
        CVector offset{ 0.0f, 0.0f, 0.0f };
        AddToActiveSequence(new CTaskSimpleTriggerLookAt(target, time, 5, offset, true, 0.25f, 1000, 3));
        return OR_CONTINUE;
    }
    // 0x48FF19: TASK_LOOK_AT_VEHICLE (-1: 20000 ms; -2: 0x7FFFFFFF)
    case 1472: {
        CollectParameters(3);
        int32 time = P(2).iParam; // esi
        const int32 pedHandle = P(0).iParam;
        if (time == -1) {
            time = EXE_DEFAULT_TIME;
        } else if (time == -2) {
            time = 0x7FFFFFFF;
        }
        CVehicle* const target = VehicleOf(P(1).iParam); // edi
        if (pedHandle != -1) {
            g_ikChainMan.LookAt("COMMAND_TASK_LOOK_AT_VEHICLE", PedOf(pedHandle), target, time, eBoneTag32{ static_cast<eBoneTag>(-1) }, nullptr, false, 0.25f, 500, 6, true);
            return OR_CONTINUE;
        }
        CVector offset{ 0.0f, 0.0f, 0.0f };
        AddToActiveSequence(new CTaskSimpleTriggerLookAt(target, time, -1, offset, true, 0.25f, 1000, 3));
        return OR_CONTINUE;
    }
    // 0x490006: TASK_SAY
    case 1473: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 sayId = P(1).iParam;
        auto* const say = new CTaskSimpleSay(static_cast<eGlobalSpeechContext>(sayId), -1);
        if (pedHandle != -1) {
            PedOf(pedHandle)->GetTaskManager().SetTaskSecondary(say, TASK_SECONDARY_SAY);
            return OR_CONTINUE;
        }
        AddToActiveSequence(new CTaskSimpleAffectSecondaryBehaviour(true, TASK_SECONDARY_SAY, say));
        return OR_CONTINUE;
    }
    // 0x4900B2: TASK_SHAKE_FIST
    case 1474: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        auto* const fist = new CTaskSimpleShakeFist();
        if (pedHandle != -1) {
            PedOf(pedHandle)->GetTaskManager().SetTaskSecondary(fist, TASK_SECONDARY_PARTIAL_ANIM);
            return OR_CONTINUE;
        }
        AddToActiveSequence(new CTaskSimpleAffectSecondaryBehaviour(true, TASK_SECONDARY_PARTIAL_ANIM, fist));
        return OR_CONTINUE;
    }
    // 0x490143: TASK_COWER
    case 1475: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        GiveTask(pedHandle, new CTaskSimpleCower());
        return OR_CONTINUE;
    }
    // 0x49017F: TASK_HANDS_UP (-1: 20000 ms; -2: 0x7FFFFFFF)
    case 1476: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        int32 time = P(1).iParam;
        if (time == -1) {
            time = EXE_DEFAULT_TIME;
        } else if (time == -2) {
            time = 0x7FFFFFFF;
        }
        GiveTask(pedHandle, new CTaskSimpleHandsUp(time));
        return OR_CONTINUE;
    }
    // 0x4901D8: TASK_DUCK (-2: duracion 0; -1: 20000 ms; otro: el valor, como uint16)
    case 1477: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        const int32 time = P(1).iParam;
        CTask* task;
        if (time == -2) {
            task = new CTaskSimpleDuck(static_cast<eDuckControlType>(0), 0, -1);
        } else if (time == -1) {
            task = new CTaskSimpleDuck(static_cast<eDuckControlType>(0), static_cast<uint16>(EXE_DEFAULT_TIME), -1);
        } else {
            task = new CTaskSimpleDuck(static_cast<eDuckControlType>(0), static_cast<uint16>(time), -1);
        }
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x49029B: TASK_USE_ATM
    case 1479: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        GiveTask(pedHandle, new CTaskSimpleUseAtm());
        return OR_CONTINUE;
    }
    // 0x4902D7: TASK_SCRATCH_HEAD
    case 1480: {
        CollectParameters(1);
        const int32 pedHandle = P(0).iParam;
        GiveTask(pedHandle, new CTaskSimpleScratchHead());
        return OR_CONTINUE;
    }
    // 0x490313: TASK_LOOK_ABOUT (-1: 20000 ms; -2: 0x7FFFFFFF)
    case 1481: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        int32 time = P(1).iParam;
        if (time == -1) {
            time = EXE_DEFAULT_TIME;
        } else if (time == -2) {
            time = 0x7FFFFFFF;
        }
        GiveTask(pedHandle, new CTaskSimpleLookAbout(time));
        return OR_CONTINUE;
    }
    // 0x49036C: TASK_ENTER_CAR_AS_PASSENGER (-2: tiempo -1; -1: 20000 ms; asiento -1: sin puerta calculada)
    case 1482: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam;
        const int32 time = P(2).iParam;
        const int32 seatArg = P(3).iParam;
        CVehicle* const veh = VehicleOf(P(1).iParam);
        int32 door = 0; // edi
        if (seatArg != -1) {
            door = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seatArg);
        }
        CTaskComplexEnterCarAsPassengerTimed* task;
        if (time == -2) {
            task = new CTaskComplexEnterCarAsPassengerTimed(veh, door, static_cast<uint32>(-1), true);
        } else if (time == -1) {
            task = new CTaskComplexEnterCarAsPassengerTimed(veh, door, EXE_DEFAULT_TIME, true);
        } else {
            task = new CTaskComplexEnterCarAsPassengerTimed(veh, door, time, true);
        }
        task->SetMoveState(static_cast<eMoveState>(EnterCarMoveStateGlobal)); // [+0x1C]
        EnterCarMoveStateGlobal = 6;
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x49046B: TASK_ENTER_CAR_AS_DRIVER (-2: tiempo -1; -1: 20000 ms)
    case 1483: {
        CollectParameters(3);
        const int32 pedHandle = P(0).iParam;
        CVehicle* const veh = VehicleOf(P(1).iParam);
        const int32 time = P(2).iParam;
        CTaskComplexEnterCarAsDriverTimed* task;
        if (time == -2) {
            task = new CTaskComplexEnterCarAsDriverTimed(veh, -1);
        } else if (time == -1) {
            task = new CTaskComplexEnterCarAsDriverTimed(veh, EXE_DEFAULT_TIME);
        } else {
            task = new CTaskComplexEnterCarAsDriverTimed(veh, time);
        }
        task->m_moveState = static_cast<eMoveState>(EnterCarMoveStateGlobal); // [+0x14]
        EnterCarMoveStateGlobal = 6;
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x490554: TASK_LEAVE_CAR
    case 1485: {
        CollectParameters(2);
        const int32 pedHandle = P(0).iParam;
        CVehicle* const veh = VehicleOf(P(1).iParam);
        GiveTask(pedHandle, new CTaskComplexLeaveCar(veh, 0, 0, true, false));
        return OR_CONTINUE;
    }
    // 0x4905AC: TASK_LEAVE_CAR_AND_FLEE
    case 1487: {
        CollectParameters(5);
        const int32 pedHandle = P(0).iParam;
        CVehicle* const veh = VehicleOf(P(1).iParam);
        const CVector fleePoint{ P(2).fParam, P(3).fParam, P(4).fParam };
        GiveTask(pedHandle, new CTaskComplexLeaveCarAndFlee(veh, fleePoint, static_cast<eTargetDoor>(0), 0, false));
        return OR_CONTINUE;
    }
    // 0x490649: TASK_CAR_DRIVE_TO_COORD (vehiculo negativo: ninguno; modelo 0 -> -1, 1 -> 0x19F)
    case 1489: {
        CollectParameters(9);
        const int32 pedHandle = P(0).iParam;
        CVehicle* veh = nullptr; // ebp
        if (P(1).iParam >= 0) {
            veh = VehicleOf(P(1).iParam);
        }
        const float x = P(2).fParam;
        const float y = P(3).fParam;
        float z = P(4).fParam;
        if (z <= EXE_MIN_GROUND_Z) {
            z = CWorld::FindGroundZForCoord(x, y);
        }
        const CVector point{ x, y, z };
        const float speed = P(5).fParam;
        const int32 arg4 = P(6).iParam;
        int32 carModel = P(7).iParam; // edi
        if (carModel == 0) {
            carModel = -1;
        } else if (carModel == 1) {
            carModel = 0x19F;
        }
        const int32 drivingStyle = P(8).iParam; // ebx
        GiveTask(pedHandle, new CTaskComplexDriveToPoint(veh, point, speed, arg4, static_cast<eModelID>(carModel), -1.0f, static_cast<eCarDrivingStyle>(drivingStyle)));
        return OR_CONTINUE;
    }
    // 0x490762: TASK_CAR_DRIVE_WANDER (vehiculo negativo: ninguno)
    case 1490: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam;
        const float speed = P(2).fParam;
        const int32 drivingStyle = P(3).iParam;
        CVehicle* veh = nullptr; // edi
        if (P(1).iParam >= 0) {
            veh = VehicleOf(P(1).iParam);
        }
        GiveTask(pedHandle, new CTaskComplexCarDriveWander(veh, static_cast<eCarDrivingStyle>(drivingStyle), speed));
        return OR_CONTINUE;
    }
    // 0x4907CE: TASK_GO_STRAIGHT_TO_COORD (-2: sin tiempo y parar exacto; -1: 20000 ms)
    case 1491: {
        CollectParameters(6);
        const int32 pedHandle = P(0).iParam;
        const CVector point{ P(1).fParam, P(2).fParam, P(3).fParam };
        const int32 moveState = P(4).iParam; // ebx
        const int32 time = P(5).iParam;      // edi
        CTask* task;
        if (time == -2) {
            task = new CTaskComplexGoToPointAndStandStill(static_cast<eMoveState>(moveState), point, EXE_GOTO_RADIUS, EXE_GOTO_MOVE_RADIUS, false, true);
        } else if (time == -1) {
            task = new CTaskComplexGoToPointAndStandStillTimed(static_cast<eMoveState>(moveState), point, EXE_GOTO_RADIUS, EXE_GOTO_MOVE_RADIUS, EXE_DEFAULT_TIME);
        } else {
            task = new CTaskComplexGoToPointAndStandStillTimed(static_cast<eMoveState>(moveState), point, EXE_GOTO_RADIUS, EXE_GOTO_MOVE_RADIUS, time);
        }
        GiveTask(pedHandle, task);
        return OR_CONTINUE;
    }
    // 0x49090B: TASK_ACHIEVE_HEADING (el rumbo del script va en grados)
    case 1492: {
        CollectParameters(2);
        const float heading = P(1).fParam * EXE_DEG_TO_RAD;
        const int32 pedHandle = P(0).iParam;
        GiveTask(pedHandle, new CTaskSimpleAchieveHeading(heading, EXE_TURN_RATE, EXE_TURN_MAXHEAD));
        return OR_CONTINUE;
    }
    // 0x490981: FLUSH_ROUTE (pone a cero el contador de la ruta global 0xC18D50)
    case 1494: {
        ScriptPointRoute.Clear();
        return OR_CONTINUE;
    }
    // 0x490992: EXTEND_ROUTE (CPointRoute::AddPoint 0x48E180)
    case 1495: {
        CollectParameters(3);
        const CVector point{ P(0).fParam, P(1).fParam, P(2).fParam };
        ScriptPointRoute.AddPoint(point);
        return OR_CONTINUE;
    }
    // 0x4909D4: TASK_FOLLOW_POINT_ROUTE
    case 1496: {
        CollectParameters(3);
        const int32 pedHandle = P(0).iParam;
        const int32 moveState = P(1).iParam;
        const int32 mode = P(2).iParam;
        GiveTask(pedHandle, new CTaskComplexFollowPointRoute(
            static_cast<eMoveState>(moveState),
            ScriptPointRoute,
            static_cast<CTaskComplexFollowPointRoute::Mode>(mode),
            EXE_ROUTE_RADIUS,
            EXE_ROUTE_SLOWDOWN,
            false,
            true,
            true
        ));
        return OR_CONTINUE;
    }
    // 0x490A55: TASK_GOTO_CHAR (-2: sin tiempo, -1; -1: 50000 ms)
    case 1497: {
        CollectParameters(4);
        const int32 pedHandle = P(0).iParam; // ebp
        CPed* const target = PedOf(P(1).iParam); // ebx
        const float maxDistance = P(3).fParam;
        int32 time = P(2).iParam; // edi
        if (time == -2) {
            time = -1;
        } else if (time == -1) {
            time = 0xC350;
        }
        GiveTask(pedHandle, new CTaskComplexSeekEntityStandard(target, time, 1000, maxDistance, 2.0f, 2.0f, true, true));
        return OR_CONTINUE;
    }
    // 0x490AE6: TASK_FLEE_POINT
    case 1498: {
        CollectParameters(6);
        const int32 pedHandle = P(0).iParam;
        const CVector point{ P(1).fParam, P(2).fParam, P(3).fParam };
        const float safeDistance = P(4).fParam;
        const int32 time = P(5).iParam;
        GiveTask(pedHandle, new CTaskComplexFleePoint(point, true, safeDistance, time));
        return OR_CONTINUE;
    }
    // 0x490B7E: TASK_FLEE_CHAR (la distancia segura es un ENTERO del script: fild)
    case 1499: {
        CollectParameters(4);
        const float safeDistance = static_cast<float>(P(2).iParam);
        const int32 pedHandle = P(0).iParam;
        const int32 time = P(3).iParam;
        CPed* const target = PedOf(P(1).iParam);
        GiveTask(pedHandle, new CTaskComplexFleeEntity(target, true, safeDistance, time, EXE_FLEE_POS_PERIOD, EXE_FLEE_POS_TOLERANCE));
        return OR_CONTINUE;
    }
    case COMMAND_SET_CHAR_IN_DISGUISE: // 1493: el EXE (0x490C0B: xor al,al) no lee parametros y devuelve OR_CONTINUE
        return OR_CONTINUE;
    default:
        return OR_INTERRUPT; // 0x490C0F: or al, 0xFF
    }
}
