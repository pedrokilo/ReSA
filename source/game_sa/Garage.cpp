/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGarage
 * Reconstrucción: COMPLETA — 17/17 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 17/17 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Garage.h"
#include "X87Math.h"
#include "Collision/CollisionData.h"
#include "Collision/ColModel.h"

// 0x448740: ECX = this, EDX de entrada y 12 bytes de pila (ret 0Ch). Como el original: EDX queda intacto (los bucles del EXE de 0x448900, 0x4489A0 y 0x44961C lo
// usan de contador a traves de la llamada); ECX = this en las salidas por la z y los bits de dy en las demas (0x448764 sobrescribe con `fstp [esp]` el ECX guardado
// por `push ecx` y el `pop ecx` final lo devuelve); AL = resultado. Lo ultimo que ejecuta es ResidueRegsCpp (ver X87Math.h y contrib/audit_register_residue.py).
#pragma runtime_checks("", off)
static __declspec(safebuffers) uint32 __fastcall GarageIsPointInsideRegs(CGarage* self, uint32 entryEdx, CVector point) {
    uint32     ecxOut = static_cast<uint32>(reinterpret_cast<uintptr_t>(self));
    const bool inside = GarageInsidePointCpp(reinterpret_cast<const float*>(self), &point.x, &ecxOut);
    return ResidueRegsCpp(ecxOut, entryEdx, inside ? 1u : 0u);
}

// 0x4487D0: ECX = this, EDX de entrada y 16 bytes de pila (ret 10h). Como el original: ECX y EDX quedan intactos (FindGarageForObject, 0x449BDC y 0x44A296 los
// reutilizan tras la llamada); AL = resultado. No es static: el banco DependenciesWorld15 declara su direccion como destino del hook.
__declspec(safebuffers) uint32 __fastcall GarageIsPointInsideRadiusRegs(CGarage* self, uint32 entryEdx, CVector point, float radius) {
    const bool inside = GarageInsideRadiusCpp(reinterpret_cast<const float*>(self), &point.x, radius);
    return ResidueRegsCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(self)), entryEdx, inside ? 1u : 0u);
}

// 0x4486C0: ECX = this y EDX de entrada. Como el original: ECX y EDX quedan intactos (el bucle de 0x449E60 los usa de contador y de valor a traves de la llamada: `add ecx,0D8h` y el
// `mov [0x96C01C],edx` final); la funcion no devuelve nada (EAX es residuo que ningun llamador lee). No es static: el banco Garage63 declara su direccion como destino del hook.
__declspec(safebuffers) uint32 __fastcall GaragePlayerArrestedOrDiedRegs(CGarage* self, uint32 entryEdx) {
    self->PlayerArrestedOrDied();
    return ResidueRegsCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(self)), entryEdx, 0u);
}
#pragma runtime_checks("", restore)

void CGarage::InjectHooks() {
    RH_ScopedClass(CGarage);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(BuildRotatedDoorMatrix, 0x4479F0);
    RH_ScopedInstall(TidyUpGarageClose, 0x449D10);
    RH_ScopedInstall(TidyUpGarage, 0x449C50);
    RH_ScopedInstall(StoreAndRemoveCarsForThisHideOut, 0x449900);
    RH_ScopedInstall(EntityHasASphereWayOutsideGarage, 0x449050);
    RH_ScopedInstall(RemoveCarsBlockingDoorNotInside, 0x449690);
    RH_ScopedInstall(IsEntityTouching3D, 0x448EE0);
    RH_ScopedInstall(IsEntityEntirelyOutside, 0x448D30);
    RH_ScopedInstall(IsStaticPlayerCarEntirelyInside, 0x44A830);
    RH_ScopedInstall(IsEntityEntirelyInside3D, 0x448BE0);
    RH_ScopedNamedGlobalInstall(GarageIsPointInsideRegs, "IsPointInsideGarage-point", 0x448740); // EDX/ECX del EXE: ver GarageIsPointInsideRegs
    RH_ScopedNamedGlobalInstall(GaragePlayerArrestedOrDiedRegs, "PlayerArrestedOrDied", 0x4486C0); // ECX/EDX del EXE: ver GaragePlayerArrestedOrDiedRegs
    RH_ScopedInstall(OpenThisGarage, 0x447D50);
    RH_ScopedInstall(CloseThisGarage, 0x447D70);
    RH_ScopedInstall(InitDoorsAtStart, 0x447600);
    RH_ScopedNamedGlobalInstall(GarageIsPointInsideRadiusRegs, "IsPointInsideGarage-radius", 0x4487D0); // ECX/EDX del EXE: ver GarageIsPointInsideRadiusRegs
    // RH_ScopedInstall(Update, 0x44AA50);
    RH_ScopedInstall(IsGarageEmpty, 0x44A9C0);
}

