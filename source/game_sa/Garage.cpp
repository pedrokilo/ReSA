/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGarage
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
#pragma runtime_checks("", restore)

void CGarage::InjectHooks() {
    RH_ScopedClass(CGarage);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(BuildRotatedDoorMatrix, 0x4479F0);
    // RH_ScopedInstall(TidyUpGarageClose, 0x449D10);
    // RH_ScopedInstall(TidyUpGarage, 0x449C50);
    RH_ScopedInstall(StoreAndRemoveCarsForThisHideOut, 0x449900);
    RH_ScopedInstall(EntityHasASphereWayOutsideGarage, 0x449050);
    // RH_ScopedInstall(RemoveCarsBlockingDoorNotInside, 0x449690);
    RH_ScopedInstall(IsEntityTouching3D, 0x448EE0);
    // RH_ScopedInstall(IsEntityEntirelyOutside, 0x448D30);
    // RH_ScopedInstall(IsStaticPlayerCarEntirelyInside, 0x44A830);
    // RH_ScopedInstall(IsEntityEntirelyInside3D, 0x448BE0);
    RH_ScopedNamedGlobalInstall(GarageIsPointInsideRegs, "IsPointInsideGarage-point", 0x448740); // EDX/ECX del EXE: ver GarageIsPointInsideRegs
    // RH_ScopedInstall(PlayerArrestedOrDied, 0x4486C0);
    RH_ScopedInstall(OpenThisGarage, 0x447D50);
    RH_ScopedInstall(CloseThisGarage, 0x447D70);
    // RH_ScopedInstall(InitDoorsAtStart, 0x447600);
    RH_ScopedNamedGlobalInstall(GarageIsPointInsideRadiusRegs, "IsPointInsideGarage-radius", 0x4487D0); // ECX/EDX del EXE: ver GarageIsPointInsideRadiusRegs
    // RH_ScopedInstall(Update, 0x44AA50);
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
void CGarage::TidyUpGarageClose() {
    plugin::CallMethod<0x449D10, CGarage*>(this);
}

// 0x449C50
void CGarage::TidyUpGarage() {
    plugin::CallMethod<0x449C50, CGarage*>(this);
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
void CGarage::RemoveCarsBlockingDoorNotInside() {
    plugin::CallMethod<0x449690, CGarage*>(this);
}

// 0x448EE0
// Como el original (0x448EE0..0x449041): radio = esfera delimitadora del CColModel del modelo (no el de CEntity::GetColModel), rechazo por caja ampliada con los seis
// limites del garaje (GarageEntityNearCpp, x87) y, para cada esfera de colision, centro transformado con 0x59C890 (por su direccion) e
// IsPointInsideGarage(centro, radio de la esfera).
bool CGarage::IsEntityTouching3D(CEntity* entity) {
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
bool CGarage::IsEntityEntirelyOutside(CEntity* entity, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x448D30, CGarage*, CEntity*, float>(this, entity, radius);
}

// 0x44A830
bool CGarage::IsStaticPlayerCarEntirelyInside() {
    return plugin::CallMethodAndReturn<bool, 0x44A830, CGarage*>(this);
}

// 0x448BE0
bool CGarage::IsEntityEntirelyInside3D(CEntity* entity, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x448BE0, CGarage*, CEntity*, float>(this, entity, radius);
}

// 0x448740
// Version de la DLL (las llamadas desde C++ no leen el residuo de registros); el hook del EXE es GarageIsPointInsideRegs.
bool CGarage::IsPointInsideGarage(CVector point) {
    uint32 ecxUnused = 0;
    return GarageInsidePointCpp(reinterpret_cast<const float*>(this), &point.x, &ecxUnused);
}

// 0x4486C0
eGarageDoorState CGarage::PlayerArrestedOrDied() {
    return plugin::CallMethodAndReturn<eGarageDoorState, 0x4486C0, CGarage*>(this);
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
void CGarage::InitDoorsAtStart() {
    plugin::CallMethod<0x447600, CGarage*>(this);
}

// 0x4487D0
// Version de la DLL; el hook del EXE es GarageIsPointInsideRadiusRegs.
bool CGarage::IsPointInsideGarage(CVector point, float radius) {
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
bool CGarage::IsGarageEmpty() {
    return plugin::CallMethodAndReturn<bool, 0x44A9C0, CGarage*>(this);

    CVector cornerA = { m_fLeftCoord, m_fFrontCoord, m_vPosn.z };
    CVector cornerB = { m_fRightCoord, m_fBackCoord, m_fTopZ   };

    int16 outCount[2];
    CEntity* outEntities[16];
    CWorld::FindObjectsIntersectingCube(cornerA, cornerB, outCount, static_cast<int16>(std::size(outEntities)), outEntities, false, true, true, false, false);
    if (outCount[0] <= 0)
        return true;

    int16 entityIndex = 0;

    while (!IsEntityTouching3D(outEntities[entityIndex])) {
        if (++entityIndex >= outCount[0])
            return true;
    }
    return false;
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
