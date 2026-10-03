/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CUpsideDownCarCheck
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "UpsideDownCarCheck.h"

void CUpsideDownCarCheck::InjectHooks() {
    RH_ScopedClass(CUpsideDownCarCheck);
    RH_ScopedCategoryGlobal();

    // RH_ScopedOverloadedInstall(IsCarUpsideDown, "handle", 0x0);
    RH_ScopedOverloadedInstall(IsCarUpsideDown, "vehicle", 0x463830, bool(CUpsideDownCarCheck::*)(const CVehicle*));
    RH_ScopedInstall(UpdateTimers, 0x4655E0);
    RH_ScopedInstall(AddCarToCheck, 0x4638D0);
    RH_ScopedInstall(RemoveCarFromCheck, 0x463910);
    RH_ScopedInstall(HasCarBeenUpsideDownForAWhile, 0x463940);
}

// completely inlined, no addr, see 0x468DCB
void CUpsideDownCarCheck::Init() {
    std::ranges::fill(m_aUpsideDownCars, UpsideDownCar());
}

bool CUpsideDownCarCheck::IsCarUpsideDown(int32 carHandle) {
    const auto vehicle = CPools::GetVehicle(carHandle);
    return IsCarUpsideDown(vehicle);
}

// 0x463830
bool CUpsideDownCarCheck::IsCarUpsideDown(const CVehicle* vehicle) {
    const auto GetNumContactWheels = [=]() -> uint32 {
        switch (vehicle->m_nVehicleType) {
        case eVehicleType::VEHICLE_TYPE_BIKE:       return static_cast<const CBike*>(vehicle)->m_nNoOfContactWheels;
        case eVehicleType::VEHICLE_TYPE_AUTOMOBILE: return static_cast<const CAutomobile*>(vehicle)->m_nNumContactWheels;
        default: return 0u;
        }
    };

    // Como el original: primero la orientación y solo entonces CanPedStepOutCar(false), cuyo resultado se devuelve
    // (antes se llamaba primero y se invertía)
    const auto numContactWheels = GetNumContactWheels();
    const float upZ = vehicle->GetUp().z;
    if (upZ < 0.3f && numContactWheels < 4 // Not totally up-side down
        || upZ < 0.f                       // Literally up-side down
    ) {
        return vehicle->CanPedStepOutCar(false);
    }
    return false;
}

// Process
// 0x4655E0
void CUpsideDownCarCheck::UpdateTimers() {
    ZoneScoped;

    // Como el original: ms_fTimeStep * 0.02f * 1000.0f truncado con _ftol2, calculado una vez antes del bucle
    const auto timeStepMs = static_cast<uint32>(static_cast<int64>(CTimer::GetTimeStep() * 0.02f * 1000.0f));
    auto* const vehiclePool = reinterpret_cast<uint8* const*>(GetVehiclePool());
    for (auto& car : m_aUpsideDownCars) {
        // Como el original: CPool::GetAtRef inlinado sin comprobar los límites del índice (también con el manejador -1
        // de los huecos vacíos) y sin saltarse el manejador 0
        const auto idx = car.m_nHandle >> 8;
        CVehicle* vehicle = nullptr;
        if (vehiclePool[1][idx] == static_cast<uint8>(car.m_nHandle)) {
            vehicle = reinterpret_cast<CVehicle*>(reinterpret_cast<uintptr_t>(vehiclePool[0]) + static_cast<uint32>(idx) * 0xA18u);
        }
        if (vehicle) {
            if (IsCarUpsideDown(vehicle)) {
                car.m_nTime += timeStepMs;
            } else {
                car.m_nTime = 0;
            }
        } else {
            // Como el original: manejador -1 (no 0) y tiempo a cero
            car.m_nHandle = -1;
            car.m_nTime = 0;
        }
    }
}

// 0x4638A0
// never used
bool CUpsideDownCarCheck::AreAnyCarsUpsideDown() {
    for (auto& car : m_aUpsideDownCars) {
        if (car.m_nHandle >= 0 && car.m_nTime > UPSIDE_DOWN_CAR_MIN_TIME) { // Como el original: ja (estrictamente mayor)
            return true;
        }
    }
    return false;
}

// 0x4638D0
void CUpsideDownCarCheck::AddCarToCheck(int32 carHandle) {
    for (auto& car : m_aUpsideDownCars) {
        if (car.m_nHandle < 0) {
            // Como el original: solo se ocupa el primer hueco libre
            car.m_nHandle = carHandle;
            car.m_nTime = 0;
            break;
        }
    }
}

// 0x463910
void CUpsideDownCarCheck::RemoveCarFromCheck(int32 carHandle) {
    for (auto& car : m_aUpsideDownCars) {
        if (car.m_nHandle == carHandle) {
            car.Clear();
        }
    }
}

// 0x463940
bool CUpsideDownCarCheck::HasCarBeenUpsideDownForAWhile(int32 carHandle) {
    for (auto& car : m_aUpsideDownCars) {
        if (car.m_nHandle == carHandle) {
            return car.m_nTime > UPSIDE_DOWN_CAR_MIN_TIME;
        }
    }
    return false;
}
