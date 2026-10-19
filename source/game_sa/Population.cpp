/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPopulation
 * Reconstrucción: COMPLETA — 67/67 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 67/67 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Population.h"
#include "Glass.h"
#include "X87Math.h"
#include <PedPlacement.h>
#include <Attractors/PedAttractorPedPlacer.h>

#include <TaskTypes/TaskComplexWanderCop.h>
#include <TaskTypes/TaskSimpleStandStill.h>
#include <TaskTypes/TaskComplexWander.h>
#include <TaskTypes/TaskComplexDie.h>
#include <TaskTypes/TaskComplexKillPedOnFoot.h>
#include <TaskTypes/TaskComplexBeInCouple.h>
#include <TaskTypes/TaskComplexFollowLeaderInFormation.h>
#include <TaskTypes/TaskSimpleHoldEntity.h>

#include <Events/EventSexyPed.h>
#include "Events/EventAcquaintancePedHate.h"

//! Define this to have extra NOTSA_LOG_DEBUG's of CPopulation
#define EXTRA_DEBUG_LOGS

#ifdef EXTRA_DEBUG_LOGS
#define POP_LOG_DEBUG NOTSA_LOG_DEBUG
#else
#define POP_LOG_DEBUG(...)
#endif

void CPopulation::InjectHooks() {
    RH_ScopedClass(CPopulation);
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(FindPedRaceFromName, 0x5B6D40);
    RH_ScopedGlobalInstall(RandomIntInLimitRange, 0x610DB0);

    RH_ScopedGlobalInstall(LoadPedGroups, 0x5BCFE0);
    RH_ScopedGlobalInstall(LoadCarGroups, 0x5BD1A0);

    RH_ScopedGlobalInstall(DoesCarGroupHaveModelId, 0x406F50);
    RH_ScopedGlobalInstall(ManagePed, 0x611FC0);
    RH_ScopedGlobalInstall(FindNumberOfPedsWeCanPlaceOnBenches, 0x612240);
    RH_ScopedGlobalInstall(RemoveAllRandomPeds, 0x6122C0);
    RH_ScopedGlobalInstall(TestRoomForDummyObject, 0x612320);
    RH_ScopedGlobalInstall(TestSafeForRealObject, 0x6123A0);
    RH_ScopedGlobalInstall(AddPed, 0x612710);
    RH_ScopedGlobalInstall(AddDeadPedInFrontOfCar, 0x612CD0);
    RH_ScopedGlobalInstall(ChooseCivilianOccupation, 0x612F90);
    RH_ScopedGlobalInstall(ChooseCivilianCoupleOccupations, 0x613180);
    RH_ScopedGlobalInstall(ChooseCivilianOccupationForVehicle, 0x613260);
    RH_ScopedGlobalInstall(CreateWaitingCoppers, 0x6133F0);
    RH_ScopedGlobalInstall(AddPedInCar, 0x613A00);
    RH_ScopedGlobalInstall(PlaceMallPedsAsStationaryGroup, 0x613CD0);
    RH_ScopedGlobalInstall(PlaceCouple, 0x613D60);
    RH_ScopedGlobalInstall(AddPedAtAttractor, 0x614210);
    RH_ScopedGlobalInstall(FindDistanceToNearestPedOfType, 0x6143E0);
    RH_ScopedGlobalInstall(PickGangCar, 0x614490);
    RH_ScopedGlobalInstall(PickRiotRoadBlockCar, 0x6144B0);
    RH_ScopedGlobalInstall(ConvertToRealObject, 0x614580);
    RH_ScopedGlobalInstall(ConvertToDummyObject, 0x614670);
    RH_ScopedGlobalInstall(AddToPopulation, 0x614720);
    RH_ScopedGlobalInstall(GeneratePedsAtAttractors, 0x615970);
    RH_ScopedGlobalInstall(GeneratePedsAtStartOfGame, 0x615C90);
    RH_ScopedGlobalInstall(ManageObject, 0x615DC0);
    RH_ScopedGlobalInstall(ManageDummy, 0x616000);
    RH_ScopedGlobalInstall(ManageAllPopulation, 0x6160A0);
    RH_ScopedGlobalInstall(ManagePopulation, 0x616190);
    RH_ScopedGlobalInstall(RemovePedsIfThePoolGetsFull, 0x616300);
    RH_ScopedGlobalInstall(ConvertAllObjectsToDummyObjects, 0x616420);
    RH_ScopedGlobalInstall(FindPedMultiplierMotorway, 0x611B80);
    RH_ScopedGlobalInstall(FindCarMultiplierMotorway, 0x611B60);
    RH_ScopedGlobalInstall(IsCorrectTimeOfDayForEffect, 0x611B20);
    RH_ScopedGlobalInstall(RemoveSpecificDriverModelsForCar, 0x6119D0);
    RH_ScopedGlobalInstall(Initialise, 0x610E10);
    RH_ScopedGlobalInstall(Shutdown, 0x610EC0);
    RH_ScopedGlobalInstall(FindDummyDistForModel, 0x610ED0);
    RH_ScopedGlobalInstall(FindPedDensityMultiplierCullZone, 0x610F00);
    RH_ScopedGlobalInstall(RemovePed, 0x610F20);
    RH_ScopedGlobalInstall(ChoosePolicePedOccupation, 0x610F40);
    RH_ScopedGlobalInstall(ArePedStatsCompatible, 0x610F50);
    RH_ScopedGlobalInstall(PedMICanBeCreatedAtAttractor, 0x6110C0);
    RH_ScopedGlobalInstall(PedMICanBeCreatedAtThisAttractor, 0x6110E0);
    RH_ScopedGlobalInstall(PedMICanBeCreatedInInterior, 0x611450);
    RH_ScopedGlobalInstall(IsMale, 0x611470);
    RH_ScopedGlobalInstall(PopulateInterior, 0x616470);
    RH_ScopedGlobalInstall(IsFemale, 0x611490);
    RH_ScopedGlobalInstall(IsSkateable, 0x6114C0);
    RH_ScopedGlobalInstall(ChooseGangOccupation, 0x611550);
    RH_ScopedGlobalInstall(AddExistingPedInCar, 0x611560);
    RH_ScopedGlobalInstall(UpdatePedCount, 0x611570);
    RH_ScopedGlobalInstall(MoveCarsAndPedsOutOfAbandonedZones, 0x6116A0);
    RH_ScopedGlobalInstall(DealWithZoneChange, 0x6116B0);
    RH_ScopedGlobalInstall(PedCreationDistMultiplier, 0x6116C0);
    RH_ScopedGlobalInstall(CanSolicitPlayerOnFoot, 0x611780);
    RH_ScopedGlobalInstall(CanSolicitPlayerInCar, 0x611790);
    RH_ScopedGlobalInstall(CanJeerAtStripper, 0x6117B0);
    RH_ScopedGlobalInstall(PlaceGangMembers, 0x6117D0);
    RH_ScopedGlobalInstall(LoadSpecificDriverModelsForCar, 0x6117F0);
    RH_ScopedGlobalInstall(FindSpecificDriverModelForCar_ToUse, 0x611900);
    RH_ScopedGlobalInstall(IsSecurityGuard, 0x6114B0);
    RH_ScopedGlobalInstall(Update, 0x616650);

    RH_ScopedGlobalOverloadedInstall(IsSunbather, "ModelID", 0x611760, bool(*)(eModelID));
    //RH_ScopedGlobalOverloadedInstall(IsSunbather, "Ped", 0x611760, bool(*)(CPed*));
}

// 0x610DB0
// Como el original: lee primero el limite superior (0x86C6C8) y el inferior (0x86C6C4), tira el RNG (el EXE usa rand, 0x821B1E) y enmascara a 16 bits;
// fild * 2^-15 (0x858B14), fimul (superior - inferior) y _ftol2 en X87Math.cpp (PopulationRandomRangeCpp); suma entera del limite inferior.
// Unico llamador del EXE: CPopulation::AddToPopulation (0x614AC6), que solo guarda EAX (recarga ECX y EDX), asi que no hay residuo que reproducir.
int32 CPopulation::RandomIntInLimitRange() {
    const int32 upper = StaticRef<int32>(0x86C6C8);
    const int32 lower = StaticRef<int32>(0x86C6C4);
    const uint32 random = CGeneral::GetRandomNumber() & 0xFFFFu;
    return static_cast<int32>(static_cast<uint32>(lower) + static_cast<uint32>(PopulationRandomRangeCpp(static_cast<int>(random), static_cast<int>(static_cast<uint32>(upper) - static_cast<uint32>(lower)))));
}

// 0x5B6D40
// Como el original (0x5B6D40..0x5B6D74): mira solo las dos primeras letras, con toupper en linea ('a'..'z') y sin
// cortar en el NUL (el EXE sigue con name[1] aunque name[0] sea 0). Tabla: B->1, W->2, O/I->3, H->4.
ePedRace CPopulation::FindPedRaceFromName(const char* modelName) {
    for (size_t i{}; i < 2; i++) {
        uint8 c = static_cast<uint8>(modelName[i]);
        if (c >= 'a' && c <= 'z') {
            c -= 0x20;
        }
        switch (c) {
        case 'B': return RACE_BLACK;
        case 'H': return RACE_HISPANIC;
        case 'O':
        case 'I': return RACE_ORIENTAL;
        case 'W': return RACE_WHITE;
        }
    }
    return RACE_DEFAULT;
}

//! Como el original (0x5BCFE0 pedgrp.dat / 0x5BD1A0 cargrp.dat): se lee con CFileMgr::ReadLine; ',' y '\r' pasan a ' ' hasta el
//! '\n' (que pasa a NUL); cada linea admite como mucho 21 (peds) / 23 (coches) fichas; cada ficha se resuelve con
//! CModelInfo::GetModelInfo(nombre, &indice) y solo se guarda (como int16) si existe; los huecos se rellenan con 2000.
//! Diferencia entre los dos EXE (0x5BD122 / 0x5BD2DE): en peds se crea un grupo por cada linea con al menos una ficha (aunque
//! ningun modelo exista: cuenta 0); en coches solo si se hallo al menos un modelo.
//! NOTSA: el EXE no limita el numero de grupos (corrompe memoria): aqui se detiene al llenar el arreglo.
template<bool IsPedFile, size_t ModelsPerGroup>
void LoadGroupFile(const char* fileName, auto& outModelsInGroup, auto& outNumOfModelsPerGroup) {
    CFileMgr::ChangeDir("\\DATA\\");
    const auto file = CFileMgr::OpenFile(fileName, "r");
    CFileMgr::ChangeDir("\\");

    size_t grp{};
    char   line[0x400];
    while (grp < outModelsInGroup.size() && CFileMgr::ReadLine(file, line, static_cast<int32>(sizeof(line)))) {
        // ',' y '\r' -> ' ' hasta el '\n', que pasa a NUL
        size_t len{};
        if (line[0] != '\n') {
            char next;
            do {
                if (line[len] == ',' || line[len] == '\r') {
                    line[len] = ' ';
                }
                next = line[len + 1];
                len++;
            } while (next != '\n' && next != '\0');
        }
        line[len] = '\0';

        auto&& models = outModelsInGroup[grp];
        size_t pos{};
        size_t numTokens{};
        size_t numModels{};
        bool   anyToken{};
        for (;;) {
            // Salta los caracteres <= 0x20 (sin pasar del NUL)
            while (static_cast<uint8>(line[pos]) <= 0x20 && line[pos] != '\0') {
                pos++;
            }
            const uint8 first = static_cast<uint8>(line[pos]);
            if (first == '#' || first <= 0x20) {
                break; // comentario o fin de linea
            }
            size_t end = pos;
            do {
                end++;
            } while (static_cast<uint8>(line[end]) > 0x20);

            char name[0x400];
            std::memcpy(name, line + pos, end - pos);
            name[end - pos] = '\0';

            int32 modelIdx{};
            if (CModelInfo::GetModelInfo(name, &modelIdx)) {
                models[numModels++] = static_cast<int16>(modelIdx);
            }
            numTokens++;
            anyToken = true;
            pos      = end;
            if (numTokens >= ModelsPerGroup) {
                break;
            }
        }

        if constexpr (IsPedFile) {
            if (!anyToken) {
                continue; // linea en blanco o comentario
            }
        } else {
            if (numModels == 0) {
                continue;
            }
        }
        outNumOfModelsPerGroup[grp] = static_cast<uint16>(numModels);
        for (size_t k = numModels; k < ModelsPerGroup; k++) {
            models[k] = static_cast<int16>(CPopulation::m_DefaultModelIDForUnusedSlot);
        }
        grp++;
    }

    CFileMgr::CloseFile(file);
}

// 0x5BCFE0
void CPopulation::LoadPedGroups() {
    LoadGroupFile<true, 21>("PEDGRP.DAT", m_PedGroups, m_nNumPedsInGroup);
}

// 0x5BD1A0
void CPopulation::LoadCarGroups() {
    LoadGroupFile<false, 23>("CARGRP.DAT", m_CarGroups, m_nNumCarsInGroup);
}

// 0x610E10
void CPopulation::Initialise() {
    ms_nNumCivMale = 0;
    ms_nNumCivFemale = 0;
    ms_nNumCop = 0;
    ms_nNumEmergency = 0;
    rng::fill(ms_nNumGang, 0);
    ms_nTotalCarPassengerPeds = 0;
    ms_nTotalCivPeds = 0;
    ms_nTotalGangPeds = 0;
    ms_nTotalPeds = 0;
    ms_nTotalMissionPeds = 0;
    m_CountDownToPedsAtStart = 5;
    bZoneChangeHasHappened = 0;
    PedDensityMultiplier = 1.0f;
    m_AllRandomPedsThisType = -1;
    MaxNumberOfPedsInUse = 25;
    NumberOfPedsInUseInterior = 40;
    bInPoliceStation = 0;
    m_bDontCreateRandomGangMembers = 0;
    m_bOnlyCreateRandomGangMembers = 0;
    m_bDontCreateRandomCops = 0;
    m_bMoreCarsAndFewerPeds = 0;
    LoadPedGroups();
    LoadCarGroups();
}

// 0x610EC0
void CPopulation::Shutdown() {
    /* nop */
}

// 0x610ED0
float CPopulation::FindDummyDistForModel(eModelID modelIndex) {
    return (modelIndex == ModelIndices::MI_SAMSITE || modelIndex == ModelIndices::MI_SAMSITE2)
        ? 750.f
        : 80.f;
}

// 0x610F00
float CPopulation::FindPedDensityMultiplierCullZone() {
    return CCullZones::FewerPeds()
        ? 0.6f
        : 1.f;
}

// 0x610F20
void CPopulation::RemovePed(CPed* ped) {
    CWorld::Remove(ped);
    delete ped;
}

// 0x610F40
eModelID CPopulation::ChoosePolicePedOccupation() {
    return (eModelID)COP_TYPE_CITYCOP; // See CCopPed::GetPedModelForCopType
}

