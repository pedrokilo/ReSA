/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGarages
 * Reconstrucción: COMPLETA — 26/26 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 26/26 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include "Garages.h"

static auto& LastUpdatedGarageId = StaticRef<uint32>(0x96EA78);

void CGarages::InjectHooks() {
    RH_ScopedClass(CGarages);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x447120);
    RH_ScopedInstall(Init_AfterRestart, 0x448B60);
    RH_ScopedInstall(Shutdown, 0x4471B0);
    RH_ScopedInstall(AddOne, 0x4471E0);
    RH_ScopedInstall(CloseHideOutGaragesBeforeSave, 0x44A170);
    RH_ScopedInstall(PlayerArrestedOrDied, 0x449E60);
    RH_ScopedInstall(AllRespraysCloseOrOpen, 0x448B30);
    RH_ScopedInstall(IsModelIndexADoor, 0x448AF0);
    RH_ScopedInstall(FindSafeHouseIndexForGarageType, 0x4489F0);
    RH_ScopedNamedInstall(FindGarageForObjectRegs, "FindGarageForObject", 0x44A240);
    RH_ScopedInstall(IsPointWithinHideOutGarage, 0x448900);
    RH_ScopedInstall(IsGarageOpen, 0x447D00);
    RH_ScopedInstall(IsGarageClosed, 0x447D30);
    RH_ScopedInstall(IsCarSprayable, 0x4479A0);
    RH_ScopedInstall(Update, 0x44C8C0);
    RH_ScopedInstall(ActivateGarage, 0x447CD0);
    RH_ScopedInstall(DeActivateGarage, 0x447CB0);
    RH_ScopedInstall(SetTargetCarForMissionGarage, 0x447C40);
    RH_ScopedInstall(StoreCarInNearestImpoundingGarage, 0x44A3C0);
    RH_ScopedInstall(TriggerMessage, 0x447B80);
    RH_ScopedInstall(PrintMessages, 0x447790);
    RH_ScopedInstall(ChangeGarageType, 0x4476D0);
    RH_ScopedInstall(GetGarageNumberByName, 0x447680);
    RH_ScopedInstall(CountCarsInHideoutGarage, 0x44A210);
    RH_ScopedInstall(Load, 0x5D3270);
    RH_ScopedInstall(Save, 0x5D3160);
}

// 0x447120
void CGarages::Init() {
    ZoneScoped;

    NumGarages = 0;
    MessageEndTime = 0;
    MessageStartTime = 0;
    PlayerInGarage = 0;
    BombsAreFree = 0;
    CarsCollected = 0;
    BankVansCollected = 0;
    PoliceCarsCollected = 0;
    std::ranges::fill(CarTypesCollected, 0);
    CrushedCarId = -1;
    LastTimeHelpMessage = 0;
    LastGaragePlayerWasIn = -1;
    NoResprays = false;

    for (auto& safeHouseCars : aCarsInSafeHouse) {
        for (auto& car : safeHouseCars) {
            car.m_wModelIndex = 0;
        }
    }
}

// 0x448B60
void CGarages::Init_AfterRestart() {
    if (NumGarages) {
        for (uint32 i = 0; i < (uint32)NumGarages; i++) { // Como el original: comparación sin signo (jb)
            auto& garage = GetGarage(i);
            garage.m_nType = garage.m_nOriginalType;
            garage.InitDoorsAtStart();
            garage.m_GarageAudio.Reset();
        }
    }

    NoResprays = false;

    for (auto& safeHouseCars : aCarsInSafeHouse) { // TODO: Seems like inlined?
        for (auto& car : safeHouseCars) {
            car.Init();
        }
    }
}

// 0x4471B0
void CGarages::Shutdown() {
    NumGarages = 0; // Como el original: pone a cero el número de garajes antes de resetear el audio
    for (auto& garage : aGarages) {
        garage.m_GarageAudio.Reset();
    }
}

