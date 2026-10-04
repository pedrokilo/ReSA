/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDamageManager
 * Reconstrucción: COMPLETA — 27/27 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 27/27 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "DamageManager.h"

// Como el original: tabla de multiplicadores por grupo (0x8D32A0). El EXE la indexa con el dword del grupo sin
// comprobar límites (con un componente negativo el grupo queda con el valor del argumento, ver ApplyDamage).
static const float* const s_afDamageMultByGroup = reinterpret_cast<const float*>(0x8D32A0);

// Como el original: los desplazamientos se calculan sobre un byte (shl al,N) y shl/shr los enmascaran a 5 bits.
static uint32 PanelShift(uint32 panel)     { return (uint32)(uint8)(panel << 2) & 31u; }
static uint32 LightShift(uint32 light)     { return (uint32)(uint8)(light << 1) & 31u; }
static uint32 AeroplaneShift(uint32 frame) { return (uint32)(uint8)((frame - 12u) << 1) & 31u; }

static uint32 GetPanelStatusRaw(const CDamageManager* self, uint32 panel) {
    return (self->m_nPanelsStatus >> PanelShift(panel)) & 0xFu;
}

static void SetPanelStatusRaw(CDamageManager* self, uint32 panel, uint32 status) {
    const auto shift = PanelShift(panel);
    self->m_nPanelsStatus = (status << shift) | (self->m_nPanelsStatus & ~(0xFu << shift));
}

static uint32 GetLightStatusRaw(const CDamageManager* self, uint32 light) {
    return (self->m_nLightsStatus >> LightShift(light)) & 3u;
}

static void SetLightStatusRaw(CDamageManager* self, uint32 light, uint32 status) {
    const auto shift = LightShift(light);
    self->m_nLightsStatus = (status << shift) | (self->m_nLightsStatus & ~(3u << shift));
}

// Como el original (0x6C2040): compara el dword completo con signo, escribe el índice relativo como byte (0xFE de
// entrada) y el grupo como dword. 11..15 (alas y parabrisas) son el grupo 5 y 16 o más el grupo 0 (paneles).
static bool GetComponentGroupRaw(int32 comp, int32& outGroup, uint8& outIdx) {
    outIdx = 0xFE;
    if (comp >= 11) {
        outGroup = comp >= 16 ? COMPGROUP_PANEL : COMPGROUP_LIGHT;
        outIdx   = (uint8)(comp - 11);
        return true;
    }
    if (comp >= 5) {
        outGroup = comp == 5 ? COMPGROUP_BONNET : comp == 6 ? COMPGROUP_BOOT : COMPGROUP_DOOR;
        outIdx   = (uint8)(comp - 5);
        return true;
    }
    if (comp >= 1) {
        outGroup = COMPGROUP_WHEEL;
        outIdx   = (uint8)(comp - 1);
        return true;
    }
    if (comp >= 0) {
        outGroup = COMPGROUP_NA;
        outIdx   = 0;
        return true;
    }
    return false;
}

// Como el original (0x6C24B0): el componente es un dword; fColDmgMult no se usa.
static bool ApplyDamageRaw(CDamageManager* self, CAutomobile* vehicle, int32 compId, float fIntensity) {
    if (!vehicle->autoFlags.bCanBeVisiblyDamaged)
        return false;

    // Como el original: si GetComponentGroup falla, el grupo conserva el valor del argumento (misma ranura de pila)
    int32 group = compId;
    uint8 relIdx{};
    GetComponentGroupRaw(compId, group, relIdx);

    fIntensity *= s_afDamageMultByGroup[group];
    if (compId == COMPONENT_WINDSCREEN)
        fIntensity *= 0.6f;

    if (!(fIntensity > 150.0f)) // Como el original: NaN tampoco daña (test ah,41h)
        return false;

    switch ((uint32)group) {
    case COMPGROUP_PANEL: {
        if (self->ProgressPanelDamage((ePanels)relIdx))
            vehicle->SetBumperDamage((ePanels)relIdx, false);
        break;
    }
    case COMPGROUP_WHEEL: {
        self->ProgressWheelDamage((eCarWheel)relIdx);
        break;
    }
    case COMPGROUP_DOOR:
    case COMPGROUP_BONNET:
    case COMPGROUP_BOOT: {
        if (self->ProgressDoorDamage((eDoors)relIdx, vehicle))
            vehicle->SetDoorDamage((eDoors)relIdx, false);
        break;
    }
    case COMPGROUP_LIGHT: {
        self->SetLightStatus((eLights)relIdx, VEHICLE_LIGHT_SMASHED);
        if (self->ProgressPanelDamage((ePanels)relIdx))
            vehicle->SetPanelDamage((ePanels)relIdx, false);
        break;
    }
    }
    return true;
}

