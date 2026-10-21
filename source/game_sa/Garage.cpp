/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGarage
 * Reconstrucción: COMPLETA — 18/18 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 18/18 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Garage.h"
#include "Garages.h"
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
    RH_ScopedInstall(Update, 0x44AA50);
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
__declspec(noinline) void CGarage::StoreAndRemoveCarsForThisHideOut(CStoredCar* storedCars, int32 maxSlot) {
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
__declspec(noinline) void CGarage::RemoveCarsBlockingDoorNotInside() {
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
__declspec(noinline) bool CGarage::IsEntityEntirelyOutside(CEntity* entity, float radius) {
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
__declspec(noinline) bool CGarage::IsStaticPlayerCarEntirelyInside() {
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
__declspec(noinline) bool CGarage::IsEntityEntirelyInside3D(CEntity* entity, float radius) {
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

namespace {
// Acceso por desplazamiento a campos que el EXE lee/escribe sin nombre en las cabeceras (ver comentarios de CGarage::Update).
template<class T> inline T& GaField(const void* base, uint32 offset) {
    return *reinterpret_cast<T*>(const_cast<uint8*>(reinterpret_cast<const uint8*>(base)) + offset);
}
inline const char* GaKey(uint32 address) { return reinterpret_cast<const char*>(address); }   // clave GXT del EXE
// Posicion de la entidad como el EXE: matriz + 0x30 si existe, si no m_placement (+4).
inline const float* GaPos(CEntity* entity) {
    return entity->m_matrix ? &entity->m_matrix->GetPosition().x : &entity->m_placement.m_vPosn.x;
}
inline CVehicle* GaFpv() { return FindPlayerVehicle(-1, false); }
inline CPed* GaFpp() { return FindPlayerPed(-1); }
// Metodos de CGarage que siguen siendo del EXE (sin definicion en C++): colaboradores por direccion.
inline bool  GaSlideDoorOpen(CGarage* g) { return plugin::CallMethodAndReturn<bool, 0x44A660, CGarage*>(g); }
inline bool  GaSlideDoorClosed(CGarage* g) { return plugin::CallMethodAndReturn<bool, 0x44A750, CGarage*>(g); }
inline bool  GaIsPlayerOutside(CGarage* g, float radius) { return plugin::CallMethodAndReturn<bool, 0x448E50, CGarage*, float>(g, radius); }
inline bool  GaIsAnyCarBlockingDoor(CGarage* g) { return plugin::CallMethodAndReturn<bool, 0x4494F0, CGarage*>(g); }
inline int32 GaCountCarsCenterWithin(CGarage* g, CVehicle* ignored) { return plugin::CallMethodAndReturn<int32, 0x4495F0, CGarage*, CVehicle*>(g, ignored); }
inline bool  GaIsAnyOtherPedTouching(CGarage* g, CPed* ignored) { return plugin::CallMethodAndReturn<bool, 0x449100, CGarage*, CPed*>(g, ignored); }
inline bool  GaIsAnyOtherCarTouching(CGarage* g, CVehicle* ignored) { return plugin::CallMethodAndReturn<bool, 0x447720, CGarage*, CVehicle*>(g, ignored); }
inline void  GaThrowCarsNearDoorOut(CGarage* g, CVehicle* ignored) { plugin::CallMethod<0x449220, CGarage*, CVehicle*>(g, ignored); }
inline float GaCalcDistToRectSq(CGarage* g, float x, float y) { return plugin::CallMethodAndReturn<float, 0x447D80, CGarage*, float, float>(g, x, y); }
inline bool  GaRestoreHideOut(CGarage* g, CStoredCar* cars) { return plugin::CallMethodAndReturn<bool, 0x448550, CGarage*, CStoredCar*>(g, cars); }
inline void  GaNeatlyLineUp(CGarage* g, CStoredCar* cars) { plugin::CallMethod<0x448330, CGarage*, CStoredCar*>(g, cars); }
inline bool  GaRestoreImpounding(CGarage* g, CStoredCar* cars) { return plugin::CallMethodAndReturn<bool, 0x4485C0, CGarage*, CStoredCar*>(g, cars); }
inline void  GaStoreAndRemoveImpounding(CGarage* g, CStoredCar* cars, int32 maxSlot) { plugin::CallMethod<0x449A50, CGarage*, CStoredCar*, int32>(g, cars, maxSlot); }
inline void  GaFindDoorsWithGarage(CGarage* g, CObject** first, CObject** second) { plugin::CallMethod<0x449FF0, CGarage*, CObject**, CObject**>(g, first, second); }
inline int32& GaMoney() { return *reinterpret_cast<int32*>(0xB7CE50 + static_cast<uint32>(CWorld::PlayerInFocus) * 0x190u); }   // CPlayerInfo::m_nMoney del jugador en foco
inline CStoredCar* GaStoredCars(eGarageType type) { return reinterpret_cast<CStoredCar*>(0x96ABD8 + static_cast<uint32>(CGarages::FindSafeHouseIndexForGarageType(type)) * 0x100u); }
inline void GaAudio(uint32 event) { AudioEngine.ReportFrontendAudioEvent(static_cast<eAudioEvents>(event), 0.0f, 1.0f); }
inline void GaMessage(uint32 key) { CGarages::TriggerMessage(GaKey(key), -1, 0xFA0, -1); }
// CPad::GetPad(0) + 0x10E: bit 2 desactiva los controles del jugador.
inline void GaPadControls(bool disable) {
    uint8& flags = GaField<uint8>(CPad::GetPad(0), 0x10E);
    flags = disable ? static_cast<uint8>(flags | 4) : static_cast<uint8>(flags & 0xFB);
}
// CWanted + 0x1E: bit 1 (policia sin perseguir mientras el jugador esta en el garaje).
inline void GaWantedBit(bool set) {
    uint8& flags = GaField<uint8>(FindPlayerWanted(-1), 0x1E);
    flags = set ? static_cast<uint8>(flags | 2) : static_cast<uint8>(flags & 0xFD);
}
}

// 0x44AA50
// Como el original (0x44AA50..0x44C8BF), instruccion por instruccion: el cuerpo conserva la estructura de saltos del EXE (etiquetas L_xxxx con la direccion del EXE) porque muchas ramas
// comparten colas (L_C7C4 = salida, L_B3C1 = aviso a la policia del area, L_BCBE = abrir la puerta...). Orden y numero de llamadas a FindPlayerVehicle/FindPlayerPed/FindPlayerCoors,
// como las del EXE (cada una es una llamada distinta). Los campos sin nombre en las cabeceras se leen por desplazamiento (GaField): ped +0x46C/+0x58C, vehiculo +0x22 (modelo),
// +0x42E/+0x42F (banderas), +0x434..+0x437 (colores), +0x4A8 (bomba, bits 0..2), +0x4B0, +0x4C0 (salud), +0x4CC (propietario de la bomba), +0x590/+0x594 (tipos), +0x7BC, +0x868, +0x8E4.
void CGarage::Update(int32 garageId) {
    CVehicle* vehicle;
    CVehicle* vehicleB;
    CEntity*  subject;
    CVector   c1, c2, c3, c4;
    float     dist;
    bool      flag, repainted, hasWanted, fixedCar;
    uint32    n;
    CObject*  door1;
    CObject*  door2;
    const float *pa, *pb;

    // ---- 0x44AA50: el jugador (o su vehiculo) dentro del garaje -----------------------------------------------------------------------------------------------------------
    if (m_nType == 13)
        goto L_ABB6;
    if (static_cast<uint8>(m_nDoorState) > 5)
        goto L_ABB6;
    if (!GaFpp())
        goto L_ABB6;
    if (m_nFlags & 0x20)
        goto L_ABB6;
    vehicle = GaFpv();
    subject = GaFpp();
    if (GaField<uint32>(GaFpp(), 0x46C) & 0x100) {
        if (GaField<CVehicle*>(GaFpp(), 0x58C)) {
            if (GaField<uint16>(GaField<CVehicle*>(GaFpp(), 0x58C), 0x22) == 0x23B)
                subject = GaField<CEntity*>(GaFpp(), 0x58C);
        }
    }
    if (IsEntityEntirelyInside3D(subject, 0.25f)) {
        CGarages::bCamShouldBeOutside = true;
        StaticRef<CGarage*>(0xB6F844) = this;
    }
    if (vehicle) {
        if (!IsEntityEntirelyOutside(vehicle, 0.0f))
            StaticRef<CGarage*>(0xB6F848) = this;
        if (GaField<uint16>(vehicle, 0x22) == 0x1A7) {
            const float* const p = GaPos(vehicle);
            const float         pos[3]{ p[0], p[1], p[2] };
            if (Garage63InsideRectStrictCpp(reinterpret_cast<const float*>(this), 0.5f, pos)) {
                CGarages::bCamShouldBeOutside = true;
                StaticRef<CGarage*>(0xB6F844) = this;
            }
        }
    }

    // ---- 0x44ABB6: banderas de la puerta y salto por tipo de garaje -------------------------------------------------------------------------------------------------------
L_ABB6:
    if ((m_nFlags & 2) && m_nDoorState == 0)
        goto L_C7C4;
    if (m_nFlags & 8) {
        uint8 flags = m_nFlags;
        if ((m_nDoorState == 3 && m_fDoorPosition > 0.4f) || m_nDoorState == 1)
            flags = static_cast<uint8>(flags & 0xBF);
        else
            flags = static_cast<uint8>(flags | 0x40);
        m_nFlags = flags;
    }
    {
        const uint32 index = static_cast<uint32>(static_cast<uint8>(m_nType)) - 1u;
        if (index > 0x2C)
            goto L_C7C4;
        // tabla de 45 bytes del EXE (0x44C80C): tipo - 1 -> caso
        static constexpr uint8 kCaseOfType[45]{ 0, 1, 1, 1, 2, 13, 13, 13, 13, 13, 3, 13, 13, 4, 5, 6, 6, 6, 7, 8, 4, 13, 9, 6, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 11, 11, 11, 6, 6, 6, 6, 12, 6, 6 };
        switch (kCaseOfType[index]) {
        case 0:  goto L_B7EE;
        case 1:  goto L_B419;
        case 2:  goto L_AC16;
        case 3:  goto L_B9FF;
        case 4:  goto L_BA5C;
        case 5:  goto L_BCA4;
        case 6:  goto L_BCDE;
        case 7:  goto L_BCAA;
        case 8:  goto L_C1FF;
        case 9:  goto L_C2F3;
        case 10: goto L_C395;
        case 11: goto L_C564;
        case 12: goto L_C70A;
        default: goto L_C7C4;
        }
    }

    // ---- tipo 5 (PAYNSPRAY): 0x44AC16 ----------------------------------------------------------------------------------------------------------------------------------------
L_AC16:
    c1 = FindPlayerCoors(-1);
    if (!(c1.z < 950.0f))
        goto L_C7C4;
    switch (static_cast<uint8>(m_nDoorState)) {
    case 0:  goto L_AEC0;
    case 1:  goto L_AC50;
    case 2:  goto L_AE0C;
    case 3:  goto L_B79C;
    case 4:  goto L_B40C;
    default: goto L_C7C4;
    }

L_AC50:
    if (CGarages::NoResprays)
        goto L_C7C4;
    if (IsStaticPlayerCarEntirelyInside()) {
        vehicle = GaFpv();
        if (CGarages::IsCarSprayable(vehicle)) {
            if (GaMoney() < 100 && !CGarages::RespraysAreFree) {
                GaMessage(0x859A58);
                m_nDoorState = static_cast<eGarageDoorState>(4);
                GaAudio(0xE);
                goto L_AD2D;
            }
            m_nDoorState = static_cast<eGarageDoorState>(2);
            GaPadControls(true);
            GaField<uint32>(GaFpv(), 0x4B0) = 0;
            goto L_AD2D;
        }
        vehicle = GaFpv();
        GaMessage(GaField<int32>(vehicle, 0x594) == 0xA ? 0x859A50u : 0x859A48u);
        m_nDoorState = static_cast<eGarageDoorState>(4);
        GaAudio(0xF);
        goto L_AD2D;
    }
    if (GaIsPlayerOutside(this, 0.0f)) {
        if (garageId == CGarages::LastGaragePlayerWasIn)
            GaWantedBit(false);
        goto L_ADA3;
    }
    GaWantedBit(true);
    CGarages::LastGaragePlayerWasIn = garageId;
    goto L_ADA3;
L_AD2D:
    GaWantedBit(true);
    CGarages::LastGaragePlayerWasIn = garageId;
L_ADA3:
    if (!GaFpv())
        goto L_C7C4;
    vehicle  = GaFpv();
    pa       = GaPos(vehicle);
    vehicleB = GaFpv();
    pb       = GaPos(vehicleB);
    dist     = GaCalcDistToRectSq(this, pb[0], pa[1]);
    if (!(dist < 64.0f))
        goto L_C7C4;
    goto L_B3C1;

L_AE0C:   // PAYNSPRAY, estado 2 (cerrando)
    if (GaFpv()) {
        vehicle = GaFpv();
        GaThrowCarsNearDoorOut(this, vehicle);
    }
    if (GaSlideDoorClosed(this)) {
        m_nDoorState = static_cast<eGarageDoorState>(0);
        GaAudio(0x10);
        m_nTimeToOpen = CTimer::GetTimeInMS() + 0x7D0;
        CStats::IncrementStat(static_cast<eStats>(0xB1), CStats::GetStatValue(static_cast<eStats>(0xB0)));
        CStats::SetStatValue(static_cast<eStats>(0xB0), 0.0f);
    }
    if (!GaFpv())
        goto L_B3C1;
    GaField<uint32>(GaFpv(), 0x8E4) = 0;
    {
        uint8& byte42E = GaField<uint8>(GaFpv(), 0x42E);
        byte42E = static_cast<uint8>(byte42E | 0x80);
    }
    goto L_B3C1;

L_AEC0:   // PAYNSPRAY, estado 0 (abierto: espera al coche)
    if (CGarages::NoResprays)
        goto L_C7C4;
    if (!(CTimer::GetTimeInMS() > m_nTimeToOpen))
        goto L_B3C1;
    m_nDoorState = static_cast<eGarageDoorState>(3);
    fixedCar  = false;
    hasWanted = GaField<uint32>(FindPlayerWanted(-1), 0x2C) != 0;
    if (hasWanted) {
        fixedCar = true;
        FindPlayerWanted(-1)->ClearWantedLevelAndGoOnParole();
    }
    if (!GaFpv())
        goto L_B2DA;
    if (GaField<int32>(GaFpv(), 0x590) != 0) {
        if (GaField<int32>(GaFpv(), 0x590) != 9)
            goto L_B2DA;
    }
    if (GaField<float>(GaFpv(), 0x4C0) < 970.0f)
        fixedCar = true;
    {
        const float health = GaField<float>(GaFpv(), 0x4C0);
        const float maxed  = (1000.0f > health) ? 1000.0f : health;   // 0x420800
        GaField<float>(GaFpv(), 0x4C0) = maxed;
    }
    if (GaField<int32>(GaFpv(), 0x590) == 0)
        GaField<uint32>(GaFpv(), 0x8E4) = 0;
    else
        GaField<uint32>(GaFpv(), 0x7BC) = 0;
    GaFpv()->Fix();
    CStats::IncrementStat(static_cast<eStats>(0x8A), 1.0f);
    if (GaField<CMatrix*>(GaFpv(), 0x14)->GetUp().z < 0.0f) {
        // da la vuelta al coche: niega los ejes (+0x20, +0x24, +0x28) y (+0, +4, +8) de la matriz, cada componente con dos FindPlayerVehicle
        static constexpr uint32 kOffsets[6]{ 0x20, 0x24, 0x28, 0x00, 0x04, 0x08 };
        for (const uint32 offset : kOffsets) {
            CMatrix* const source = GaField<CMatrix*>(GaFpv(), 0x14);
            CMatrix* const target = GaField<CMatrix*>(GaFpv(), 0x14);
            GaField<float>(target, offset) = -GaField<float>(source, offset);
        }
    }
    vehicle   = GaFpv();
    repainted = false;
    if (!(GaField<uint8>(vehicle, 0x868) & 2) && vehicle->GetRemapIndex() < 0) {
        uint8 prim, sec, tert, quat;
        reinterpret_cast<CVehicleModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(vehicle->m_nModelIndex)])->ChooseVehicleColour(prim, sec, tert, quat, 1);
        if (GaField<uint8>(vehicle, 0x434) != prim || GaField<uint8>(vehicle, 0x435) != sec || GaField<uint8>(vehicle, 0x436) != tert || GaField<uint8>(vehicle, 0x437) != quat)
            repainted = true;
        GaField<uint8>(vehicle, 0x434) = prim;
        GaField<uint8>(vehicle, 0x435) = sec;
        GaField<uint8>(vehicle, 0x436) = tert;
        GaField<uint8>(vehicle, 0x437) = quat;
        vehicle->SetRemap(-1);
        if (repainted) {
            FxPrtMult_c mult{ 1.0f, 0.0f, 0.0f, 0.6f, 0.7f, 1.0f, 0.4f };
            const uint32 color = reinterpret_cast<const uint32*>(0xB4E480)[GaField<uint8>(vehicle, 0x434)];
            constexpr float kInv255 = std::bit_cast<float>(0x3B808081u);   // 0x859A3C
            mult.m_Color.red   = static_cast<float>(color & 0xFF) * kInv255;
            mult.m_Color.green = static_cast<float>((color >> 8) & 0xFF) * kInv255;
            mult.m_Color.blue  = static_cast<float>((color >> 16) & 0xFF) * kInv255;
            const CVector velocityBase{ 0.0f, 0.0f, 0.0f };
            for (n = 0; n < 10; ++n) {
                const float* const p = GaPos(vehicle);
                CVector position{ p[0], p[1], p[2] };
                CVector velocity = velocityBase;
                position.x = RpNanAddCpp(CGeneral::GetRandomNumberInRange(-3.0f, 3.0f), position.x);
                position.y = RpNanAddCpp(CGeneral::GetRandomNumberInRange(-3.0f, 3.0f), position.y);
                velocity.z = CGeneral::GetRandomNumberInRange(0.0f, 0.05f);
                StaticRef<FxSystem_c*>(0xA9AE20)->AddParticle(position, velocity, 0.0f, mult, -1.0f, 1.2f, 0.6f, false);
            }
        }
    }
    GaField<uint8>(vehicle, 0x42E) = static_cast<uint8>(GaField<uint8>(vehicle, 0x42E) & 0x7F);
    GaField<uint32>(vehicle, 0x4B0) = 0;
    flag = repainted;
    goto L_B2DE;
L_B2DA:
    flag = hasWanted;
L_B2DE:
    if (m_nFlags & 0x80) {
        GaMessage(0x859A34);
        goto L_B38D;
    }
    if (fixedCar && !CGarages::RespraysAreFree) {
        if (GaMoney() > 0) {
            int32 money = GaMoney() - 100;
            if (money < 0)
                money = 0;
            GaMoney() = money;
        }
        CStats::IncrementStat(static_cast<eStats>(0x10), 100.0f);
        GaMessage(hasWanted ? 0x859A2Cu : 0x859A24u);
        goto L_B38D;
    }
    if (flag) {
        GaMessage((plugin::CallAndReturn<int32, 0x821B1E>() & 1) ? 0x859A1Cu : 0x859A14u);
    }
L_B38D:
    m_nFlags = static_cast<uint8>(m_nFlags | 4);
    if (GaFpv()) {
        uint8& byte42F = GaField<uint8>(GaFpv(), 0x42F);
        byte42F = static_cast<uint8>(byte42F | 1);
    }
L_B3C1:
    CWorld::CallOffChaseForArea(GarageDiffCpp(m_fLeftCoord, 10.0f), GarageDiffCpp(m_fFrontCoord, 10.0f), RpNanAddCpp(m_fRightCoord, 10.0f), RpNanAddCpp(m_fBackCoord, 10.0f));
    goto L_C7C4;

L_B79C:   // estado 3 (abriendo)
    if (GaSlideDoorOpen(this))
        m_nDoorState = static_cast<eGarageDoorState>(4);
    if (!(m_fDoorPosition > 0.5f))
        goto L_C7C4;
    GaPadControls(false);
    GaWantedBit(false);
    goto L_C7C4;

L_B40C:   // estado 4 (esperando que salga el jugador)
    if (!GaIsPlayerOutside(this, 0.0f))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(1);
    goto L_C7C4;

    // ---- tipos 2, 3 y 4 (tiendas de bombas): 0x44B419 -------------------------------------------------------------------------------------------------------------------------
L_B419:
    switch (static_cast<uint8>(m_nDoorState)) {
    case 0:  goto L_B5CA;
    case 1:  goto L_B42D;
    case 2:  goto L_B564;
    case 3:  goto L_B79C;
    case 4:  goto L_B40C;
    default: goto L_C7C4;
    }

L_B42D:
    if (!IsStaticPlayerCarEntirelyInside())
        goto L_C7C4;
    if (!GaFpv())
        goto L_C7C4;
    if (GaField<int32>(GaFpv(), 0x594) == 9)
        goto L_C7C4;
    if (GaField<int32>(GaFpv(), 0x594) == 0xA)
        goto L_C7C4;
    if (GaField<uint8>(GaFpv(), 0x4A8) & 7) {
        GaMessage(0x859A04);
        m_nDoorState = static_cast<eGarageDoorState>(4);
        GaAudio(0x11);
        goto L_C7C4;
    }
    if (!CGarages::BombsAreFree && GaMoney() < 500) {
        GaMessage(0x859A0C);
        m_nDoorState = static_cast<eGarageDoorState>(4);
        GaAudio(0xE);
        goto L_C7C4;
    }
    m_nDoorState = static_cast<eGarageDoorState>(2);
    GaPadControls(true);
    GaWantedBit(true);
    goto L_C7C4;

L_B564:   // estado 2
    if (GaFpv()) {
        vehicle = GaFpv();
        GaThrowCarsNearDoorOut(this, vehicle);
    }
    if (GaSlideDoorClosed(this)) {
        m_nDoorState = static_cast<eGarageDoorState>(0);
        m_nTimeToOpen = CTimer::GetTimeInMS() + 0x7D0;
    }
    if (m_nType != 4)
        goto L_C7C4;
L_B5AE:
    CStreaming::RequestModel(0x16C, 2);
    goto L_C7C4;

L_B5CA:   // estado 0
    if (!(CTimer::GetTimeInMS() > m_nTimeToOpen))
        goto L_C7C4;
    if (m_nType == 4) {
        if (GaField<uint8>(reinterpret_cast<void*>(0x8E6940), 0) != 1)
            goto L_B5AE;
    }
    if (m_nType == 2)
        GaAudio(0x12);
    else if (m_nType == 3)
        GaAudio(0x13);
    else if (m_nType == 4)
        GaAudio(0x14);
    m_nDoorState = static_cast<eGarageDoorState>(3);
    if (!CGarages::BombsAreFree) {
        if (GaMoney() > 0) {
            int32 money = GaMoney() - 500;
            if (money < 0)
                money = 0;
            GaMoney() = money;
        }
    }
    if (GaFpv()) {
        if (GaField<int32>(GaFpv(), 0x590) == 0 || GaField<int32>(GaFpv(), 0x590) == 9) {
            vehicle = GaFpv();
            uint8& bomb = GaField<uint8>(vehicle, 0x4A8);
            const uint8 mixed = static_cast<uint8>((static_cast<uint8>(m_nType) - 1) ^ bomb);
            bomb = static_cast<uint8>(bomb ^ (mixed & 7));
            CPed* const owner = GaFpp();
            GaField<CPed*>(GaFpv(), 0x4CC) = owner;
            if (m_nType == 4)
                CGarages::GivePlayerDetonator();
            CStats::IncrementStat(static_cast<eStats>(0x7F), 10.0f);
        }
    }
    {
        const uint32 type = static_cast<uint8>(m_nType);
        uint32       key;
        if (type == 2 || type == 3) {
            const uint16 button = GaField<uint16>(CPad::GetPad(0), 0x10A);
            if (button <= 2)
                key = type == 2 ? 0x8599DCu : 0x8599ECu;
            else if (button == 3)
                key = type == 2 ? 0x8599E4u : 0x8599F4u;
            else
                goto L_C7C4;
        } else if (type == 4) {
            key = 0x8599FC;
        } else {
            goto L_C7C4;
        }
        CHud::SetHelpMessage(TheText.Get(GaKey(key)), false, false, true);
    }
    goto L_C7C4;

    // ---- tipo 1 (ONLY_TARGET_VEH): 0x44B7EE ------------------------------------------------------------------------------------------------------------------------------------
L_B7EE:
    switch (static_cast<uint8>(m_nDoorState)) {
    case 0:  goto L_B9AB;
    case 1:  goto L_B802;
    case 2:  goto L_B963;
    case 3:  goto L_BCBE;
    default: goto L_C7C4;
    }

L_B802:   // estado 1: el jugador se aleja del centro del garaje
    c1 = FindPlayerCoors(-1);
    c2 = FindPlayerCoors(-1);
    c3 = FindPlayerCoors(-1);
    c4 = FindPlayerCoors(-1);
    if (!(Garage63CenterDistSqCpp(reinterpret_cast<const float*>(this), c1.x, c2.x, c3.y, c4.y) > 900.0))
        goto L_B8BE;
    if (CTimer::GetFrameCounter() & 0x1F)
        goto L_C7C4;
    if (!m_pTargetCar)
        goto L_C662;
    if (IsEntityTouching3D(m_pTargetCar))
        goto L_C7C4;
    goto L_C662;
L_B8BE:
    {
        CVehicle* const target = m_pTargetCar;
        if (GaFpv() == target)
            goto L_C7C4;
        if (!target)
            goto L_C7C4;
        if (!IsEntityEntirelyInside3D(target, 0.0f))
            goto L_C7C4;
        CEntity* const who = GaFpv() ? static_cast<CEntity*>(GaFpv()) : static_cast<CEntity*>(GaFpp());
        if (!IsEntityEntirelyOutside(who, 2.0f))
            goto L_C7C4;
        GaPadControls(true);
        GaWantedBit(true);
        m_nFlags = static_cast<uint8>(m_nFlags & 0xFE);
        m_nDoorState = static_cast<eGarageDoorState>(2);
        goto L_C7C4;
    }
L_B963:   // estado 2: cerrando
    if (m_pTargetCar)
        GaThrowCarsNearDoorOut(this, m_pTargetCar);
    if (!GaSlideDoorClosed(this))
        goto L_C7C4;
    if (m_nFlags & 1)
        goto L_C6AC;
    if (m_pTargetCar) {
        m_nDoorState = static_cast<eGarageDoorState>(5);
        CVehicle::DestroyVehicleAndDriverAndPassengers(m_pTargetCar);
        m_pTargetCar = nullptr;
    } else {
        goto L_BBC8;
    }
    goto L_BBCC;
L_B9AB:   // estado 0: el jugador llega con el coche del objetivo
    {
        CVehicle* const target = m_pTargetCar;
        if (GaFpv() != target)
            goto L_C7C4;
        if (!target)
            goto L_C7C4;
    }
L_B9C9:
    vehicle  = GaFpv();
    pa       = GaPos(vehicle);
    vehicleB = GaFpv();
    pb       = GaPos(vehicleB);
    dist     = GaCalcDistToRectSq(this, pb[0], pa[1]);
    if (!(dist < 64.0f))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(3);
    goto L_C7C4;

    // ---- tipo 11 (UNKN_CLOSESONTOUCH): 0x44B9FF ---------------------------------------------------------------------------------------------------------------------------------
L_B9FF:
    switch (static_cast<uint8>(m_nDoorState)) {
    case 1:  goto L_BA3C;
    case 2:  goto L_BA0D;
    case 3:  goto L_BCBE;
    default: goto L_C7C4;
    }
L_BA0D:
    if (GaSlideDoorClosed(this))
        m_nDoorState = static_cast<eGarageDoorState>(0);
    if (IsGarageEmpty())
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(3);
    goto L_C7C4;
L_BA3C:
    if (!IsGarageEmpty())
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(2);
    goto L_C7C4;

    // ---- tipos 14 y 21: 0x44BA5C -------------------------------------------------------------------------------------------------------------------------------------------------
L_BA5C:
    switch (static_cast<uint8>(m_nDoorState)) {
    case 0:  goto L_BBFB;
    case 1:  goto L_BA70;
    case 2:  goto L_BB83;
    case 3:  goto L_BCBE;
    case 5:  goto L_BC7B;
    default: goto L_C7C4;
    }
L_BA70:
    c1 = FindPlayerCoors(-1);
    c2 = FindPlayerCoors(-1);
    c3 = FindPlayerCoors(-1);
    c4 = FindPlayerCoors(-1);
    if (Garage63CenterDistSqCpp(reinterpret_cast<const float*>(this), c1.x, c2.x, c3.y, c4.y) > 900.0)
        goto L_C662;
    if (!m_pTargetCar)
        goto L_C662;
    if (m_pTargetCar != GaFpv())
        goto L_C7C4;
    if (!IsStaticPlayerCarEntirelyInside())
        goto L_C7C4;
    if (GaIsAnyCarBlockingDoor(this))
        goto L_C7C4;
    GaPadControls(true);
    GaWantedBit(true);
    m_nFlags = static_cast<uint8>(m_nFlags & 0xFE);
    m_nDoorState = static_cast<eGarageDoorState>(2);
    goto L_C7C4;
L_BB83:
    if (m_pTargetCar)
        GaThrowCarsNearDoorOut(this, m_pTargetCar);
    if (!GaSlideDoorClosed(this))
        goto L_C7C4;
    if (m_nFlags & 1)
        goto L_C6AC;
    if (m_pTargetCar) {
        m_nDoorState = static_cast<eGarageDoorState>(5);
        m_nTimeToOpen = CTimer::GetTimeInMS() + 0x7D0;
        m_pTargetCar = nullptr;
    } else {
        goto L_BBC8;
    }
    goto L_BBCC;
L_BBC8:
    m_nDoorState = static_cast<eGarageDoorState>(0);
L_BBCC:
    GaPadControls(false);
    GaWantedBit(false);
    goto L_C7C4;
L_BBFB:
    {
        CVehicle* const target = m_pTargetCar;
        if (GaFpv() != target)
            goto L_C7C4;
        if (!target)
            goto L_C7C4;
    }
    vehicle  = GaFpv();
    pa       = GaPos(vehicle);
    vehicleB = GaFpv();
    pb       = GaPos(vehicleB);
    dist     = GaCalcDistToRectSq(this, pb[0], pa[1]);
    if (!(dist < 289.0f))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(3);
    goto L_C7C4;
L_BC7B:
    if (m_nType != 14)
        goto L_C7C4;
    if (!(CTimer::GetTimeInMS() > m_nTimeToOpen))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(3);
    goto L_C7C4;

    // ---- tipos 15 y 19 (script) ---------------------------------------------------------------------------------------------------------------------------------------------------
L_BCA4:
    if (m_nDoorState != 3)
        goto L_C7C4;
    goto L_BCBE;
L_BCAA:
    if (m_nDoorState == 2)
        goto L_C69D;
    if (m_nDoorState != 3)
        goto L_C7C4;
L_BCBE:
    if (!GaSlideDoorOpen(this))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(1);
    goto L_C7C4;

    // ---- refugios (tipos 16..18, 24..32, 39..42, 44, 45): 0x44BCDE ----------------------------------------------------------------------------------------------------------------
L_BCDE:
    switch (static_cast<uint8>(m_nDoorState)) {
    case 0:  goto L_BE56;
    case 1:  goto L_BCF2;
    case 2:  goto L_BDF1;
    case 3:  goto L_BCBE;
    default: goto L_C7C4;
    }
L_BCF2:
    c1 = FindPlayerCoors(-1);
    c2 = FindPlayerCoors(-1);
    dist = GaCalcDistToRectSq(this, c1.x, c2.y);
    if (dist > 225.0f) {
        // BD6A
        if (!GaIsAnyCarBlockingDoor(this))
            goto L_C7C0;
        goto L_BD79;
    }
    if (!(dist > 16.0f))
        goto L_BD79;
    if (GaFpv()) {
        if (GaField<int32>(GaFpv(), 0x594) != 0xA)
            goto L_BD79;
    }
    if (!GaIsAnyCarBlockingDoor(this))
        goto L_C7C0;
L_BD79:
    if (GaFpv()) {
        const int32 needed = (m_nType != 0x10 ? 1 : 0) * 2 + 2;
        if (GaCountCarsCenterWithin(this, GaFpv()) >= needed) {
            if (GaIsPlayerOutside(this, 0.25f))
                goto L_C7C0;
        }
    }
    if (!(dist > 4900.0f))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(2);
    RemoveCarsBlockingDoorNotInside();
    goto L_C7C4;
L_BDF1:
    GaSlideDoorClosed(this);
    if (!GaIsPlayerOutside(this, 0.0f))
        goto L_C6F9;
    if (!(m_fDoorPosition == 0.0f))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(0);
    if (m_nType == 0x2C)
        goto L_C7C4;
    StoreAndRemoveCarsForThisHideOut(GaStoredCars(m_nType), 4);
    goto L_C7C4;
L_BE56:
    c1 = FindPlayerCoors(-1);
    if (!(c1.z < 950.0f))
        goto L_C7C4;
    c2   = FindPlayerCoors(-1);
    c3   = FindPlayerCoors(-1);
    dist = GaCalcDistToRectSq(this, c2.x, c3.y);
    if (dist < 12.25f)
        goto L_BEF9;
    if (!(dist < 100.0f))
        goto L_C7C4;
    if (!GaFpv())
        goto L_C7C4;
    if (GaField<int32>(GaFpv(), 0x594) == 0xA)
        goto L_C7C4;
L_BEF9:
    if (!GaFpv())
        goto L_C1C3;
    if (m_nType == 0x2C)
        goto L_C1C3;
    {
        const int32 needed = (m_nType != 0x10 ? 1 : 0) * 2 + 2;
        if (CGarages::CountCarsInHideoutGarage(m_nType) < needed)
            goto L_C1C3;
    }
    GaFindDoorsWithGarage(this, &door1, &door2);
    if (!door1)
        goto L_C045;
    {
        // las cuatro lecturas de la posicion del jugador (una FindPlayerVehicle por lectura) alternan con las de la posicion de la puerta, como el EXE (0xBF54..0xC02C)
        const float* const d0 = GaPos(door1);
        const float* const v0 = GaPos(GaFpv());
        const float* const d1 = GaPos(door1);
        const float* const v1 = GaPos(GaFpv());
        const float* const d2 = GaPos(door1);
        const float* const v2 = GaPos(GaFpv());
        const float* const d3 = GaPos(door1);
        const float* const v3 = GaPos(GaFpv());
        if (Garage63DoorDistSqCpp(d0[0], v0[0], d1[0], v1[0], d2[1], v2[1], d3[1], v3[1]) < 25.0)
            goto L_C13E;
    }
L_C045:
    if (!door2)
        goto L_C7C4;
    {
        const float* const d0 = GaPos(door2);
        const float* const v0 = GaPos(GaFpv());
        const float* const d1 = GaPos(door2);
        const float* const v1 = GaPos(GaFpv());
        const float* const d2 = GaPos(door2);
        const float* const v2 = GaPos(GaFpv());
        const float* const d3 = GaPos(door2);
        const float* const v3 = GaPos(GaFpv());
        if (!(Garage63DoorDistSqCpp(d0[0], v0[0], d1[0], v1[0], d2[1], v2[1], d3[1], v3[1]) < 25.0))
            goto L_C7C4;
    }
L_C13E:
    if (!(CTimer::GetTimeInMS() - static_cast<uint32>(CGarages::LastTimeHelpMessage) > 0x4650))
        goto L_C7C4;
    if (GaFpv()->GetVehicleAppearance() == static_cast<eVehicleAppearance>(3))
        goto L_C7C4;
    if (GaFpv()->GetVehicleAppearance() == static_cast<eVehicleAppearance>(5))
        goto L_C7C4;
    CHud::SetHelpMessage(TheText.Get(GaKey(0x8599C0)), false, false, true);
    CGarages::LastTimeHelpMessage = CTimer::GetTimeInMS();
    goto L_C7C4;
L_C1C3:
    if (m_nType != 0x2C) {
        if (!GaRestoreHideOut(this, GaStoredCars(m_nType)))
            goto L_C7C4;
    }
    m_nDoorState = static_cast<eGarageDoorState>(3);
    goto L_C7C4;

    // ---- tipo 20 (STAY_OPEN_WITH_CAR_INSIDE): 0x44C1FF ----------------------------------------------------------------------------------------------------------------------------
L_C1FF:
    switch (static_cast<uint8>(m_nDoorState)) {
    case 0:  goto L_B9AB;
    case 1:  goto L_C213;
    case 2:  goto L_C2E3;
    case 3:  goto L_BCBE;
    default: goto L_C7C4;
    }
L_C213:
    c1 = FindPlayerCoors(-1);
    c2 = FindPlayerCoors(-1);
    c3 = FindPlayerCoors(-1);
    c4 = FindPlayerCoors(-1);
    if (!(Garage63CenterDistSqCpp(reinterpret_cast<const float*>(this), c1.x, c2.x, c3.y, c4.y) > 900.0))
        goto L_C7C4;
    if (!m_pTargetCar)
        goto L_C7C4;
    if (!IsEntityEntirelyOutside(m_pTargetCar, 0.0f))
        goto L_C7C4;
    m_nFlags = static_cast<uint8>(m_nFlags | 1);
    m_nDoorState = static_cast<eGarageDoorState>(2);
    goto L_C7C4;
L_C2E3:
    if (!m_pTargetCar)
        goto L_C69D;
    GaThrowCarsNearDoorOut(this, m_pTargetCar);
    goto L_C69D;

    // ---- tipo 23 (SCRIPT_OPEN_FREEZE_WHEN_CLOSING): 0x44C2F3 --------------------------------------------------------------------------------------------------------------------------
L_C2F3:
    if (m_nDoorState == 1)
        goto L_C331;
    if (m_nDoorState != 2) {
        if (m_nDoorState != 3)
            goto L_C7C4;
        goto L_BCBE;
    }
    if (!GaSlideDoorClosed(this))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(0);
    GaPadControls(false);
    goto L_C7C4;
L_C331:
    if (!m_pTargetCar)
        goto L_C7C4;
    if (!IsEntityEntirelyInside3D(m_pTargetCar, 0.0f))
        goto L_C7C4;
    if (GaIsAnyCarBlockingDoor(this))
        goto L_C7C4;
    if (!GaIsPlayerOutside(this, 0.0f))
        goto L_C7C4;
    GaPadControls(true);
    m_nFlags = static_cast<uint8>(m_nFlags & 0xFE);
    m_nDoorState = static_cast<eGarageDoorState>(2);
    goto L_C7C4;

    // ---- tipos 33..35 (garajes de embargo): 0x44C395 ---------------------------------------------------------------------------------------------------------------------------
L_C395:
    if (m_nDoorState == 0)
        goto L_C477;
    if (static_cast<uint8>(m_nDoorState) > 2)
        goto L_C7C4;
    c1   = FindPlayerCoors(-1);
    c2   = FindPlayerCoors(-1);
    dist = GaCalcDistToRectSq(this, c1.x, c2.y);
    c3   = FindPlayerCoors(-1);
    flag = false;
    if (Garage63TopMinus2GreaterCpp(m_fTopZ, c3.z)) {
        c4 = FindPlayerCoors(-1);
        if (c4.z > m_vPosn.z)
            flag = true;
    }
    if (!(dist > 4225.0f) && flag) {
        if (m_nDoorState != 2)
            goto L_C7C4;
    }
    m_nDoorState = static_cast<eGarageDoorState>(0);
    GaStoreAndRemoveImpounding(this, GaStoredCars(m_nType), 3);
    goto L_C7C4;
L_C477:
    c1   = FindPlayerCoors(-1);
    c2   = FindPlayerCoors(-1);
    dist = GaCalcDistToRectSq(this, c1.x, c2.y);
    c3   = FindPlayerCoors(-1);
    flag = false;
    if (Garage63TopMinus2GreaterCpp(m_fTopZ, c3.z)) {
        c4 = FindPlayerCoors(-1);
        if (c4.z > m_vPosn.z)
            flag = true;
    }
    if (!(dist < 3600.0f))
        goto L_C7C4;
    if (!flag)
        goto L_C7C4;
    GaNeatlyLineUp(this, GaStoredCars(m_nType));
    if (!GaRestoreImpounding(this, GaStoredCars(m_nType)))
        goto L_C7C4;
    m_nDoorState = static_cast<eGarageDoorState>(1);
    goto L_C7C4;

    // ---- tipos 36..38 (talleres de tuning): 0x44C564 ------------------------------------------------------------------------------------------------------------------------------
L_C564:
    switch (static_cast<uint8>(m_nDoorState)) {
    case 0:  goto L_C6BD;
    case 1:  goto L_C578;
    case 2:  goto L_C67B;
    case 3:  goto L_BCBE;
    default: goto L_C7C4;
    }
L_C578:
    c1 = FindPlayerCoors(-1);
    c2 = FindPlayerCoors(-1);
    c3 = FindPlayerCoors(-1);
    c4 = FindPlayerCoors(-1);
    if (!(Garage63CenterDistSqCpp(reinterpret_cast<const float*>(this), c1.x, c2.x, c3.y, c4.y) > 900.0))
        goto L_C7C4;
    if (CTimer::GetFrameCounter() & 0x1F)
        goto L_C7C4;
    if (GaIsAnyOtherCarTouching(this, GaFpv())) {
        if (IsEntityTouching3D(GaFpv()))
            goto L_C7C4;
    }
    if (GaIsAnyOtherPedTouching(this, nullptr))
        goto L_C7C4;
    goto L_C662;
L_C67B:
    if (GaFpv())
        GaThrowCarsNearDoorOut(this, GaFpv());
    goto L_C69D;
L_C6BD:
    if (!GaIsAnyOtherCarTouching(this, GaFpv()))
        goto L_C7C4;
    goto L_B9C9;

    // ---- tipo 43 (BURGLARY): 0x44C70A ------------------------------------------------------------------------------------------------------------------------------------------------
L_C70A:
    if (m_nDoorState == 1)
        goto L_C71D;
    if (m_nDoorState == 2)
        goto L_C67B;
    if (m_nDoorState != 3)
        goto L_C7C4;
    goto L_BCBE;
L_C71D:
    c1 = FindPlayerCoors(-1);
    c2 = FindPlayerCoors(-1);
    c3 = FindPlayerCoors(-1);
    c4 = FindPlayerCoors(-1);
    if (!(Garage63CenterDistSqCpp(reinterpret_cast<const float*>(this), c1.x, c2.x, c3.y, c4.y) > 900.0))
        goto L_C7C4;
    if (GaIsAnyOtherPedTouching(this, nullptr))
        goto L_C7C4;
    goto L_C7C0;

    // ---- colas comunes -----------------------------------------------------------------------------------------------------------------------------------------------------------------
L_C662:
    m_nFlags = static_cast<uint8>(m_nFlags | 1);
    m_nDoorState = static_cast<eGarageDoorState>(2);
    goto L_C7C4;
L_C69D:
    if (!GaSlideDoorClosed(this))
        goto L_C7C4;
L_C6AC:
    m_nDoorState = static_cast<eGarageDoorState>(0);
    goto L_C7C4;
L_C6F9:
    m_nDoorState = static_cast<eGarageDoorState>(3);
    goto L_C7C4;
L_C7C0:
    m_nDoorState = static_cast<eGarageDoorState>(2);
L_C7C4:
    return;
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
__declspec(noinline) bool CGarage::IsGarageEmpty() {
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

void CStoredCar::InjectHooks() {
    RH_ScopedClass(CStoredCar);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(StoreCar, 0x449760);
    RH_ScopedInstall(RestoreCar, 0x447E40);
}

namespace {
// fld dword + fstp dword del x87: un NaN senalizador sale silenciado (bit 22); el resto de patrones (subnormales incluidos) se conserva.
inline uint32 GaX87QuietFloatBits(uint32 bits) {
    return (bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) ? (bits | 0x00400000u) : bits;
}
// operator new (0x6E2D50) + constructor del vehiculo (thiscall (modelo, 1)); una reserva nula no se construye (el EXE se cae despues al usar el vehiculo).
template<uint32 Size, uint32 Constructor> inline CVehicle* GaConstructStored(uint32 model) {
    void* const memory = plugin::CallAndReturn<void*, 0x6E2D50, uint32>(Size);
    return memory ? plugin::CallMethodAndReturn<CVehicle*, Constructor, CVehicle*, int32, int32>(static_cast<CVehicle*>(memory), static_cast<int32>(model), 1) : nullptr;
}
}

// 0x449760
// Como el original (0x449760..0x4498F8): posicion de la matriz (+0x30) o del placement (+4) copiada como palabras; avance (matriz +0x10/+0x14/+0x18) por 100.0f [0x858628] truncado con
// _ftol2 y guardado como byte; colores, radio, modelo, banderas de fisica empaquetadas, tipo de bomba (solo si el tipo del vehiculo es 0 o 9), 15 mejoras y remapeo (0x6D0B70, por su
// direccion). Como el EXE, la matriz del vehiculo se desreferencia sin comprobar para el avance.
void CStoredCar::StoreCar(CVehicle* vehicle) {
    m_wModelIndex = GaField<uint16>(vehicle, 0x22);

    const uint32* const pos = GaField<const uint8*>(vehicle, 0x14) ? reinterpret_cast<const uint32*>(GaField<const uint8*>(vehicle, 0x14) + 0x30) : &GaField<uint32>(vehicle, 0x04);
    GaField<uint32>(this, 0x00) = pos[0];
    GaField<uint32>(this, 0x04) = pos[1];
    GaField<uint32>(this, 0x08) = pos[2];

    const float* const forward = reinterpret_cast<const float*>(GaField<const uint8*>(vehicle, 0x14) + 0x10);
    const uint32 matrixAddress = GaField<uint32>(vehicle, 0x14);
    m_nPackedForwardX = static_cast<uint8>(StoredCarPackAxisCpp(forward[0], matrixAddress).eax);
    m_nPackedForwardY = static_cast<uint8>(StoredCarPackAxisCpp(forward[1], matrixAddress).eax);
    m_nPackedForwardZ = static_cast<uint8>(StoredCarPackAxisCpp(forward[2], matrixAddress).eax);

    m_nPrimaryColor    = GaField<uint8>(vehicle, 0x434);
    m_nSecondaryColor  = GaField<uint8>(vehicle, 0x435);
    m_nTertiaryColor   = GaField<uint8>(vehicle, 0x436);
    m_nQuaternaryColor = GaField<uint8>(vehicle, 0x437);
    m_nRadioStation    = GaField<uint8>(vehicle, 0x1D2);
    m_nHandlingFlags   = GaField<uint32>(vehicle, 0x38C);
    m_anCompsToUse[0]  = GaField<uint8>(vehicle, 0x438);
    m_anCompsToUse[1]  = GaField<uint8>(vehicle, 0x439);

    // m_nStoredCarFlags (+0x10) y el relleno (+0x11): palabra de 16 bits como el EXE (el relleno queda en 0)
    const uint32 physicalFlags = GaField<uint32>(vehicle, 0x40);
    uint8        flags         = 0;
    if (physicalFlags & 0x40000)  flags = 1;
    if (physicalFlags & 0x80000)  flags |= 2;
    if (physicalFlags & 0x800000) flags |= 4;
    if (physicalFlags & 0x100000) flags |= 8;
    if (physicalFlags & 0x200000) flags |= 0x10;
    if (GaField<uint8>(vehicle, 0x42E) & 0x10) flags |= 0x20;
    if (GaField<uint32>(vehicle, 0x38C) & 0x20000) flags |= 0x40;
    if (GaField<uint32>(vehicle, 0x38C) & 0x80000) flags |= 0x80;
    m_nStoredCarFlags = flags;
    _pad0             = 0;

    const uint32 vehicleType = GaField<uint32>(vehicle, 0x590);
    if (vehicleType == 0 || vehicleType == 9)
        m_nBombType = GaField<uint8>(vehicle, 0x4A8) & 7;

    for (auto i = 0; i < 15; i++)
        m_awCarMods[i] = GaField<int16>(vehicle, 0x43A + 2 * i);

    m_nPaintJob    = static_cast<uint8>(plugin::CallMethodAndReturn<uint32, 0x6D0B70, CVehicle*>(vehicle)); // CVehicle::GetRemapIndex
    m_nNitroBoosts = GaField<uint8>(vehicle, 0x48A);
}

// 0x447E40
// Como el original (0x447E40..0x4482FD): pide el modelo (flags 8) y cada mejora; si el modelo (byte de estado 1 en 0x8E4CC8 + 20 * modelo + 8) o alguna mejora no estan cargados devuelve 0.
// Si lo estan, fija CVehicleModelInfo::ms_compsToUse (0x8A6458/9), reserva (0x6E2D50) y construye el vehiculo segun el tipo del modelo (tabla 0x448300: tipo - 1 sin signo; 6..8 y fuera de
// rango: coche), lo coloca (posicion con fld/fstp: un NaN senalizador sale silenciado), pone el avance (byte con signo * 0.01f [0x858C58]) y su perpendicular, y copia banderas, colores,
// mejoras y remapeo (0x6E3400 y 0x6D0C00 por su direccion). Tambien deja el byte +0x36 en (+0x36 & 7) | 0x20 y las filas up de la matriz (+0x20/+0x24/+0x28) en (0, 0, 1). Como el EXE, no comprueba la reserva ni la matriz del vehiculo nuevo (el EXE se cae); devuelve el vehiculo.
CVehicle* CStoredCar::RestoreCar() {
    const uint32 model = m_wModelIndex;
    plugin::Call<0x4087E0, int32, int32>(static_cast<int32>(model), 8); // CStreaming::RequestModel

    for (auto i = 0; i < 15; i++) {
        if (m_awCarMods[i] != static_cast<int16>(0xFFFF))
            plugin::Call<0x408C70, int32, int32>(static_cast<int32>(m_awCarMods[i]), 0); // CStreaming::RequestVehicleUpgrade
    }

    if (*reinterpret_cast<const uint8*>(0x8E4CD0 + model * 20) != 1)
        return nullptr;

    for (auto i = 0; i < 15; i++) {
        if (m_awCarMods[i] != static_cast<int16>(0xFFFF)) {
            if (!plugin::CallAndReturn<bool, 0x407820, int32>(static_cast<int32>(m_awCarMods[i]))) // CStreaming::HasVehicleUpgradeLoaded
                return nullptr;
        }
    }

    *reinterpret_cast<uint8*>(0x8A6459) = m_anCompsToUse[1];
    *reinterpret_cast<uint8*>(0x8A6458) = m_anCompsToUse[0];

    const uint32 vehicleType = GaField<uint32>(reinterpret_cast<const void*>(reinterpret_cast<const uint32*>(0xA9B0C8)[model]), 0x3C);
    const uint32 index       = vehicleType - 1;
    CVehicle*    vehicle;
    switch (index > 10 ? 99u : index) {
    case 0:  vehicle = GaConstructStored<0x99C, 0x6C8D60>(model); break;
    case 1:  vehicle = GaConstructStored<0x9BC, 0x6CE370>(model); break;
    case 2:  vehicle = GaConstructStored<0xA18, 0x6C4190>(model); break;
    case 3:  vehicle = GaConstructStored<0xA04, 0x6C8E20>(model); break;
    case 4:  vehicle = GaConstructStored<0x7E8, 0x6F2940>(model); break;
    case 8:
        vehicle = GaConstructStored<0x814, 0x6BF430>(model);
        GaField<uint8>(vehicle, 0x614) |= 0x10;
        break;
    case 9:
        vehicle = GaConstructStored<0x838, 0x6BF820>(model);
        GaField<uint8>(vehicle, 0x614) |= 0x10;
        break;
    case 10: vehicle = GaConstructStored<0x9F4, 0x6D03A0>(model); break;
    default: {
        void* const memory = plugin::CallAndReturn<void*, 0x6E2D50, uint32>(0x988);
        vehicle = memory ? plugin::CallMethodAndReturn<CVehicle*, 0x6B0A90, CVehicle*, int32, int32, int32>(static_cast<CVehicle*>(memory), static_cast<int32>(model), 1, 1) : nullptr;
        break;
    }
    }

    // posicion: matriz + 0x30 si existe, si no placement + 4 (fld/fstp)
    {
        uint8* const  matrix = GaField<uint8*>(vehicle, 0x14);
        uint32* const dst    = matrix ? reinterpret_cast<uint32*>(matrix + 0x30) : &GaField<uint32>(vehicle, 0x04);
        dst[0] = GaX87QuietFloatBits(GaField<uint32>(this, 0x00));
        dst[1] = GaX87QuietFloatBits(GaField<uint32>(this, 0x04));
        dst[2] = GaX87QuietFloatBits(GaField<uint32>(this, 0x08));
    }

    // avance = byte con signo * 0.01f y su perpendicular (y, -x, 0); la matriz se desreferencia sin comprobar
    float forward[3];
    StoredCarUnpackForwardCpp(static_cast<int8>(m_nPackedForwardX), static_cast<int8>(m_nPackedForwardY), static_cast<int8>(m_nPackedForwardZ), forward);
    {
        float* const m = GaField<float*>(vehicle, 0x14);
        m[4] = forward[0];
        m[5] = forward[1];
        m[6] = forward[2];
        m[0] = forward[1];
        m[1] = -forward[0];
        m[2] = 0.0f;
        GaField<uint8>(vehicle, 0x36) = static_cast<uint8>((GaField<uint8>(vehicle, 0x36) & 7) | 0x20);
        m[8]  = 0.0f;
        m[9]  = 0.0f;
        m[10] = 1.0f;
    }

    GaField<uint32>(vehicle, 0x460) = 0;
    GaField<uint8>(vehicle, 0x1D2)  = m_nRadioStation;
    GaField<uint32>(vehicle, 0x38C) = m_nHandlingFlags;
    GaField<uint8>(vehicle, 0x428) &= 0x7F;
    GaField<uint8>(vehicle, 0x42A) |= 2;
    GaField<uint32>(vehicle, 0x4F8) = 1;
    if (GaField<uint32>(vehicle, 0x590) == 0) {
        uint8& bomb = GaField<uint8>(vehicle, 0x4A8);
        bomb        = static_cast<uint8>((bomb & ~7u) | (m_nBombType & 7u));
    }
    if (m_nStoredCarFlags & 0x01) GaField<uint32>(vehicle, 0x40) |= 0x40000;
    if (m_nStoredCarFlags & 0x02) GaField<uint32>(vehicle, 0x40) |= 0x80000;
    if (m_nStoredCarFlags & 0x04) GaField<uint32>(vehicle, 0x40) |= 0x800000;
    if (m_nStoredCarFlags & 0x08) GaField<uint32>(vehicle, 0x40) |= 0x100000;
    if (m_nStoredCarFlags & 0x10) GaField<uint32>(vehicle, 0x40) |= 0x200000;
    if (m_nStoredCarFlags & 0x20) {
        GaField<uint8>(vehicle, 0x42E) |= 0x10;
        GaField<uint8>(vehicle, 0x1BE) = 1;
    }
    if (m_nStoredCarFlags & 0x40) GaField<uint32>(vehicle, 0x38C) |= 0x20000;
    if (m_nStoredCarFlags & 0x80) GaField<uint32>(vehicle, 0x38C) |= 0x80000;

    for (auto i = 0; i < 15; i++)
        GaField<int16>(vehicle, 0x43A + 2 * i) = m_awCarMods[i];

    plugin::CallMethod<0x6E3400, CVehicle*>(vehicle);                                         // CVehicle::SetupUpgradesAfterLoad
    plugin::CallMethod<0x6D0C00, CVehicle*, int32>(vehicle, static_cast<int8>(m_nPaintJob)); // CVehicle::SetRemap

    GaField<uint8>(vehicle, 0x428) &= 0xEF;
    GaField<uint8>(vehicle, 0x48A)  = m_nNitroBoosts;
    GaField<uint8>(vehicle, 0x434)  = m_nPrimaryColor;
    GaField<uint8>(vehicle, 0x435)  = m_nSecondaryColor;
    GaField<uint8>(vehicle, 0x436)  = m_nTertiaryColor;
    GaField<uint8>(vehicle, 0x42F) |= 4;
    GaField<uint8>(vehicle, 0x437)  = m_nQuaternaryColor;
    return vehicle;
}