// 0x44C8C0
void CGarages::Update() {
    ZoneScoped;

    if (CReplay::Mode == eReplayMode::MODE_PLAYBACK || CGameLogic::IsCoopGameGoingOn())
        return;

    bCamShouldBeOutside = false;
    pOldToGarageWeAreIn = TheCamera.m_pToGarageWeAreIn;
    TheCamera.m_pToGarageWeAreIn = nullptr;
    TheCamera.m_pToGarageWeAreInForHackAvoidFirstPerson = nullptr;

    for (int32 i = 0; auto& garage : aGarages) {
        // Como el original: solo se actualizan los garajes con tipo válido
        if (garage.m_nType != eGarageType::INVALID) {
            garage.Update(i);
        }
        i++;
    }

    if ((CTimer::GetFrameCounter() & 0xF) != 12) // todo: magic number
        return;

    // Como el original: se incrementa el índice primero y se revisa el garaje del nuevo índice
    if (++LastUpdatedGarageId >= std::size(aGarages))
        LastUpdatedGarageId = 0;
    auto& garageToCheck = GetGarage(LastUpdatedGarageId);

    if (garageToCheck.m_nType == eGarageType::INVALID)
        return;

    // Como el original: cámara - coordenada (no al revés) y prueba "no mayor que 40" (NaN cuenta como cerca)
    const auto& camPos = TheCamera.GetPosition();
    if (   !(std::abs(camPos.x - garageToCheck.m_fLeftCoord) > 40.f)
        && !(std::abs(camPos.y - garageToCheck.m_fFrontCoord) > 40.f)
    ) {
        garageToCheck.TidyUpGarageClose();
    } else {
        garageToCheck.TidyUpGarage();
    }
}

// 0x448660
void CGarages::GivePlayerDetonator() {
    auto player = FindPlayerPed();
    auto slot = CWeaponInfo::GetWeaponInfo(WEAPON_DETONATOR, eWeaponSkill::STD)->m_nSlot;
    player->GiveWeapon(WEAPON_DETONATOR, 1, true);
    player->m_aWeapons[slot].m_State = WEAPONSTATE_READY;
    player->GetPlayerData()->m_nChosenWeapon = slot;
    if (player->m_nSavedWeapon != WEAPON_UNIDENTIFIED)
        player->m_nSavedWeapon = WEAPON_DETONATOR;
}

// 0x4471E0
// Garage flags
// 0x1	door opens up and rotate
// 0x2	door goes in
// 0x4	camera follow players
// TODO...
// Como el original (0x4471E0..0x4475F3): no comprueba el limite de garajes y `a10` no se lee. Los limites son min/max con `a < b ? a : b` / `a > b ? a : b` (el orden de los operandos decide
// el resultado con NaN) sobre x1, frontX, x2 y la cuarta esquina (frontX + x2) - x1, guardada como float; el EXE los pasa por la x87 (fld/fstp), asi que un NaN senal sale silencioso.
// Las sumas y restas, la raiz y las divisiones se hacen en X87Math.cpp (24 bits de mantisa con exponente extendido, como el EXE).
static float GarageQuietNan(float value) {
    uint32 bits = std::bit_cast<uint32>(value);
    if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0) {
        bits |= 0x00400000u;
    }
    return std::bit_cast<float>(bits);
}