// Envoltorios de los hooks con los anchos de argumentos y retorno del EXE.

// Como el original (0x6A0520 / 0x6A04E0): devuelven this en EAX.
static CDamageManager* __fastcall Init_Hook(CDamageManager* self) {
    self->Init();
    return self;
}

static CDamageManager* __fastcall ResetDamageStatusAndWheelDamage_Hook(CDamageManager* self) {
    self->ResetDamageStatusAndWheelDamage();
    return self;
}

static bool __fastcall ApplyDamage_Hook(CDamageManager* self, void*, CAutomobile* vehicle, int32 compId, float fIntensity, float fColDmgMult) {
    return ApplyDamageRaw(self, vehicle, compId, fIntensity);
}

// Como el original: thiscall que no usa ECX y limpia sus 3 argumentos (ret 0Ch); la versión estática de la DLL es cdecl.
static bool __stdcall GetComponentGroup_Hook(int32 comp, int32* outGroup, uint8* outIdx) {
    return GetComponentGroupRaw(comp, *outGroup, *outIdx);
}

// Como el original (0x6C2230 / 0x6C2250): devuelven el estado extendido a 32 bits (movzx eax, byte).
// Los llamadores del EXE comparan EAX completo; la versión C++ devuelve un uint8 y deja basura en los
// bits altos, así que se enganchan estos envoltorios con el mismo nombre de hook.
static uint32 __fastcall GetDoorStatus_Zx(const CDamageManager* self, void*, int32 door) {
    // El EXE compara el dword completo del argumento (0 <= door < 6)
    return door >= 0 && door < 6 ? (uint32)self->GetDoorStatus((eDoors)door) : 4u;
}

static uint32 __fastcall GetDoorStatus_Component_Zx(const CDamageManager* self, void*, uint32 comp) {
    // El EXE compara el dword completo del argumento (comp - 8 <= 3 sin signo)
    return comp - 8u <= 3u ? (uint32)self->GetDoorStatus_Component((tComponent)comp) : 4u;
}

// Como el original (0x6C21C0): puerta como dword con signo (0 <= door < 6), estado como byte.
static void __fastcall SetDoorStatus_Hook(CDamageManager* self, void*, int32 door, uint32 status) {
    if (door >= 0 && door < 6)
        self->SetDoorStatus((eDoors)door, (eDoorStatus)(uint8)status);
}

// Como el original (0x6C21E0): componente como dword (comp - 8 <= 3 sin signo).
static void __fastcall SetDoorStatus_Component_Hook(CDamageManager* self, void*, uint32 comp, uint32 status) {
    if (comp - 8u <= 3u)
        self->SetDoorStatus_Component((tComponent)comp, (eDoorStatus)(uint8)status);
}

// Como el original (0x6C2180 / 0x6C2130): índice como byte y retorno en EAX completo; los Set usan el estado de 32 bits.
static uint32 __fastcall GetPanelStatus_Hook(const CDamageManager* self, void*, uint32 panel) {
    return GetPanelStatusRaw(self, panel);
}

static void __fastcall SetPanelStatus_Hook(CDamageManager* self, void*, uint32 panel, uint32 status) {
    SetPanelStatusRaw(self, panel, status);
}

static uint32 __fastcall GetLightStatus_Hook(const CDamageManager* self, void*, uint32 light) {
    return GetLightStatusRaw(self, light);
}