// 0x4479F0
void CGarage::BuildRotatedDoorMatrix(CEntity* entity, float fDoorPosition) {
    // Como el original: fsin/fcos del x87 (precision completa) sobre fDoorPosition * (-pi/2 exacto, 0x859998; el HALF_PI de common.h es impreciso), cada uno guardado
    // como float; up = {-(sin * forward.y), sin * forward.x, cos} con los productos del x87 (GarageDoorUpCpp); right = forward x up con CrossProduct (0x59C730).
    CMatrix& matrix     = entity->GetMatrix();
    auto&    vecForward = matrix.GetForward();
    float    up[3];
    GarageDoorUpCpp(fDoorPosition, std::bit_cast<float>(0xBFC90FDBu), vecForward.x, vecForward.y, up);
    float right[3];
    GarageDoorRightCpp(&vecForward.x, up, right); // 0x59C730 con los productos del x87 (CrossProduct de la DLL es SSE: difiere en subnormales y NaN)
    std::memcpy(&matrix.GetRight().x, right, sizeof(right)); // copia de bits como los mov enteros del EXE (sin pasar por el x87)
    std::memcpy(&matrix.GetUp().x, up, sizeof(up));
}

// 0x449D10
// Como el original (0x449D10..0x449E5C): recorre el pool de vehiculos (ds:[0xB74494], leido una vez) desde el indice size-1 hasta el 1 (el 0 no se mira; con size == 1 no hay vuelta y
// con size == 0 el contador sin signo da la vuelta, como el `dec`/`je` del EXE). Solo toca vehiculos tipo 0 o 9 (m_nVehicleType) en estado WRECKED (bits 3..7 del byte +0x36 == 0x28)
// que IsEntityTouching3D da por dentro. Con la puerta cerrada (estado 0) se elimina sin mas; si no, se eliminan si alguna de sus esferas de colision (centro transformado con
// 0x59C890 y la matriz del vehiculo, que NO se crea) queda fuera del garaje (IsPointInsideGarage(centro, radio de la esfera)); sin esferas no se elimina. Eliminar es CWorld::Remove
// (0x563280) y el destructor virtual con flag 1. El pool, el puntero de estados y el de objetos se releen en cada vuelta.
void CGarage::TidyUpGarageClose() {
    auto* const pool = GetVehiclePool();
    uint32 index = static_cast<uint32>(pool->GetSize()) - 1u;
    if (index == 0)
        return;
    for (; index != 0; --index) {
        CVehicle* const vehicle = pool->GetAt(index);
        if (!vehicle)
            continue;
        if (vehicle->m_nVehicleType != VEHICLE_TYPE_AUTOMOBILE && vehicle->m_nVehicleType != VEHICLE_TYPE_BIKE)
            continue;
        if (vehicle->GetStatus() != STATUS_WRECKED)
            continue;
        if (!IsEntityTouching3D(vehicle))
            continue;
        bool remove = true;
        if (m_nDoorState != GARAGE_DOOR_CLOSED) {
            remove = false;
            CCollisionData* const data = vehicle->GetColModel()->m_pColData;
            if (static_cast<int16>(data->m_nNumSpheres) <= 0)
                continue;
            CColSphere* spheres = data->m_pSpheres;
            int32       sphereIndex = 0;
            do {
                CVector        transformed;
                CVector* const point = plugin::CallAndReturn<CVector*, 0x59C890, CVector*, CMatrix*, CVector*>(&transformed, vehicle->m_matrix, &spheres[sphereIndex].m_vecCenter);
                spheres = data->m_pSpheres;
                if (!IsPointInsideGarage(*point, spheres[sphereIndex].m_fRadius))
                    remove = true;
                ++sphereIndex;
            } while (sphereIndex < static_cast<int16>(data->m_nNumSpheres));
        }
        if (remove) {
            CWorld::Remove(vehicle);
            delete vehicle;
        }
    }
}