void CGarages::AddOne(float x1, float y1, float z1, float frontX, float frontY, float x2, float y2, float z2, uint8 type, uint32 a10, char* name, uint32 argFlags) {
    CGarage& garage = aGarages[NumGarages];

    const float cornerX = GarageSumDiffCpp(frontX, x2, x1);
    float       minX    = x1 < frontX ? x1 : frontX;
    minX                = minX < x2 ? minX : x2;
    garage.m_fLeftCoord = GarageQuietNan(minX < cornerX ? minX : cornerX);
    float maxX           = x1 > frontX ? x1 : frontX;
    maxX                 = maxX > x2 ? maxX : x2;
    garage.m_fRightCoord = GarageQuietNan(maxX > cornerX ? maxX : cornerX);

    const float cornerY  = GarageSumDiffCpp(frontY, y2, y1);
    float       minY     = y1 < frontY ? y1 : frontY;
    minY                 = minY < y2 ? minY : y2;
    garage.m_fFrontCoord = GarageQuietNan(minY < cornerY ? minY : cornerY);
    float maxY           = y1 > frontY ? y1 : frontY;
    maxY                 = maxY > y2 ? maxY : y2;
    garage.m_fBackCoord  = GarageQuietNan(maxY > cornerY ? maxY : cornerY);

    garage.m_vPosn.x = x1;
    garage.m_vPosn.y = y1;
    garage.m_vPosn.z = z1;
    const float dirAx = GarageDiffCpp(frontX, x1);
    const float dirAy = GarageDiffCpp(frontY, y1);
    const float dirBx = GarageDiffCpp(x2, x1);
    const float dirBy = GarageDiffCpp(y2, y1);
    garage.m_fTopZ = z2;

    GarageNormalizeCpp(dirAx, dirAy, &garage.m_vDirectionA.x, &garage.m_vDirectionA.y, &garage.m_fWidth);
    GarageNormalizeCpp(dirBx, dirBy, &garage.m_vDirectionB.x, &garage.m_vDirectionB.y, &garage.m_fHeight);

    garage.m_nType         = static_cast<eGarageType>(type);
    garage.m_nOriginalType = static_cast<eGarageType>(type);
    strncpy(garage.m_anName, name, 7);

    garage.m_bDoorOpensUp         = (argFlags & 1) != 0;
    garage.m_bDoorGoesIn          = (argFlags & 2) != 0;
    garage.m_bCameraFollowsPlayer = (argFlags & 4) != 0;
    NumGarages++;
}

// 0x44A170
void CGarages::CloseHideOutGaragesBeforeSave() {
    for (auto& garage : aGarages) {
        if (garage.IsHideOut() && garage.m_nDoorState != GARAGE_DOOR_CLOSED) { // Como el original: cualquier estado no cerrado
            garage.SetClosed();
            garage.StoreAndRemoveCarsForThisHideOut(GetStoredCarsInSafehouse(FindSafeHouseIndexForGarageType(garage.m_nType)), 4); // todo: 4 is NUM_GARAGE_STORED_CARS?
            garage.RemoveCarsBlockingDoorNotInside();
            garage.ResetDoorPosition();
        }
    }
}

// 0x449E60
void CGarages::PlayerArrestedOrDied() {
    for (auto& garage : aGarages) {
        if (garage.m_nType != eGarageType::INVALID) {
            garage.PlayerArrestedOrDied();
        }
    }

    MessageEndTime = 0;
    MessageStartTime = 0;
}

// 0x448B30
void CGarages::AllRespraysCloseOrOpen(bool bOpen) {
    if (NumGarages) {
        // Como el original: el número de garajes se lee una vez (sin signo)
        auto* garage = &aGarages[0];
        for (auto n = (uint32)NumGarages; n; n--, garage++) {
            if (garage->m_nType == eGarageType::PAYNSPRAY) {
                garage->m_nDoorState = bOpen ? GARAGE_DOOR_OPEN : GARAGE_DOOR_CLOSED;
            }
        }
    }
}

// 0x448AF0
bool CGarages::IsModelIndexADoor(int32 model) {
    if (model >= 0) {
        if (const auto mi = CModelInfo::GetModelInfo(model)) {
            // Como el original: AsAtomicModelInfoPtr se llama dos veces (comprobación y uso)
            if (mi->AsAtomicModelInfoPtr()) {
                return mi->AsAtomicModelInfoPtr()->IsGarageDoor();
            }
        }
    }
    return false;
}