static void __fastcall SetLightStatus_Hook(CDamageManager* self, void*, uint32 light, uint32 status) {
    SetLightStatusRaw(self, light, status);
}

// Como el original (0x6C26A0 / 0x6C26F0): cdecl, índice como dword sin signo, -1 fuera de rango.
static eCarNodes __cdecl GetCarNodeIndexFromPanel_Hook(uint32 panel) {
    return panel <= 6u ? CDamageManager::GetCarNodeIndexFromPanel((ePanels)panel) : (eCarNodes)-1;
}

static eCarNodes __cdecl GetCarNodeIndexFromDoor_Hook(uint32 door) {
    return door <= 5u ? CDamageManager::GetCarNodeIndexFromDoor((eDoors)door) : (eCarNodes)-1;
}

void CDamageManager::InjectHooks() {
    RH_ScopedClass(CDamageManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(Init_Hook, "Init", 0x6A0520);
    RH_ScopedNamedGlobalInstall(ResetDamageStatusAndWheelDamage_Hook, "ResetDamageStatusAndWheelDamage", 0x6A04E0);
    RH_ScopedInstall(FuckCarCompletely, 0x6C25D0);
    RH_ScopedNamedGlobalInstall(ApplyDamage_Hook, "ApplyDamage", 0x6C24B0);
    RH_ScopedInstall(ProgressAeroplaneDamage, 0x6C2460);
    RH_ScopedInstall(ProgressWheelDamage, 0x6C2440);
    RH_ScopedInstall(ProgressPanelDamage, 0x6C23C0);
    RH_ScopedInstall(ProgressEngineDamage, 0x6C23B0);
    RH_ScopedInstall(ProgressDoorDamage, 0x6C2320);
    RH_ScopedInstall(GetAeroplaneCompStatus, 0x6C2300);
    RH_ScopedInstall(SetAeroplaneCompStatus, 0x6C22D0);
    RH_ScopedInstall(GetEngineStatus, 0x6C22C0);
    RH_ScopedInstall(SetEngineStatus, 0x6C22A0);
    RH_ScopedNamedGlobalInstall(GetCarNodeIndexFromPanel_Hook, "GetCarNodeIndexFromPanel", 0x6C26A0);
    RH_ScopedNamedGlobalInstall(GetDoorStatus_Component_Zx, "GetDoorStatus_Component", 0x6C2250);
    RH_ScopedNamedGlobalInstall(GetDoorStatus_Zx, "GetDoorStatus", 0x6C2230);
    RH_ScopedNamedGlobalInstall(SetDoorStatus_Component_Hook, "SetDoorStatus_Component", 0x6C21E0);
    RH_ScopedNamedGlobalInstall(SetDoorStatus_Hook, "SetDoorStatus-", 0x6C21C0);
    RH_ScopedInstall(SetWheelStatus, 0x6C21A0);
    RH_ScopedNamedGlobalInstall(GetPanelStatus_Hook, "GetPanelStatus", 0x6C2180);
    RH_ScopedNamedGlobalInstall(SetPanelStatus_Hook, "SetPanelStatus", 0x6C2150);
    RH_ScopedNamedGlobalInstall(GetLightStatus_Hook, "GetLightStatus", 0x6C2130);
    RH_ScopedNamedGlobalInstall(SetLightStatus_Hook, "SetLightStatus", 0x6C2100);
    RH_ScopedInstall(ResetDamageStatus, 0x6C20E0);
    RH_ScopedNamedGlobalInstall(GetComponentGroup_Hook, "GetComponentGroup", 0x6C2040);
    RH_ScopedNamedGlobalInstall(GetCarNodeIndexFromDoor_Hook, "GetCarNodeIndexFromDoor", 0x6C26F0);
    RH_ScopedNamedInstall(GetWheelStatus_Hooked, "GetWheelStatus", 0x6C21B0);
}

CDamageManager::CDamageManager(float wheelDamageEffect) :
    m_fWheelDamageEffect{ wheelDamageEffect }
{
    ResetDamageStatus();
}

// 0x6A0520
void CDamageManager::Init() {
    // Como el original: rep stosd sobre las 6 dwords (incluido el byte de relleno 0xF que ResetDamageStatus no toca)
    std::memset(this, 0, sizeof(CDamageManager));
}

// 0x6A04E0
void CDamageManager::ResetDamageStatusAndWheelDamage() {
    ResetDamageStatus();
    m_fWheelDamageEffect = 0.5f;
}

// 0x6C25D0
void CDamageManager::FuckCarCompletely(bool bDontDetachWheel) {
    if (!bDontDetachWheel) {
        // Como el original: (rand() & 0xFFFF) * (1/32768) * -3.0, truncado y restado al puntero: ruedas 0..2 (nunca la 3)
        const auto k = (int32)((float)CGeneral::GetRandomNumber() * 3.0517578125e-05f * -3.0f);
        m_anWheelsStatus.data()[-k] = WHEEL_STATUS_MISSING;
    }

    m_aDoorsStatus.fill(DAMSTATE_NOTPRESENT);

    for (size_t i = 0; i < 3; i++) {
        ProgressPanelDamage(ePanels::FRONT_BUMPER);
        ProgressPanelDamage(ePanels::REAR_BUMPER);
    }

    // Como el original: se ponen a cero las dwords completas de luces y paneles
    m_nLightsStatus = 0;
    m_nPanelsStatus = 0;
    m_nEngineStatus = 250;
}

// 0x6C24B0
bool CDamageManager::ApplyDamage(CAutomobile* vehicle, tComponent compId, float fIntensity, float fColDmgMult) {
    return ApplyDamageRaw(this, vehicle, (int32)compId, fIntensity);
}

// 0x6C2460
bool CDamageManager::ProgressAeroplaneDamage(uint8 nFrameId) {
    const auto current = GetAeroplaneCompStatus(nFrameId);
    if (current == 2)
        return false;

    SetAeroplaneCompStatus(nFrameId, (uint8)(current + 1));
    return true;
}

// 0x6C2440
bool CDamageManager::ProgressWheelDamage(eCarWheel wheel) {
    // Como el original: índice como byte (movzx)
    auto& status = m_anWheelsStatus.data()[(uint8)wheel];
    if (status == WHEEL_STATUS_MISSING)
        return false;

    status = (eCarWheelStatus)(uint8)(status + 1);
    return true;
}

// 0x6C23C0
bool CDamageManager::ProgressPanelDamage(ePanels panel) {
    const auto current = (uint8)GetPanelStatusRaw(this, panel);
    if (current == DAMSTATE_DAMAGED) {
        // Como el original: bits bajos de rand() (parabrisas 1/2, resto 1/8)
        if (panel == WINDSCREEN_PANEL) {
            if (CGeneral::GetRandomNumber() & 1)
                return false;
        } else if (CGeneral::GetRandomNumber() & 7) {
            return false;
        }
    } else if (current == DAMSTATE_OPENED_DAMAGED) {
        return false;
    }
    SetPanelStatusRaw(this, panel, (uint8)(current + 1));
    return true;
}

// 0x6C23B0
bool CDamageManager::ProgressEngineDamage(int32 unused) {
    return false;
}

// 0x6C2320
bool CDamageManager::ProgressDoorDamage(eDoors door, CAutomobile* pAuto) {
    if ((uint8)door >= (uint8)MAX_DOORS) {
        return false;
    }

    auto status = (uint8)m_aDoorsStatus[door];
    switch (status) {
    case DAMSTATE_OK:
    case DAMSTATE_OPENED:
        pAuto->GetDoor(door).Open(0.0f);
        status = DAMSTATE_DAMAGED;
        break;
    case DAMSTATE_DAMAGED:
        status = DAMSTATE_OPENED_DAMAGED;
        break;
    case DAMSTATE_OPENED_DAMAGED:
        if (CGeneral::GetRandomNumber() & 7) // Como el original: rand() & 7
            return false;
        status = DAMSTATE_NOTPRESENT;
        break;
    case DAMSTATE_NOTPRESENT:
        return false;
    default: // Como el original: un estado mayor que 4 se vuelve a escribir igual
        break;
    }
    m_aDoorsStatus[door] = (eDoorStatus)status;
    return true;
}

// 0x6C2300
uint32 CDamageManager::GetAeroplaneCompStatus(uint8 frame) {
    return (m_nPanelsStatus >> AeroplaneShift(frame)) & 3u;
}

// 0x6C22D0
void CDamageManager::SetAeroplaneCompStatus(uint8 frame, uint32 status) {
    const auto shift = AeroplaneShift(frame);
    m_nPanelsStatus = (status << shift) | (m_nPanelsStatus & ~(3u << shift));
}

// 0x6C22C0
uint32 CDamageManager::GetEngineStatus() {
    return m_nEngineStatus;
}

// 0x6C22A0
void CDamageManager::SetEngineStatus(uint32 status) {
    // Como el original: se compara el dword completo sin signo
    m_nEngineStatus = (uint8)std::min<uint32>(status, 250u);
}

// 0x6C26A0
eCarNodes CDamageManager::GetCarNodeIndexFromPanel(ePanels panel) {
    switch (panel) {
    case ePanels::FRONT_LEFT_PANEL:
        return eCarNodes::CAR_WING_LF;
    case ePanels::FRONT_RIGHT_PANEL:
        return eCarNodes::CAR_WING_RF;
    case ePanels::WINDSCREEN_PANEL:
        return eCarNodes::CAR_WINDSCREEN;
    case ePanels::FRONT_BUMPER:
        return eCarNodes::CAR_BUMP_FRONT;
    case ePanels::REAR_BUMPER:
        return eCarNodes::CAR_BUMP_REAR;
    default:
        return (eCarNodes)-1;
    }
}

// 0x6C2250
eDoorStatus CDamageManager::GetDoorStatus_Component(tComponent doorComp) const {
    /* Enums don't seem to match up... */
    switch (doorComp) {
    case tComponent::COMPONENT_DOOR_RF:
        return GetDoorStatus(eDoors::DOOR_RIGHT_FRONT);
    case tComponent::COMPONENT_DOOR_LR:
        return GetDoorStatus(eDoors::DOOR_RIGHT_REAR);
    case tComponent::COMPONENT_DOOR_RR:
        return GetDoorStatus(eDoors::DOOR_LEFT_FRONT);
    case tComponent::COMPONENT_WING_LF:
        return GetDoorStatus(eDoors::DOOR_LEFT_REAR);
    default:
        return eDoorStatus::DAMSTATE_NOTPRESENT;
    }
}

// 0x6C21E0
void CDamageManager::SetDoorStatus_Component(tComponent doorComp, eDoorStatus status) {
    switch (doorComp) {
    case tComponent::COMPONENT_DOOR_RF:
        SetDoorStatus(eDoors::DOOR_RIGHT_FRONT, status);
        break;
    case tComponent::COMPONENT_DOOR_LR:
        SetDoorStatus(eDoors::DOOR_RIGHT_REAR, status);
        break;
    case tComponent::COMPONENT_DOOR_RR:
        SetDoorStatus(eDoors::DOOR_LEFT_FRONT, status);
        break;
    case tComponent::COMPONENT_WING_LF:
        SetDoorStatus(eDoors::DOOR_LEFT_REAR, status);
        break;
    }
}

// 0x6C21C0
void CDamageManager::SetDoorStatus(eDoors door, eDoorStatus status) {
    switch (door) {
    case eDoors::DOOR_BONNET:
    case eDoors::DOOR_BOOT:
    case eDoors::DOOR_LEFT_FRONT:
    case eDoors::DOOR_RIGHT_FRONT:
    case eDoors::DOOR_LEFT_REAR:
    case eDoors::DOOR_RIGHT_REAR:
        m_aDoorsStatus[(unsigned)door] = status;
        break;
    }
}

// 0x6C21B0
eCarWheelStatus CDamageManager::GetWheelStatus(eCarWheel wheel) const {
    return m_anWheelsStatus.data()[wheel]; // Como el original: índice dword sin límites
}

// 0x6C21A0
void CDamageManager::SetWheelStatus(eCarWheel wheel, eCarWheelStatus status) {
    m_anWheelsStatus.data()[wheel] = status; // Como el original: índice dword sin límites
}

// 0x6C2180
ePanelDamageState CDamageManager::GetPanelStatus(ePanels panel) const {
    return (ePanelDamageState)GetPanelStatusRaw(this, panel);
}

// 0x6C2150
void CDamageManager::SetPanelStatus(ePanels panel, ePanelDamageState status) {
    SetPanelStatusRaw(this, panel, status);
}

// 0x6C2130
eLightsState CDamageManager::GetLightStatus(eLights light) const {
    return (eLightsState)GetLightStatusRaw(this, light);
}

// 0x6C2100
void CDamageManager::SetLightStatus(eLights light, eLightsState status) {
    SetLightStatusRaw(this, light, status);
}

// 0x6C20E0
void CDamageManager::ResetDamageStatus() {
    // Como el original: se ponen a cero las dwords completas de luces y paneles (el byte de relleno 0xF no se toca)
    m_nEngineStatus = 0;
    m_anWheelsStatus.fill(WHEEL_STATUS_OK);
    m_aDoorsStatus.fill(DAMSTATE_OK);
    m_nLightsStatus = 0;
    m_nPanelsStatus = 0;
}

// 0x6C2040
bool CDamageManager::GetComponentGroup(tComponent nComp, tComponentGroup& outCompGroup, uint8& outComponentRelativeIdx) {
    int32 group{};
    if (!GetComponentGroupRaw((int32)nComp, group, outComponentRelativeIdx))
        return false;
    outCompGroup = (tComponentGroup)group;
    return true;
}

// NOTSA
void CDamageManager::SetAllWheelsState(eCarWheelStatus state) {
    constexpr eCarWheel wheels[]{CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_REAR_LEFT, CAR_WHEEL_FRONT_RIGHT, CAR_WHEEL_REAR_RIGHT};
    for (auto&& wheel : wheels) {
        SetWheelStatus(wheel, state);
    }
}

// NOTSA
void CDamageManager::SetDoorStatus(std::initializer_list<eDoors> doors, eDoorStatus status) {
    for (auto&& door : doors) {
        SetDoorStatus(door, status);
    }
}

auto CDamageManager::GetAllLightsState() const->std::array<eLightsState, 4> {
    return {
        GetLightStatus(eLights::LIGHT_FRONT_LEFT),
        GetLightStatus(eLights::LIGHT_FRONT_RIGHT),
        GetLightStatus(eLights::LIGHT_REAR_LEFT),
        GetLightStatus(eLights::LIGHT_REAR_RIGHT)
    };
}

auto CDamageManager::GetAllDoorsStatus() const -> std::array<eDoorStatus, MAX_DOORS> {
    return {
        GetDoorStatus(eDoors::DOOR_BONNET),
        GetDoorStatus(eDoors::DOOR_BOOT),
        GetDoorStatus(eDoors::DOOR_LEFT_FRONT),
        GetDoorStatus(eDoors::DOOR_RIGHT_FRONT),
        GetDoorStatus(eDoors::DOOR_LEFT_REAR),
        GetDoorStatus(eDoors::DOOR_RIGHT_REAR)
    };
}

/*!
* @notsa
* @brief Should only be called if the door is present (asserts in debug)
* @returns If door's state is either DAMSTATE_OPENED or DAMSTATE_OPENED_DAMAGED
*/
bool CDamageManager::IsDoorOpen(eDoors door) const {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OPENED:
    case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
        return true;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        assert(0 && "Door not present @ IsDoorOpen"); // Otherwise `!IsDoorOpen() == IsDoorClosed()` may not always be true which may cause bugs :D
        return false;
    }
    return false;
}