// 0x610F50
// Como el original (0x610F50..0x6110B9): false si alguno de los dos es PLAYER/COP/MEDIC/FIREMAN (0..3), de TRAMP_MALE a
// COWARD (0x1A..0x2A) o GANG1..GANG7 (4..10, comparaciones con signo); si el primero es OLD_GUY (0x12) / OLD_GIRL (0x18) el
// segundo tambien debe serlo. (La version anterior retornaba en la primera vuelta del bucle y nunca miraba el segundo.)
bool CPopulation::ArePedStatsCompatible(ePedStats st1, ePedStats st2) {
    const auto a = static_cast<int32>(st1), b = static_cast<int32>(st2);
    const auto IsExcluded = [](int32 st) {
        return (st >= 0 && st <= 3) || (st >= 0x1A && st <= 0x2A);
    };
    if (IsExcluded(a) || IsExcluded(b)) {
        return false;
    }
    if ((a >= 4 && a <= 0x0A) || (b >= 4 && b <= 0x0A)) {
        return false;
    }
    const auto IsOld = [](int32 st) {
        return st == 0x12 || st == 0x18;
    };
    return !IsOld(a) || IsOld(b);
}
// 0x6110C0
bool CPopulation::PedMICanBeCreatedAtAttractor(eModelID modelIndex) {
    switch (CModelInfo::GetPedModelInfo(modelIndex)->GetPedType()) {
    case PED_TYPE_DEALER:
    case PED_TYPE_MEDIC:
    case PED_TYPE_FIREMAN:
    case PED_TYPE_CRIMINAL:
    case PED_TYPE_BUM:
    case PED_TYPE_PROSTITUTE:
        return false;
    }
    return true;
}

// 0x6110E0
bool CPopulation::PedMICanBeCreatedAtThisAttractor(eModelID modelId, const char* attrName) {
    if (!attrName) {
        return true;
    }

    const auto NameIsAnyOf = [&](auto... anyOfValues) {
        return (... || (_stricmp(attrName, anyOfValues) == 0));
    };

    const auto pedType = CModelInfo::GetPedModelInfo(modelId)->GetPedType();

    if (NameIsAnyOf("COPSIT", "COPLOOK", "BROWSE")) {
        return pedType == PED_TYPE_COP;
    }

    if (pedType == PED_TYPE_COP) {
        return false;
    }

    if (NameIsAnyOf("DANCER")) {
        switch (modelId) {
        case MODEL_BFYRI:
        case MODEL_BMYRI:
        case MODEL_BMYST:
        case MODEL_HFYRI:
        case MODEL_HMYRI:
        case MODEL_OFYST:
        case MODEL_OMOST:
        case MODEL_OMYRI:
        case MODEL_OMYST:
        case MODEL_WFYRI:
        case MODEL_WFYST:
        case MODEL_WMYRI:
        case MODEL_WMYST:
            return true;
        }
        return false;
    }

    if (NameIsAnyOf("BARGUY", "PEDROUL", "PEDCARD", "PEDSLOT")) {
        switch (modelId) {
        case MODEL_WMYCON:
        case MODEL_HMOGAR:
        case MODEL_WMYMECH:
        case MODEL_SBFYST:
        case MODEL_WMYSGRD:
        case MODEL_SWMYHP1:
        case MODEL_SWMYHP2:
        case MODEL_SWMOTR1:
        case MODEL_SBMOTR2SBMOTR2:
        case MODEL_SWMOTR2:
        case MODEL_SBMYTR3:
        case MODEL_SWMOTR3:
        case MODEL_SBMYST:
        case MODEL_WMYCONB:
        case MODEL_SOMYST:
        case MODEL_SBFOST:
        case MODEL_SOFOST:
        case MODEL_SOFYST:
        case MODEL_SOMOST:
        case MODEL_SWMOTR5:
        case MODEL_SWFOST:
        case MODEL_SWFYST:
        case MODEL_SWMOST:
        case MODEL_SWMOTR4:
            return false; // TODO/BUG: Is this correct? Shouldn't it return true?
        }
        return true;
    }

    if (NameIsAnyOf("STRIPW")) {
        switch (modelId) {
        case MODEL_VWFYST1:
        case MODEL_VBFYST2:
        case MODEL_VHFYST3:
        case MODEL_SBFYSTR:
        case MODEL_SWFYSTR:
            return true;
        }
        return false;
    }

    // Como el original (0x6111F0..0x61120C): STRIPM devuelve "no es PED_TYPE_CIVFEMALE" y cualquier otro nombre devuelve true
    // (la version anterior tenia ambos al reves).
    if (NameIsAnyOf("STRIPM")) {
        return pedType != PED_TYPE_CIVFEMALE;
    }

    return true;
}

// 0x611450
bool CPopulation::PedMICanBeCreatedInInterior(eModelID modelIndex) {
    switch (CModelInfo::GetPedModelInfo(modelIndex)->GetPedType()) {
    case PED_TYPE_COP:
    case PED_TYPE_GANG1:
    case PED_TYPE_GANG2:
    case PED_TYPE_GANG3:
    case PED_TYPE_GANG4:
    case PED_TYPE_GANG5:
    case PED_TYPE_GANG6:
    case PED_TYPE_GANG7:
    case PED_TYPE_GANG8:
    case PED_TYPE_GANG9:
    case PED_TYPE_GANG10:
    case PED_TYPE_DEALER:
    case PED_TYPE_MEDIC:
    case PED_TYPE_FIREMAN:
    case PED_TYPE_CRIMINAL:
    case PED_TYPE_BUM:
    case PED_TYPE_PROSTITUTE:
        return false;
    }
    return true;
}

// 0x611470
bool CPopulation::IsMale(eModelID modelIndex) {
    return CModelInfo::GetPedModelInfo(modelIndex)->GetPedType() == PED_TYPE_CIVMALE;
}

// 0x611490
bool CPopulation::IsFemale(eModelID modelIndex) {
    return CModelInfo::GetPedModelInfo(modelIndex)->GetPedType() == PED_TYPE_CIVFEMALE;
}

// 0x6114B0
bool CPopulation::IsSecurityGuard(ePedType pedType) {
    return false;
}

// 0x6114C0
bool CPopulation::IsSkateable(const CVector& point) {
    CColPoint cp{};
    CEntity* hitEntity{};
    CWorld::ProcessVerticalLine(point + CVector{ 0.f, 0.f, 2.f }, point.z - 2.f, cp, hitEntity, true);
    return hitEntity && g_surfaceInfos.IsSkateable(cp.m_nSurfaceTypeB);
}

// 0x611550
eModelID CPopulation::ChooseGangOccupation(eGangID gangId) {
    return CGangs::ChooseGangPedModel(gangId);
}

// 0x611560 (Unused)
// Como el original (0x611560: mov eax,[esp+4] / ret): devuelve el peaton sin hacer nada mas.
CPed* CPopulation::AddExistingPedInCar(CPed* ped, CVehicle* vehicle) {
    return ped;
}

// 0x611570
void CPopulation::UpdatePedCount(CPed* ped, bool pedAddedOrRemoved) {
    if (pedAddedOrRemoved != ped->bHasBeenAddedToPopulation) {
        return;
    }

    ped->bHasBeenAddedToPopulation = !pedAddedOrRemoved;

    const auto DoOp = [&](auto& value) {
        if (pedAddedOrRemoved) {
            value--;
        } else {
            value++;
        }
    };

    switch (ped->m_nPedType) {
    case PED_TYPE_CIVMALE:
    case PED_TYPE_CRIMINAL:
        DoOp(ms_nNumCivMale);
        break;
    case PED_TYPE_CIVFEMALE:
    case PED_TYPE_PROSTITUTE:
        DoOp(ms_nNumCivFemale);
        break;
    case PED_TYPE_COP:
        DoOp(ms_nNumCop);
        break;
    case PED_TYPE_GANG1:
    case PED_TYPE_GANG2:
    case PED_TYPE_GANG3:
    case PED_TYPE_GANG4:
    case PED_TYPE_GANG5:
    case PED_TYPE_GANG6:
    case PED_TYPE_GANG7:
    case PED_TYPE_GANG8:
    case PED_TYPE_GANG9:
    case PED_TYPE_GANG10:
        // Como el original (0x6115C8: inc [ecx*4+0xC0EC20], es decir ms_nNumGang[tipo - PED_TYPE_GANG1]); sin condicion FIX_BUGS.
        DoOp(ms_nNumGang[ped->m_nPedType - PED_TYPE_GANG1]);
        break;
    case PED_TYPE_DEALER:
        DoOp(ms_nNumDealers);
        break;
    case PED_TYPE_MEDIC:
    case PED_TYPE_FIREMAN:
        DoOp(ms_nNumEmergency);
        break;
    }
}

// 0x6116A0
void CPopulation::MoveCarsAndPedsOutOfAbandonedZones() {
    /* nop */
}

// 0x6116B0
void CPopulation::DealWithZoneChange(eLevelName arg0, eLevelName arg1, bool arg2) {
    /* nop */
}

// 0x6116C0
// Como el original (0x6116C0): tras el primer FindPlayerVehicle(-1, false) no nulo el EXE lo llama 4 veces mas (esi, edi, ebx, eax)
// y calcula sqrt(y(eax)*y(ebx) + x(edi)*x(esi)) - 0.1 + 1.0, limitado a [1, 1.5].
float CPopulation::PedCreationDistMultiplier() {
    if (FindPlayerVehicle(-1, false)) {
        const auto v1 = FindPlayerVehicle(-1, false);
        const auto v2 = FindPlayerVehicle(-1, false);
        const auto v3 = FindPlayerVehicle(-1, false);
        const auto v4 = FindPlayerVehicle(-1, false);
        const float sum = v4->m_vecMoveSpeed.y * v3->m_vecMoveSpeed.y + v2->m_vecMoveSpeed.x * v1->m_vecMoveSpeed.x;
        return std::clamp(std::sqrt(sum) - 0.1f + 1.f, 1.f, 1.5f);
    }
    return 1.f;
}

// 0x611760
bool CPopulation::IsSunbather(eModelID modelIndex) {
    switch (CModelInfo::GetPedModelInfo(modelIndex)->GetPedStatType()) {
    case ePedStats::BEACH_GUY:
    case ePedStats::BEACH_GIRL:
        return true;
    }
    return false;
}

bool CPopulation::IsSunbather(CPed* ped) {
    return IsSunbather(ped->GetModelId());
}

// 0x611780
bool CPopulation::CanSolicitPlayerOnFoot(eModelID modelIndex) {
    return false;
}

// 0x611790
bool CPopulation::CanSolicitPlayerInCar(eModelID modelIndex) {
    return CModelInfo::GetPedModelInfo(modelIndex)->GetPedType() == PED_TYPE_PROSTITUTE;
}

// 0x6117B0
bool CPopulation::CanJeerAtStripper(eModelID modelIndex) {
    return CModelInfo::GetPedModelInfo(modelIndex)->GetPedType() == PED_TYPE_CIVMALE;
}

// 0x6117D0
void CPopulation::PlaceGangMembers(ePedType pedType, uint32 numOfPeds, const CVector& posn) {
    CPedGroupPlacer{}.PlaceGroup(pedType, numOfPeds, posn, ePedGroupDefaultTaskAllocatorType::RANDOM);
}

//! NOTSA - Helper
void ProcessPossibleDriverModelsOfCar(eModelID carModelIndex, auto&& ProcessModel) {
    if (carModelIndex == MODEL_FREEWAY) {
        ProcessModel(MODEL_BIKERA);
        ProcessModel(MODEL_BIKERB);
    } else {
        if (carModelIndex == MODEL_TAXI || carModelIndex == MODEL_CABBIE) {
            ProcessModel((eModelID)(CStreaming::GetDefaultCabDriverModel())); // Como el original (0x611829): sin comprobar -1
            return;
        }
        const auto model = [&] {
            switch (carModelIndex) {
            case MODEL_STRETCH:     return MODEL_WMYCH;
            case MODEL_MRWHOOP:     return MODEL_WMOICE;
            case MODEL_SECURICA:    return MODEL_WMYSGRD;
            case MODEL_PIZZABOY:    return MODEL_WMYPIZZ;
            case MODEL_FREEWAY:     return MODEL_BIKERA;
            case MODEL_BMX:         return MODEL_WMYBMX;
            default:                return MODEL_INVALID;
        }}();
        if (model != MODEL_INVALID) {
            ProcessModel(model);
        }
    }
}

// 0x6117F0
void CPopulation::LoadSpecificDriverModelsForCar(eModelID carModelIndex) {
    ProcessPossibleDriverModelsOfCar(carModelIndex, [&](eModelID model) {
        CStreaming::RequestModel(model, STREAMING_KEEP_IN_MEMORY | STREAMING_GAME_REQUIRED);
    });
}

// 0x6119D0
void CPopulation::RemoveSpecificDriverModelsForCar(eModelID carModelIndex) {
    ProcessPossibleDriverModelsOfCar(carModelIndex, CStreaming::SetModelAndItsTxdDeletable);
}

// 0x611900
eModelID CPopulation::FindSpecificDriverModelForCar_ToUse(eModelID carModelIndex) {
    switch (carModelIndex) {
    case MODEL_STRETCH:  return MODEL_WMYCH;
    case MODEL_TAXI:
    case MODEL_CABBIE:   return (eModelID)(CStreaming::GetDefaultCabDriverModel());
    case MODEL_MRWHOOP:  return MODEL_WMOICE;
    case MODEL_SECURICA: return MODEL_WMYSGRD;
    case MODEL_PIZZABOY: return MODEL_WMYPIZZ;
    case MODEL_BMX:      return MODEL_WMYBMX;
    case MODEL_FREEWAY: {
        // Como el original (0x61192D: call 0x407180 con (0, 3)): GetRandomIntInRange, no la plantilla entera de GetRandomNumberInRange.
        switch (CGeneral::GetRandomIntInRange(0, 3)) {
        case 0:  return MODEL_BIKERA;
        case 1:  return MODEL_BIKERB;
        default: return MODEL_INVALID;
        }
    }
    }
    return MODEL_INVALID;
}

// 0x611B20
// Como el original (0x611B20): solo ATM (0), SEAT (1), STOP (2) y PARK (8) dependen de la hora (8 < hora < 20); cualquier
// otro tipo (incluido el indefinido -1) es siempre valido. (La version anterior trataba PARK como siempre valido.)
bool CPopulation::IsCorrectTimeOfDayForEffect(const C2dEffectPedAttractor& fx) {
    switch (fx.m_nAttractorType) {
    case PED_ATTRACTOR_ATM:
    case PED_ATTRACTOR_SEAT:
    case PED_ATTRACTOR_STOP:
    case PED_ATTRACTOR_PARK:
        return CClock::GetIsTimeInRange(9, 20); // 9 is correct, because the function uses `>=` not `>`
    }
    return true;
}

// 0x611B60
float CPopulation::FindCarMultiplierMotorway() {
    return m_bMoreCarsAndFewerPeds
        ? 1.7f
        : 1.f;
}

// 0x611B80
float CPopulation::FindPedMultiplierMotorway() {
    return 1.f;
}