// wrong
// 0x4489F0
int32 CGarages::FindSafeHouseIndexForGarageType(eGarageType type) {
    // 0x4489F4..0x448A78: tabla exacta, Ganton (16) también devuelve 0.
    static constexpr std::array<int32,30> indices{0,1,2,0,0,0,0,0,3,4,5,6,7,8,9,10,11,12,13,14,0,0,0,15,16,17,18,0,0,19};
    const auto index=static_cast<uint32>(type)-16u;
    return index<indices.size() ? indices[index] : 0;

    switch (type) {
    case SAFEHOUSE_SANTAMARIA:     return 1;
    case SAGEHOUSE_ROCKSHORE:      return 2;
    case SAFEHOUSE_FORTCARSON:     return 3;
    case SAFEHOUSE_VERDANTMEADOWS: return 4;
    case SAFEHOUSE_DILLIMORE:      return 5;
    case SAFEHOUSE_PRICKLEPINE:    return 6;
    case SAFEHOUSE_WHITEWOOD:      return 7;
    case SAFEHOUSE_PALOMINOCREEK:  return 8;
    case SAFEHOUSE_REDSANDSWEST:   return 9;
    case SAFEHOUSE_ELCORONA:       return 10;
    case SAFEHOUSE_MULHOLLAND:     return 11;
    case IMPOUND_LS:               return 12;
    case IMPOUND_SF:               return 13;
    case IMPOUND_LV:               return 14;
    case SAFEHOUSE_CALTONHEIGHTS:  return 15;
    case SAFEHOUSE_PARADISO:       return 16;
    case SAFEHOUSE_DOHERTY:        return 17;
    case SAFEHOUSE_HASHBURY:       return 18;
    case HANGAR_ABANDONED_AIRPORT: return 19;
    default:                       return 0;
    }
}

// 0x44A240
// Como el original: se elige el garaje cuya posicion central queda mas cerca del objeto (el primero con la distancia minima
// estrictamente menor), entre los que contienen el punto con radio 7. El centro es posicion + direccionA·ancho/2 + direccionB·alto/2
// (cada producto y suma se redondea como el FPU a 24 bits; la z usa productos por 0). Sin garajes el EXE sale con EAX = -1, ECX = 0 y
// EDX intacto; con garajes deja ECX = fin del arreglo recorrido y EDX = matriz del objeto (ResidueRegsCpp / ResidueEcx::Get, ver X87Math.h).
// Sin comprobaciones de ejecucion para que el codigo posterior a la ultima llamada no cambie ECX/EDX.
#pragma runtime_checks("", off)
uint32 CGarages::FindGarageForObjectRegs(CObject* object) {
    if (static_cast<uint32>(NumGarages) == 0) {
        return reinterpret_cast<ResidueEcx*>(static_cast<uintptr_t>(0))->Get(0xFFFFFFFFu);
    }
    uint32 best    = 0xFFFFFFFFu;
    float  minDist = std::bit_cast<float>(0x47C34FF3u);
    const auto* const matrix = object->m_matrix;
    const CVector&    objPos = matrix ? matrix->GetPosition() : object->m_placement.m_vPosn;

    uint32 i = 0;
    do {
        auto& garage = aGarages[i];
        if (garage.IsPointInsideGarage(objPos, 7.0f)) {
            const float halfWidth  = garage.m_fWidth * 0.5f;
            const float ax         = garage.m_vDirectionA.x * halfWidth;
            const float ay         = garage.m_vDirectionA.y * halfWidth;
            const float az         = halfWidth * 0.0f;
            const float x1         = ax + garage.m_vPosn.x;
            const float y1         = ay + garage.m_vPosn.y;
            const float z1         = az + garage.m_vPosn.z;
            const float halfHeight = garage.m_fHeight * 0.5f;
            const float bx         = garage.m_vDirectionB.x * halfHeight;
            const float by         = garage.m_vDirectionB.y * halfHeight;
            const float bz         = halfHeight * 0.0f;
            const float dx         = objPos.x - (bx + x1);
            const float dy         = objPos.y - (by + y1);
            const float dz         = objPos.z - (bz + z1);
            const float dist       = std::sqrt(dz * dz + dy * dy + dx * dx);
            if (dist < minDist) {
                minDist = dist;
                best    = i;
            }
        }
        i++;
    } while (i < static_cast<uint32>(NumGarages));

    const uint32 arrayEnd = static_cast<uint32>(reinterpret_cast<uintptr_t>(&aGarages[0])) + i * static_cast<uint32>(sizeof(CGarage));
    return ResidueRegsCpp(arrayEnd, static_cast<uint32>(reinterpret_cast<uintptr_t>(matrix)), best);
}
#pragma runtime_checks("", restore)