/*!
* @notsa
* @brief Should only be called if the door is present (asserts in debug)
* @returns If door's state is either DAMSTATE_OK or DAMSTATE_DAMAGED
*/
bool CDamageManager::IsDoorClosed(eDoors door) const {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OK:
    case eDoorStatus::DAMSTATE_DAMAGED:
        return true;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        assert(0 && "Door not present @ IsDoorClosed"); // Otherwise `!IsDoorOpen() == IsDoorClosed()` may not always be true which may cause bugs :D
        return false;
    }
    return false;
}

bool CDamageManager::IsDoorPresent(eDoors door) const {
    return GetDoorStatus(door) != DAMSTATE_NOTPRESENT;
}

/*!
* @notsa
* @brief   Checks if door is damaged.
* @returns Returns if door's state is neither DAMSTATE_OK or DAMSTATE_OPENED
*/
bool CDamageManager::IsDoorDamaged(eDoors door) const {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OK:
    case eDoorStatus::DAMSTATE_OPENED:
        return false;
    }
    return true;
}

/*!
* @notsa
* @brief Sets door open, unless it's missing (In which case nothing's done)
*/
void CDamageManager::SetDoorOpen(eDoors door) {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OK:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_OPENED);
        break;
    case eDoorStatus::DAMSTATE_DAMAGED:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_OPENED_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        break; /* do nothing */
    }
}

