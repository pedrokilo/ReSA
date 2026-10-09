"""Registro historico de CPickups y CWeapon (60 funciones) del banco simple PickWeap1 (corrida 20261009-162447-757920). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'PickWeap1',
 'run_id': '20261009-162447-757920',
 'probe_prefix': 'a15771ff',
 'tag': 'pickweap1',
 'registry_before': 7418,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CPickups::Init': 64,
            'CPickups::ReInit': 128,
            'CPickups::AddToCollectedPickupsArray': 128,
            'CPickups::GetActualPickupIndex': 256,
            'CPickups::GetNewUniquePickupIndex': 256,
            'CPickups::GetUniquePickupIndex': 128,
            'CPickups::IsPickUpPickedUp': 128,
            'CPickups::PassTime': 128,
            'CPickups::UpdateMoneyPerDay': 128,
            'CPickups::RemovePickUp': 128,
            'CPickups::RemoveMissionPickUps': 64,
            'CPickups::RemovePickUpsInArea': 192,
            'CPickups::RemovePickupObjects': 64,
            'CPickups::RemoveUnnecessaryPickups': 192,
            'CPickups::TestForPickupsInBubble': 192,
            'CPickups::TryToMerge_WeaponType': 256,
            'CPickups::FindPickUpForThisObject': 128,
            'CPickups::DetonateMinesHitByGunShot': 64,
            'CPickups::ModelForWeapon': 94,
            'CPickups::WeaponForModel': 256,
            'CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment': 128,
            'CPickups::ModifyStringLabelForControlSetting': 128,
            'CPickups::Load': 64,
            'CPickups::Save': 64,
            'CPickups::DoMoneyEffects': 128,
            'CPickups::DoCollectableEffects': 256,
            'CPickups::DoMineEffects': 256,
            'CPickups::CreateSomeMoney': 192,
            'CPickups::GenerateNewOne_WeaponType': 128,
            'CPickups::PickedUpOyster': 128,
            'CPickups::PickedUpHorseShoe': 128,
            'CPickups::PictureTaken': 256,
            'CPickups::RenderPickUpText': 256,
            'CPickups::GivePlayerGoodiesWithPickUpMI': 384,
            'CWeapon::IsTypeMelee': 94,
            'CWeapon::IsType2Handed': 94,
            'CWeapon::IsTypeProjectile': 94,
            'CWeapon::HasWeaponAmmoToBeUsed': 128,
            'CWeapon::CanBeUsedFor2Player-Method': 94,
            'CWeapon::CanBeUsedFor2Player-Static': 128,
            'CWeapon::Constructor': 256,
            'CWeapon::Initialise': 256,
            'CWeapon::Reload': 192,
            'CWeapon::Shutdown': 128,
            'CWeapon::StopWeaponEffect': 94,
            'CWeapon::TargetWeaponRangeMultiplier': 256,
            'CWeapon::InitialiseWeapons': 8,
            'CWeapon::ShutdownWeapons': 8,
            'CWeapon::UpdateWeapons': 8,
            'CWeapon::EvaluateTargetForHeatSeekingMissile': 256,
            'CWeapon::PickTargetForHeatSeekingMissile': 256,
            'CWeapon::SetUpPelletCol': 256,
            'CWeapon::AddGunshell': 192,
            'CWeapon::FireFromCar': 256,
            'CWeapon::DoDoomAiming': 256,
            'CWeapon::LaserScopeDot': 192,
            'CWeapon::Fire': 768,
            'CWeapon::FireAreaEffect': 384,
            'CWeapon::FireM16_1stPerson': 384,
            'CWeapon::Update': 512},
 'total': 11020,
 'new': ['CPickups::AddToCollectedPickupsArray',
         'CPickups::CreateSomeMoney',
         'CPickups::DetonateMinesHitByGunShot',
         'CPickups::DoCollectableEffects',
         'CPickups::DoMineEffects',
         'CPickups::DoMoneyEffects',
         'CPickups::FindPickUpForThisObject',
         'CPickups::GenerateNewOne_WeaponType',
         'CPickups::GetActualPickupIndex',
         'CPickups::GetNewUniquePickupIndex',
         'CPickups::GetUniquePickupIndex',
         'CPickups::GivePlayerGoodiesWithPickUpMI',
         'CPickups::Init',
         'CPickups::IsPickUpPickedUp',
         'CPickups::Load',
         'CPickups::ModelForWeapon',
         'CPickups::ModifyStringLabelForControlSetting',
         'CPickups::PassTime',
         'CPickups::PickedUpHorseShoe',
         'CPickups::PickedUpOyster',
         'CPickups::PictureTaken',
         'CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment',
         'CPickups::ReInit',
         'CPickups::RemoveMissionPickUps',
         'CPickups::RemovePickUp',
         'CPickups::RemovePickUpsInArea',
         'CPickups::RemovePickupObjects',
         'CPickups::RemoveUnnecessaryPickups',
         'CPickups::RenderPickUpText',
         'CPickups::Save',
         'CPickups::TestForPickupsInBubble',
         'CPickups::TryToMerge_WeaponType',
         'CPickups::UpdateMoneyPerDay',
         'CPickups::WeaponForModel',
         'CWeapon::AddGunshell',
         'CWeapon::CanBeUsedFor2Player-Method',
         'CWeapon::CanBeUsedFor2Player-Static',
         'CWeapon::Constructor',
         'CWeapon::DoDoomAiming',
         'CWeapon::EvaluateTargetForHeatSeekingMissile',
         'CWeapon::Fire',
         'CWeapon::FireAreaEffect',
         'CWeapon::FireFromCar',
         'CWeapon::FireM16_1stPerson',
         'CWeapon::HasWeaponAmmoToBeUsed',
         'CWeapon::Initialise',
         'CWeapon::InitialiseWeapons',
         'CWeapon::IsType2Handed',
         'CWeapon::IsTypeMelee',
         'CWeapon::IsTypeProjectile',
         'CWeapon::LaserScopeDot',
         'CWeapon::PickTargetForHeatSeekingMissile',
         'CWeapon::Reload',
         'CWeapon::SetUpPelletCol',
         'CWeapon::Shutdown',
         'CWeapon::ShutdownWeapons',
         'CWeapon::StopWeaponEffect',
         'CWeapon::TargetWeaponRangeMultiplier',
         'CWeapon::Update',
         'CWeapon::UpdateWeapons'],
 'rejected': [],
 'addr': {'CPickups::AddToCollectedPickupsArray': '0x00455240',
          'CPickups::CreateSomeMoney': '0x00458970',
          'CPickups::DetonateMinesHitByGunShot': '0x004590C0',
          'CPickups::DoCollectableEffects': '0x00455E20',
          'CPickups::DoMineEffects': '0x004560E0',
          'CPickups::DoMoneyEffects': '0x00454E80',
          'CPickups::FindPickUpForThisObject': '0x004551C0',
          'CPickups::GenerateNewOne_WeaponType': '0x00457380',
          'CPickups::GetActualPickupIndex': '0x004552A0',
          'CPickups::GetNewUniquePickupIndex': '0x00456A30',
          'CPickups::GetUniquePickupIndex': '0x00455280',
          'CPickups::GivePlayerGoodiesWithPickUpMI': '0x004564F0',
          'CPickups::Init': '0x00454A70',
          'CPickups::IsPickUpPickedUp': '0x00454B40',
          'CPickups::Load': '0x005D35A0',
          'CPickups::ModelForWeapon': '0x00454AC0',
          'CPickups::ModifyStringLabelForControlSetting': '0x00454B70',
          'CPickups::PassTime': '0x00455200',
          'CPickups::PickedUpHorseShoe': '0x00455390',
          'CPickups::PickedUpOyster': '0x004552D0',
          'CPickups::PictureTaken': '0x00456A70',
          'CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment': '0x004554C0',
          'CPickups::ReInit': '0x00456E60',
          'CPickups::RemoveMissionPickUps': '0x00456DE0',
          'CPickups::RemovePickUp': '0x004573D0',
          'CPickups::RemovePickUpsInArea': '0x00456D30',
          'CPickups::RemovePickupObjects': '0x00455470',
          'CPickups::RemoveUnnecessaryPickups': '0x004563A0',
          'CPickups::RenderPickUpText': '0x00455000',
          'CPickups::Save': '0x005D3540',
          'CPickups::TestForPickupsInBubble': '0x00456450',
          'CPickups::TryToMerge_WeaponType': '0x004555A0',
          'CPickups::UpdateMoneyPerDay': '0x00455680',
          'CPickups::WeaponForModel': '0x00454AE0',
          'CWeapon::AddGunshell': '0x0073A3E0',
          'CWeapon::CanBeUsedFor2Player-Method': '0x0073DEF0',
          'CWeapon::CanBeUsedFor2Player-Static': '0x0073B240',
          'CWeapon::Constructor': '0x0073B430',
          'CWeapon::DoDoomAiming': '0x0073CDC0',
          'CWeapon::EvaluateTargetForHeatSeekingMissile': '0x0073E560',
          'CWeapon::Fire': '0x00742300',
          'CWeapon::FireAreaEffect': '0x0073E800',
          'CWeapon::FireFromCar': '0x0073FA20',
          'CWeapon::FireM16_1stPerson': '0x00741C00',
          'CWeapon::HasWeaponAmmoToBeUsed': '0x0073B2A0',
          'CWeapon::Initialise': '0x0073B4A0',
          'CWeapon::InitialiseWeapons': '0x0073A300',
          'CWeapon::IsType2Handed': '0x0073B1E0',
          'CWeapon::IsTypeMelee': '0x0073B1C0',
          'CWeapon::IsTypeProjectile': '0x0073B210',
          'CWeapon::LaserScopeDot': '0x0073A8D0',
          'CWeapon::PickTargetForHeatSeekingMissile': '0x0073F910',
          'CWeapon::Reload': '0x0073AEB0',
          'CWeapon::SetUpPelletCol': '0x0073C710',
          'CWeapon::Shutdown': '0x0073A380',
          'CWeapon::ShutdownWeapons': '0x0073A330',
          'CWeapon::StopWeaponEffect': '0x0073B360',
          'CWeapon::TargetWeaponRangeMultiplier': '0x0073B380',
          'CWeapon::Update': '0x0073DB40',
          'CWeapon::UpdateWeapons': '0x0073A360'},
 'types': {'CPickups::AddToCollectedPickupsArray': 'S',
           'CPickups::CreateSomeMoney': 'S',
           'CPickups::DetonateMinesHitByGunShot': 'S',
           'CPickups::DoCollectableEffects': 'S',
           'CPickups::DoMineEffects': 'S',
           'CPickups::DoMoneyEffects': 'S',
           'CPickups::FindPickUpForThisObject': 'S',
           'CPickups::GenerateNewOne_WeaponType': 'S',
           'CPickups::GetActualPickupIndex': 'S',
           'CPickups::GetNewUniquePickupIndex': 'S',
           'CPickups::GetUniquePickupIndex': 'S',
           'CPickups::GivePlayerGoodiesWithPickUpMI': 'S',
           'CPickups::Init': 'S',
           'CPickups::IsPickUpPickedUp': 'S',
           'CPickups::Load': 'S',
           'CPickups::ModelForWeapon': 'S',
           'CPickups::ModifyStringLabelForControlSetting': 'S',
           'CPickups::PassTime': 'S',
           'CPickups::PickedUpHorseShoe': 'S',
           'CPickups::PickedUpOyster': 'S',
           'CPickups::PictureTaken': 'S',
           'CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment': 'S',
           'CPickups::ReInit': 'S',
           'CPickups::RemoveMissionPickUps': 'S',
           'CPickups::RemovePickUp': 'S',
           'CPickups::RemovePickUpsInArea': 'S',
           'CPickups::RemovePickupObjects': 'S',
           'CPickups::RemoveUnnecessaryPickups': 'S',
           'CPickups::RenderPickUpText': 'S',
           'CPickups::Save': 'S',
           'CPickups::TestForPickupsInBubble': 'S',
           'CPickups::TryToMerge_WeaponType': 'S',
           'CPickups::UpdateMoneyPerDay': 'S',
           'CPickups::WeaponForModel': 'S',
           'CWeapon::AddGunshell': 'S',
           'CWeapon::CanBeUsedFor2Player-Method': 'S',
           'CWeapon::CanBeUsedFor2Player-Static': 'S',
           'CWeapon::Constructor': 'S',
           'CWeapon::DoDoomAiming': 'S',
           'CWeapon::EvaluateTargetForHeatSeekingMissile': 'S',
           'CWeapon::Fire': 'S',
           'CWeapon::FireAreaEffect': 'S',
           'CWeapon::FireFromCar': 'S',
           'CWeapon::FireM16_1stPerson': 'S',
           'CWeapon::HasWeaponAmmoToBeUsed': 'S',
           'CWeapon::Initialise': 'S',
           'CWeapon::InitialiseWeapons': 'S',
           'CWeapon::IsType2Handed': 'S',
           'CWeapon::IsTypeMelee': 'S',
           'CWeapon::IsTypeProjectile': 'S',
           'CWeapon::LaserScopeDot': 'S',
           'CWeapon::PickTargetForHeatSeekingMissile': 'S',
           'CWeapon::Reload': 'S',
           'CWeapon::SetUpPelletCol': 'S',
           'CWeapon::Shutdown': 'S',
           'CWeapon::ShutdownWeapons': 'S',
           'CWeapon::StopWeaponEffect': 'S',
           'CWeapon::TargetWeaponRangeMultiplier': 'S',
           'CWeapon::Update': 'S',
           'CWeapon::UpdateWeapons': 'S'},
 'pending_after': ['CWeapon::DoBulletImpact',
                   'CWeapon::DoWeaponEffect',
                   'CWeapon::FireInstantHitFromCar2',
                   'CWeapon::FireOneInstantHitRound',
                   'CWeapon::FireProjectile',
                   'CWeapon::FireSniper',
                   'CWeapon::GenerateDamageEvent',
                   'CWeapon::ProcessLineOfSight',
                   'CWeapon::TakePhotograph',
                   'CPickups::Update'],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled PickWeap1 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree '
               'at registration; full 11020-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 11020 isolated CPU pairs for '
           '60 pending functions of CPickups, CWeapon, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural "complete" event with '
           'state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case index) in both legs and compares the '
           'return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 bits) and the whole trace of doubled collaborators with raw '
           'arguments; foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == current working tree at '
           'registration (class sources match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Limits: '
           'CWeapon::Fire compares al!=0 (the EXE returns a garbage raw bl, 0x742794/0x7429FB, the DLL returns bool); fake pickup pool at 0x9788C0 (copied and '
           'restored), fake weapons/peds/vehicles and temporary swaps of Players[0].m_pPed, camera matrix, vehicle pool, pellet col data, g_fx.m_GunShell and ModelInfo, '
           'always restored; fixtures do not make the EXE read uninitialised locals; padding and uninitialised bytes masked. NOT covered by the bank and NOT registered: '
           'CWeapon::FireSniper, DoBulletImpact, TakePhotograph, FireInstantHitFromCar2, GenerateDamageEvent, FireProjectile, DoWeaponEffect, FireOneInstantHitRound, '
           'ProcessLineOfSight and CPickups::Update (real event group, bullet array, FX systems, real pools and world; see the bank README).',
 'specific': {'CPickups::AddToCollectedPickupsArray': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                      'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                      'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::CreateSomeMoney': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                           '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                           'fsin/fcos/fpatan by __asm.',
              'CPickups::DetonateMinesHitByGunShot': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                     'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                     'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::DoCollectableEffects': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                                '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                                'fsin/fcos/fpatan by __asm.',
              'CPickups::DoMineEffects': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                         '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                         'fsin/fcos/fpatan by __asm.',
              'CPickups::DoMoneyEffects': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                          '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                          'fsin/fcos/fpatan by __asm.',
              'CPickups::FindPickUpForThisObject': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                   'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                   'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::GenerateNewOne_WeaponType': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                     'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                     'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::GetActualPickupIndex': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                                '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                                'fsin/fcos/fpatan by __asm.',
              'CPickups::GetNewUniquePickupIndex': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                   'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                   'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::GetUniquePickupIndex': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                                '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                                'fsin/fcos/fpatan by __asm.',
              'CPickups::GivePlayerGoodiesWithPickUpMI': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                         'pickup pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                         'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::Init': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) copied '
                                'and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::IsPickUpPickedUp': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                            '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                            'fsin/fcos/fpatan by __asm.',
              'CPickups::Load': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) copied '
                                'and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::ModelForWeapon': 'Corpus 94; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                          '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                          'fsin/fcos/fpatan by __asm.',
              'CPickups::ModifyStringLabelForControlSetting': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                              'fake pickup pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the '
                                                              'EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::PassTime': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                    'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::PickedUpHorseShoe': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                             '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                             'fsin/fcos/fpatan by __asm.',
              'CPickups::PickedUpOyster': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                          '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                          'fsin/fcos/fpatan by __asm.',
              'CPickups::PictureTaken': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                        '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                        'fsin/fcos/fpatan by __asm.',
              'CPickups::PlayerCanPickUpThisWeaponTypeAtThisMoment': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git '
                                                                     'diff); fake pickup pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals '
                                                                     'read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::ReInit': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                  'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::RemoveMissionPickUps': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                                '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                                'fsin/fcos/fpatan by __asm.',
              'CPickups::RemovePickUp': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                        '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                        'fsin/fcos/fpatan by __asm.',
              'CPickups::RemovePickUpsInArea': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                               '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                               'fsin/fcos/fpatan by __asm.',
              'CPickups::RemovePickupObjects': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                               '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                               'fsin/fcos/fpatan by __asm.',
              'CPickups::RemoveUnnecessaryPickups': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                    'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                    'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::RenderPickUpText': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                            '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                            'fsin/fcos/fpatan by __asm.',
              'CPickups::Save': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) copied '
                                'and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CPickups::TestForPickupsInBubble': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                  'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                                  'fsin/fcos/fpatan by __asm.',
              'CPickups::TryToMerge_WeaponType': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                                 '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                                 'fsin/fcos/fpatan by __asm.',
              'CPickups::UpdateMoneyPerDay': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                             '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                             'fsin/fcos/fpatan by __asm.',
              'CPickups::WeaponForModel': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                          '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                          'fsin/fcos/fpatan by __asm.',
              'CWeapon::AddGunshell': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                      'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::CanBeUsedFor2Player-Method': 'Corpus 94; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                     'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                     'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::CanBeUsedFor2Player-Static': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                     'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                     'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::Constructor': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                      'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::DoDoomAiming': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                       '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                       'fsin/fcos/fpatan by __asm.',
              'CWeapon::EvaluateTargetForHeatSeekingMissile': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                              'fake pickup pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the '
                                                              'EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::Fire': 'Corpus 768; the EXE returns the raw bl (at the default of the switch, 0x742794 mov bl,[esp+60h], bl is the low byte of the targetPosn '
                               'pointer, exit 0x7429FB mov al,bl); the DLL returns bool, so the bench compares al!=0, not the exact byte.',
              'CWeapon::FireAreaEffect': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                         '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                         'fsin/fcos/fpatan by __asm.',
              'CWeapon::FireFromCar': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                      'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::FireM16_1stPerson': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                            '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                            'fsin/fcos/fpatan by __asm.',
              'CWeapon::HasWeaponAmmoToBeUsed': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                                '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                                'fsin/fcos/fpatan by __asm.',
              'CWeapon::Initialise': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                     'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::InitialiseWeapons': 'Corpus 8; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                            '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                            'fsin/fcos/fpatan by __asm.',
              'CWeapon::IsType2Handed': 'Corpus 94; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                        '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                        'fsin/fcos/fpatan by __asm.',
              'CWeapon::IsTypeMelee': 'Corpus 94; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                      'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::IsTypeProjectile': 'Corpus 94; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                           '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                           'fsin/fcos/fpatan by __asm.',
              'CWeapon::LaserScopeDot': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                        '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                        'fsin/fcos/fpatan by __asm.',
              'CWeapon::PickTargetForHeatSeekingMissile': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                          'pickup pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                          'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::Reload': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                 'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::SetUpPelletCol': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                         '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                         'fsin/fcos/fpatan by __asm.',
              'CWeapon::Shutdown': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                   'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::ShutdownWeapons': 'Corpus 8; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                          '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                          'fsin/fcos/fpatan by __asm.',
              'CWeapon::StopWeaponEffect': 'Corpus 94; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool '
                                           '(0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; '
                                           'fsin/fcos/fpatan by __asm.',
              'CWeapon::TargetWeaponRangeMultiplier': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup '
                                                      'pool (0x9788C0) copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with '
                                                      'std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::Update': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                 'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by __asm.',
              'CWeapon::UpdateWeapons': 'Corpus 8; source reviewed against the EXE and corrected before the run where needed (see git diff); fake pickup pool (0x9788C0) '
                                        'copied and restored, fake weapons/peds/vehicles; float literals read from the EXE with std::bit_cast; fsin/fcos/fpatan by '
                                        '__asm.'},
 'source_changes': {'source/game_sa/Pickups.cpp': 'CPickups functions corrected against the EXE (tPickupReference returned in EAX by the EXE, ExeAbi wrappers); see git '
                                                  'diff',
                    'source/game_sa/Pickups.h': 'declaration adjustments (see git diff)',
                    'source/game_sa/Weapon.cpp': 'CWeapon functions corrected against the EXE (Fire raw return, Add(CEvent*,bool), x87 PC24, __asm for '
                                                 'fsin/fcos/fpatan); see git diff',
                    'source/game_sa/Scripts/TheScripts.h': 'modified in the working tree but NOT in the probe build manifest'},
 'notes': 'PickWeap1 registers the 60 functions with cases (34 of CPickups and 26 of CWeapon). CPickups::Update and CWeapon::FireSniper, DoBulletImpact, TakePhotograph, '
          'FireInstantHitFromCar2, GenerateDamageEvent, FireProjectile, DoWeaponEffect, FireOneInstantHitRound and ProcessLineOfSight stay pending (outside the bank). '
          'CWeapon::Fire is registered comparing al!=0.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-162447-757920`: passed=true, 11020 pares, 60 funciones con 0 fallos (34 de CPickups y 26 de CWeapon), evento complete '
                   '(state_restored, layout_ok), evento layout correcto, saves y configuraciones intactos. Registrada con `register_pickweap1.py` (evidencia en `runs/`, '
                   '`validation-summary.json`). Solo se registran las funciones con casos en la corrida; siguen PENDIENTES CPickups::Update y CWeapon::FireSniper, '
                   'DoBulletImpact, TakePhotograph, FireInstantHitFromCar2, GenerateDamageEvent, FireProjectile, DoWeaponEffect, FireOneInstantHitRound y '
                   'ProcessLineOfSight (estas tres no figuran en el README del banco ni tienen casos). Límites: `CWeapon::Fire` compara `al!=0` (el EXE devuelve un byte '
                   'basura); pool de pickups, armas, peds y vehículos falsos; colaboradores doblados sin crédito.'}
BANK=CFG['bank'];RUN_ID=CFG['run_id']
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')/('resultados-'+BANK.lower())/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT=CFG['expect'];TOTAL=sum(EXPECT.values());NEW=CFG['new'];REJECTED=CFG['rejected'];PASSED=CFG['passed']
assert TOTAL==CFG['total'] and set(NEW)|set(REJECTED)==set(EXPECT) and not set(NEW)&set(REJECTED)
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
out=dict(passed=PASSED,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=len(NEW),functions=PAIRS,covered_functions={k:True for k in NEW},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=PASSED,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,reaudit=reaudit,source_corrections=SOURCE_CHANGES,notes=CFG['notes'])
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