// 0x611FC0
// Como el original (0x611FC0..0x612231), sin reordenar algebra: la distancia NO se divide entre PedCreationDistMultiplier(); cada umbral
// es PedCreationDistMultiplier() * m_fGenerationDistMultiplier * K y se compara con la distancia (el EXE vuelve a llamar a
// PedCreationDistMultiplier() en cada comparacion, en este orden), con la diferencia de muerte con signo (jg/jle), los
// colaboradores en el orden del EXE y GetIsOnScreen() llamado dos veces en la rama de eliminacion por camara.
void CPopulation::ManagePed(CPed* ped, const CVector& playerPosn) {
    if (ped->IsPlayer()) {
        return;
    }

    if (!ped->CanBeDeleted()) {
        return;
    }

    if (ped->bInVehicle) {
        return;
    }

    if (ped->m_pAttachedTo && ped->m_pAttachedTo->GetIsTypeVehicle()) {
        return;
    }

    // Tiempo desde la muerte (int32 con signo, como el EXE: sub edi,ecx / jg)
    const int32 deathDelta = static_cast<int32>(CTimer::GetTimeInMS() - static_cast<uint32>(ped->m_nDeathTimeMS));
    if (ped->IsStateDead()) {
        if (   deathDelta > 30'000
            || (CDarkel::FrenzyOnGoing() && deathDelta > 15'000)
            || (deathDelta > 8'000 && CGangWars::GangWarFightingGoingOn())
        ) {
            ped->bFadeOut = true;
        }
    }

    // Si el fundido ya termino (alfa 0), se elimina
    if (ped->bFadeOut && CVisibilityPlugins::GetClumpAlpha(ped->GetRpClump()) == 0) {
        RemovePed(ped);
        return;
    }

    // Distancia 2D al jugador por el multiplicador de eliminacion del peaton
    const auto& pedPos = ped->GetPosition();
    const float dx     = pedPos.x - playerPosn.x;
    const float dy     = pedPos.y - playerPosn.y;
    float removalDist  = std::sqrt(dy * dy + dx * dx) * ped->m_fRemovalDistMultiplier;
    if (IsPedTypeGang(ped->m_nPedType)) {
        removalDist -= 30.f;
    } else if (ped->bDeadPedInFrontOfCar && ped->m_VehDeadInFrontOf) {
        removalDist = 0.f;
    }

    const auto SetTimeTillNeeded = [&](uint32 now) {
        ped->m_nTimeTillWeNeedThisPed = now + (ped->m_nPedType == PED_TYPE_COP ? 10'000u : 4'000u);
    };
    const auto FadeOrRemove = [&] { // 0x6121C2
        if (ped->GetIsOnScreen()) {
            ped->bFadeOut = true;
        } else {
            RemovePed(ped);
        }
    };

    // Demasiado lejos (umbral 65 si bCullExtraFarAway, si no 54.5; el EXE comprueba primero 65 y despues, si no hay
    // bCullExtraFarAway, 54.5)
    if (PedCreationDistMultiplier() * TheCamera.m_fGenerationDistMultiplier * 65.f < removalDist) {
        FadeOrRemove();
        return;
    }
    if (!ped->bCullExtraFarAway) {
        if (PedCreationDistMultiplier() * TheCamera.m_fGenerationDistMultiplier * 54.5f < removalDist) {
            FadeOrRemove();
            return;
        }
    }

    // Cerca (distancia <= 25 * multiplicador, o NaN): se renueva el tiempo
    if (!(PedCreationDistMultiplier() * 25.f < removalDist)) {
        SetTimeTillNeeded(CTimer::GetTimeInMS());
        return;
    }

    // Lejos pero dentro del limite
    const bool   onScreen = ped->GetIsOnScreen();
    const uint32 now      = CTimer::GetTimeInMS();
    if (now <= ped->m_nTimeTillWeNeedThisPed) {
        if (onScreen) {
            SetTimeTillNeeded(now);
        }
        return;
    }
    if (onScreen) {
        SetTimeTillNeeded(now);
        return;
    }

    // Peaton ya no necesario: no se elimina con ciertas camaras
    const auto& activeCam = TheCamera.GetActiveCam();
    switch (activeCam.m_nMode) {
    case MODE_SNIPER:
    case MODE_SNIPER_RUNABOUT:
    case MODE_CAMERA:
        return;
    }
    if (activeCam.m_bLookingLeft || activeCam.m_bLookingRight || activeCam.m_bLookingBehind) {
        return;
    }
    if (!ped->GetIsOnScreen()) { // 0x6121C2: segunda llamada
        RemovePed(ped);
    } else {
        ped->bFadeOut = true;
    }
}

// 0x612240
// Como el original (0x612240..0x6122B2): min(Max, otros) con la comparacion de x87 (se queda con Max solo si es estrictamente menor),
// truncado a entero ANTES de multiplicar, CCullZones::FewerPeds() se llama siempre (incluso en interiores) y el producto
// (entero * PedDensityMultiplier) * multiplicador se trunca de nuevo; en interiores el valor se sustituye por NumberOfPedsInUseInterior.
int32 CPopulation::FindNumberOfPedsWeCanPlaceOnBenches() {
    const float maxPeds = static_cast<float>(static_cast<int32>(MaxNumberOfPedsInUse));
    const float others  = CPopCycle::m_NumOther_Peds;
    const float chosen  = (maxPeds < others) ? maxPeds : others;
    const int32 chosenI = static_cast<int32>(static_cast<int64>(chosen));

    const float cullMultiplier = CCullZones::FewerPeds() ? 0.6f : 1.f;
    int32       base           = static_cast<int32>(static_cast<int64>(static_cast<float>(chosenI) * PedDensityMultiplier * cullMultiplier));
    if (CGame::currArea) {
        base = static_cast<int32>(NumberOfPedsInUseInterior);
    }
    return base - static_cast<int32>(ms_nNumCivFemale) - static_cast<int32>(ms_nNumCivMale) + 2;
}

// 0x6122C0
// Como el original (0x6122C0): recorre el pool de peatones de atras hacia delante (ranuras libres fuera).
void CPopulation::RemoveAllRandomPeds() {
    auto* const pool = GetPedPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const ped = pool->GetAt(i);
        if (ped && ped->CanBeDeleted()) {
            RemovePed(ped);
        }
    }
}

// 0x612320
// Como el original (0x612320): el centro de la busqueda es el del dummy ([object+0x170]), no el del objeto; el radio es el
// de la esfera de colision del modelo del objeto; b2D=false, 2 entidades maximo, solo vehiculos y peatones.
bool CPopulation::TestRoomForDummyObject(CObject* object) {
    const CVector centre = object->m_pDummyObject->GetBoundCentre();
    const float   radius = CModelInfo::GetModelInfo(static_cast<int16>(object->m_nModelIndex))->GetColModel()->GetBoundRadius();
    int16 ncolliding{};
    CWorld::FindObjectsKindaColliding(
        centre,
        radius,
        false,
        &ncolliding,
        2,              // TODO: Why 2?
        nullptr,
        false,
        true,
        true,
        false,
        false
    );
    return ncolliding == 0;
}

// 0x6123A0
// Como el original (0x6123A0..0x612704): el rectangulo de sectores sale de la POSICION del dummy +- el radio del modelo
// (indice = floor((v * 0.02f) + 60.0f) calculado en float; minimos limitados por abajo a 0 y maximos por arriba a 120), la
// esfera de busqueda es (centro del dummy, radio); solo se procesan los vehiculos de la lista del sector que aun no tienen el
// codigo de escaneo actual y que tocan esa esfera (se les marca el codigo), reservando la matriz que falte al vehiculo y al
// dummy; ProcessColModels(dummy, vehiculo, ..) > 0 -> false. El EXE tambien activa el bit 0 del guarda de la estatica local
// 0xC10280.
bool CPopulation::TestSafeForRealObject(CDummyObject* obj) {
    CColModel* const objCM   = CModelInfo::GetModelInfo(static_cast<int16>(obj->m_nModelIndex))->GetColModel();
    const CVector&   pos     = obj->GetPosition();
    const float      radius  = objCM->GetBoundRadius();

    const auto SectorOf = [](float v) { // floor(double(v * 0.02f + 60.0f)) -> int
        return static_cast<int32>(std::floor(static_cast<double>((v * 0.02f) + 60.0f)));
    };
    int32 minX = SectorOf(pos.x - radius);
    if (minX <= 0) {
        minX = 0;
    }
    int32 minY = SectorOf(pos.y - radius);
    if (minY <= 0) {
        minY = 0;
    }
    int32 maxX = SectorOf(radius + pos.x);
    if (maxX >= 120) {
        maxX = 120;
    }
    int32 maxY = SectorOf(radius + pos.y);
    if (maxY >= 120) {
        maxY = 120;
    }

    CVector centre;
    obj->GetBoundCentre(centre);
    *reinterpret_cast<uint32*>(0xC10280) |= 1u; // guarda de la estatica local (el EXE lo activa en cada llamada)

    for (int32 y = minY; y <= maxY; ++y) {
        for (int32 x = minX; x <= maxX; ++x) {
            auto* node = CWorld::GetRepeatSector(x, y).Vehicles.GetNode();
            while (node) {
                CVehicle* const veh  = node->Item;
                auto* const     next = node->Next;
                if (veh->GetScanCode() != CWorld::ms_nCurrentScanCode && veh->GetIsTouching(centre, radius)) {
                    veh->SetScanCode(CWorld::ms_nCurrentScanCode);
                    if (!veh->m_matrix) {
                        veh->AllocateMatrix();
                        veh->m_placement.UpdateMatrix(veh->m_matrix);
                    }
                    if (!obj->m_matrix) {
                        obj->AllocateMatrix();
                        obj->m_placement.UpdateMatrix(obj->m_matrix);
                    }
                    if (CCollision::ProcessColModels(
                        *obj->m_matrix, *objCM,
                        *veh->m_matrix, *veh->GetColModel(),
                        CWorld::m_aTempColPts,
                        nullptr,
                        nullptr,
                        false
                    ) > 0) {
                        return false;
                    }
                }
                node = next;
            }
        }
    }
    return true;
}

// 0x612710
// Como el original (0x612710..0x612C79). Diferencias corregidas respecto a la version anterior: el arma de "todos armados" sale de
// rand() % 5; en las bandas la probabilidad es GetRandomNumberInRange(0, 100) < 33 (formula entera de 0x407180) y el arma se
// elige con los umbrales 33/66 o 50 del propio EXE; el arma se fija por ranura (GetWeaponInfo(arma, STD)->m_nSlot ->
// SetCurrentWeapon(ranura)) salvo con el truco de las prostitutas, que usa SetCurrentWeapon(tipo) y municion 0; los policias
// reciben su tarea de patrulla construida en linea (CTaskComplexWanderCop(4, GetRandomNumberInRange(0, 8))); el truco del palo de
// golf busca en todo el pool el peaton mas cercano (distancia al cuadrado, mas pequena estricta, de atras hacia delante).
CPed* CPopulation::AddPed(ePedType pedType, eModelID modelIndex, const CVector& createAtPos, bool makeWander) {
    const auto GiveAndSetPedWeapon = [](CPed* ped, eWeaponType wtype) { // 0x612970
        ped->GiveDelayedWeapon(wtype, 25001);
        ped->SetCurrentWeapon(CWeaponInfo::GetWeaponInfo(wtype, eWeaponSkill::STD)->m_nSlot); // sobrecarga por ranura (0x5E61F0)
    };

    // Create the ped
    const auto ped = [&]() -> CPed* {
        switch (pedType) {
        case PED_TYPE_CIVMALE:
        case PED_TYPE_CIVFEMALE: { // 0x61274E
            const auto ped = new CCivilianPed(pedType, modelIndex);

            if (CCheat::IsAnyActive({ CHEAT_EVERYONE_ARMED, CHEAT_PEDS_ATTACK_OTHER_WITH_GOLFCLUB })) {
                constexpr eWeaponType WEAPONS[5]{ WEAPON_PISTOL, WEAPON_BASEBALLBAT, WEAPON_SHOTGUN, WEAPON_M4, WEAPON_RLAUNCHER };
                GiveAndSetPedWeapon(ped, WEAPONS[CGeneral::GetRandomNumber() % 5]); // rand() % 5 (idiv)
            }

            return ped;
        }
        case PED_TYPE_GANG1:
        case PED_TYPE_GANG2:
        case PED_TYPE_GANG3:
        case PED_TYPE_GANG4:
        case PED_TYPE_GANG5:
        case PED_TYPE_GANG6:
        case PED_TYPE_GANG7:
        case PED_TYPE_GANG8:
        case PED_TYPE_GANG9:
        case PED_TYPE_GANG10: { // 0x6128C4
            const auto ped = new CCivilianPed(pedType, modelIndex);

            if (CCheat::IsActive(CHEAT_NINJA_THEME) && pedType == PED_TYPE_GANG7) {
                GiveAndSetPedWeapon(ped, WEAPON_KATANA);
            } else if (CGeneral::GetRandomIntInRange(0, 100) < 33) { // Give random weapon
                // Como CGangInfo::GetRandomWeapon en linea (0x61291C..0x612964)
                const auto& gangWeapons = CGangs::Gang[GetGangOfPedType(pedType)].m_nGangWeapons;
                const int32 roll        = CGeneral::GetRandomIntInRange(0, 100);
                eWeaponType wtype{};
                if (gangWeapons[2] != WEAPON_UNARMED) {
                    wtype = roll < 33 ? gangWeapons[0] : (roll >= 66 ? gangWeapons[2] : gangWeapons[1]);
                } else if (gangWeapons[1] != WEAPON_UNARMED) {
                    wtype = roll >= 50 ? gangWeapons[1] : gangWeapons[0];
                } else {
                    wtype = gangWeapons[0];
                }
                if (wtype != WEAPON_UNARMED) {
                    GiveAndSetPedWeapon(ped, wtype);
                }
            }

            return ped;
        }
        case PED_TYPE_DEALER:
        case PED_TYPE_CRIMINAL:
        case PED_TYPE_PROSTITUTE:
        case PED_TYPE_SPECIAL:
        case PED_TYPE_MISSION1:
        case PED_TYPE_MISSION2:
        case PED_TYPE_MISSION3:
        case PED_TYPE_MISSION4:
        case PED_TYPE_MISSION5:
        case PED_TYPE_MISSION6:
        case PED_TYPE_MISSION7:
        case PED_TYPE_MISSION8: // 0x612992
            return new CCivilianPed(pedType, modelIndex);
        case PED_TYPE_MEDIC: // 0x612848
        case PED_TYPE_FIREMAN: // 0x612886
            return new CEmergencyPed(pedType, modelIndex);
        case PED_TYPE_COP: // 0x61280C
            return new CCopPed(modelIndex);
        default:
            NOTSA_UNREACHABLE();
        }
    }();

    // 0x6129D9

    ped->SetPosn(createAtPos);
    ped->SetOrientation(0.f, 0.f, 0.f);

    CWorld::Add(ped);

    if (!makeWander) {
        return ped;
    }

    auto& tmgr = ped->GetTaskManager();

    // Set it's default tasks
    if (ped->m_nPedType == PED_TYPE_COP) { // 0x612A2A
        // Tarea de patrulla en linea (0x612A34): CTaskComplexWanderCop(PEDMOVE_WALK, rand [0, 8))
        tmgr.SetTask(
            new CTaskComplexWanderCop(PEDMOVE_WALK, static_cast<uint8>(CGeneral::GetRandomIntInRange(0, 8))),
            TASK_PRIMARY_PRIMARY,
            false
        );
        tmgr.SetTask(
            new CTaskSimpleStandStill{ 0, true, false, 8.f },
            TASK_PRIMARY_DEFAULT,
            false
        );
    } else {
        tmgr.SetTask(
            CTaskComplexWander::GetWanderTaskByPedType(ped),
            TASK_PRIMARY_DEFAULT,
            false
        );
    }

    if (CCheat::IsActive(CHEAT_SLUT_MAGNET) && pedType == PED_TYPE_PROSTITUTE) { // 0x612AEC
        CEventSexyPed sexyEvent{ FindPlayerPed(), TASK_COMPLEX_GANG_HASSLE_PED };
        ped->GetEventGroup().Add(&sexyEvent, false);
        const auto wtype = static_cast<eWeaponType>(CGeneral::GetRandomIntInRange(WEAPON_DILDO1, WEAPON_DILDO1 + 4)); // 0x407180 con (10, 14)
        ped->GiveDelayedWeapon(wtype, 0);
        ped->SetCurrentWeapon(wtype);
    }

    if (CCheat::IsActive(CHEAT_PEDS_ATTACK_OTHER_WITH_GOLFCLUB)) { // 0x612B5E
        CPed* closest{};
        float closestDistSq{};
        auto* const pool = GetPedPool();
        for (auto i = pool->GetSize(); i-- > 0;) {
            auto* const other = pool->GetAt(i);
            if (!other || other == ped) {
                continue;
            }
            const auto& a      = ped->GetPosition();
            const auto& b      = other->GetPosition();
            const float dz     = a.z - b.z;
            const float dy     = a.y - b.y;
            const float dx     = a.x - b.x;
            const float distSq = (dx * dx + dy * dy) + dz * dz;
            if (!closest || distSq < closestDistSq) {
                closest       = other;
                closestDistSq = distSq;
            }
        }
        if (closest) {
            CEventAcquaintancePedHate hateEvent{ closest, TASK_COMPLEX_KILL_PED_ON_FOOT };
            ped->GetEventGroup().Add(&hateEvent, false);
        }
    }

    return ped;
}

// 0x612CD0 - Unused
// Como el original (0x612CD0..0x612F88): la camara se consulta con IsSphereVisible(pos, 2.0f) y, si ve el punto, se rechaza si
// PedCreationDistMultiplier() * 42.5f > distancia 2D al jugador (sin elevar al cuadrado); el suelo es FindGroundZFor3DCoord(..)+1;
// el vehiculo se registra con RegisterReference sobre el campo (sin CleanUpOldReference); el evento vive hasta el final de la
// funcion; la zona se comprueba con IsPositionClearForPed(.., 2.0f, 3, array, true, true, true) tratando como libres las entradas
// nulas, el vehiculo y el propio peaton; ProcessColModels recibe (vehiculo, peaton) con un array local de puntos y se elimina el
// peaton si devuelve distinto de 0.
CPed* CPopulation::AddDeadPedInFrontOfCar(const CVector& createPedAt, CVehicle* vehicle) {
    if (TheCamera.IsSphereVisible(createPedAt, 2.f)) {
        const auto& plyrPos = FindPlayerPed()->GetPosition();
        const float dy      = createPedAt.y - plyrPos.y;
        const float dx      = createPedAt.x - plyrPos.x;
        const float dist    = std::sqrt(dx * dx + dy * dy);
        if (PedCreationDistMultiplier() * 42.5f > dist) {
            return nullptr;
        }
    }

    bool bGroundHit{};
    const float groundZ1 = CWorld::FindGroundZFor3DCoord(CVector{ createPedAt.x, createPedAt.y, createPedAt.z + 1.f }, &bGroundHit, nullptr) + 1.f;
    if (!bGroundHit) {
        return nullptr;
    }
    const float groundZ = (groundZ1 < createPedAt.z) ? createPedAt.z : groundZ1; // std::max(groundZ1, z) con la comparacion del EXE

    if (!CModelInfo::GetModelInfo(MODEL_MALE01)->GetRwObject()) {
        NOTSA_LOG_DEBUG("Didn't create ped, because MODEL_MALE01 has no RW object!");
        return nullptr;
    }

    const auto ped = AddPed(PED_TYPE_CIVMALE, MODEL_MALE01, CVector{ createPedAt.x, createPedAt.y, groundZ }, false);

    CEventScriptCommand deadEvent{
        TASK_PRIMARY_DEFAULT,
        new CTaskComplexDie{ WEAPON_UNARMED, ANIM_GROUP_DEFAULT, ANIM_ID_KO_SHOT_FRONT_0 }
    };
    ped->GetEventGroup().Add(&deadEvent, false);

    ped->m_nMoneyCount        = 0;
    ped->bDeadPedInFrontOfCar = true;
    ped->m_VehDeadInFrontOf   = vehicle;
    vehicle->RegisterReference(reinterpret_cast<CEntity**>(&ped->m_VehDeadInFrontOf));

    // Check if it's colliding with anything...
    CEntity* hits[3]{};
    bool     clear = CPedPlacement::IsPositionClearForPed(createPedAt, 2.f, 3, hits, true, true, true);
    if (!clear) {
        // Como el original: las entradas nulas, el vehiculo y el propio peaton no cuentan
        clear = true;
        for (auto* const hit : hits) {
            if (hit && hit != vehicle && hit != ped) {
                clear = false;
                break;
            }
        }
    }
    if (!clear) {
        RemovePed(ped);
        return nullptr;
    }

    std::array<CColPoint, 32> colPoints;
    if (CCollision::ProcessColModels(
        *vehicle->m_matrix, *vehicle->GetColModel(),
        *ped->m_matrix, *CModelInfo::GetModelInfo(static_cast<int16>(ped->m_nModelIndex))->GetColModel(),
        colPoints,
        nullptr,
        nullptr,
        false
    ) != 0) {
        CWorld::Remove(ped);
        delete ped;
        return nullptr;
    }

    CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(), 0);

    return ped;
}