/*!
* @notsa
* @brief Sets door open, unless it's missing (In which case nothing's done)
*/
void CDamageManager::SetDoorOpen_Component(tComponent door) {
    switch (GetDoorStatus_Component(door)) {
    case eDoorStatus::DAMSTATE_OK:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_OPENED);
        break;
    case eDoorStatus::DAMSTATE_DAMAGED:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_OPENED_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        break; /* do nothing */
    }
}

/*!
* @notsa
* @brief Sets door closed, unless it's missing (In which case nothing's done)
*/
void CDamageManager::SetDoorClosed(eDoors door) {
    switch (GetDoorStatus(door)) {
    case eDoorStatus::DAMSTATE_OPENED:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_OK);
        break;
    case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
        SetDoorStatus(door, eDoorStatus::DAMSTATE_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        break; /* do nothing */
    }
}

/*!
* @notsa
* @brief Sets door closed, unless it's missing (In which case nothing's done)
*/
void CDamageManager::SetDoorClosed_Component(tComponent door) {
    switch (GetDoorStatus_Component(door)) {
    case eDoorStatus::DAMSTATE_OPENED:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_OK);
        break;
    case eDoorStatus::DAMSTATE_OPENED_DAMAGED:
        SetDoorStatus_Component(door, eDoorStatus::DAMSTATE_DAMAGED);
        break;
    case eDoorStatus::DAMSTATE_NOTPRESENT:
        break; /* do nothing */
    }
}