// 0x447680
int16 CGarages::FindGarageIndex(char* name) {
    for (auto i = 0; i < NumGarages; i++) {
        if (_stricmp(name, GetGarage(i).m_anName) == 0)
            return i;
    }
    return -1;
}

// 0x449C30
float CGarages::FindDoorHeightForMI(uint32 modelIndex) {
    const auto* mi = CModelInfo::GetModelInfo(modelIndex);
    return mi->GetColModel()->GetBoundingBox().GetHeight() - 0.1f;
}

// 0x448900
bool CGarages::IsPointWithinHideOutGarage(Const CVector& point) {
    for (auto& garage : aGarages) {
        if (garage.IsHideOut()) {
            // Como el original: sigue buscando en el resto de escondites (sin break)
            if (garage.IsPointInsideGarage(point))
                return true;
        }
    }
    return false;
}

// 0x447D00
bool CGarages::IsGarageOpen(int16 garageId) {
    return GetGarage(garageId).IsOpen();
}

// 0x447D30
bool CGarages::IsGarageClosed(int16 garageId) {
    return GetGarage(garageId).IsClosed();
}

// 0x4479A0
bool CGarages::IsCarSprayable(CVehicle* vehicle) {
    if (vehicle->IsLawEnforcementVehicle() || vehicle->IsSubBMX())
        return false;

    switch ((int16)vehicle->m_nModelIndex) { // Como el original: movsx (con signo)
    case eModelID::MODEL_BUS:
    case eModelID::MODEL_COACH:
    case -2: // Como el original: compara con -2 (no con MODEL_ARTICT1)
    case eModelID::MODEL_FIRETRUK:
    case eModelID::MODEL_AMBULAN:
        return false;
    }
    return true;
}

// 0x449740
bool CGarages::IsThisCarWithinGarageArea(int16 garageId, CEntity* entity) {
    return GetGarage(garageId).IsEntityEntirelyInside3D(entity, 0.0f);
}

// 0x448990
bool CGarages::IsPointWithinAnyGarage(CVector& point) {
    for (auto& garage : aGarages) {
        if (garage.m_nType != eGarageType::INVALID || garage.IsPointInsideGarage(point)) {
            return true;
        }
    }
    return false;
}

// 0x449BA0
bool CGarages::IsPointInAGarageCameraZone(CVector point) {
    for (auto& garage : aGarages) {
        if (garage.m_nType && garage.IsPointInsideGarage(point, 0.5f)) // todo: m_nType == ONLY_TARGET_VEH or != INVALID
            return true;
    }
    return false;
}

// 0x447CD0
void CGarages::ActivateGarage(int16 garageId) {
    auto& garage = GetGarage(garageId);
    garage.m_bInactive = false;
    if (   garage.m_nType == eGarageType::UNKN_CLOSESONTOUCH
        && garage.m_nDoorState == GARAGE_DOOR_CLOSED // Como el original: solo si está cerrada
    ) {
        garage.m_nDoorState = GARAGE_DOOR_OPENING;
    }
}

// 0x447CB0
void CGarages::DeActivateGarage(int16 garageId) {
    GetGarage(garageId).m_bInactive = true;
}