// 0x612F90
// Como el original (0x612F90..0x61316D). La busqueda es un doble bucle: para cada contador de referencias 0..N-1 (N = 3, 5 si
// hay mas de 20 peatones en interiores, 7 si no se piden "ocupaciones usadas") se recorren los 8 modelos cargados
// (CStreaming::ms_pedsLoaded) y se acepta el primero cuyo contador de referencias coincide. PedMICanBeCreatedAtAttractor y
// PedMICanBeCreatedInInterior van en linea; en atractores tambien se consulta PedMICanBeCreatedAtThisAttractor con el nombre
// del script; con lluvia (no estrictamente < 0.1f) se descartan los que toman el sol.
eModelID CPopulation::ChooseCivilianOccupation(
    bool         mustBeMale,
    bool         mustBeFemale,
    AssocGroupId mustUseThisAnimGroup,
    eModelID     mustNotBeThisModel,
    ePedStats    mustBeCompatibleWithThisPedStat,
    bool         bOnlyOnFoots,
    bool         doTestForUsedOccupations,
    bool         isAtAttractor,
    const char*  attractorScriptName
) {
    int32 maxRefCount = 3;
    if (CGame::currArea && static_cast<int32>(NumberOfPedsInUseInterior) > 20) {
        maxRefCount = 5;
    }
    if (!doTestForUsedOccupations) {
        maxRefCount = 7;
    }

    for (int32 refCount = 0; refCount < maxRefCount; refCount++) {
        for (const auto modelId : CStreaming::ms_pedsLoaded) {
            if (static_cast<int32>(modelId) < 0) {
                continue;
            }
            if (!CStreaming::IsModelLoaded(modelId)) { // So why the fuck is it in the `ms_pedsLoaded` array if it's not loaded?
                continue;
            }
            const auto mi = CModelInfo::GetPedModelInfo(modelId);
            if (static_cast<int16>(mi->m_nRefCount) != refCount) {
                continue;
            }
            if (mustNotBeThisModel == modelId) {
                continue;
            }
            if (!CPopCycle::PedIsAcceptableInCurrentZone(modelId) && CGame::CanSeeOutSideFromCurrArea()) {
                continue;
            }
            if (bOnlyOnFoots && (mi->m_nCarsCanDriveMask & 0x1000) == 0) {
                continue;
            }
            if (mustBeMale && mi->GetPedType() != PED_TYPE_CIVMALE) {
                continue;
            }
            if (mustBeFemale && mi->GetPedType() != PED_TYPE_CIVFEMALE) {
                continue;
            }
            if (static_cast<int32>(mustUseThisAnimGroup) >= 0 && mi->m_nAnimType != mustUseThisAnimGroup) {
                continue;
            }
            if (isAtAttractor) {
                const auto pedType = mi->GetPedType();
                if (pedType >= PED_TYPE_DEALER && pedType <= PED_TYPE_PROSTITUTE) { // PedMICanBeCreatedAtAttractor en linea
                    continue;
                }
                if (!PedMICanBeCreatedAtThisAttractor(modelId, attractorScriptName)) {
                    continue;
                }
            }
            if (CGame::currArea) {
                const auto pedType = mi->GetPedType();
                if (pedType >= PED_TYPE_COP && pedType <= PED_TYPE_PROSTITUTE) { // PedMICanBeCreatedInInterior en linea
                    continue;
                }
            }
            if (static_cast<int32>(mustBeCompatibleWithThisPedStat) > -1 && !ArePedStatsCompatible(mi->GetPedStatType(), mustBeCompatibleWithThisPedStat)) {
                continue;
            }
            if (!(CWeather::Rain < 0.1f) && IsSunbather(modelId)) { // fcomp + test ah,5 + jnp: con NaN tambien se consulta
                continue;
            }
            return modelId;
        }
    }
    return doTestForUsedOccupations
        ? MODEL_INVALID
        : MODEL_MALE01;
}

// 0x613180
// Como el original (0x613180..0x613259): la segunda llamada a ChooseCivilianOccupation excluye el modelo del marido y exige que
// su estadistica de peaton sea compatible con la del marido; el azar usa CGeneral::GetRandomNumber() (rand()).
void CPopulation::ChooseCivilianCoupleOccupations(eModelID& husbandOccupation, eModelID& wifeyOccupation) {
    wifeyOccupation = husbandOccupation = MODEL_INVALID;

    const auto [husbandMustBeMale, wifeMustBeFemale] = []() -> std::pair<bool, bool> {
        if (CWeather::WeatherRegion != WEATHER_REGION_SF || (CGeneral::GetRandomNumber() & 16) == 0) {
            return { true, false };
        } else if (CGeneral::GetRandomNumber() & 32) {
            return { true, true };
        } else {
            return { false, false };
        }
    }();

    if (static_cast<int32>(husbandOccupation = ChooseCivilianOccupation(husbandMustBeMale, wifeMustBeFemale)) >= 0) {
        const auto husbandStat = CModelInfo::GetPedModelInfo(husbandOccupation)->GetPedStatType();
        if (static_cast<int32>(wifeyOccupation = ChooseCivilianOccupation(wifeMustBeFemale, husbandMustBeMale, ANIM_GROUP_NONE, husbandOccupation, husbandStat)) >= 0) {
            const auto IsSkater = [](eModelID model) {
                return CModelInfo::GetPedModelInfo(model)->GetPedStatType() == ePedStats::SKATER;
            };
            if (IsSkater(husbandOccupation) == IsSkater(wifeyOccupation)) { // Either they are both skaters, or they both aren't
                return; // All good
            }
        }
    }

    // Fail
    wifeyOccupation = husbandOccupation = MODEL_INVALID;
}

// 0x613260
// Como el original (0x613260..0x6133E8): el bucle de contadores de referencias es k = 0..4 (con 4 no se comprueba el contador),
// se exige ser hombre si mustBeMale, y "ya hay un ocupante con ese modelo" mira solo al conductor y los tres primeros pasajeros.
eModelID CPopulation::ChooseCivilianOccupationForVehicle(bool mustBeMale, CVehicle* vehicle) {
    const auto vehClass = vehicle->GetVehicleModelInfo()->m_nVehicleClass;
    for (int32 i = 0; i < 2; i++) { // In 0th iteration we check if the model is inside the vehicle already, in the second we dont
        for (int32 k = 0; k <= 4; k++) {
            for (const auto pedModelId : CStreaming::ms_pedsLoaded) {
                if (static_cast<int32>(pedModelId) < 0) {
                    continue;
                }
                if (!CStreaming::IsModelLoaded(pedModelId)) {
                    continue;
                }
                const auto mi = CModelInfo::GetPedModelInfo(pedModelId);
                if (k != 4 && static_cast<int16>(mi->m_nRefCount) != k) {
                    continue;
                }
                if (!CCheat::IsAnyActive({
                    CHEAT_ELVIS_IS_EVERYWHERE,
                    CHEAT_PEDS_ATTACK_YOU_WITH_ROCKETS,
                    CHEAT_BEACH_PARTY,
                    CHEAT_GANGMEMBERS_EVERYWHERE,
                    CHEAT_NINJA_THEME,
                    CHEAT_SLUT_MAGNET,
                    CHEAT_FUNHOUSE_THEME,
                    CHEAT_COUNTRY_TRAFFIC,
                })) {
                    if (!CPopCycle::PedIsAcceptableInCurrentZone(pedModelId)) {
                        continue;
                    }
                    if (!mi->CanPedDriveVehicleClass(vehClass)) {
                        continue;
                    }
                }
                if (mustBeMale && mi->GetPedType() != PED_TYPE_CIVMALE) {
                    continue;
                }
                if (i == 1) {
                    return pedModelId;
                }
                bool isInside = false;
                for (auto* const occupant : { vehicle->m_pDriver, vehicle->m_apPassengers[0], vehicle->m_apPassengers[1], vehicle->m_apPassengers[2] }) {
                    if (occupant && static_cast<int32>(static_cast<int16>(occupant->m_nModelIndex)) == static_cast<int32>(pedModelId)) {
                        isInside = true;
                    }
                }
                if (!isInside) {
                    return pedModelId;
                }
            }
        }
    }
    return MODEL_MALE01;
}

