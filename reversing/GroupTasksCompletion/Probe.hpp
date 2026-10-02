#pragma once
#include <array>
// Contadores de entrada de las funciones candidatas (solo en la sonda temporal).
// 0: CPedGroupDefaultTaskAllocatorRandom::AllocateDefaultTasks
// 1: CPedGroupDefaultTaskAllocatorSitInLeaderCar::AllocateDefaultTasks
// 2: CTaskComplexGangFollower::SetCanEnterLeadersCar
namespace GroupAllocProbe { inline std::array<unsigned,3> calls{}; }