// 0x447C40
void CGarages::SetTargetCarForMissionGarage(int16 garageId, CVehicle* vehicle) {
    auto& garage = GetGarage(garageId);
    if (vehicle) {
        garage.m_pTargetCar = vehicle;
        vehicle->RegisterReference(reinterpret_cast<CEntity**>(&garage.m_pTargetCar));
        if (garage.m_nDoorState == GARAGE_DOOR_CLOSED_DROPPED_CAR)
            garage.SetClosed();
    } else {
        // Como el original: no limpia la referencia anterior (CleanUpOldReference)
        garage.m_pTargetCar = nullptr;
    }
}

// 0x447B80
void CGarages::TriggerMessage(const char* tagMsg, int16 numInStr1, uint16 time, int16 numInStr2) {
    if (   strcmp(tagMsg, MessageIDString) != 0 // Different strings
        || CTimer::GetTimeInMS() < MessageStartTime
        || CTimer::GetTimeInMS() > MessageEndTime // Como el original: estrictamente mayor (ja)
    ) {
        strcpy(MessageIDString, tagMsg); // Como el original: strcpy sin relleno de depuración
        MessageStartTime = CTimer::GetTimeInMS();
    } else {
        if (CTimer::GetTimeInMS() - MessageStartTime <= 500)
            return;
        MessageStartTime = CTimer::GetTimeInMS() - 500;
    }
    MessageEndTime = MessageStartTime + time;
    // Como el original: el primer número va a 0x96C010 (MessageNumberInString2) y el segundo a 0x96C00C
    MessageNumberInString2 = numInStr1;
    MessageNumberInString = numInStr2;
}

// 0x447790
void CGarages::PrintMessages() {
    if (   CTimer::GetTimeInMS() < MessageStartTime
        || CTimer::GetTimeInMS() >= MessageEndTime
    ) {
        // Time over
        MessageIDString[0] = 0;
        return;
    }

    // Draw it
    // Como el original: ancho * (1/640) * k y alto * (1/448) * k redondeados a float en ese orden
    const float fW = (float)RsGlobal.maximumWidth, fH = (float)RsGlobal.maximumHeight;
    const float fInvW = 1.f / 640.f, fInvH = 1.f / 448.f;
    CFont::SetScale(fW * fInvW * 0.5f, fH * fInvH * 1.4f);
    CFont::SetProportional(true);
    CFont::SetBackground(false, false);
    CFont::SetCentreSize(fW - fW * fInvW * 230.f);
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetFontStyle(eFontStyle::FONT_MENU);
    CFont::SetColor(HudColour.GetRGB(eHudColours::HUD_COLOUR_LIGHT_BLUE));
    CFont::SetEdge(2);
    CFont::SetDropColor({ 0, 0, 0, 255 });

    const auto DrawFormattedString = [&](auto... formatArgs) {
        CMessages::InsertNumberInString(TheText.Get(MessageIDString), formatArgs..., gGxtString);
        CFont::PrintString((float)(RsGlobal.maximumWidth / 2), fH * fInvH * 155.f, gGxtString);
    };

    // Como el original: x = mitad entera del ancho (división entera con signo)
    if (MessageNumberInString < 0) {
        if (MessageNumberInString2 < 0) {
            const auto text = TheText.Get(MessageIDString);
            CFont::PrintString((float)(RsGlobal.maximumWidth / 2), fH * fInvH * 155.f, text);
        } else {
            DrawFormattedString(MessageNumberInString2, -1, -1, -1, -1, -1);
        }
    } else {
        // Como el original: primero 0x96C010 y luego 0x96C00C
        DrawFormattedString(MessageNumberInString2, MessageNumberInString, -1, -1, -1, -1);
    }
}

// 0x4476D0
void CGarages::ChangeGarageType(int16 garageId, eGarageType type, uint32 unused) {
    auto& garage = GetGarage(garageId); // Como el original: modifica el garaje real (no una copia)
    garage.m_nType = type;

    if (type < BOMBSHOP_TIMED) {
        garage.SetClosed();
        garage.ResetDoorPosition();
        return;
    }

    if (type > PAYNSPRAY) {
        if (type == BURGLARY)
            return;

        garage.SetClosed();
        garage.ResetDoorPosition();
        return;
    }

    if (garage.IsClosed()) {
        garage.SetOpened();
        garage.m_fDoorPosition = 1.0f;
    }
}