// 0x6133F0
// Como el original (0x6133F0..0x6139E1). Diferencias corregidas: el nivel de busqueda se lee a traves de FindPlayerPed() ->
// m_pPlayerData -> m_pWanted (el EXE no llama a FindPlayerWanted) y se vuelve a leer para decidir el arma; el modelo de coche
// policia se pide antes de buscar nodos y si es <= 0 no se crean coches; un nodo cuenta si su area no es 0xFFFF; los coches
// reciben el estado, la posicion (nodo comprimido * 0.125), el rumbo normalizado con CVector::Normalise y, si son de la ley, la
// sirena con 50% de probabilidad; en los policias el azar se consume primero para la distancia (8..10) y despues para el rumbo,
// con fsin/fcos/fpatan reales de x87, el suelo es FindGroundZFor3DCoord(..) + 1, el rumbo del policia sale de la direccion
// normalizada y el evento vive hasta despues de CPedIntelligence::Process().
void CPopulation::CreateWaitingCoppers(CVector createAt, float createaWithHeading) {
    constexpr uint32 NUM_COPS_FOR_WANTED_LEVEL[7]{ 0, 1, 2, 4, 5, 6, 7 };
    constexpr uint32 NUM_CARS_FOR_WANTED_LEVEL[7]{ 0, 0, 0, 1, 1, 2, 2 };

    constexpr float kRandReciprocal = std::bit_cast<float>(0x38000100u); // 0x858C7C (1 / 32767)

    createAt.z += 1.f;

    const auto GetPlayerWantedLevel = [] {
        const auto* const playerData = FindPlayerPed()->GetPlayerData();
        const auto* const wanted     = playerData ? playerData->m_pWanted : nullptr;
        return static_cast<int32>(wanted->GetWantedLevel());
    };

    // El EXE usa (nivel - 1) sin signo > 5 como "sin policias ni coches"
    const auto plyrWantedLvl = GetPlayerWantedLevel();
    uint32 numOfCars{}, numOfCopPeds{};
    if (static_cast<uint32>(plyrWantedLvl - 1) <= 5u) {
        numOfCars    = NUM_CARS_FOR_WANTED_LEVEL[plyrWantedLvl];
        numOfCopPeds = NUM_COPS_FOR_WANTED_LEVEL[plyrWantedLvl];
    }

    // Create cop cars
    if (numOfCars) {
        const auto copCarModel = CStreaming::GetDefaultCopCarModel(true);
        if (static_cast<int32>(copCarModel) > 0) {
            CNodeAddress createPosNodes[3]{};
            createPosNodes[0] = ThePaths.FindNthNodeClosestToCoors(
                createAt,
                0,
                30.f,
                false,
                false,
                3,
                false,
                true,
                &createPosNodes[1]
            );

            for (const auto nodeAddr : createPosNodes) {
                if (!numOfCars || nodeAddr.m_wAreaId == 0xFFFF) {
                    continue;
                }

                const auto veh = new CAutomobile{ copCarModel, RANDOM_VEHICLE, true };

                veh->SetStatus(STATUS_ABANDONED);

                // Posicion del nodo (coordenadas comprimidas * 0.125)
                veh->SetPosn(ThePaths.m_pPathNodes[nodeAddr.m_wAreaId][nodeAddr.m_wNodeId].GetPosition());

                // Adjust vehicle to be pointing at the creation coords
                auto& vehMat = veh->GetMatrix();
                const auto& vehPos = veh->GetPosition();
                CVector dirToCreateAt{ createAt.x - vehPos.x, createAt.y - vehPos.y, 0.f };
                dirToCreateAt.Normalise();
                vehMat.GetForward() = dirToCreateAt;
                vehMat.GetRight()   = CVector{ -dirToCreateAt.y, dirToCreateAt.x, 0.f };

                veh->PlaceOnRoadProperly();
                veh->SetIsStatic(false);

                // Now, update the RW matrix too
                if (veh->GetRwObject()) {
                    auto* const rwMatrix = RwFrameGetMatrix(RpClumpGetFrame(veh->GetRpClump()));
                    if (veh->m_matrix) {
                        veh->m_matrix->UpdateRwMatrix(rwMatrix);
                    } else {
                        veh->m_placement.UpdateRwMatrix(rwMatrix);
                    }
                }

                CCarCtrl::JoinCarWithRoadSystem(veh);

                if (veh->UsesSiren() && (CGeneral::GetRandomNumber() & 1)) { // Como el original: llama a 0x6D8470 (UsesSiren), no a IsLawEnforcementVehicle (0x6D2370)
                    veh->vehicleFlags.bSirenOrAlarm = true;
                }

                CWorld::Add(veh);

                numOfCars--;
            }
        }
    }

    // Create cop peds
    if (numOfCopPeds) {
        for (int32 i{}; i < 20; i++) { // int32 angleOffset = 0; angleOffset > -20; angleOffset--
            if (!numOfCopPeds) {
                break;
            }

            // Primero el azar de la distancia (8..10), despues el del rumbo (EXE: 0x6136E0 y 0x61370D)
            const float distRoll = (static_cast<float>(CGeneral::GetRandomNumber()) * kRandReciprocal) * 2.f + 8.f;
            const float scaledJ  = static_cast<float>(-i) * 0.2f;
            const float rndRoll  = static_cast<float>(CGeneral::GetRandomNumber()) * kRandReciprocal;
            const float scaledI  = static_cast<float>(i) * 0.2f;
            const float heading  = (rndRoll * (scaledI - scaledJ) + scaledJ) + createaWithHeading;

            // fsin/fcos x87 sin redondear, producto y suma en 24 bits (0x613781..0x613796): helpers compartidos de X87Math
            CVector copPedPos{
                TrigCompoundSinMulAdd(heading, distRoll, createAt.x),
                TrigCompoundCosMulAdd(heading, distRoll, createAt.y),
                createAt.z
            };

            if (!CWorld::GetIsLineOfSightClear(createAt, copPedPos, true, true, false, true)) {
                continue;
            }

            int16 numColliding{};
            CEntity* entitiesInRange[1];
            CWorld::FindObjectsInRange(copPedPos, 1.f, true, &numColliding, (int16)std::size(entitiesInRange), entitiesInRange, false, true, true, false, false);
            if (numColliding) {
                continue;
            }

            bool bGroundHit{};
            copPedPos.z = CWorld::FindGroundZFor3DCoord(copPedPos, &bGroundHit, nullptr) + 1.f;
            if (!bGroundHit) {
                continue;
            }

            const auto ped = new CCopPed{ 0 };

            ped->SetPosn(copPedPos);

            // Direccion (normalizada en 3D) hacia el punto de creacion; angulo = atan2(-x, y) con fpatan
            CVector toCreateAt{ createAt.x - copPedPos.x, createAt.y - copPedPos.y, createAt.z - copPedPos.z };
            toCreateAt.Normalise();
            const float angle = TrigAtan2Cpp(-toCreateAt.x, toCreateAt.y); // fld -x; fld y; fpatan (0x6138B1..0x6138B7)
            ped->m_fCurrentRotation = angle;
            ped->m_fAimingRotation  = angle;
            ped->SetHeading(angle);

            CWorld::Add(ped);

            if (GetPlayerWantedLevel() > 1) {
                ped->GiveWeapon(WEAPON_PISTOL, 30000, true);
                ped->SetCurrentWeapon(WEAPON_PISTOL);
            }

            {
                // Como el original (0x61391A..0x61395D): FindPlayerPed(-1) se llama ANTES de reservar la tarea; el constructor de 6 argumentos deja los
                // bits 4..6 del byte 0xC a 0 y despues se hace `or [task+0Ch],30h` (bits 4 y 5 = m_bAimImmediate y m_bWaitForPlayerToBeSafe).
                const auto target = FindPlayerPed();
                const auto killTask = new CTaskComplexKillPedOnFoot{target, -1, 0, 0, 0, 2};
                killTask->m_bAimImmediate = true;
                killTask->m_bWaitForPlayerToBeSafe = true;
                CEventScriptCommand killEvent{
                    TASK_PRIMARY_PRIMARY,
                    killTask
                };
                ped->GetEventGroup().Add(&killEvent, false);
                ped->GetIntelligence()->Process();
            }

            numOfCopPeds--;
        }
    }
}

// 0x613A00
// Como el original (0x613A00..0x613BDC). Diferencias corregidas: FindSpecificDriverModelForCar_ToUse se llama siempre (consume
// azar con el Freeway aunque no sea conductor); policias/bomberos/medicos se crean con el tipo y el "modelo" del EXE (los
// policias reciben el tipo de policia 0/1/2/4/5 como segundo argumento de AddPed, no un ID de modelo; bomberos y medicos
// llevan siempre PED_TYPE_FIREMAN / PED_TYPE_MEDIC); en las bandas el tipo sale de la valoracion del coche (14..23 -> 7..16)
// incluso si el modelo se sustituye por MALE01; y para los criminales se resta el contador antes de cambiar el tipo y se suma
// despues (el orden anterior estaba invertido).
CPed* CPopulation::AddPedInCar(
    CVehicle* veh,
    bool      addAsDriver,
    int32     carRating,
    int32     seatNumber,
    bool      mustBeMale,
    bool      isCriminal
) {
    const CVector vehPos = veh->GetPosition();

    // Pick a model and ped type to use
    const auto driverModel = static_cast<int32>(FindSpecificDriverModelForCar_ToUse(veh->GetModelId()));

    ePedType pedType{};
    int32    pedModel{};
    if (addAsDriver && driverModel >= 0 && CStreaming::IsModelLoaded(driverModel)) {
        pedModel = driverModel;
        pedType  = CModelInfo::GetPedModelInfo(pedModel)->GetPedType();
    } else {
        const auto FixInvalid = [](int32 model) { return model < 0 ? static_cast<int32>(MODEL_MALE01) : model; };

        switch (static_cast<int16>(veh->m_nModelIndex)) {
        case MODEL_FIRETRUK:
            pedType  = PED_TYPE_FIREMAN;
            pedModel = FixInvalid(CStreaming::GetDefaultFiremanModel());
            break;
        case MODEL_AMBULAN:
            pedType  = PED_TYPE_MEDIC;
            pedModel = FixInvalid(CStreaming::GetDefaultMedicModel());
            break;
        case MODEL_ENFORCER:
            pedType  = PED_TYPE_COP;
            pedModel = COP_TYPE_SWAT1;
            break;
        case MODEL_PREDATOR:
        case MODEL_POLMAV:
        case MODEL_COPCARLA:
        case MODEL_COPCARSF:
        case MODEL_COPCARVG:
        case MODEL_COPCARRU:
            pedType  = PED_TYPE_COP;
            pedModel = COP_TYPE_CITYCOP;
            break;
        case MODEL_RHINO:
        case MODEL_BARRACKS:
            pedType  = PED_TYPE_COP;
            pedModel = COP_TYPE_ARMY;
            break;
        case MODEL_FBIRANCH:
            pedType  = PED_TYPE_COP;
            pedModel = COP_TYPE_FBI;
            break;
        case MODEL_COPBIKE:
            pedType  = PED_TYPE_COP;
            pedModel = COP_TYPE_LAPDM1;
            break;
        case MODEL_STREAKC:
            pedModel = FixInvalid(ChooseCivilianOccupation());
            pedType  = CModelInfo::GetPedModelInfo(pedModel)->GetPedType();
            break;
        default: {
            if (carRating >= 14 && carRating <= 23) {
                pedType  = static_cast<ePedType>(carRating - 7);
                pedModel = FixInvalid(CGangs::ChooseGangPedModel(static_cast<eGangID>(carRating - 14)));
            } else {
                pedModel = FixInvalid(ChooseCivilianOccupationForVehicle(mustBeMale, veh));
                pedType  = CModelInfo::GetPedModelInfo(pedModel)->GetPedType();
            }
            if (!CModelInfo::GetModelInfo(pedModel)->GetRwObject()) { // modelo sin objeto RW: MALE01
                pedType  = CModelInfo::GetPedModelInfo(MODEL_MALE01)->GetPedType();
                pedModel = MODEL_MALE01;
            }
            break;
        }
        }
    }

    const auto ped = AddPed(pedType, static_cast<eModelID>(pedModel), vehPos, false);

    CCarEnterExit::SetPedInCarDirect(
        ped,
        veh,
        seatNumber >= 0 ? CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seatNumber) : 0,
        addAsDriver
    );

    if (isCriminal) {
        UpdatePedCount(ped, true);
        ped->m_nPedType = PED_TYPE_CRIMINAL;
        UpdatePedCount(ped, false);
    }

    return ped;
}

// 0x613CD0
// Como el original (0x613CD0..0x613D55): el modelo se elige antes de tirar el azar y el numero de peatones es
// 1 + (int)(4 * rand/32768) (formula de 0x407180, constantes 1 y 5 del EXE en 0x86C6C4/0x86C6C8).
void CPopulation::PlaceMallPedsAsStationaryGroup(const CVector& posn) {
    if (CGame::currArea == eAreaCodes::AREA_CODE_4) {
        const auto model   = ChooseCivilianOccupation();
        const auto pedType = CModelInfo::GetPedModelInfo(model)->GetPedType();
        const auto count   = CGeneral::GetRandomIntInRange(1, 5);
        CPedGroupPlacer{}.PlaceGroup(
            pedType,
            count,
            posn,
            ePedGroupDefaultTaskAllocatorType::STAND_STILL
        );
    }
}

// 0x613D60
// Como el original (0x613D60..0x614205). Diferencias corregidas: los tipos se comprueban antes que LaRiotsActiveHere; la camara
// usa IsSphereVisible(pos, 1.5f) y la distancia 2D sin elevar al cuadrado (> comparado con PedCreationDistMultiplier * 42.5); el
// radio de la zona es el de la esfera del modelo del marido; las velocidades se piden primero a la mujer; la tarea de pareja
// se reparte segun wifeSpeed > husbSpeed estricto (marido primero) o no (mujer primero); la posicion de la MUJER pasa a ser la
// del marido + ms_offsets.Offsets[4] (con suelo + 1) y si no hay suelo se eliminan marido y mujer; la comprobacion final usa la
// posicion sin ajustar de suelo y el radio del modelo de la mujer, y se elimina primero al marido y luego a la mujer.
void CPopulation::PlaceCouple(ePedType husbandPedType, eModelID husbandModelId, ePedType wifeyPedType, eModelID wifeyModelId, CVector placeAt) {
    if (husbandPedType != PED_TYPE_CIVMALE) {
        return;
    }

    if (wifeyPedType != PED_TYPE_CIVFEMALE) {
        return;
    }

    if (CGameLogic::LaRiotsActiveHere()) {
        return;
    }

    if (TheCamera.IsSphereVisible(placeAt, 1.5f)) {
        const auto& plyrPos = FindPlayerPed()->GetPosition();
        const float dy      = placeAt.y - plyrPos.y;
        const float dx      = placeAt.x - plyrPos.x;
        const float dist    = std::sqrt(dx * dx + dy * dy);
        if (PedCreationDistMultiplier() * 42.5f > dist) {
            return;
        }
    }

    if (!CPedPlacement::IsPositionClearForPed(placeAt, CModelInfo::GetModelInfo(husbandModelId)->GetColModel()->GetBoundRadius(), -1, nullptr, true, true, true)) {
        return;
    }

    bool bGroundHit{};
    const float groundZ1 = CWorld::FindGroundZFor3DCoord(CVector{ placeAt.x, placeAt.y, placeAt.z + 1.f }, &bGroundHit, nullptr) + 1.f;
    if (!bGroundHit) {
        return;
    }
    const float z = (placeAt.z > groundZ1) ? placeAt.z : groundZ1;

    if (!CModelInfo::GetModelInfo(husbandModelId)->GetRwObject()) {
        return;
    }

    const CVector pedPos{ placeAt.x, placeAt.y, z };
    const auto husb = AddPed(PED_TYPE_CIVMALE, husbandModelId, pedPos, true);
    if (!husb) {
        return;
    }
    CVisibilityPlugins::SetClumpAlpha(husb->GetRpClump(), 0);

    if (!CModelInfo::GetModelInfo(wifeyModelId)->GetRwObject()) {
        return;
    }

    const auto wifey = AddPed(PED_TYPE_CIVFEMALE, wifeyModelId, pedPos, true);
    if (!wifey) {
        return;
    }

    // Si alguno camina despacio o la diferencia es grande => no hay pareja
    const float wifeyWalkSpeed = wifey->GetWalkAnimSpeed();
    const float husbWalkSpeed  = husb->GetWalkAnimSpeed();
    if (wifeyWalkSpeed < 0.75f || husbWalkSpeed < 0.75f || std::abs(wifeyWalkSpeed - husbWalkSpeed) > 0.45f) {
        return;
    }

    // 0x613F97: el reparto depende de wifeSpeed > husbSpeed (estricto)
    if (wifeyWalkSpeed > husbWalkSpeed) {
        husb->GetTaskManager().SetTask(new CTaskComplexBeInCouple{ wifey, false, true, true, 10.f }, TASK_PRIMARY_PRIMARY, false);
        wifey->GetTaskManager().SetTask(new CTaskComplexBeInCouple{ husb, true, true, true, 10.f }, TASK_PRIMARY_PRIMARY, false);
    } else {
        wifey->GetTaskManager().SetTask(new CTaskComplexBeInCouple{ husb, false, true, true, 10.f }, TASK_PRIMARY_PRIMARY, false);
        husb->GetTaskManager().SetTask(new CTaskComplexBeInCouple{ wifey, true, true, true, 10.f }, TASK_PRIMARY_PRIMARY, false);
    }

    // Posicion de la mujer: la del marido + offset
    const auto& offset = CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[4];
    const CVector newPos = husb->GetPosition() + CVector{ offset.x, offset.y, 0.f };
    bGroundHit = false;
    const float newGroundZ1 = CWorld::FindGroundZFor3DCoord(CVector{ newPos.x, newPos.y, newPos.z + 1.f }, &bGroundHit, nullptr) + 1.f;
    if (!bGroundHit) {
        RemovePed(husb);
        RemovePed(wifey);
        return;
    }
    const float newZ = (newPos.z > newGroundZ1) ? newPos.z : newGroundZ1;
    wifey->SetPosn(newPos.x, newPos.y, newZ);

    CEntity* hits[3]{};
    CPedPlacement::IsPositionClearForPed(newPos, CModelInfo::GetModelInfo(wifeyModelId)->GetColModel()->GetBoundRadius(), 3, hits, true, true, true);
    for (auto* const hit : hits) {
        if (hit && hit != husb && hit != wifey) { // Blocked by something
            RemovePed(husb);
            RemovePed(wifey);
            return;
        }
    }
    CVisibilityPlugins::SetClumpAlpha(wifey->GetRpClump(), 0); // All good
}

