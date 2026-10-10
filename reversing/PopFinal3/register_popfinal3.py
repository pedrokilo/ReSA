"""Registro historico de las 55 funciones de CPopulation del banco PopFinal3 (corrida 20261009-202521-590640). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'PopFinal3',
 'run_id': '20261009-202521-590640',
 'probe_prefix': '629aed67',
 'tag': 'popfinal3',
 'registry_before': 7754,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CPopulation::AddExistingPedInCar': 64,
            'CPopulation::ArePedStatsCompatible': 1024,
            'CPopulation::CanJeerAtStripper': 128,
            'CPopulation::CanSolicitPlayerInCar': 128,
            'CPopulation::CanSolicitPlayerOnFoot': 64,
            'CPopulation::ChooseCivilianCoupleOccupations': 512,
            'CPopulation::ChooseCivilianOccupation': 1024,
            'CPopulation::ChooseCivilianOccupationForVehicle': 512,
            'CPopulation::ChooseGangOccupation': 64,
            'CPopulation::ChoosePolicePedOccupation': 64,
            'CPopulation::ConvertAllObjectsToDummyObjects': 128,
            'CPopulation::ConvertToDummyObject': 256,
            'CPopulation::ConvertToRealObject': 256,
            'CPopulation::DealWithZoneChange': 64,
            'CPopulation::DoesCarGroupHaveModelId': 256,
            'CPopulation::FindCarMultiplierMotorway': 64,
            'CPopulation::FindDistanceToNearestPedOfType': 256,
            'CPopulation::FindDummyDistForModel': 256,
            'CPopulation::FindNumberOfPedsWeCanPlaceOnBenches': 512,
            'CPopulation::FindPedDensityMultiplierCullZone': 64,
            'CPopulation::FindPedMultiplierMotorway': 64,
            'CPopulation::FindPedRaceFromName': 256,
            'CPopulation::FindSpecificDriverModelForCar_ToUse': 256,
            'CPopulation::GeneratePedsAtStartOfGame': 128,
            'CPopulation::Initialise': 128,
            'CPopulation::IsCorrectTimeOfDayForEffect': 256,
            'CPopulation::IsFemale': 128,
            'CPopulation::IsMale': 128,
            'CPopulation::IsSecurityGuard': 64,
            'CPopulation::IsSkateable': 256,
            'CPopulation::IsSunbather-ModelID': 128,
            'CPopulation::LoadCarGroups': 64,
            'CPopulation::LoadPedGroups': 64,
            'CPopulation::LoadSpecificDriverModelsForCar': 128,
            'CPopulation::ManageAllPopulation': 256,
            'CPopulation::ManageDummy': 256,
            'CPopulation::ManagePed': 1024,
            'CPopulation::ManagePopulation': 256,
            'CPopulation::MoveCarsAndPedsOutOfAbandonedZones': 64,
            'CPopulation::PedCreationDistMultiplier': 128,
            'CPopulation::PedMICanBeCreatedAtAttractor': 128,
            'CPopulation::PedMICanBeCreatedAtThisAttractor': 1024,
            'CPopulation::PedMICanBeCreatedInInterior': 128,
            'CPopulation::PickGangCar': 128,
            'CPopulation::PickRiotRoadBlockCar': 256,
            'CPopulation::PlaceGangMembers': 128,
            'CPopulation::PlaceMallPedsAsStationaryGroup': 128,
            'CPopulation::RemoveAllRandomPeds': 128,
            'CPopulation::RemovePed': 128,
            'CPopulation::RemovePedsIfThePoolGetsFull': 256,
            'CPopulation::RemoveSpecificDriverModelsForCar': 128,
            'CPopulation::Shutdown': 64,
            'CPopulation::TestRoomForDummyObject': 256,
            'CPopulation::Update': 512,
            'CPopulation::UpdatePedCount': 256},
 'total': 13376,
 'new': ['CPopulation::AddExistingPedInCar',
         'CPopulation::ArePedStatsCompatible',
         'CPopulation::CanJeerAtStripper',
         'CPopulation::CanSolicitPlayerInCar',
         'CPopulation::CanSolicitPlayerOnFoot',
         'CPopulation::ChooseCivilianCoupleOccupations',
         'CPopulation::ChooseCivilianOccupation',
         'CPopulation::ChooseCivilianOccupationForVehicle',
         'CPopulation::ChooseGangOccupation',
         'CPopulation::ChoosePolicePedOccupation',
         'CPopulation::ConvertAllObjectsToDummyObjects',
         'CPopulation::ConvertToDummyObject',
         'CPopulation::ConvertToRealObject',
         'CPopulation::DealWithZoneChange',
         'CPopulation::DoesCarGroupHaveModelId',
         'CPopulation::FindCarMultiplierMotorway',
         'CPopulation::FindDistanceToNearestPedOfType',
         'CPopulation::FindDummyDistForModel',
         'CPopulation::FindNumberOfPedsWeCanPlaceOnBenches',
         'CPopulation::FindPedDensityMultiplierCullZone',
         'CPopulation::FindPedMultiplierMotorway',
         'CPopulation::FindPedRaceFromName',
         'CPopulation::FindSpecificDriverModelForCar_ToUse',
         'CPopulation::GeneratePedsAtStartOfGame',
         'CPopulation::Initialise',
         'CPopulation::IsCorrectTimeOfDayForEffect',
         'CPopulation::IsFemale',
         'CPopulation::IsMale',
         'CPopulation::IsSecurityGuard',
         'CPopulation::IsSkateable',
         'CPopulation::IsSunbather-ModelID',
         'CPopulation::LoadCarGroups',
         'CPopulation::LoadPedGroups',
         'CPopulation::LoadSpecificDriverModelsForCar',
         'CPopulation::ManageAllPopulation',
         'CPopulation::ManageDummy',
         'CPopulation::ManagePed',
         'CPopulation::ManagePopulation',
         'CPopulation::MoveCarsAndPedsOutOfAbandonedZones',
         'CPopulation::PedCreationDistMultiplier',
         'CPopulation::PedMICanBeCreatedAtAttractor',
         'CPopulation::PedMICanBeCreatedAtThisAttractor',
         'CPopulation::PedMICanBeCreatedInInterior',
         'CPopulation::PickGangCar',
         'CPopulation::PickRiotRoadBlockCar',
         'CPopulation::PlaceGangMembers',
         'CPopulation::PlaceMallPedsAsStationaryGroup',
         'CPopulation::RemoveAllRandomPeds',
         'CPopulation::RemovePed',
         'CPopulation::RemovePedsIfThePoolGetsFull',
         'CPopulation::RemoveSpecificDriverModelsForCar',
         'CPopulation::Shutdown',
         'CPopulation::TestRoomForDummyObject',
         'CPopulation::Update',
         'CPopulation::UpdatePedCount'],
 'rejected': [],
 'addr': {'CPopulation::AddExistingPedInCar': '0x00611560',
          'CPopulation::ArePedStatsCompatible': '0x00610F50',
          'CPopulation::CanJeerAtStripper': '0x006117B0',
          'CPopulation::CanSolicitPlayerInCar': '0x00611790',
          'CPopulation::CanSolicitPlayerOnFoot': '0x00611780',
          'CPopulation::ChooseCivilianCoupleOccupations': '0x00613180',
          'CPopulation::ChooseCivilianOccupation': '0x00612F90',
          'CPopulation::ChooseCivilianOccupationForVehicle': '0x00613260',
          'CPopulation::ChooseGangOccupation': '0x00611550',
          'CPopulation::ChoosePolicePedOccupation': '0x00610F40',
          'CPopulation::ConvertAllObjectsToDummyObjects': '0x00616420',
          'CPopulation::ConvertToDummyObject': '0x00614670',
          'CPopulation::ConvertToRealObject': '0x00614580',
          'CPopulation::DealWithZoneChange': '0x006116B0',
          'CPopulation::DoesCarGroupHaveModelId': '0x00406F50',
          'CPopulation::FindCarMultiplierMotorway': '0x00611B60',
          'CPopulation::FindDistanceToNearestPedOfType': '0x006143E0',
          'CPopulation::FindDummyDistForModel': '0x00610ED0',
          'CPopulation::FindNumberOfPedsWeCanPlaceOnBenches': '0x00612240',
          'CPopulation::FindPedDensityMultiplierCullZone': '0x00610F00',
          'CPopulation::FindPedMultiplierMotorway': '0x00611B80',
          'CPopulation::FindPedRaceFromName': '0x005B6D40',
          'CPopulation::FindSpecificDriverModelForCar_ToUse': '0x00611900',
          'CPopulation::GeneratePedsAtStartOfGame': '0x00615C90',
          'CPopulation::Initialise': '0x00610E10',
          'CPopulation::IsCorrectTimeOfDayForEffect': '0x00611B20',
          'CPopulation::IsFemale': '0x00611490',
          'CPopulation::IsMale': '0x00611470',
          'CPopulation::IsSecurityGuard': '0x006114B0',
          'CPopulation::IsSkateable': '0x006114C0',
          'CPopulation::IsSunbather-ModelID': '0x00611760',
          'CPopulation::LoadCarGroups': '0x005BD1A0',
          'CPopulation::LoadPedGroups': '0x005BCFE0',
          'CPopulation::LoadSpecificDriverModelsForCar': '0x006117F0',
          'CPopulation::ManageAllPopulation': '0x006160A0',
          'CPopulation::ManageDummy': '0x00616000',
          'CPopulation::ManagePed': '0x00611FC0',
          'CPopulation::ManagePopulation': '0x00616190',
          'CPopulation::MoveCarsAndPedsOutOfAbandonedZones': '0x006116A0',
          'CPopulation::PedCreationDistMultiplier': '0x006116C0',
          'CPopulation::PedMICanBeCreatedAtAttractor': '0x006110C0',
          'CPopulation::PedMICanBeCreatedAtThisAttractor': '0x006110E0',
          'CPopulation::PedMICanBeCreatedInInterior': '0x00611450',
          'CPopulation::PickGangCar': '0x00614490',
          'CPopulation::PickRiotRoadBlockCar': '0x006144B0',
          'CPopulation::PlaceGangMembers': '0x006117D0',
          'CPopulation::PlaceMallPedsAsStationaryGroup': '0x00613CD0',
          'CPopulation::RemoveAllRandomPeds': '0x006122C0',
          'CPopulation::RemovePed': '0x00610F20',
          'CPopulation::RemovePedsIfThePoolGetsFull': '0x00616300',
          'CPopulation::RemoveSpecificDriverModelsForCar': '0x006119D0',
          'CPopulation::Shutdown': '0x00610EC0',
          'CPopulation::TestRoomForDummyObject': '0x00612320',
          'CPopulation::Update': '0x00616650',
          'CPopulation::UpdatePedCount': '0x00611570'},
 'types': {'CPopulation::AddExistingPedInCar': 'S',
           'CPopulation::ArePedStatsCompatible': 'S',
           'CPopulation::CanJeerAtStripper': 'S',
           'CPopulation::CanSolicitPlayerInCar': 'S',
           'CPopulation::CanSolicitPlayerOnFoot': 'S',
           'CPopulation::ChooseCivilianCoupleOccupations': 'S',
           'CPopulation::ChooseCivilianOccupation': 'S',
           'CPopulation::ChooseCivilianOccupationForVehicle': 'S',
           'CPopulation::ChooseGangOccupation': 'S',
           'CPopulation::ChoosePolicePedOccupation': 'S',
           'CPopulation::ConvertAllObjectsToDummyObjects': 'S',
           'CPopulation::ConvertToDummyObject': 'S',
           'CPopulation::ConvertToRealObject': 'S',
           'CPopulation::DealWithZoneChange': 'S',
           'CPopulation::DoesCarGroupHaveModelId': 'S',
           'CPopulation::FindCarMultiplierMotorway': 'S',
           'CPopulation::FindDistanceToNearestPedOfType': 'S',
           'CPopulation::FindDummyDistForModel': 'S',
           'CPopulation::FindNumberOfPedsWeCanPlaceOnBenches': 'S',
           'CPopulation::FindPedDensityMultiplierCullZone': 'S',
           'CPopulation::FindPedMultiplierMotorway': 'S',
           'CPopulation::FindPedRaceFromName': 'S',
           'CPopulation::FindSpecificDriverModelForCar_ToUse': 'S',
           'CPopulation::GeneratePedsAtStartOfGame': 'S',
           'CPopulation::Initialise': 'S',
           'CPopulation::IsCorrectTimeOfDayForEffect': 'S',
           'CPopulation::IsFemale': 'S',
           'CPopulation::IsMale': 'S',
           'CPopulation::IsSecurityGuard': 'S',
           'CPopulation::IsSkateable': 'S',
           'CPopulation::IsSunbather-ModelID': 'S',
           'CPopulation::LoadCarGroups': 'S',
           'CPopulation::LoadPedGroups': 'S',
           'CPopulation::LoadSpecificDriverModelsForCar': 'S',
           'CPopulation::ManageAllPopulation': 'S',
           'CPopulation::ManageDummy': 'S',
           'CPopulation::ManagePed': 'S',
           'CPopulation::ManagePopulation': 'S',
           'CPopulation::MoveCarsAndPedsOutOfAbandonedZones': 'S',
           'CPopulation::PedCreationDistMultiplier': 'S',
           'CPopulation::PedMICanBeCreatedAtAttractor': 'S',
           'CPopulation::PedMICanBeCreatedAtThisAttractor': 'S',
           'CPopulation::PedMICanBeCreatedInInterior': 'S',
           'CPopulation::PickGangCar': 'S',
           'CPopulation::PickRiotRoadBlockCar': 'S',
           'CPopulation::PlaceGangMembers': 'S',
           'CPopulation::PlaceMallPedsAsStationaryGroup': 'S',
           'CPopulation::RemoveAllRandomPeds': 'S',
           'CPopulation::RemovePed': 'S',
           'CPopulation::RemovePedsIfThePoolGetsFull': 'S',
           'CPopulation::RemoveSpecificDriverModelsForCar': 'S',
           'CPopulation::Shutdown': 'S',
           'CPopulation::TestRoomForDummyObject': 'S',
           'CPopulation::Update': 'S',
           'CPopulation::UpdatePedCount': 'S'},
 'pending_after': ['CPopulation::AddDeadPedInFrontOfCar',
                   'CPopulation::AddPed',
                   'CPopulation::AddPedAtAttractor',
                   'CPopulation::AddPedInCar',
                   'CPopulation::CreateWaitingCoppers',
                   'CPopulation::GeneratePedsAtAttractors',
                   'CPopulation::PlaceCouple',
                   'CPopulation::PopulateInterior',
                   'CPopulation::TestSafeForRealObject'],
 'allow_src_change': [],
 'allow_bank_change': [],
 'extra_unregistered': [],
 'provenance': 'Exact compiled PopFinal3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal '
               'to the working tree at registration; full 13376-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 13376 '
           'isolated CPU pairs for 55 functions of CPopulation, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural '
           '"complete" event with state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case '
           'index) in both legs and compares the return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 '
           'bits) and the whole trace of doubled collaborators with raw arguments; foreign threads ignored. Traced doubles get NO credit. Only the '
           'own part of each function is validated. Source tested == current working tree at registration (class sources match the probe build '
           'manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Padding and uninitialised bytes '
           'masked. Limits are declared per function in the function-specific scope and in the bank README.',
 'specific': {'CPopulation::AddExistingPedInCar': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                                  'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y '
                                                  'sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ArePedStatsCompatible': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras '
                                                    'y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza '
                                                    'y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::CanJeerAtStripper': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                                'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin '
                                                'credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::CanSolicitPlayerInCar': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras '
                                                    'y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza '
                                                    'y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::CanSolicitPlayerOnFoot': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                     'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados '
                                                     'con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ChooseCivilianCoupleOccupations': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed '
                                                              '(see git diff). salidas con -1 (el EXE lee locales sin inicializar en otras ramas). '
                                                              'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                              'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores '
                                                              'doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy '
                                                              '(0x6146F0).',
              'CPopulation::ChooseCivilianOccupation': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see '
                                                       'git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                       'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores '
                                                       'doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ChooseCivilianOccupationForVehicle': 'Corpus 512; source reviewed against the EXE and corrected before the run where '
                                                                 'needed (see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y '
                                                                 'dummies (0x38) de 16 ranuras y model infos falsos intercambiados solo durante la '
                                                                 'ejecucion; colaboradores doblados con traza y sin credito; fixtures sin objetos de '
                                                                 'cristal sin dummy (0x6146F0).',
              'CPopulation::ChooseGangOccupation': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git '
                                                   'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras '
                                                   'y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza '
                                                   'y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ChoosePolicePedOccupation': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                        'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores '
                                                        'doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ConvertAllObjectsToDummyObjects': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                              '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                              '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                              'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin '
                                                              'dummy (0x6146F0).',
              'CPopulation::ConvertToDummyObject': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                   'diff). fixtures sin objetos de cristal sin dummy (el EXE no comprueba el dummy nulo, 0x6146F0). '
                                                   'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model '
                                                   'infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin '
                                                   'credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ConvertToRealObject': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). el dummy nunca es nulo (el EXE no lo comprueba); colaboradores doblados. Limites del '
                                                  'banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos falsos '
                                                  'intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; '
                                                  'fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::DealWithZoneChange': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git '
                                                 'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                                 'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y '
                                                 'sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::DoesCarGroupHaveModelId': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see '
                                                      'git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                      'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados '
                                                      'con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::FindCarMultiplierMotorway': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                        'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores '
                                                        'doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::FindDistanceToNearestPedOfType': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                             '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                             'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin '
                                                             'dummy (0x6146F0).',
              'CPopulation::FindDummyDistForModel': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                    'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras '
                                                    'y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza '
                                                    'y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::FindNumberOfPedsWeCanPlaceOnBenches': 'Corpus 512; source reviewed against the EXE and corrected before the run where '
                                                                  'needed (see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) '
                                                                  'y dummies (0x38) de 16 ranuras y model infos falsos intercambiados solo durante '
                                                                  'la ejecucion; colaboradores doblados con traza y sin credito; fixtures sin '
                                                                  'objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::FindPedDensityMultiplierCullZone': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed '
                                                               '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                               '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                               'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin '
                                                               'dummy (0x6146F0).',
              'CPopulation::FindPedMultiplierMotorway': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                        'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores '
                                                        'doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::FindPedRaceFromName': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                                  'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y '
                                                  'sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::FindSpecificDriverModelForCar_ToUse': 'Corpus 256; source reviewed against the EXE and corrected before the run where '
                                                                  'needed (see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) '
                                                                  'y dummies (0x38) de 16 ranuras y model infos falsos intercambiados solo durante '
                                                                  'la ejecucion; colaboradores doblados con traza y sin credito; fixtures sin '
                                                                  'objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::GeneratePedsAtStartOfGame': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                        'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores '
                                                        'doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::Initialise': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                         'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                         'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures '
                                         'sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::IsCorrectTimeOfDayForEffect': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                          '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                          '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                          'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy '
                                                          '(0x6146F0).',
              'CPopulation::IsFemale': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                       'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                       'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures '
                                       'sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::IsMale': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                     'del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos falsos '
                                     'intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures sin objetos '
                                     'de cristal sin dummy (0x6146F0).',
              'CPopulation::IsSecurityGuard': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                              'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model '
                                              'infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin '
                                              'credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::IsSkateable': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                          'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                          'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures '
                                          'sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::IsSunbather-ModelID': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                                  'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y '
                                                  'sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::LoadCarGroups': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'lee ficheros de datos reales en solo lectura (tablas rellenas de basura antes y comparadas completas). '
                                            'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                            'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; '
                                            'fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::LoadPedGroups': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                            'lee ficheros de datos reales en solo lectura (tablas rellenas de basura antes y comparadas completas). '
                                            'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                            'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; '
                                            'fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::LoadSpecificDriverModelsForCar': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                             '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                             'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin '
                                                             'dummy (0x6146F0).',
              'CPopulation::ManageAllPopulation': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                                  'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y '
                                                  'sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ManageDummy': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                          'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                          'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures '
                                          'sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ManagePed': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                        'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                        'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures '
                                        'sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::ManagePopulation': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                               'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin '
                                               'credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::MoveCarsAndPedsOutOfAbandonedZones': 'Corpus 64; source reviewed against the EXE and corrected before the run where '
                                                                 'needed (see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y '
                                                                 'dummies (0x38) de 16 ranuras y model infos falsos intercambiados solo durante la '
                                                                 'ejecucion; colaboradores doblados con traza y sin credito; fixtures sin objetos de '
                                                                 'cristal sin dummy (0x6146F0).',
              'CPopulation::PedCreationDistMultiplier': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see '
                                                        'git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                        'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores '
                                                        'doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::PedMICanBeCreatedAtAttractor': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                           '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                           '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                           'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin '
                                                           'dummy (0x6146F0).',
              'CPopulation::PedMICanBeCreatedAtThisAttractor': 'Corpus 1024; source reviewed against the EXE and corrected before the run where '
                                                               'needed (see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y '
                                                               'dummies (0x38) de 16 ranuras y model infos falsos intercambiados solo durante la '
                                                               'ejecucion; colaboradores doblados con traza y sin credito; fixtures sin objetos de '
                                                               'cristal sin dummy (0x6146F0).',
              'CPopulation::PedMICanBeCreatedInInterior': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                          '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                          '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                          'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy '
                                                          '(0x6146F0).',
              'CPopulation::PickGangCar': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                          'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                          'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures '
                                          'sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::PickRiotRoadBlockCar': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                   'diff). grupos del fixture solo con IDs de vehiculo validos (el EXE recorre 23 ranuras sin mirar '
                                                   'el recuento). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 '
                                                   'ranuras y model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados '
                                                   'con traza y sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::PlaceGangMembers': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                               'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                               'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin '
                                               'credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::PlaceMallPedsAsStationaryGroup': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed '
                                                             '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                             '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                             'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin '
                                                             'dummy (0x6146F0).',
              'CPopulation::RemoveAllRandomPeds': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                  'diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y '
                                                  'model infos falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y '
                                                  'sin credito; fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::RemovePed': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                        'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                        'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures '
                                        'sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::RemovePedsIfThePoolGetsFull': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed '
                                                          '(see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies '
                                                          '(0x38) de 16 ranuras y model infos falsos intercambiados solo durante la ejecucion; '
                                                          'colaboradores doblados con traza y sin credito; fixtures sin objetos de cristal sin dummy '
                                                          '(0x6146F0).',
              'CPopulation::RemoveSpecificDriverModelsForCar': 'Corpus 128; source reviewed against the EXE and corrected before the run where '
                                                               'needed (see git diff). Limites del banco: pools de peds (0x7C4), objetos (0x19C) y '
                                                               'dummies (0x38) de 16 ranuras y model infos falsos intercambiados solo durante la '
                                                               'ejecucion; colaboradores doblados con traza y sin credito; fixtures sin objetos de '
                                                               'cristal sin dummy (0x6146F0).',
              'CPopulation::Shutdown': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                       'del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos falsos '
                                       'intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures sin '
                                       'objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::TestRoomForDummyObject': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                     'diff). el dummy nunca es nulo (el EXE no lo comprueba en 0x612320). Limites del banco: pools '
                                                     'de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos falsos '
                                                     'intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; '
                                                     'fixtures sin objetos de cristal sin dummy (0x6146F0).',
              'CPopulation::Update': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). Limites '
                                     'del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos falsos '
                                     'intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; fixtures sin objetos '
                                     'de cristal sin dummy (0x6146F0).',
              'CPopulation::UpdatePedCount': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff). '
                                             'Limites del banco: pools de peds (0x7C4), objetos (0x19C) y dummies (0x38) de 16 ranuras y model infos '
                                             'falsos intercambiados solo durante la ejecucion; colaboradores doblados con traza y sin credito; '
                                             'fixtures sin objetos de cristal sin dummy (0x6146F0).'},
 'source_changes': {'source/game_sa/Population.cpp': 'Fuente de CPopulation corregida contra el EXE antes de la corrida (correcciones por funcion en '
                                                     'el README del banco y en el alcance de cada funcion; ver git diff)',
                    'source/game_sa/Population.h': 'Fuente de CPopulation corregida contra el EXE antes de la corrida (correcciones por funcion en '
                                                   'el README del banco y en el alcance de cada funcion; ver git diff)'},
 'notes': 'PopFinal3 registers 55 functions of CPopulation (13376 pairs, 0 differences). Limits are declared per function; see scope and the bank '
          'README.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-202521-590640`: passed=true, 13376 pares, 55 funciones registradas con 0 fallos, evento complete '
                   '(state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_popfinal3.py` (evidencia en `runs/`, '
                   '`validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.'}
BANK=CFG['bank'];RUN_ID=CFG['run_id']
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')/('resultados-'+BANK.lower())/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT=CFG['expect'];TOTAL=sum(EXPECT.values());NEW=CFG['new'];REJECTED=CFG['rejected'];PASSED=CFG['passed']
EXTRA=CFG.get('extra_unregistered',[])
assert TOTAL==CFG['total'] and set(NEW)|set(REJECTED)|set(EXTRA)==set(EXPECT) and not set(NEW)&set(REJECTED) and not set(EXTRA)&set(NEW)
assert summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith(CFG['probe_prefix']) and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl')
if PASSED:
    assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True and summary['pairs']==TOTAL and summary['pairs_per_function']==EXPECT and log_sha==summary['log_sha256']
else:
    assert summary['passed'] is False and summary['error']==CFG['error'] and 'pairs' not in summary
events={};per={};bad={};fsum={};cases=set();complete=None;layout=None;err=None;begin=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='error':err=row
        elif ev=='layout':layout=row
        elif ev=='begin':begin=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
term=complete if PASSED else err
if PASSED:
    assert err is None and complete is not None and complete['pairs']==TOTAL and complete['state_restored'] is True and complete.get('layout_ok',True) is True
else:
    assert complete is None and err is not None and set(err)<={'event','layout_ok','pairs','state_restored'} and err['pairs']==TOTAL and err['state_restored'] is True and err.get('layout_ok') is True
assert layout is not None and layout.get('ok') is True and events.get('layout')==1 and begin is None
if begin is not None and isinstance(begin.get('sites'),list):assert all(x.get('ok') is True for x in begin['sites'])
assert events.get('comparison')==TOTAL and events.get('function_summary')==len(EXPECT) and len(cases)==TOTAL
assert events.get('case_start',TOTAL)==TOTAL and events.get('complete',0)+events.get('error',0)==1
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad==CFG['bad'],bad
for f in NEW:assert f not in bad
PAIRS={f:per[f] for f in NEW}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW+REJECTED:
    cls,fn=name.split('::',1);m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR==CFG['addr'],ADDR
for name,t in CFG['types'].items():assert TYPE[name]==t,(name,TYPE[name])
CLS=sorted({n.split('::',1)[0] for n in NEW})
pend_after=[r['class']+'::'+r['fn_name'] for r in rows if r['class'] in CLS and r['validated']=='0' and (r['class']+'::'+r['fn_name']) not in NEW]
assert sorted(pend_after)==sorted(CFG['pending_after']),pend_after
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
CHANGED_SRC={};CHANGED_BANK={}
for p,h in manifest['sources'].items():
    if sha(ROOT/p)!=h:CHANGED_SRC[p]=dict(tested_sha256=h,current_sha256=sha(ROOT/p))
for n,h in manifest['instrumentation'].items():
    if sha(HERE/n)!=h:CHANGED_BANK[n]=dict(tested_sha256=h,current_sha256=sha(HERE/n))
assert set(CHANGED_SRC)<=set(CFG['allow_src_change']),CHANGED_SRC
assert set(CHANGED_BANK)<=set(CFG['allow_bank_change']),CHANGED_BANK
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
assert sha(HERE/('artifacts/gta_reversed-%s-probe.asi'%BANK.lower()))==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/('gta_reversed-%s-probe.pdb'%BANK.lower());assert sha(pdb)==manifest['binaries']['pdb']
assert not RUN.exists();RUN.mkdir(parents=True)
shutil.copy2(SRC/'summary.json',RUN/'summary.json');shutil.copy2(SRC/'RESULTADO.txt',RUN/'RESULTADO.txt')
raw=(SRC/'comparisons.jsonl').read_bytes();assert hashlib.sha256(raw).hexdigest()==log_sha
with open(RUN/'comparisons.jsonl.gz','wb') as out:
    with gzip.GzipFile(filename='',mode='wb',fileobj=out,compresslevel=9,mtime=0) as z:z.write(raw)
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(1<<20),b''):digest.update(block)
assert digest.hexdigest()==log_sha
assert sha(RUN/'summary.json')==sha(SRC/'summary.json') and sha(RUN/'RESULTADO.txt')==sha(SRC/'RESULTADO.txt')
storage=dict(uncompressed_sha256=log_sha,uncompressed_bytes=len(raw),compressed_path=rel(RUN/'comparisons.jsonl.gz'),compressed_sha256=sha(RUN/'comparisons.jsonl.gz'),compression='gzip',gzip_mtime=0,lossless_roundtrip_sha256=digest.hexdigest(),original_location=str(SRC/'comparisons.jsonl'))
COMMON=CFG['common'];SPECIFIC=CFG['specific'];SOURCE_CHANGES=CFG['source_changes']
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the run; not registered, no credit (another agent is diagnosing it)') for f in REJECTED}
reaudit=dict(comparison_events=TOTAL,complete_events=events.get('complete',0),terminal_error_events=events.get('error',0),terminal_event=term,layout_events=events.get('layout',0),begin_events=events.get('begin',0),begin_checked=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=len(EXPECT),functions_with_zero_failures=len(NEW),unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per)
out=dict(passed=PASSED,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=len(NEW),not_registered_extra=EXTRA,functions=PAIRS,covered_functions={k:True for k in NEW},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=PASSED,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,reaudit=reaudit,source_corrections=SOURCE_CHANGES,notes=CFG['notes'])
if not PASSED:
    out.update(partial_per_function_approval=True,rejected_functions=rejected)
if CHANGED_SRC or CHANGED_BANK:
    out.update(source_changed_after_run=CHANGED_SRC,bank_changed_after_run=CHANGED_BANK)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/('reversing/validation-registry.backup-before-%s.json'%CFG['tag'])
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
assert n==CFG['registry_before'],n
for name in NEW:
    cls,fn=name.split('::',1);hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources=dict(manifest['sources']),
     source_provenance=CFG['provenance'],
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/%s/README.md'%BANK))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.%s.tmp'%CFG['tag']);temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
# README: only append the approval section at the end (append mode, bank file otherwise untouched)
readme=HERE/'README.md';raw_r=readme.read_bytes();nl='\r\n' if b'\r\n' in raw_r else '\n'
section=CFG['readme_section'].replace('\n',nl)
assert '## Aprobación' not in raw_r.decode('utf-8')
with open(readme,'ab') as fh:fh.write((nl+section+nl).encode('utf-8'))
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp),changed_src=list(CHANGED_SRC),changed_bank=list(CHANGED_BANK)),indent=2))