// 0x449C50
// Como el original (0x449C50..0x449D0A): mismo recorrido (size-1 hasta 1, ranuras ocupadas, tipo 0 o 9) pero la posicion del vehiculo (matriz o m_placement) debe estar dentro del garaje
// (IsPointInsideGarage(posicion)) y el vehiculo se elimina si esta WRECKED o, si no, si la componente z del eje "up" de su matriz (+0x28; el puntero de matriz se lee antes de la llamada y
// se conserva: el EXE no lo comprueba) es menor que 0.5f (comparacion ordenada: con NaN no se elimina).
void CGarage::TidyUpGarage() {
    auto* const pool = GetVehiclePool();
    uint32 index = static_cast<uint32>(pool->GetSize()) - 1u;
    if (index == 0)
        return;
    for (; index != 0; --index) {
        CVehicle* const vehicle = pool->GetAt(index);
        if (!vehicle)
            continue;
        if (vehicle->m_nVehicleType != VEHICLE_TYPE_AUTOMOBILE && vehicle->m_nVehicleType != VEHICLE_TYPE_BIKE)
            continue;
        CMatrixLink* const matrix = vehicle->m_matrix;
        if (!IsPointInsideGarage(vehicle->GetPosition()))
            continue;
        if (vehicle->GetStatus() != STATUS_WRECKED && !(matrix->GetUp().z < 0.5f))
            continue;
        CWorld::Remove(vehicle);
        delete vehicle;
    }
}