// 0x6C2230
eDoorStatus CDamageManager::GetDoorStatus(eDoors nDoorIdx) const {
    switch (nDoorIdx) {
    case eDoors::DOOR_BONNET:
    case eDoors::DOOR_BOOT:
    case eDoors::DOOR_LEFT_FRONT:
    case eDoors::DOOR_RIGHT_FRONT:
    case eDoors::DOOR_LEFT_REAR:
    case eDoors::DOOR_RIGHT_REAR:
        return m_aDoorsStatus[(unsigned)nDoorIdx];
    default:
        return eDoorStatus::DAMSTATE_NOTPRESENT;
    }
}

// 0x6C26F0
eCarNodes CDamageManager::GetCarNodeIndexFromDoor(eDoors door) {
    switch (door) {
    case eDoors::DOOR_BONNET:      return eCarNodes::CAR_BONNET;
    case eDoors::DOOR_BOOT:        return eCarNodes::CAR_BOOT;
    case eDoors::DOOR_LEFT_FRONT:  return eCarNodes::CAR_DOOR_LF;
    case eDoors::DOOR_RIGHT_FRONT: return eCarNodes::CAR_DOOR_RF;
    case eDoors::DOOR_LEFT_REAR:   return eCarNodes::CAR_DOOR_LR;
    case eDoors::DOOR_RIGHT_REAR:  return eCarNodes::CAR_DOOR_RR;
    default:                       return (eCarNodes)-1; // Como el original: -1 fuera de rango
    }
}