// 0x447680
int16 CGarages::GetGarageNumberByName(const char* name) {
    // Como el original: índice int16 comparado sin signo con NumGarages
    for (int16 i = 0; (uint32)i < (uint32)NumGarages; i++) {
        const auto& garage = aGarages[i];
        if (_stricmp(name, garage.m_anName) == 0)
            return i;
    }
    return -1;
}

// 0x5D3270
bool CGarages::Load() {
    CloseHideOutGaragesBeforeSave();

    CGenericGameStorage::LoadDataFromWorkBuffer(NumGarages);
    CGenericGameStorage::LoadDataFromWorkBuffer(BombsAreFree);
    CGenericGameStorage::LoadDataFromWorkBuffer(RespraysAreFree);
    CGenericGameStorage::LoadDataFromWorkBuffer(NoResprays);
    CGenericGameStorage::LoadDataFromWorkBuffer(CarsCollected);
    CGenericGameStorage::LoadDataFromWorkBuffer(BankVansCollected);
    CGenericGameStorage::LoadDataFromWorkBuffer(PoliceCarsCollected);

    for (auto& v : CarTypesCollected) {
        CGenericGameStorage::LoadDataFromWorkBuffer(v);
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(LastTimeHelpMessage);

    // NOTE: Here they messed up the order of loops
    //       C/C++ is row-major, so, the row loop should've been the outer one..
    //       This is important, because the array isn't accessed contiguously
    //       Which means the data isn't saved contiguously either, so watch out for that.
    for (auto c = 0; c < 4; c++) {
        for (auto r = 0; r < 20; r++) {
            CGenericGameStorage::LoadDataFromWorkBuffer(aCarsInSafeHouse[r][c]);
        }
    }

    for (uint32 i = 0; i < (uint32)NumGarages; i++) { // Como el original: sin signo (jb)
        auto sg = CGenericGameStorage::LoadDataFromWorkBuffer<CSaveGarage>();
        sg.CopyGarageOutOfSaveGarage(aGarages[i]);
    }

    MessageEndTime = 0;
    MessageStartTime = 0;
    bCamShouldBeOutside = false;

    return true;
}

// todo: fix Update()
// 0x5D3160
bool CGarages::Save() {
    CloseHideOutGaragesBeforeSave(); // Como el original: también al guardar

    CGenericGameStorage::SaveDataToWorkBuffer(NumGarages);
    CGenericGameStorage::SaveDataToWorkBuffer(BombsAreFree);
    CGenericGameStorage::SaveDataToWorkBuffer(RespraysAreFree);
    CGenericGameStorage::SaveDataToWorkBuffer(NoResprays);
    CGenericGameStorage::SaveDataToWorkBuffer(CarsCollected);
    CGenericGameStorage::SaveDataToWorkBuffer(BankVansCollected);
    CGenericGameStorage::SaveDataToWorkBuffer(PoliceCarsCollected);

    for (auto& carType : CarTypesCollected) { // Como el original: guarda desde el global (no una copia)
        CGenericGameStorage::SaveDataToWorkBuffer(carType);
    }

    CGenericGameStorage::SaveDataToWorkBuffer(LastTimeHelpMessage);

    // NOTE: Here they messed up the order of loops
    //       C/C++ is row-major, so, the row loop should've been the outer one..
    //       This is important, because the array isn't accessed contiguously
    //       Which means the data isn't saved contiguously either, so watch out for that.
    for (auto c = 0; c < 4; c++) {
        for (auto r = 0; r < 20; r++) {
            CGenericGameStorage::SaveDataToWorkBuffer(aCarsInSafeHouse[r][c]);
        }
    }

    for (uint32 i = 0; i < (uint32)NumGarages; i++) { // Como el original: sin signo (jb)
        CSaveGarage sg; // Como el original: sin inicializar (el relleno queda con basura de pila)
        sg.CopyGarageIntoSaveGarage(aGarages[i]);
        CGenericGameStorage::SaveDataToWorkBuffer(sg);
    }

    return true;
}

// 0x44A3C0
// Como el original (0x44A3C0..0x44A4C6): entre los garajes de embargo (tipos 0x21..0x23) elige el mas cercano en XY (el primero con distancia estrictamente menor; distancia inicial 0x47C34FF3)
// y guarda el coche en el primer hueco del escondite asociado, contando solo las tres primeras ranuras (modelo > 0); con las tres ocupadas desplaza la 1 a la 0 y la 2 a la 1 y usa la 2.
// La distancia sale de X87Math.cpp (GarageImpoundCloserCpp: sqrt(dy*dy + dx*dx) sin redondear antes de comparar). Sus dos llamadores del EXE no leen EAX/ECX/EDX.
void CGarages::StoreCarInNearestImpoundingGarage(CVehicle* vehicle) {
    int32 best    = -1;
    float minDist = std::bit_cast<float>(0x47C34FF3u);
    for (uint32 i = 0; i < static_cast<uint32>(NumGarages); i++) {
        const uint8 type = static_cast<uint8>(aGarages[i].m_nType);
        if (type < 0x21 || type > 0x23)
            continue;
        if (GarageImpoundCloserCpp(&aGarages[i].m_vPosn.x, &vehicle->GetPosition().x, &minDist)) {
            best = static_cast<int32>(i);
        }
    }

    if (best < 0)
        return;

    CStoredCar* const stored = GetStoredCarsInSafehouse(FindSafeHouseIndexForGarageType(aGarages[best].m_nType));
    int32 count = 0;
    if (stored[0].m_wModelIndex > 0) count = 1;
    if (stored[1].m_wModelIndex > 0) count++;
    if (stored[2].m_wModelIndex > 0) count++;
    if (count == 3) {
        std::memcpy(&stored[0], &stored[1], sizeof(CStoredCar));
        std::memcpy(&stored[1], &stored[2], sizeof(CStoredCar));
        count = 2;
    }
    stored[count].StoreCar(vehicle);
}

// unused
// 0x448890
void CGarages::StopCarFromBlowingUp(CAutomobile* vehicle) {
    return plugin::Call<0x448890, CVehicle*>(vehicle);

    // untested
    vehicle->m_fBurnTimer = 0.0f;
    vehicle->m_fHealth = vehicle->m_fHealth <= 300.0f ? 300.0f : vehicle->m_fHealth;

    auto& manager = vehicle->m_damageManager;
    if (manager.GetEngineStatus() >= 275) {
        manager.SetEngineStatus(manager.GetEngineStatus());
    } else {
        manager.SetEngineStatus(275u);
    }
}

// untested, unused (inlined?)
// 0x447E10
bool CGarages::HasResprayHappened(int16 garageId) {
    auto& garage = GetGarage(garageId);
    const auto resprayed = garage.m_bUsedRespray;
    garage.m_bUsedRespray = false;
    return resprayed;
}

// 0x44A210
int32 CGarages::CountCarsInHideoutGarage(eGarageType type) {
    // Como el original: el argumento se lee como byte (movzx), el indice sale de FindSafeHouseIndexForGarageType (0x4489F0, una sola vez) y se cuentan las 4
    // ranuras del refugio con modelo (m_wModelIndex, +0x12 de cada CStoredCar de 0x40 bytes; el original lee la tabla a partir de 0x96ABEA). El indice no se acota.
    // Su unico llamador del EXE (0x44BF27) solo lee EAX.
    const CStoredCar* const slots = GetStoredCarsInSafehouse(FindSafeHouseIndexForGarageType(static_cast<eGarageType>(static_cast<uint8>(type))));
    int32 count = 0;
    for (int32 i = 0; i < MAX_CARS_IN_SAFEHOUSE; ++i) {
        if (slots[i].m_wModelIndex != 0) {
            ++count;
        }
    }
    return count;
}