// 0x614210
// Como el original (0x614210..0x6143D6). Diferencias corregidas: el rechazo por cercania es "distancia 2D al cuadrado al peaton
// mas cercano <= 0.1225f" (0.35^2, recorrido de atras hacia delante, menor estricto); el evento del atractor lleva la ENTIDAD del
// atractor (no el peaton) y vive hasta despues de CPedIntelligence::Process().
bool CPopulation::AddPedAtAttractor(eModelID modelIndex, C2dEffectPedAttractor* attractor, CVector posn, CEntity* entity, int32 decisionMakerType) {
    float closestSq = std::bit_cast<float>(0x7F7FFFFFu); // 0x863A3C
    {
        auto* const pool = GetPedPool();
        for (auto i = pool->GetSize(); i-- > 0;) {
            auto* const nearbyPed = pool->GetAt(i);
            if (!nearbyPed) {
                continue;
            }
            const auto& nearbyPos = nearbyPed->GetPosition();
            const float dx        = posn.x - nearbyPos.x;
            const float dy        = posn.y - nearbyPos.y;
            const float distSq    = dy * dy + dx * dx;
            if (distSq < closestSq) {
                closestSq = distSq;
            }
        }
    }
    if (!(closestSq > 0.1225f)) {
        return false;
    }

    if (!GetPedAttractorManager()->HasQueueTailArrivedAtSlot(attractor, entity)) {
        return false;
    }

    const auto ped = AddPed(CModelInfo::GetPedModelInfo(modelIndex)->GetPedType(), modelIndex, posn, false);
    if (!ped) {
        return false;
    }

    ped->SetCharCreatedBy(PED_GAME);
    ped->GetIntelligence()->SetPedDecisionMakerType(decisionMakerType == -1 ? 2 : decisionMakerType);
    ped->GetTaskManager().SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped), TASK_PRIMARY_DEFAULT);

    CPedAttractorPedPlacer::PlacePedAtEffect(*attractor, entity, ped, 0.02f);
    ped->bUseAttractorInstantly = true;

    CEventAttractor attractorEvent{ attractor, entity, true, static_cast<eTaskType>(233) }; // 0x61438F: mov word [event+12h],0E9h (el enum de la DLL vale 235)
    ped->GetEventGroup().Add(&attractorEvent, false);

    ped->GetIntelligence()->ProcessEventHandler();
    ped->GetIntelligence()->Process();

    return true;
}

// 0x6143E0
// NOTSA: Added option to use `ePedType::NONE` as valid value to ignore the ped type (this way you can get the distance of the nearest ped of any type)
// Como el original (0x6143E0..0x614484): recorre el pool de atras hacia delante; cada distancia es
// sqrt((dz*dz + dy*dy) + dx*dx) en float y se queda la menor estricta; sin peatones devuelve 10000000.0f.
float CPopulation::FindDistanceToNearestPedOfType(ePedType pedType, CVector posn) {
    float closest = 10'000'000.f; // 0x86D270
    auto* const pool = GetPedPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const ped = pool->GetAt(i);
        if (!ped) {
            continue;
        }
        if (pedType != PED_TYPE_NONE /*notsa*/ && ped->m_nPedType != pedType) {
            continue;
        }
        const auto& pedPos = ped->GetPosition();
        const float dx     = posn.x - pedPos.x;
        const float dy     = posn.y - pedPos.y;
        const float dz     = posn.z - pedPos.z;
        const float dist   = std::sqrt((dz * dz + dy * dy) + dx * dx);
        if (dist < closest) {
            closest = dist;
        }
    }
    return closest;
}

float CPopulation::FindDistanceToNearestPed(CVector pos) {
    return FindDistanceToNearestPedOfType(PED_TYPE_NONE, pos);
}

// 0x614490
eModelID CPopulation::PickGangCar(eGangID forGang) {
    return (eModelID)m_LoadedGangCars[(size_t)(forGang)].PickRandomCar(false, false);
}

// 0x6144B0
// Como el original (0x6144B0..0x614579): el indice base es (int)((rand & 0xFFFF) * (1/32768) * 10) y el indice de la banda es
// (base + i) % 10; despues se recorren las 23 ranuras de cada grupo de la 0 a la 22 (el EXE no mira el
// recuento ni descarta -1) y el primer modelo cuya clase no sea BIG/MOPED/MOTORBIKE (5, 7, 8) es el elegido.
eModelID CPopulation::PickRiotRoadBlockCar() {
    // First try gang cars
    const float baseRoll = (static_cast<float>(CGeneral::GetRandomNumber() & 0xFFFF) * std::bit_cast<float>(0x38000000u)) * 10.f;
    const int32 baseIdx  = static_cast<int32>(static_cast<int64>(baseRoll));
    for (int32 i{}; i < static_cast<int32>(TOTAL_GANGS); i++) {
        const auto model = PickGangCar(static_cast<eGangID>((i + baseIdx) % 10));
        if (static_cast<int32>(model) >= 0) {
            return model;
        }
    }

    // Try appropriate/inappropriate cars
    for (const auto* const grp : { &m_AppropriateLoadedCars, &m_InAppropriateLoadedCars }) {
        const auto* const models = reinterpret_cast<const int16*>(grp); // CLoadedCarGroup: 23 x int16
        for (size_t i{}; i < 23; i++) {
            const int32 model = models[i];
            switch (CModelInfo::GetVehicleModelInfo(model)->m_nVehicleClass) {
            case VEHICLE_CLASS_BIG:
            case VEHICLE_CLASS_MOPED:
            case VEHICLE_CLASS_MOTORBIKE:
                continue;
            }
            return static_cast<eModelID>(model);
        }
    }

    // Nothing suits
    return CStreaming::GetDefaultCopCarModel(true);
}

// 0x614580
void CPopulation::ConvertToRealObject(CDummyObject* dummyObject) {
    if (!CPopulation::TestSafeForRealObject(dummyObject)) {
        return;
    }

    auto* obj = dummyObject->CreateObject();
    if (!obj) {
        return;
    }

    CWorld::Remove(dummyObject);
    dummyObject->SetIsVisible(false);
    dummyObject->ResolveReferences();

    obj->SetRelatedDummy(dummyObject);
    CWorld::Add(obj);

    if (!CGlass::IsObjectGlass(obj) || obj->GetModelInfo()->IsGlassType2()) {
        if (obj->m_nModelIndex == ModelIndices::MI_BUOY || obj->physicalFlags.bAttachedToEntity) {
            obj->SetIsStatic(false);
            obj->m_vecMoveSpeed.Set(0.0F, 0.0F, -0.001F);
            obj->physicalFlags.bTouchingWater = true;
            obj->AddToMovingList();
        }
    } else {
        obj->SetIsVisible(false);
    }
}

// 0x614670
void CPopulation::ConvertToDummyObject(CObject* object) {
    auto* dummy = object->m_pDummyObject;
    if (dummy) {
        if (!CPopulation::TestRoomForDummyObject(object)) {
            return;
        }
        dummy->SetIsVisible(true);
        dummy->UpdateFromObject(object);
    }

    if (object->GetIsTypeObject()) {
        auto* mi = object->GetModelInfo()->AsAtomicModelInfoPtr();
        if (mi && mi->IsGlassType1()) {
            if (dummy) {
                dummy->SetIsVisible(false);
            } else {
                assert(false && "FIX_BUGS: dummy == nullptr");
            }
        }
    }

    CWorld::Remove(object);
    delete object;

    if (dummy) {
        CWorld::Add(dummy);
    }
}

// 0x614720
namespace {
template<uintptr_t Address, typename... Args>
CTask* PopulationTask(size_t size, Args... args) {
    auto* memory = static_cast<CTask*>(CTask::operator new(size));
    return memory ? plugin::CallMethodAndReturn<CTask*, Address>(memory, args...) : nullptr;
}
float PopulationModelRadius(eModelID model) {
    return CModelInfo::GetModelInfo(model)->GetColModel()->m_boundSphere.m_fRadius;
}
bool PopulationModelLoaded(eModelID model) {
    return *reinterpret_cast<void* const*>(reinterpret_cast<const uint8*>(CModelInfo::GetModelInfo(model))+0x1C) != nullptr;
}
void PopulationCouple(eModelID maleModel, eModelID femaleModel, CVector position) {
    if (CGameLogic::LaRiotsActiveHere()) return;
    if (TheCamera.IsSphereVisible(position,1.5f)) {
        const auto delta=position-FindPlayerPed()->GetPosition();
        const float distance=std::sqrt(delta.y*delta.y+delta.x*delta.x);
        if (distance < CPopulation::PedCreationDistMultiplier()*42.5f) return;
    }
    if (!CPedPlacement::IsPositionClearForPed(position,PopulationModelRadius(maleModel),-1,nullptr,true,true,true)) return;
    bool groundFound;
    const float ground=CWorld::FindGroundZFor3DCoord({position.x,position.y,position.z+1.0f},&groundFound,nullptr)+1.0f;
    if (!groundFound) return;
    if (position.z <= ground) position.z=ground;
    if (!PopulationModelLoaded(maleModel)) return;
    auto* male=CPopulation::AddPed(PED_TYPE_CIVMALE,maleModel,position,true);
    if (!male) return;
    CVisibilityPlugins::SetClumpAlpha(male->GetRpClump(),0);
    if (!PopulationModelLoaded(femaleModel)) return;
    auto* female=CPopulation::AddPed(PED_TYPE_CIVFEMALE,femaleModel,position,true);
    if (!female) return;
    const float femaleSpeed=female->GetWalkAnimSpeed(),maleSpeed=male->GetWalkAnimSpeed();
    if (!(femaleSpeed >= .75f) || !(maleSpeed >= .75f) || std::abs(femaleSpeed-maleSpeed)>.45f) return;
    if (femaleSpeed>maleSpeed) {
        male->GetTaskManager().SetTask(PopulationTask<0x6836F0>(0x20,female,false,true,true,10.0f),TASK_PRIMARY_PRIMARY,false);
        female->GetTaskManager().SetTask(PopulationTask<0x6836F0>(0x20,male,true,true,true,10.0f),TASK_PRIMARY_PRIMARY,false);
    } else {
        female->GetTaskManager().SetTask(PopulationTask<0x6836F0>(0x20,male,false,true,true,10.0f),TASK_PRIMARY_PRIMARY,false);
        male->GetTaskManager().SetTask(PopulationTask<0x6836F0>(0x20,female,true,true,true,10.0f),TASK_PRIMARY_PRIMARY,false);
    }
    auto malePosition=female->GetPosition()+CVector{CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[4]};
    const float maleGround=CWorld::FindGroundZFor3DCoord({malePosition.x,malePosition.y,malePosition.z+1.0f},&groundFound,nullptr)+1.0f;
    if (!groundFound) {CPopulation::RemovePed(male);CPopulation::RemovePed(female);return;}
    if (malePosition.z<=maleGround) malePosition.z=maleGround;
    female->SetPosn(malePosition.x,malePosition.y,malePosition.z);
    std::array<CEntity*,3> hits{};
    CPedPlacement::IsPositionClearForPed(malePosition,PopulationModelRadius(femaleModel),3,hits.data(),true,true,true);
    for (auto* hit:hits) if(hit && hit!=male && hit!=female) {CPopulation::RemovePed(male);CPopulation::RemovePed(female);return;}
    CVisibilityPlugins::SetClumpAlpha(female->GetRpClump(),0);
}
void PopulationDefaultTask(CPed* ped,ePedType type) {
    if (!CGameLogic::LaRiotsActiveHere()) {
        reinterpret_cast<CScriptsForBrains*>(0xA90CF0)->CheckIfNewEntityNeedsScript(ped,0,nullptr);
    } else if ((CGeneral::GetRandomNumber()&3)!=0) {
        auto* memory=static_cast<CTask*>(CTask::operator new(0x38));
        CTask* task=nullptr;
        if(memory) task=plugin::CallMethodAndReturn<CTask*,0x48E610>(memory,6,CGeneral::GetRandomIntInRange(0,8),true);
        ped->GetTaskManager().SetTask(task,TASK_PRIMARY_DEFAULT,false);
    } else if(type!=PED_TYPE_COP) {
        auto* memory=static_cast<CObject*>(CObject::operator new(0x17C));
        auto* object=memory?::new(memory) CObject(ModelIndices::MI_TELLY,true):nullptr; // CObject::CObject(int32, bool) 0x5A1D70 en C++
        CWorld::Add(object);
        ped->GetTaskManager().SetTask(CTaskComplexWander::GetWanderTaskByPedType(ped),TASK_PRIMARY_DEFAULT,false);
        CVector offset{0.0f,.45f,.35f};
        auto* task=PopulationTask<0x6913A0>(0x3C,object,&offset,uint8(1),uint8(1),0x13E,0x51,false);
        plugin::CallMethod<0x681B60>(&ped->GetTaskManager(),task,4);
    }
}
bool PopulationSunbathe(CPed* ped,const CVector& position) {
    CColPoint hit;
    CEntity* entity=nullptr;
    CWorld::ProcessVerticalLine(position+CVector{0,0,2.0f},position.z-2.0f,hit,entity,true,false,false,false,false,false,nullptr);
    const uint8 surface=static_cast<uint8>(hit.m_nSurfaceTypeB);
    const bool force=*reinterpret_cast<bool*>(0x969159);
    if(!entity || ((!g_surfaceInfos.IsBeach(surface)&&surface!=0x22&&surface!=0xA0&&!force)) || (!plugin::CallAndReturn<bool,0x632140>()&&!force)) return false;
    bool clear=true;
    std::array<CEntity*,2> nearby{};
    if(CPedPlacement::IsPositionClearForPed(position,3.0f,2,nearby.data(),true,true,true)) {
        for(auto* other:nearby) if(other && other!=ped) clear=false;
    }
    if(!clear) return false;
    const bool towelRequired=surface!=0xA0;
    CVector at=ped->GetPosition();
    CColPoint ground;
    CEntity* groundEntity=nullptr;
    if(CWorld::ProcessVerticalLine(at+CVector{0,0,10.0f},-10.0f,ground,groundEntity,true,false,false,false,true,false,nullptr) &&
       (ground.m_vecNormal.x+ground.m_vecNormal.y)*0.0f+ground.m_vecNormal.z>.95f) {
        const float angle=CGeneral::GetRandomNumberInRange(0.0f,1.0f)*std::bit_cast<float>(0x40C90FDBu);
        ped->m_fAimingRotation=ped->m_fCurrentRotation=angle;
        plugin::CallMethod<0x43E0C0>(&ped->GetMatrix(),angle);
        CObject* towel=nullptr;
        if(towelRequired) {
            at.z=hit.m_vecPoint.z+.04f;
            towel=plugin::CallAndReturn<CObject*,0x6EABA0>(&at,0xB);
            if(towel) {
                plugin::CallMethod<0x43E0C0>(&towel->GetMatrix(),angle);
                auto& matrix=towel->GetMatrix();
                matrix.GetUp()=ground.m_vecNormal;
                matrix.GetRight()=CrossProduct(matrix.GetUp(),matrix.GetForward());
                matrix.GetForward()=CrossProduct(matrix.GetUp(),matrix.GetRight());
                towel->UpdateRwMatrix();
                towel->UpdateRwFrame();
                if((CGeneral::GetRandomNumber()&3)==0) {
                    const auto right=matrix.GetRight()*CGeneral::GetRandomNumberInRange(-.5f,.5f);
                    at+=right;
                    const auto forward=matrix.GetForward()*CGeneral::GetRandomNumberInRange(-1.0f,1.0f);
                    at+=forward;
                    plugin::CallAndReturn<CObject*,0x6EABA0>(&at,6);
                }
            }
        }
        const bool standing=(CGeneral::GetRandomNumber()&3)==0;
        ped->GetTaskManager().SetTask(PopulationTask<0x631F80>(0x38,towel,standing),TASK_PRIMARY_PRIMARY,false);
    }
    return true;
}
}