// 0x449900
void CGarage::StoreAndRemoveCarsForThisHideOut(CStoredCar* storedCars, int32 maxSlot) {
    maxSlot = std::min<int32>(maxSlot, NUM_GARAGE_STORED_CARS);

    for (auto i = 0; i < NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();

    auto pool = GetVehiclePool();
    int32 storedCarIdx{0}; // Como el original: comparación con signo contra maxSlot
    for (auto i = pool->GetSize(); i; i--) {
        if (auto vehicle = pool->GetAt(i - 1)) {
            if (IsPointInsideGarage(vehicle->GetPosition()) && vehicle->GetCreatedBy() != MISSION_VEHICLE) {
                if (storedCarIdx < maxSlot && !EntityHasASphereWayOutsideGarage(vehicle, 1.0f)) {
                    storedCars[storedCarIdx++].StoreCar(vehicle);
                }

                FindPlayerInfo().CancelPlayerEnteringCars(vehicle);
                CWorld::Remove(vehicle);
                delete vehicle;
            }
        }
    }

    // Clear slots with no vehicles in it
    for (auto i = storedCarIdx; i < (int32)NUM_GARAGE_STORED_CARS; i++)
        storedCars[i].Clear();
}


// 0x449050
// Como el original (0x449050..0x4490EF): para cada esfera de colision, centro transformado con 0x59C890 (funcion del EXE sin reconstruir, por su direccion),
// radio = fRadius + radio de la esfera (fadd del x87 guardado como float) y IsPointInsideGarage(centro, radio); devuelve true en la primera esfera que queda fuera.
// La matriz se crea (y se actualiza) en cada vuelta como el original, y el contador de esferas (int16 con signo) se vuelve a leer en cada vuelta.
bool CGarage::EntityHasASphereWayOutsideGarage(CEntity* entity, float fRadius) {
    CCollisionData* const data = entity->GetColModel()->m_pColData;
    if (static_cast<int16>(data->m_nNumSpheres) <= 0)
        return false;

    int32 index = 0;
    do {
        CColSphere* const sphere = &data->m_pSpheres[index];
        CMatrix&          matrix = entity->GetMatrix();
        CVector           transformed;
        CVector* const    point  = plugin::CallAndReturn<CVector*, 0x59C890, CVector*, CMatrix*, CVector*>(&transformed, &matrix, &sphere->m_vecCenter);
        const float       radius = RpNanAddCpp(fRadius, sphere->m_fRadius);
        if (!IsPointInsideGarage(*point, radius))
            return true;
        ++index;
    } while (index < static_cast<int16>(data->m_nNumSpheres));
    return false;
}

// 0x449690
// Como el original (0x449690..0x44973E): recorre el pool de vehiculos (ds:[0xB74494], tamano leido una vez) del ultimo indice al primero (ranuras libres fuera) y elimina el primero que
// IsEntityTouching3D da por tocando, cuya posicion queda fuera del garaje (IsPointInsideGarage), que no esta bloqueado por script (bit 3 del byte +0x428 = bIsLocked) y cuyo
// CanBeDeleted (0x6D1180) es cierto: CWorld::Remove (0x563280) y destructor virtual con flag 1; despues termina.
void CGarage::RemoveCarsBlockingDoorNotInside() {
    auto* const pool = GetVehiclePool();
    for (uint32 i = static_cast<uint32>(pool->GetSize()); i != 0; --i) {
        CVehicle* const vehicle = pool->GetAt(i - 1);
        if (!vehicle)
            continue;
        if (!IsEntityTouching3D(vehicle))
            continue;
        if (IsPointInsideGarage(vehicle->GetPosition()))
            continue;
        if (vehicle->vehicleFlags.bIsLocked)
            continue;
        if (!vehicle->CanBeDeleted())
            continue;
        CWorld::Remove(vehicle);
        delete vehicle;
        return;
    }
}

// 0x448EE0
// Como el original (0x448EE0..0x449041): radio = esfera delimitadora del CColModel del modelo (no el de CEntity::GetColModel), rechazo por caja ampliada con los seis
// limites del garaje (GarageEntityNearCpp, x87) y, para cada esfera de colision, centro transformado con 0x59C890 (por su direccion) e
// IsPointInsideGarage(centro, radio de la esfera).
__declspec(noinline) bool CGarage::IsEntityTouching3D(CEntity* entity) {
    const float radius = CModelInfo::GetModelInfo(static_cast<int32>(static_cast<int16>(entity->m_nModelIndex)))->GetColModel()->GetBoundRadius();
    if (!GarageEntityNearCpp(reinterpret_cast<const float*>(this), radius, &entity->GetPosition().x))
        return false;

    CCollisionData* const data = entity->GetColModel()->m_pColData;
    if (static_cast<int16>(data->m_nNumSpheres) <= 0)
        return false;

    int32 index = 0;
    do {
        CColSphere* const sphere = &data->m_pSpheres[index];
        CMatrix&          matrix = entity->GetMatrix();
        CVector           transformed;
        CVector* const    point  = plugin::CallAndReturn<CVector*, 0x59C890, CVector*, CMatrix*, CVector*>(&transformed, &matrix, &sphere->m_vecCenter);
        if (IsPointInsideGarage(*point, sphere->m_fRadius))
            return true;
        ++index;
    } while (index < static_cast<int16>(data->m_nNumSpheres));
    return false;
}

// 0x448D30
// Como el original (0x448D30..0x449041): si la posicion queda ESTRICTAMENTE dentro del rectangulo del garaje ampliado en `radius` en x e y (Garage63InsideRectStrictCpp, x87; con NaN
// no) la entidad no esta fuera. Si no, para cada esfera de colision (contador int16 releido en cada vuelta) la matriz se crea si falta (AllocateMatrix 0x54F560 + UpdateMatrix 0x54F1B0 en
// cada vuelta, como el EXE), el centro se transforma con 0x59C890 y la entidad NO esta fuera si IsPointInsideGarage(centro, radius + radio de la esfera) (suma con el x87 guardada como float).
bool CGarage::IsEntityEntirelyOutside(CEntity* entity, float radius) {
    if (Garage63InsideRectStrictCpp(reinterpret_cast<const float*>(this), radius, &entity->GetPosition().x))
        return false;

    CCollisionData* const data = entity->GetColModel()->m_pColData;
    if (!data || static_cast<int16>(data->m_nNumSpheres) <= 0)
        return true;

    int16 index = 0;
    do {
        if (!entity->m_matrix) {
            entity->AllocateMatrix();
            entity->m_placement.UpdateMatrix(entity->m_matrix);
        }
        CColSphere* const sphere = &data->m_pSpheres[index];
        CVector           transformed;
        CVector* const    point = plugin::CallAndReturn<CVector*, 0x59C890, CVector*, CMatrix*, CVector*>(&transformed, entity->m_matrix, &sphere->m_vecCenter);
        const float       sphereRadius = RpNanAddCpp(radius, data->m_pSpheres[index].m_fRadius);
        if (IsPointInsideGarage(*point, sphereRadius))
            return false;
        ++index;
    } while (index < static_cast<int16>(data->m_nNumSpheres));
    return true;
}

// 0x44A830
// Como el original (0x44A830..0x44A9B1): hace falta FindPlayerVehicle(-1, false) (el EXE lo vuelve a pedir en cada comprobacion), de tipo coche o moto (m_nVehicleType 0 o 9), sin tarea
// activa de tipo 0x2C0 en el jugador, con la posicion dentro del rectangulo del garaje (solo fallan las comparaciones ordenadas: izq/frente por debajo, der/fondo por encima) y casi
// quieto (Garage63StaticSpeedCpp: |v| por eje no mayor que 0.01f y |v|^2 no mayor que 0.0001f con el x87); el resultado es IsEntityEntirelyInside3D(vehiculo, 0.0f).
bool CGarage::IsStaticPlayerCarEntirelyInside() {
    if (!FindPlayerVehicle(-1, false))
        return false;
    if (FindPlayerVehicle(-1, false)->m_nVehicleType != VEHICLE_TYPE_AUTOMOBILE) {
        if (FindPlayerVehicle(-1, false)->m_nVehicleType != VEHICLE_TYPE_BIKE)
            return false;
    }
    if (FindPlayerPed(-1)->GetTaskManager().FindActiveTaskByType(static_cast<eTaskType>(0x2C0)))
        return false;

    CVehicle* const vehicle = FindPlayerVehicle(-1, false);
    const CVector&  position = vehicle->GetPosition();
    if (position.x < m_fLeftCoord)
        return false;
    if (position.x > m_fRightCoord)
        return false;
    if (position.y < m_fFrontCoord)
        return false;
    if (position.y > m_fBackCoord)
        return false;
    if (!Garage63StaticSpeedCpp(&vehicle->m_vecMoveSpeed.x))
        return false;
    return IsEntityEntirelyInside3D(vehicle, 0.0f);
}

// 0x448BE0
// Como el original (0x448BE0..0x448D2A): la posicion debe estar dentro de la caja del garaje ampliada en `radius` (los seis limites, mismo x87 y comparaciones que IsEntityTouching3D:
// GarageEntityNearCpp; con NaN pasan) y, si hay datos de colision con esferas (contador int16 releido en cada vuelta), cada esfera debe cumplir
// IsPointInsideGarage(centro transformado con GetMatrix + 0x59C890, radius - radio de la esfera) (resta del x87 guardada como float).
bool CGarage::IsEntityEntirelyInside3D(CEntity* entity, float radius) {
    if (!GarageEntityNearCpp(reinterpret_cast<const float*>(this), radius, &entity->GetPosition().x))
        return false;

    CCollisionData* const data = entity->GetColModel()->m_pColData;
    if (!data || static_cast<int16>(data->m_nNumSpheres) <= 0)
        return true;

    int16 index = 0;
    do {
        CColSphere* const sphere = &data->m_pSpheres[index];
        CMatrix&          matrix = entity->GetMatrix();
        CVector           transformed;
        CVector* const    point = plugin::CallAndReturn<CVector*, 0x59C890, CVector*, CMatrix*, CVector*>(&transformed, &matrix, &sphere->m_vecCenter);
        const float       sphereRadius = GarageDiffCpp(radius, data->m_pSpheres[index].m_fRadius);
        if (!IsPointInsideGarage(*point, sphereRadius))
            return false;
        ++index;
    } while (index < static_cast<int16>(data->m_nNumSpheres));
    return true;
}

// 0x448740
// Version de la DLL (las llamadas desde C++ no leen el residuo de registros); el hook del EXE es GarageIsPointInsideRegs.
__declspec(noinline) bool CGarage::IsPointInsideGarage(CVector point) {
    uint32 ecxUnused = 0;
    return GarageInsidePointCpp(reinterpret_cast<const float*>(this), &point.x, &ecxUnused);
}

// 0x4486C0
// Como el original (0x4486C0..0x4486FB; no devuelve nada): segun m_nType (indice = tipo - 1 sin signo, solo 0..0x2C), en los tipos 2..5 y 13 la puerta pasa a "abriendo" (3) si estaba en
// 0, 2 o 3; en los demas tipos 1..45 pasa a "cerrando" (2) si estaba en 1, 2 o 3. Con otros tipos no hace nada.
void CGarage::PlayerArrestedOrDied() {
    const uint32 index = static_cast<uint32>(m_nType) - 1u;
    if (index > 0x2C)
        return;
    const uint8 state = static_cast<uint8>(m_nDoorState);
    if ((index >= 1 && index <= 4) || index == 12) {
        if (state == 0 || (state > 1 && state <= 3))
            m_nDoorState = static_cast<eGarageDoorState>(3);
    } else {
        if (state != 0 && state <= 3)
            m_nDoorState = static_cast<eGarageDoorState>(2);
    }
}

// 0x447D50
void CGarage::OpenThisGarage() {
  if ( m_nDoorState == GARAGE_DOOR_CLOSED
    || m_nDoorState == GARAGE_DOOR_CLOSING
    || m_nDoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR)
  {
    m_nDoorState = GARAGE_DOOR_OPENING;
  }
}

// 0x447D70
void CGarage::CloseThisGarage() {
    if (m_nDoorState == GARAGE_DOOR_OPEN || m_nDoorState == GARAGE_DOOR_OPENING)
        m_nDoorState = GARAGE_DOOR_CLOSING;
}

// 0x447600
// Como el original (0x447600..0x44763C): m_nFlags = (m_nFlags & 0x39) | 0x40, estado 0 y m_nTimeToOpen 0; despues, con indice = tipo - 1 (sin signo, solo 0..0x2C): los tipos 2..5
// ponen estado 1 y posicion de puerta 1.0f, el tipo 13 no toca nada mas y los demas ponen estado 0 y posicion 0.0f. Con otros tipos (0 o mayores de 45) solo quedan los tres primeros cambios.
void CGarage::InitDoorsAtStart() {
    m_nFlags = static_cast<uint8>((m_nFlags & 0x39) | 0x40);
    m_nDoorState = GARAGE_DOOR_CLOSED;
    m_nTimeToOpen = 0;
    const uint32 index = static_cast<uint32>(m_nType) - 1u;
    if (index > 0x2C)
        return;
    if (index >= 1 && index <= 4) {
        m_nDoorState = GARAGE_DOOR_OPEN;
        m_fDoorPosition = 1.0f;
    } else if (index != 12) {
        m_nDoorState = GARAGE_DOOR_CLOSED;
        m_fDoorPosition = 0.0f;
    }
}

// 0x4487D0
// Version de la DLL; el hook del EXE es GarageIsPointInsideRadiusRegs.
__declspec(noinline) bool CGarage::IsPointInsideGarage(CVector point, float radius) {
    return GarageInsideRadiusCpp(reinterpret_cast<const float*>(this), &point.x, radius);
}

// 0x44AA50
void CGarage::Update(int32 garageId) {
    plugin::CallMethod<0x44AA50, CGarage*>(this, garageId);
}

bool CGarage::IsHideOut() const {
    switch (m_nType) {
    case eGarageType::SAFEHOUSE_GANTON:
    case eGarageType::SAFEHOUSE_SANTAMARIA:
    case eGarageType::SAGEHOUSE_ROCKSHORE:
    case eGarageType::SAFEHOUSE_FORTCARSON:
    case eGarageType::SAFEHOUSE_VERDANTMEADOWS:
    case eGarageType::SAFEHOUSE_DILLIMORE:
    case eGarageType::SAFEHOUSE_PRICKLEPINE:
    case eGarageType::SAFEHOUSE_WHITEWOOD:
    case eGarageType::SAFEHOUSE_PALOMINOCREEK:
    case eGarageType::SAFEHOUSE_REDSANDSWEST:
    case eGarageType::SAFEHOUSE_ELCORONA:
    case eGarageType::SAFEHOUSE_MULHOLLAND:
    case eGarageType::SAFEHOUSE_CALTONHEIGHTS:
    case eGarageType::SAFEHOUSE_PARADISO:
    case eGarageType::SAFEHOUSE_DOHERTY:
    case eGarageType::SAFEHOUSE_HASHBURY:
    case eGarageType::HANGAR_ABANDONED_AIRPORT:
        return true;
    default:
        return false;
    }
}

// 0x44A9C0
// Como el original (0x44A9C0..0x44AA4C): esquinas A = (izquierda, frente, z) y B = (derecha, fondo, topZ) en la pila (izquierda y derecha se copian como enteros, las demas pasan por
// el x87), CWorld::FindObjectsIntersectingCube(A, B, &cuenta, 16, lista, false, true, true, false, false) y el garaje esta vacio si ninguna entidad de la lista (contador int16, de la
// primera a la ultima) toca el garaje segun IsEntityTouching3D; sin entidades (contador <= 0) esta vacio.
bool CGarage::IsGarageEmpty() {
    const CVector cornerA = { m_fLeftCoord, m_fFrontCoord, m_vPosn.z };
    const CVector cornerB = { m_fRightCoord, m_fBackCoord, m_fTopZ };

    int16    outCount[2];
    CEntity* outEntities[16];
    CWorld::FindObjectsIntersectingCube(cornerA, cornerB, outCount, static_cast<int16>(std::size(outEntities)), outEntities, false, true, true, false, false);
    if (outCount[0] <= 0)
        return true;

    int16 entityIndex = 0;
    do {
        if (IsEntityTouching3D(outEntities[entityIndex]))
            return false;
        ++entityIndex;
    } while (entityIndex < outCount[0]);
    return true;
}

/*
void CGarage::CenterCarInGarage(CEntity* entity) {
    auto vehicle = FindPlayerVehicle();
    if (IsAnyOtherCarTouchingGarage(vehicle))
        return;

    auto player = FindPlayerPed();
    if (IsAnyOtherPedTouchingGarage(player))
        return;

    auto pos = entity->GetPosition();

    const auto halfX = (m_fRightCoord + m_fLeftCoord) * 0.5f;
    const auto halfY = (m_fBackCoord + m_fFrontCoord) * 0.5f;
    CVector p1{
        halfX - pos.x,
        halfY - pos.y,
        pos.z - pos.z
    };

    auto dist = p1.Magnitude();
    if (dist >= 0.4f) {
        auto x = halfX - pos.x * 0.4f / dist + pos.x;
        auto y = 0.4f / dist * halfY - pos.y + pos.y;
    } else {
        auto x = halfX;
        auto y = halfY;
    }

    if (!IsEntityEntirelyInside3D(entity, 0.3f))
        entity->SetPosn(entity->GetPosition());
}
*/

// 0x5D3020
void CSaveGarage::CopyGarageIntoSaveGarage(Const CGarage& g) {
    m_nType         = g.m_nType;
    m_nDoorState    = g.m_nDoorState;
    m_nFlags        = g.m_nFlags;
    m_vPosn         = g.m_vPosn;
    m_vDirectionA   = g.m_vDirectionA;
    m_vDirectionB   = g.m_vDirectionB;
    m_fTopZ         = g.m_fTopZ;
    m_fWidth        = g.m_fWidth;
    m_fHeight       = g.m_fHeight;
    m_fLeftCoord    = g.m_fLeftCoord;
    m_fRightCoord   = g.m_fRightCoord;
    m_fFrontCoord   = g.m_fFrontCoord;
    m_fBackCoord    = g.m_fBackCoord;
    m_fDoorPosition = g.m_fDoorPosition;
    m_nTimeToOpen   = g.m_nTimeToOpen;
    m_nOriginalType = g.m_nOriginalType;
    strcpy(m_anName, g.m_anName); // Como el original: strcpy (sin relleno 0xFE de depuración)
}

// 0x5D30C0
void CSaveGarage::CopyGarageOutOfSaveGarage(CGarage& g) const {
    g.m_nType         = m_nType;
    g.m_nDoorState    = m_nDoorState;
    g.m_nFlags        = m_nFlags;
    g.m_vPosn         = m_vPosn;
    g.m_vDirectionA   = m_vDirectionA;
    g.m_vDirectionB   = m_vDirectionB;
    g.m_fTopZ         = m_fTopZ;
    g.m_fWidth        = m_fWidth;
    g.m_fHeight       = m_fHeight;
    g.m_fLeftCoord    = m_fLeftCoord;
    g.m_fRightCoord   = m_fRightCoord;
    g.m_fFrontCoord   = m_fFrontCoord;
    g.m_fBackCoord    = m_fBackCoord;
    g.m_fDoorPosition = m_fDoorPosition;
    g.m_nTimeToOpen   = m_nTimeToOpen;
    g.m_nOriginalType = m_nOriginalType;
    g.m_pTargetCar    = nullptr;
    strcpy(g.m_anName, m_anName); // Como el original: strcpy (sin relleno 0xFE de depuración)
}

// todo move
// 0x449760
void CStoredCar::StoreCar(CVehicle* vehicle) {
    plugin::CallMethod<0x449760, CStoredCar*, CVehicle*>(this, vehicle);
}

// 0x447E40
CVehicle* CStoredCar::RestoreCar() {
    return plugin::CallMethodAndReturn<CVehicle*, 0x447E40, CStoredCar*>(this);
}