bool CPopulation::AddToPopulation(float minDist, float maxDist, float minDistClose, float maxDistClose) {
    ZoneScoped;
    if(CGangWars::DontCreateCivilians() || *reinterpret_cast<bool*>(0x96917A)) return false;
    const uint8 playerId=CWorld::PlayerInFocus;
    const CVector playerPosition=FindPlayerCentreOfWorld(playerId);
    auto* info=reinterpret_cast<uint8*>(0xB7CD98)+uint32(playerId)*400;
    auto* player=*reinterpret_cast<CPed**>(info);
    auto* data=*reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(player)+0x480);
    auto* wanted=data?*reinterpret_cast<uint8**>(data):nullptr;
    bool forceCop=false;
    if(*reinterpret_cast<int32*>(wanted+0x2C)>2 && static_cast<int32>(ms_nNumCop)<wanted[0x19] && !CGangWars::GangWarFightingGoingOn() &&
       (*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(player)+0x46C)&0x100)==0) {
        const int32 lawCars=static_cast<int32>(CCarCtrl::NumLawEnforcerCars);
        const float targetRandom=float(*reinterpret_cast<uint16*>(info+0x130))*CCarCtrl::CarDensityMultiplier;
        const int32 allCars=CCarCtrl::NumRandomCars+CCarCtrl::NumMissionCars+static_cast<int32>(CCarCtrl::NumParkedCars+CCarCtrl::NumLawEnforcerCars+CCarCtrl::NumAmbulancesOnDuty+CCarCtrl::NumFireTrucksOnDuty);
        if(lawCars>=wanted[0x1A] || targetRandom<=float(CCarCtrl::NumRandomCars) || allCars>=static_cast<int32>(CCarCtrl::MaxNumberOfCarsInUse)) {
            forceCop=true;
            minDist=PedCreationDistMultiplier()*42.5f;
            maxDist=PedCreationDistMultiplier()*50.5f;
        }
    }
    if(CGameLogic::LaRiotsActiveHere() && PedDensityMultiplier>.1f && ms_nNumCop<1) forceCop=true;
    float desired=static_cast<float>(static_cast<int32>(MaxNumberOfPedsInUse));
    CDarkel::FrenzyOnGoing();
    const float cycleTotal=CPopCycle::m_NumOther_Peds+CPopCycle::m_NumCops_Peds+CPopCycle::m_NumGangs_Peds+CPopCycle::m_NumDealers_Peds;
    if(cycleTotal<=desired) desired=cycleTotal;
    if(CGame::currArea) desired=static_cast<float>(static_cast<int32>(NumberOfPedsInUseInterior));
    const float target=(CCullZones::FewerPeds()?.6f:1.0f)*PedDensityMultiplier*desired;
    const bool underLimit=static_cast<float>(static_cast<int32>(ms_nTotalPeds))<target;
    ePedType type;
    eModelID model;
    eModelID maleModel=MODEL_INVALID,femaleModel=MODEL_INVALID;
    uint32 count=1;
    if(forceCop) {type=PED_TYPE_COP;model=static_cast<eModelID>(0);} else {
        if(!underLimit) return false;
        const bool high=playerPosition.z>950.0f;
        if(!CPopCycle::FindNewPedType(type,model,high,high)) return underLimit;
        if((type==PED_TYPE_CIVMALE||type==PED_TYPE_CIVFEMALE) && CGeneral::GetRandomNumberInRange(0.0f,1.0f)>.9f) {
            ChooseCivilianCoupleOccupations(maleModel,femaleModel);
            if(maleModel==MODEL_INVALID||femaleModel==MODEL_INVALID) return underLimit;
            type=static_cast<ePedType>(*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(CModelInfo::GetModelInfo(maleModel))+0x28));
        }
        if(m_AllRandomPedsThisType>0) type=static_cast<ePedType>(m_AllRandomPedsThisType);
        if(type>=PED_TYPE_GANG1 && type<=PED_TYPE_GANG10) count=RandomIntInLimitRange();
    }
    const bool gang=type>=PED_TYPE_GANG1&&type<=PED_TYPE_GANG10;
    if(gang) {minDist+=30.0f;maxDist+=30.0f;}
    bool lowTraffic=false;
    if(type==PED_TYPE_COP) lowTraffic=static_cast<int32>(FindPlayerWanted()->GetWantedLevel())>0;
    CVector position;
    CNodeAddress first,second;
    first.m_wAreaId=second.m_wAreaId=0xFFFF;
    float orientation;
    if(!plugin::CallMethodAndReturn<bool,0x44E790>(&ThePaths,playerPosition.x,playerPosition.y,minDist,maxDist,minDistClose,maxDistClose,&position,&first,&second,&orientation,lowTraffic,nullptr)) return underLimit;
    const auto traffic=[&](CNodeAddress node) {return reinterpret_cast<const uint8*>(ThePaths.GetPathNode(node))[0x1A]&0xF;};
    const uint32 density=std::min(traffic(first),traffic(second));
    if(uint32(CGeneral::GetRandomNumber()&0xF)>density) return underLimit;
    const auto random=CGeneral::GetRandomNumber();
    plugin::CallMethod<0x44DA30>(&ThePaths,first,second,random,&position.x,&position.y);
    if(gang) {PlaceGangMembers(type,count,position);return underLimit;}
    if(maleModel!=MODEL_INVALID&&femaleModel!=MODEL_INVALID) {PopulationCouple(maleModel,femaleModel,position);return underLimit;}
    CPed* firstPed=nullptr;
    for(uint32 i=0;i<count;++i) {
        if(type==PED_TYPE_COP) {
            if(int32(model)==4) {
                if(!CStreaming::IsModelLoaded(286)) return underLimit;
                const auto* weapon=CWeaponInfo::GetWeaponInfo(WEAPON_MP5,eWeaponSkill::STD);
                if(!CStreaming::IsModelLoaded(weapon->m_nModelId1)) return underLimit;
            } else if(int32(model)==2) {
                if(!CStreaming::IsModelLoaded(285)) return underLimit;
                const auto* weapon=CWeaponInfo::GetWeaponInfo(WEAPON_MICRO_UZI,eWeaponSkill::STD);
                if(!CStreaming::IsModelLoaded(weapon->m_nModelId1)) return underLimit;
            } else if(int32(model)==5) {
                if(!CStreaming::IsModelLoaded(287)) return underLimit;
                if(!CStreaming::IsModelLoaded(CWeaponInfo::GetWeaponInfo(WEAPON_MP5,eWeaponSkill::STD)->m_nModelId1)) return underLimit;
                if(!CStreaming::IsModelLoaded(CWeaponInfo::GetWeaponInfo(WEAPON_GRENADE,eWeaponSkill::STD)->m_nModelId1)) return underLimit;
            }
        } else if(!PopulationModelLoaded(model)) return underLimit;
        position.z+=.7f;
        if(i<count) {
            CGeneral::GetRandomNumber();
            if(firstPed) {
                const float low=float(i)*.75f,high=(float(i)+1.0f)*.75f;
                float x=CGeneral::GetRandomNumberInRange(low,high),y=CGeneral::GetRandomNumberInRange(low,high);
                if(CGeneral::GetRandomNumber()&1) x=-x;
                if(CGeneral::GetRandomNumber()&1) y=-y;
                const auto& firstPosition=firstPed->GetPosition();
                position.x=x+firstPosition.x;position.y=y+firstPosition.y;
            }
        }
        if(!CPedPlacement::IsPositionClearForPed(position,-1.0f,-1,nullptr,true,true,true)) return underLimit;
        if(i+1<count) {
            bool found;
            const float ground=CWorld::FindGroundZFor3DCoord({position.x,position.y,position.z+2.0f},&found,nullptr)+.7f;
            if(!found) return underLimit;
            if(position.z<=ground) position.z=ground;
        }
        bool allowed=true;
        if(TheCamera.IsSphereVisible(position,2.0f)) {
            const float y=position.y-playerPosition.y,x=position.x-playerPosition.x;
            const float distance=std::sqrt(y*y+x*x);
            if(PedCreationDistMultiplier()*42.5f>distance) allowed=false;
        }
        if(allowed && *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(CModelInfo::GetModelInfo(model))+0x2C)==0x28) allowed=IsSkateable(position);
        if(IsSunbather(model) && (CClock::ms_nGameClockHours<8 || CClock::ms_nGameClockHours>19)) allowed=false;
        if(type==PED_TYPE_DEALER) {
            if(!allowed || FindDistanceToNearestPedOfType(PED_TYPE_DEALER,position)<20.0f) return underLimit;
        }
        if(!allowed) return underLimit;
        if(type>=PED_TYPE_GANG1&&type<=PED_TYPE_GANG10&&type!=PED_TYPE_GANG2&&position.x>2400.0f&&position.x<2540.0f&&position.y>-1730.0f&&position.y<-1625.0f) return underLimit;
        auto* ped=AddPed(type,model,position,true);
        if(!IsSunbather(model) || (CGeneral::GetRandomNumber()&3)==0 || !PopulationSunbathe(ped,position)) PopulationDefaultTask(ped,type);
        if(i==0) firstPed=ped;
        CVisibilityPlugins::SetClumpAlpha(ped->GetRpClump(),0);
        if(i+1>=count || count<2) return underLimit;
    }
    return underLimit;
}

// 0x615970
// Como el original (0x615970..0x615C65). Diferencias corregidas: solo se aceptan atractores en el area EXACTA actual (sin el area
// 13); el radio se mide con la raiz de ((dz*dz + dy*dy) + dx*dx) y debe cumplir min < d < max (estricto, sin elevar al
// cuadrado); la probabilidad de usar el modelo policia es GetRandomNumberInRange(0, 100) <= 70 (formula entera de 0x407180);
// modelos <= 0 y MALE01 se descartan; el bucle externo tiene 12 pasadas que se reinician al crear un peaton, y con
// decisionMaker == -1 se sale de la funcion al crear el primero; las entidades sin matriz reciben una.
int32 CPopulation::GeneratePedsAtAttractors(
    CVector pos,
    float   minRadius,
    float   maxRadius,
    float   minRadiusClose,
    float   maxRadiusClose,
    int32   decisionMaker,
    int32   numPedsToCreate
) {
    ZoneScoped;

    if (!numPedsToCreate) {
        return 0;
    }

    CEntity* entitiesInRng[512];
    int16    numEntitiesInRng{};
    CWorld::FindObjectsInRange(pos, maxRadius, false, &numEntitiesInRng, (int16)std::size(entitiesInRng), entitiesInRng, true, false, false, true, false);

    int32 numPedsCreated{};
    if (numPedsToCreate <= 0) {
        return numPedsCreated;
    }

    int32 pass{};
    while (pass < 12) {
        pass++;
        for (int16 o{}; o < numEntitiesInRng; o++) {
            if (!numPedsToCreate) {
                return numPedsCreated;
            }

            const auto ent = entitiesInRng[o];
            if (!ent->GetRwObject()) {
                continue;
            }
            if (static_cast<int32>(static_cast<uint8>(ent->GetAreaCode())) != CGame::currArea) {
                continue;
            }
            auto* const attractor = notsa::cast_if_present<C2dEffectPedAttractor>(ent->GetRandom2dEffect(EFFECT_ATTRACTOR, true));
            if (!attractor || !IsCorrectTimeOfDayForEffect(*attractor)) {
                continue;
            }
            if (attractor->m_nFlags & 1) {
                if (!ent->GetIsTypeObject()) {
                    continue;
                }
                if (!ent->AsObject()->objectFlags.bEnableDisabledAttractors) {
                    continue;
                }
            }

            if (!ent->m_matrix) {
                ent->AllocateMatrix();
                ent->m_placement.UpdateMatrix(ent->m_matrix);
            }
            const CVector effectPosWS = ent->GetMatrix().TransformPoint(attractor->m_Pos); // ws = world space

            const float dx   = pos.x - effectPosWS.x;
            const float dy   = pos.y - effectPosWS.y;
            const float dz   = pos.z - effectPosWS.z;
            const float dist = std::sqrt((dz * dz + dy * dy) + dx * dx);
            const bool  isVisible = TheCamera.IsSphereVisible(effectPosWS, 2.f);
            const float minDist   = isVisible ? minRadius : minRadiusClose;
            const float maxDist   = isVisible ? maxRadius : maxRadiusClose;
            if (!(dist > minDist)) {
                continue;
            }
            if (!(dist < maxDist)) {
                continue;
            }

            eModelID model;
            if (bInPoliceStation
                && CGeneral::GetRandomIntInRange(0, 100) <= 70
                && PedMICanBeCreatedAtThisAttractor(CStreaming::GetDefaultCopModel(), attractor->m_szScriptName)
            ) {
                model = CStreaming::GetDefaultCopModel();
                decisionMaker = 1; // TODO: Shouldn't this be local to this iteration instead? Right now this will presist into all futher iterations...
            } else {
                model = ChooseCivilianOccupation(
                    false,
                    false,
                    ANIM_GROUP_NONE,
                    MODEL_INVALID,
                    ePedStats::NONE,
                    true,
                    true,
                    true,
                    attractor->m_szScriptName
                );
            }

            if (static_cast<int32>(model) <= 0 || model == MODEL_MALE01) {
                continue;
            }

            if (!AddPedAtAttractor(model, attractor, effectPosWS, ent, decisionMaker)) {
                continue;
            }

            numPedsCreated++;

            if (decisionMaker == -1) {
                return numPedsCreated;
            }

            numPedsToCreate--;
            pass = 0;
        }

        if (numPedsToCreate <= 0) {
            break;
        }
    }

    return numPedsCreated;
}

// 0x615C90
// Como el original (0x615C90..0x615DB7): PedCreationDistMultiplier() se evalua dos veces en cada llamada (primero el cuarto
// argumento) y FindPlayerCentreOfWorld recibe CWorld::PlayerInFocus.
void CPopulation::GeneratePedsAtStartOfGame() {
    ZoneScoped;

    for (int32 i = 100; i --> 0;) { // "down to" operator in use
        UpdatePedCounts();
        ms_nTotalPeds -= ms_nTotalCarPassengerPeds; // NOTE/TODO: I wonder why this part isnt in `UpdatePedCounts()`

        const float maxRadiusClose = PedCreationDistMultiplier() * 50.5f;
        const float maxRadius      = PedCreationDistMultiplier() * 50.5f;
        AddToPopulation(10.f, maxRadius, 10.f, maxRadiusClose);
    }

    const CVector& centre        = FindPlayerCentreOfWorld(CWorld::PlayerInFocus);
    const float maxRadiusClose = PedCreationDistMultiplier() * 50.5f;
    const float maxRadius      = PedCreationDistMultiplier() * 50.5f;
    GeneratePedsAtAttractors(centre, 10.f, maxRadius, 10.f, maxRadiusClose, -1, 1);
}

// 0x615DC0
void CPopulation::ManageObject(CObject* object, const CVector& posn) {
    if (!object->CanBeDeleted()) return;
    const auto distanceTo = [&](const CEntity* entity) {
        const auto& p = entity->GetPosition();
        const float x = p.x - posn.x, y = p.y - posn.y, z = p.z - posn.z;
        // Como el original: sumar Z, Y y despues X.
        return std::sqrt((z * z + y * y) + x * x);
    };
    const float distance = distanceTo(object);
    const int32 model = object->GetModelId();
    if (!object->IsTemporary()) {
        const float dummyDistance = object->m_pDummyObject ? distanceTo(object->m_pDummyObject) : 100000.0f;
        const float limit = model == ModelIndices::MI_SAMSITE || model == ModelIndices::MI_SAMSITE2 ? 750.0f : 80.0f;
        if (!(distance > limit)) return;
        if (dummyDistance <= FindDummyDistForModel(static_cast<eModelID>(model))) return;
        ConvertToDummyObject(object);
        return;
    }
    const auto remove = [&] { CWorld::Remove(object); delete object; };
    if (model == ModelIndices::MI_ROADWORKBARRIER1 || model == ModelIndices::MI_ROADBLOCKFUCKEDCAR1 || model == ModelIndices::MI_ROADBLOCKFUCKEDCAR2 || model == ModelIndices::MI_BEACHBALL) {
        if (!(distance <= 120.0f)) remove();
        return;
    }
    if (static_cast<uint32>(model) >= static_cast<uint32>(ModelIndices::MI_BEACHTOWEL01) && static_cast<uint32>(model) <= static_cast<uint32>(ModelIndices::MI_BEACHTOWEL04)) {
        if (distance <= 64.5f && (distance <= 35.0f || object->GetIsOnScreen())) return;
        remove();
        return;
    }
    if (distance > 54.5f || (distance > 25.0f && !object->GetIsOnScreen()) || object->m_nRemovalTime < CTimer::GetTimeInMS()) {
        remove();
        return;
    }
    if (object->GetRwObject() && RwObjectGetType(object->GetRwObject()) == rpCLUMP && object->objectFlags.bFadingIn) {
        if (!CVisibilityPlugins::GetClumpAlpha(object->GetRpClump()) || !object->IsVisible()) remove();
    }
}

// 0x616000
// Como el original (0x616000..0x616098): la distancia es sqrt((dz*dz + dy*dy) + dx*dx) y el dummy solo se convierte si es
// estrictamente menor que el limite del modelo (no se compara al cuadrado); el indice del modelo se extiende con signo.
void CPopulation::ManageDummy(CDummy* dummy, const CVector& posn) {
    if (!dummy->IsInCurrentArea() || !dummy->GetIsVisible()) {
        return;
    }
    const auto& dummyPos = dummy->GetPosition();
    const float dx       = dummyPos.x - posn.x;
    const float dy       = dummyPos.y - posn.y;
    const float dz       = dummyPos.z - posn.z;
    const float dist     = std::sqrt((dz * dz + dy * dy) + dx * dx);
    const float limit    = FindDummyDistForModel(static_cast<eModelID>(static_cast<int16>(dummy->m_nModelIndex)));
    if (!(dist < limit)) {
        return;
    }
    ConvertToRealObject(static_cast<CDummyObject*>(dummy));
}

// 0x6160A0
// Como el original (0x6160A0..0x616186): el centro se copia por valor, FindPlayerPed recibe 0, solo se ignora el objeto sujetado
// si es de tipo objeto, y los pools se recorren de atras hacia delante.
void CPopulation::ManageAllPopulation() {
    const CVector center = FindPlayerCentreOfWorld(CWorld::PlayerInFocus);

    CEntity* objPlyrIsHolding = nullptr;
    // Como el original (0x6160E7..0x616101): GetTaskHold ya devuelve CTaskSimpleHoldEntity*, asi que el dyn_cast no comprobaba nada; hay que
    // mirar GetTaskType() == 0x133 (virtual) antes de leer la entidad sujetada (+8).
    CTask* const holdTask = FindPlayerPed(0)->GetIntelligence()->GetTaskHold(false);
    if (holdTask && holdTask->GetTaskType() == TASK_SIMPLE_HOLD_ENTITY) {
        if (const auto held = static_cast<CTaskSimpleHoldEntity*>(holdTask)->GetHeldEntity(); held->GetIsTypeObject()) {
            objPlyrIsHolding = held;
        }
    }

    auto* const objPool = GetObjectPool();
    for (auto i = objPool->GetSize(); i-- > 0;) {
        auto* const obj = objPool->GetAt(i);
        if (obj && static_cast<CEntity*>(obj) != objPlyrIsHolding) {
            ManageObject(obj, center);
        }
    }

    auto* const dummyPool = GetDummyPool();
    for (auto i = dummyPool->GetSize(); i-- > 0;) {
        auto* const dummy = dummyPool->GetAt(i);
        if (dummy) {
            ManageDummy(dummy, center);
        }
    }
}

// 0x616190
// Como el original (0x616190..0x6162F1): lote = byte bajo del contador de fotogramas & 0x1F; los rangos de los pools de objetos y
// dummies se calculan con division con signo por 32 (n + ((n >> 31) & 31)) >> 5; los peatones se recorren de atras hacia delante;
// el centro del jugador se copia por valor.
void CPopulation::ManagePopulation() {
    ZoneScoped;
    constexpr int32 framePopulation = 32;
    const int32 batch = static_cast<int32>(CTimer::m_FrameCounter & 0xFF) & 0x1F;
    const CVector centre = FindPlayerCentreOfWorld(CWorld::PlayerInFocus);

    const auto BatchRange = [&](int32 poolSize) {
        const int32 from = poolSize * batch;
        const int32 to   = poolSize * (batch + 1);
        return std::pair<int32, int32>{ (from + ((from >> 31) & (framePopulation - 1))) >> 5, (to + ((to >> 31) & (framePopulation - 1))) >> 5 };
    };

    {
        ZoneScopedN("Manage Objects");
        auto* pool = GetObjectPool();
        const auto [startIdx, endIdx] = BatchRange(static_cast<int32>(pool->GetSize()));
        for (auto i = startIdx; i < endIdx; ++i) {
            if (auto* obj = pool->GetAt(i)) {
                ManageObject(obj, centre);
            }
        }
    }

    {
        ZoneScopedN("Manage Dummies");
        auto* pool = GetDummyPool();
        const auto [startIdx, endIdx] = BatchRange(static_cast<int32>(pool->GetSize()));
        for (auto i = startIdx; i < endIdx; ++i) {
            if (auto* dummy = pool->GetAt(i)) {
                ManageDummy(dummy, centre);
            }
        }
    }

    {
        ZoneScopedN("Manage Peds");
        auto* pool = GetPedPool();
        for (auto i = pool->GetSize(); i-- > 0;) {
            if (auto* ped = pool->GetAt(i)) {
                ManagePed(ped, centre);
            }
        }
    }
}

// 0x616300
// Como el original (0x616300..0x61641D): solo en el fotograma 5 de cada 8; con menos de 8 huecos libres se elimina el peaton
// que se puede borrar (CanBeDeleted) mas cercano a la camara (distancia sqrt((dz*dz + dx*dx) + dy*dy), menor estricta, inicio
// 10000000.0f, de atras hacia delante).
void CPopulation::RemovePedsIfThePoolGetsFull() {
    ZoneScoped;

    if ((CTimer::GetFrameCounter() & 7) != 5) {
        return;
    }
    auto* const pool = GetPedPool();
    const int32 poolSize = static_cast<int32>(pool->GetSize());
    int32 numUsed{};
    for (int32 i{}; i < poolSize; i++) {
        if (!pool->IsFreeSlotAtIndex(i)) {
            numUsed++;
        }
    }
    if (poolSize - numUsed >= 8) {
        return;
    }

    CPed* closest{};
    float closestDist = 10'000'000.f;
    for (auto i = poolSize; i-- > 0;) {
        auto* const ped = pool->GetAt(i);
        if (!ped || !ped->CanBeDeleted()) {
            continue;
        }
        const auto& pedPos = ped->GetPosition();
        const auto& camPos = TheCamera.GetPosition();
        const float dx     = camPos.x - pedPos.x;
        const float dy     = camPos.y - pedPos.y;
        const float dz     = camPos.z - pedPos.z;
        const float dist   = std::sqrt((dz * dz + dx * dx) + dy * dy);
        if (dist < closestDist) {
            closestDist = dist;
            closest     = ped;
        }
    }
    if (closest) {
        CWorld::Remove(closest);
        delete closest;
    }
}

// 0x616420
// Como el original (0x616420..0x61646D): el pool de objetos se recorre de atras hacia delante.
void CPopulation::ConvertAllObjectsToDummyObjects() {
    auto* const pool = GetObjectPool();
    for (auto i = pool->GetSize(); i-- > 0;) {
        auto* const obj = GetObjectPool()->GetAt(i);
        if (!obj || obj->m_nObjectType != OBJECT_GAME) {
            continue;
        }
        ConvertToDummyObject(obj);
    }
}

// 0x616470
// Como el original (0x616470..0x616642). Diferencias corregidas: con numPeds <= 0 se sale; el numero de peatones de atractores es
// (int)((float)numPeds * 0.95f); el camino de nodos se pide con direcciones de nodo locales (area 0xFFFF) y el peaton se crea
// en la posicion generada (+0.9 en Z), no en la del parametro.
void CPopulation::PopulateInterior(int32 numPedsToCreate, CVector pos) {
    if (pos.z < 900.f) {
        return; // Not in interior
    }

    RemoveAllRandomPeds();
    CColStore::LoadCollision(pos, true);
    CStreaming::LoadAllRequestedModels(false);

    if (numPedsToCreate <= 0) {
        return;
    }

    numPedsToCreate -= GeneratePedsAtAttractors(pos, 0.f, 150.f, 0.f, 150.f, 7, static_cast<int32>(static_cast<int64>(static_cast<float>(numPedsToCreate) * 0.95f)));

    for (int32 i{}; numPedsToCreate > 0 && i < 25; i++) {
        float        orientation{};
        CVector      nodePos{};
        CNodeAddress nodeAddrA{}, nodeAddrB{};
        if (!ThePaths.GeneratePedCreationCoors_Interior(pos.x, pos.y, &nodePos, &nodeAddrA, &nodeAddrB, &orientation)) {
            continue;
        }

        ePedType ptype{};
        eModelID pmodel{};
        if (!CPopCycle::FindNewPedType(ptype, pmodel, true, true)) {
            continue;
        }

        nodePos.z += 0.9f;

        const auto ped = AddPed(ptype, pmodel, nodePos, true);

        numPedsToCreate--;

        ped->GetIntelligence()->SetPedDecisionMakerType(7);

        if (ped->m_nAnimGroup == CAnimManager::GetAnimationGroupIdByName("jogger")) { // TODO: Move `GetAnimationGroupId` out the loop?
            ped->m_nAnimGroup = CAnimManager::GetAnimationGroupIdByName("man");
        }
    }
}

// 0x616650
// Como el original (0x616650..0x616840): con region desconocida (> 4) CurrentWorldZone no cambia; las cuatro distancias se
// calculan con una llamada a PedCreationDistMultiplier() cada una (de la cuarta a la primera) y el centro se pide con
// CWorld::PlayerInFocus.
void CPopulation::Update(bool generatePeds) {
    ZoneScoped;

    CurrentWorldZone = [] {
        switch (CWeather::WeatherRegion) {
        case WEATHER_REGION_DEFAULT:
        case WEATHER_REGION_LA:
        case WEATHER_REGION_DESERT:
            return 0;
        case WEATHER_REGION_SF:
            return 1;
        case WEATHER_REGION_LV:
            return 2;
        default:
            return static_cast<int>(CurrentWorldZone); // Como el original: sin cambios
        }
    }();

    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    ManagePopulation();
    RemovePedsIfThePoolGetsFull();

    if (m_CountDownToPedsAtStart) {
        if (--m_CountDownToPedsAtStart == 0) {
            GeneratePedsAtStartOfGame();
        }
        return;
    }

    UpdatePedCounts();

    if (CCutsceneMgr::IsCutsceneProcessing() || !generatePeds) {
        return;
    }

    const float maxDistClose = PedCreationDistMultiplier() * 25.f;
    const float minDistClose = PedCreationDistMultiplier() * 25.f - 10.f;
    const float maxDist      = PedCreationDistMultiplier() * TheCamera.m_fGenerationDistMultiplier * 50.5f;
    const float minDist      = PedCreationDistMultiplier() * TheCamera.m_fGenerationDistMultiplier * 42.5f;

    if (AddToPopulation(minDist, maxDist, minDistClose, maxDistClose)) {
        const CVector& centre = FindPlayerCentreOfWorld(CWorld::PlayerInFocus);
        const int32 decisionMaker = CGame::currArea ? 7 : -1;
        const float maxRadiusClose = PedCreationDistMultiplier() * 25.f;
        const float minRadiusClose = PedCreationDistMultiplier() * 25.f - 10.f;
        const float maxRadius      = PedCreationDistMultiplier() * TheCamera.m_fGenerationDistMultiplier * 50.5f;
        const float minRadius      = PedCreationDistMultiplier() * TheCamera.m_fGenerationDistMultiplier * 42.5f;
        GeneratePedsAtAttractors(centre, minRadius, maxRadius, minRadiusClose, maxRadiusClose, decisionMaker, 1);
    }
}

// 0x406F50
// Como el original (0x406F50): recorre las primeras N ranuras (N = recuento con signo de 16 bits, la ranura se extiende sin
// signo a 32 bits).
bool CPopulation::DoesCarGroupHaveModelId(int32 carGroupId, int32 modelId) {
    const int32 count = static_cast<int16>(m_nNumCarsInGroup[carGroupId]);
    const auto* const models = reinterpret_cast<const uint16*>(&m_CarGroups[carGroupId][0]);
    for (int32 i{}; i < count; i++) {
        if (static_cast<int32>(models[i]) == modelId) {
            return true;
        }
    }
    return false;
}


// NOTSA
uint32 CPopulation::CalculateTotalNumGangPeds() {
    return notsa::accumulate(ms_nNumGang, 0u);
}

// NOTSA - Moved here for reuseability
void CPopulation::UpdatePedCounts() {
    ZoneScoped;

    ms_nTotalGangPeds = CalculateTotalNumGangPeds();
    ms_nTotalCivPeds = ms_nNumCivMale + ms_nNumCivFemale;
    ms_nTotalPeds = ms_nTotalCivPeds + ms_nTotalGangPeds + ms_nNumCop + ms_nNumEmergency;
}
