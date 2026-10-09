"""Registro historico de CGangWars y CGroupEventHandler (62 funciones) del banco simple GangEvent1 (corrida 20261009-162300-336352). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'GangEvent1',
 'run_id': '20261009-162300-336352',
 'probe_prefix': '1e7495ea',
 'tag': 'gangevent1',
 'registry_before': 7356,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CGroupEventHandler::IsKillTaskAppropriate': 256,
            'CGroupEventHandler::ComputeWalkAlongsideResponse': 256,
            'CGroupEventHandler::ComputeStareResponse': 384,
            'CGroupEventHandler::ComputeResponseVehicleDamage': 384,
            'CGroupEventHandler::ComputeResponseShotFired': 256,
            'CGroupEventHandler::ComputeResponseSexyPed': 384,
            'CGroupEventHandler::ComputeResponseSeenCop': 256,
            'CGroupEventHandler::ComputeResponsePlayerCommand': 256,
            'CGroupEventHandler::ComputeResponsePedThreat': 384,
            'CGroupEventHandler::ComputeResponsePedFriend': 384,
            'CGroupEventHandler::ComputeResponseNewGangMember': 256,
            'CGroupEventHandler::ComputeResponseLeaderExitedCar': 512,
            'CGroupEventHandler::ComputeResponseLeaderEnterExit': 256,
            'CGroupEventHandler::ComputeResponseGunAimedAt': 384,
            'CGroupEventHandler::ComputeResponseGather': 192,
            'CGroupEventHandler::ComputeResponseDraggedOutCar': 384,
            'CGroupEventHandler::ComputeResponseDanger': 192,
            'CGroupEventHandler::ComputeResponseDamage': 384,
            'CGroupEventHandler::ComputeMemberResponses': 384,
            'CGroupEventHandler::ComputeLeanOnVehicleResponse': 192,
            'CGroupEventHandler::ComputeKillThreatsBasicResponse': 256,
            'CGroupEventHandler::ComputeKillPlayerBasicResponse': 256,
            'CGroupEventHandler::ComputeHassleThreatResponse': 384,
            'CGroupEventHandler::ComputeHassleSexyPedResponse': 256,
            'CGroupEventHandler::ComputeHandSignalResponse': 192,
            'CGroupEventHandler::ComputeGreetResponse': 512,
            'CGroupEventHandler::ComputeFleePedResponse': 384,
            'CGroupEventHandler::ComputeDrivebyResponse': 128,
            'CGroupEventHandler::ComputeEventResponseTasks': 768,
            'CGroupEventHandler::ComputeDoDealResponse': 512,
            'CGangWars::InitAtStartOfGame': 64,
            'CGangWars::AttackWaveOvercome': 384,
            'CGangWars::CalculateTimeTillNextAttack': 64,
            'CGangWars::CanPlayerStartAGangWarHere': 384,
            'CGangWars::CheerVictory': 192,
            'CGangWars::ClearSpecificZonesToTriggerGangWar': 64,
            'CGangWars::ClearTheStreets': 256,
            'CGangWars::CreateDefendingGroup': 512,
            'CGangWars::DoStuffWhenPlayerVictorious': 128,
            'CGangWars::DoesPlayerControlThisZone': 192,
            'CGangWars::DontCreateCivilians': 64,
            'CGangWars::EndGangWar': 192,
            'CGangWars::GangWarFightingGoingOn': 64,
            'CGangWars::GangWarGoingOn': 64,
            'CGangWars::Load': 192,
            'CGangWars::MakeEnemyGainInfluenceInZone': 384,
            'CGangWars::MakePlayerGainInfluenceInZone': 384,
            'CGangWars::PedStreamedInForThisGang': 256,
            'CGangWars::PickStreamedInPedForThisGang': 384,
            'CGangWars::PickZoneToAttack': 512,
            'CGangWars::ReleaseCarsInAttackWave': 192,
            'CGangWars::ReleasePedsInAttackWave': 384,
            'CGangWars::Save': 192,
            'CGangWars::SetGangWarsActive': 192,
            'CGangWars::SetSpecificZoneToTriggerGangWar': 128,
            'CGangWars::StartDefensiveGangWar': 256,
            'CGangWars::StartOffensiveGangWar': 512,
            'CGangWars::StrengthenPlayerInfluenceInZone': 256,
            'CGangWars::SwitchGangWarsActive': 64,
            'CGangWars::TellGangMembersTo': 384,
            'CGangWars::TellStreamingWhichGangsAreNeeded': 128,
            'CGangWars::UpdateTerritoryUnderControlPercentage': 384},
 'total': 17792,
 'new': ['CGangWars::AttackWaveOvercome',
         'CGangWars::CalculateTimeTillNextAttack',
         'CGangWars::CanPlayerStartAGangWarHere',
         'CGangWars::CheerVictory',
         'CGangWars::ClearSpecificZonesToTriggerGangWar',
         'CGangWars::ClearTheStreets',
         'CGangWars::CreateDefendingGroup',
         'CGangWars::DoStuffWhenPlayerVictorious',
         'CGangWars::DoesPlayerControlThisZone',
         'CGangWars::DontCreateCivilians',
         'CGangWars::EndGangWar',
         'CGangWars::GangWarFightingGoingOn',
         'CGangWars::GangWarGoingOn',
         'CGangWars::InitAtStartOfGame',
         'CGangWars::Load',
         'CGangWars::MakeEnemyGainInfluenceInZone',
         'CGangWars::MakePlayerGainInfluenceInZone',
         'CGangWars::PedStreamedInForThisGang',
         'CGangWars::PickStreamedInPedForThisGang',
         'CGangWars::PickZoneToAttack',
         'CGangWars::ReleaseCarsInAttackWave',
         'CGangWars::ReleasePedsInAttackWave',
         'CGangWars::Save',
         'CGangWars::SetGangWarsActive',
         'CGangWars::SetSpecificZoneToTriggerGangWar',
         'CGangWars::StartDefensiveGangWar',
         'CGangWars::StartOffensiveGangWar',
         'CGangWars::StrengthenPlayerInfluenceInZone',
         'CGangWars::SwitchGangWarsActive',
         'CGangWars::TellGangMembersTo',
         'CGangWars::TellStreamingWhichGangsAreNeeded',
         'CGangWars::UpdateTerritoryUnderControlPercentage',
         'CGroupEventHandler::ComputeDoDealResponse',
         'CGroupEventHandler::ComputeDrivebyResponse',
         'CGroupEventHandler::ComputeEventResponseTasks',
         'CGroupEventHandler::ComputeFleePedResponse',
         'CGroupEventHandler::ComputeGreetResponse',
         'CGroupEventHandler::ComputeHandSignalResponse',
         'CGroupEventHandler::ComputeHassleSexyPedResponse',
         'CGroupEventHandler::ComputeHassleThreatResponse',
         'CGroupEventHandler::ComputeKillPlayerBasicResponse',
         'CGroupEventHandler::ComputeKillThreatsBasicResponse',
         'CGroupEventHandler::ComputeLeanOnVehicleResponse',
         'CGroupEventHandler::ComputeMemberResponses',
         'CGroupEventHandler::ComputeResponseDamage',
         'CGroupEventHandler::ComputeResponseDanger',
         'CGroupEventHandler::ComputeResponseDraggedOutCar',
         'CGroupEventHandler::ComputeResponseGather',
         'CGroupEventHandler::ComputeResponseGunAimedAt',
         'CGroupEventHandler::ComputeResponseLeaderEnterExit',
         'CGroupEventHandler::ComputeResponseLeaderExitedCar',
         'CGroupEventHandler::ComputeResponseNewGangMember',
         'CGroupEventHandler::ComputeResponsePedFriend',
         'CGroupEventHandler::ComputeResponsePedThreat',
         'CGroupEventHandler::ComputeResponsePlayerCommand',
         'CGroupEventHandler::ComputeResponseSeenCop',
         'CGroupEventHandler::ComputeResponseSexyPed',
         'CGroupEventHandler::ComputeResponseShotFired',
         'CGroupEventHandler::ComputeResponseVehicleDamage',
         'CGroupEventHandler::ComputeStareResponse',
         'CGroupEventHandler::ComputeWalkAlongsideResponse',
         'CGroupEventHandler::IsKillTaskAppropriate'],
 'rejected': [],
 'addr': {'CGangWars::AttackWaveOvercome': '0x00445B30',
          'CGangWars::CalculateTimeTillNextAttack': '0x00443DB0',
          'CGangWars::CanPlayerStartAGangWarHere': '0x00443F80',
          'CGangWars::CheerVictory': '0x00444040',
          'CGangWars::ClearSpecificZonesToTriggerGangWar': '0x00443FF0',
          'CGangWars::ClearTheStreets': '0x004444B0',
          'CGangWars::CreateDefendingGroup': '0x004453D0',
          'CGangWars::DoStuffWhenPlayerVictorious': '0x00446400',
          'CGangWars::DoesPlayerControlThisZone': '0x00443AE0',
          'CGangWars::DontCreateCivilians': '0x004439C0',
          'CGangWars::EndGangWar': '0x004464C0',
          'CGangWars::GangWarFightingGoingOn': '0x00443AC0',
          'CGangWars::GangWarGoingOn': '0x00443AA0',
          'CGangWars::InitAtStartOfGame': '0x00443920',
          'CGangWars::Load': '0x005D3EB0',
          'CGangWars::MakeEnemyGainInfluenceInZone': '0x00445FD0',
          'CGangWars::MakePlayerGainInfluenceInZone': '0x00445E80',
          'CGangWars::PedStreamedInForThisGang': '0x004439D0',
          'CGangWars::PickStreamedInPedForThisGang': '0x00443A20',
          'CGangWars::PickZoneToAttack': '0x00443B00',
          'CGangWars::ReleaseCarsInAttackWave': '0x00445E20',
          'CGangWars::ReleasePedsInAttackWave': '0x00445C30',
          'CGangWars::Save': '0x005D5530',
          'CGangWars::SetGangWarsActive': '0x00446570',
          'CGangWars::SetSpecificZoneToTriggerGangWar': '0x00444010',
          'CGangWars::StartDefensiveGangWar': '0x00444300',
          'CGangWars::StartOffensiveGangWar': '0x00446050',
          'CGangWars::StrengthenPlayerInfluenceInZone': '0x00445F50',
          'CGangWars::SwitchGangWarsActive': '0x004465F0',
          'CGangWars::TellGangMembersTo': '0x00444530',
          'CGangWars::TellStreamingWhichGangsAreNeeded': '0x00443D50',
          'CGangWars::UpdateTerritoryUnderControlPercentage': '0x00443DE0',
          'CGroupEventHandler::ComputeDoDealResponse': '0x005FA290',
          'CGroupEventHandler::ComputeDrivebyResponse': '0x005F7A00',
          'CGroupEventHandler::ComputeEventResponseTasks': '0x005FC200',
          'CGroupEventHandler::ComputeFleePedResponse': '0x005FA130',
          'CGroupEventHandler::ComputeGreetResponse': '0x005FA550',
          'CGroupEventHandler::ComputeHandSignalResponse': '0x005FA820',
          'CGroupEventHandler::ComputeHassleSexyPedResponse': '0x005FA020',
          'CGroupEventHandler::ComputeHassleThreatResponse': '0x005F9D50',
          'CGroupEventHandler::ComputeKillPlayerBasicResponse': '0x005FB670',
          'CGroupEventHandler::ComputeKillThreatsBasicResponse': '0x005FB590',
          'CGroupEventHandler::ComputeLeanOnVehicleResponse': '0x005F9B20',
          'CGroupEventHandler::ComputeMemberResponses': '0x005FAA50',
          'CGroupEventHandler::ComputeResponseDamage': '0x005FBF50',
          'CGroupEventHandler::ComputeResponseDanger': '0x005FB540',
          'CGroupEventHandler::ComputeResponseDraggedOutCar': '0x005FBE70',
          'CGroupEventHandler::ComputeResponseGather': '0x005F99F0',
          'CGroupEventHandler::ComputeResponseGunAimedAt': '0x005FBD10',
          'CGroupEventHandler::ComputeResponseLeaderEnterExit': '0x005F9710',
          'CGroupEventHandler::ComputeResponseLeaderExitedCar': '0x005F90A0',
          'CGroupEventHandler::ComputeResponseNewGangMember': '0x005F9840',
          'CGroupEventHandler::ComputeResponsePedFriend': '0x005FB2D0',
          'CGroupEventHandler::ComputeResponsePedThreat': '0x005FBB90',
          'CGroupEventHandler::ComputeResponsePlayerCommand': '0x005FB470',
          'CGroupEventHandler::ComputeResponseSeenCop': '0x005FBCB0',
          'CGroupEventHandler::ComputeResponseSexyPed': '0x005FB390',
          'CGroupEventHandler::ComputeResponseShotFired': '0x005FBDF0',
          'CGroupEventHandler::ComputeResponseVehicleDamage': '0x005FC070',
          'CGroupEventHandler::ComputeStareResponse': '0x005F9BD0',
          'CGroupEventHandler::ComputeWalkAlongsideResponse': '0x005FA910',
          'CGroupEventHandler::IsKillTaskAppropriate': '0x005F7A60'},
 'types': {'CGangWars::AttackWaveOvercome': 'S',
           'CGangWars::CalculateTimeTillNextAttack': 'S',
           'CGangWars::CanPlayerStartAGangWarHere': 'S',
           'CGangWars::CheerVictory': 'S',
           'CGangWars::ClearSpecificZonesToTriggerGangWar': 'S',
           'CGangWars::ClearTheStreets': 'S',
           'CGangWars::CreateDefendingGroup': 'S',
           'CGangWars::DoStuffWhenPlayerVictorious': 'S',
           'CGangWars::DoesPlayerControlThisZone': 'S',
           'CGangWars::DontCreateCivilians': 'S',
           'CGangWars::EndGangWar': 'S',
           'CGangWars::GangWarFightingGoingOn': 'S',
           'CGangWars::GangWarGoingOn': 'S',
           'CGangWars::InitAtStartOfGame': 'S',
           'CGangWars::Load': 'S',
           'CGangWars::MakeEnemyGainInfluenceInZone': 'S',
           'CGangWars::MakePlayerGainInfluenceInZone': 'S',
           'CGangWars::PedStreamedInForThisGang': 'S',
           'CGangWars::PickStreamedInPedForThisGang': 'S',
           'CGangWars::PickZoneToAttack': 'S',
           'CGangWars::ReleaseCarsInAttackWave': 'S',
           'CGangWars::ReleasePedsInAttackWave': 'S',
           'CGangWars::Save': 'S',
           'CGangWars::SetGangWarsActive': 'S',
           'CGangWars::SetSpecificZoneToTriggerGangWar': 'S',
           'CGangWars::StartDefensiveGangWar': 'S',
           'CGangWars::StartOffensiveGangWar': 'S',
           'CGangWars::StrengthenPlayerInfluenceInZone': 'S',
           'CGangWars::SwitchGangWarsActive': 'S',
           'CGangWars::TellGangMembersTo': 'S',
           'CGangWars::TellStreamingWhichGangsAreNeeded': 'S',
           'CGangWars::UpdateTerritoryUnderControlPercentage': 'S',
           'CGroupEventHandler::ComputeDoDealResponse': 'S',
           'CGroupEventHandler::ComputeDrivebyResponse': 'S',
           'CGroupEventHandler::ComputeEventResponseTasks': 'S',
           'CGroupEventHandler::ComputeFleePedResponse': 'S',
           'CGroupEventHandler::ComputeGreetResponse': 'S',
           'CGroupEventHandler::ComputeHandSignalResponse': 'S',
           'CGroupEventHandler::ComputeHassleSexyPedResponse': 'S',
           'CGroupEventHandler::ComputeHassleThreatResponse': 'S',
           'CGroupEventHandler::ComputeKillPlayerBasicResponse': 'S',
           'CGroupEventHandler::ComputeKillThreatsBasicResponse': 'S',
           'CGroupEventHandler::ComputeLeanOnVehicleResponse': 'S',
           'CGroupEventHandler::ComputeMemberResponses': 'S',
           'CGroupEventHandler::ComputeResponseDamage': 'S',
           'CGroupEventHandler::ComputeResponseDanger': 'S',
           'CGroupEventHandler::ComputeResponseDraggedOutCar': 'S',
           'CGroupEventHandler::ComputeResponseGather': 'S',
           'CGroupEventHandler::ComputeResponseGunAimedAt': 'S',
           'CGroupEventHandler::ComputeResponseLeaderEnterExit': 'S',
           'CGroupEventHandler::ComputeResponseLeaderExitedCar': 'S',
           'CGroupEventHandler::ComputeResponseNewGangMember': 'S',
           'CGroupEventHandler::ComputeResponsePedFriend': 'S',
           'CGroupEventHandler::ComputeResponsePedThreat': 'S',
           'CGroupEventHandler::ComputeResponsePlayerCommand': 'S',
           'CGroupEventHandler::ComputeResponseSeenCop': 'S',
           'CGroupEventHandler::ComputeResponseSexyPed': 'S',
           'CGroupEventHandler::ComputeResponseShotFired': 'S',
           'CGroupEventHandler::ComputeResponseVehicleDamage': 'S',
           'CGroupEventHandler::ComputeStareResponse': 'S',
           'CGroupEventHandler::ComputeWalkAlongsideResponse': 'S',
           'CGroupEventHandler::IsKillTaskAppropriate': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled GangEvent1 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working '
               'tree at registration; full 17792-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 17792 isolated CPU pairs for '
           '62 pending functions of CGangWars, CGroupEventHandler, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural "complete" event '
           'with state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case index) in both legs and compares the '
           'return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 bits) and the whole trace of doubled collaborators with raw '
           'arguments; foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == current working tree at '
           'registration (class sources match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation. Limits: '
           'LeaderEnterExit only with ProcessPed==true; fake peds, intelligences, matrices, vehicles and groups in own buffers with a fake ped/vehicle pool swapped '
           '(0xB74490/0xB74494) only during the call; tasks and events allocated in arenas filled with 0xCD (unwritten padding ignored; SetTask describes a clone of the '
           'task); returned allocations are NOT deleted with delete (the deleting destructor of the EXE, 0x69DB00, frees inline in the real pool 0xB744BC; only the '
           'destructor is called); CGangWars statics (0x96AB28..0x96ABD8), zones, population/streaming and clock 0xB7CB84 saved and restored. Collaborators doubled with '
           'trace (no credit): FindPlayerPed, SetTask, FindTaskByType, rand, LookAt, CEventGroup::Add, ComputeEventResponseTask, radar, stats, messages, population, '
           'save I/O.',
 'specific': {'CGangWars::AttackWaveOvercome': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                               'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::CalculateTimeTillNextAttack': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                        'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::CanPlayerStartAGangWarHere': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                       'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::CheerVictory': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                         'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::ClearSpecificZonesToTriggerGangWar': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                               'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::ClearTheStreets': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                            'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::CreateDefendingGroup': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                 'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::DoStuffWhenPlayerVictorious': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                        'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::DoesPlayerControlThisZone': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                      'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::DontCreateCivilians': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::EndGangWar': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, intelligences, '
                                       'groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::GangWarFightingGoingOn': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                   'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::GangWarGoingOn': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                           'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::InitAtStartOfGame': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                              'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::Load': 'Corpus 192; see Save: CGangWarsSaveStructure::Construct corrected in the source and not masked.',
              'CGangWars::MakeEnemyGainInfluenceInZone': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                         'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::MakePlayerGainInfluenceInZone': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                          'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::PedStreamedInForThisGang': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                     'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::PickStreamedInPedForThisGang': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                         'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::PickZoneToAttack': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                             'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::ReleaseCarsInAttackWave': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                    'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::ReleasePedsInAttackWave': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                    'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::Save': 'Corpus 192; CGangWarsSaveStructure::Construct was corrected in the source (0x5D2671 sets GangWarNavigationZoneIndex=-1 always, its '
                                 'loop depends on pZoneToFightOver) and is no longer masked.',
              'CGangWars::SetGangWarsActive': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                              'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::SetSpecificZoneToTriggerGangWar': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                            'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::StartDefensiveGangWar': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                  'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::StartOffensiveGangWar': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                  'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::StrengthenPlayerInfluenceInZone': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                            'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::SwitchGangWarsActive': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                                 'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::TellGangMembersTo': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake peds, '
                                              'intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::TellStreamingWhichGangsAreNeeded': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                             'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGangWars::UpdateTerritoryUnderControlPercentage': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeDoDealResponse': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                           'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeDrivebyResponse': 'Corpus 128; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                            'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeEventResponseTasks': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                               'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeFleePedResponse': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                            'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeGreetResponse': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                          'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeHandSignalResponse': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                               'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeHassleSexyPedResponse': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeHassleThreatResponse': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                 'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeKillPlayerBasicResponse': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                                    'diff); fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the '
                                                                    'call.',
              'CGroupEventHandler::ComputeKillThreatsBasicResponse': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git '
                                                                     'diff); fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the '
                                                                     'call.',
              'CGroupEventHandler::ComputeLeanOnVehicleResponse': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeMemberResponses': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                            'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseDamage': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                           'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseDanger': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                           'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseDraggedOutCar': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseGather': 'Corpus 192; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                           'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseGunAimedAt': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                               'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseLeaderEnterExit': 'Corpus 256; LIMITED SCOPE: only the ProcessPed==true path is covered (the other path reaches '
                                                                    'CAnimManager::BlendAnimation with a fake ped).',
              'CGroupEventHandler::ComputeResponseLeaderExitedCar': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git '
                                                                    'diff); fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the '
                                                                    'call.',
              'CGroupEventHandler::ComputeResponseNewGangMember': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponsePedFriend': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                              'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponsePedThreat': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                              'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponsePlayerCommand': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseSeenCop': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                            'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseSexyPed': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                            'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseShotFired': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                              'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeResponseVehicleDamage': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeStareResponse': 'Corpus 384; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                          'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::ComputeWalkAlongsideResponse': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); '
                                                                  'fake peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.',
              'CGroupEventHandler::IsKillTaskAppropriate': 'Corpus 256; source reviewed against the EXE and corrected before the run where needed (see git diff); fake '
                                                           'peds, intelligences, groups and vehicles in own buffers, pools swapped only during the call.'},
 'source_changes': {'source/game_sa/Events/GroupEventHandler.cpp': 'CGroupEventHandler responses corrected against the EXE; see git diff',
                    'source/game_sa/GangWars.cpp': 'CGangWars functions corrected against the EXE; see git diff',
                    'source/game_sa/GangWars.h': 'declaration adjustments (see git diff)',
                    'source/game_sa/GangWarsSaveStructure.cpp': 'CGangWarsSaveStructure::Construct corrected (0x5D2671); this file and '
                                                                'source/game_sa/Scripts/TheScripts.h are modified in the working tree but are NOT in the probe build '
                                                                'manifest'},
 'notes': 'GangEvent1 registers the 62 functions with cases (30 of CGroupEventHandler and 32 of CGangWars); both classes are left without pending entries. '
          'ComputeResponseLeaderEnterExit is registered with the ProcessPed==true path only. GangWarsSaveStructure.cpp and Scripts/TheScripts.h have uncommitted changes '
          'that are not in the probe build manifest.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-162300-336352`: passed=true, 17792 pares, 62 funciones con 0 fallos (30 de CGroupEventHandler y 32 de CGangWars), evento complete '
                   '(state_restored), saves y configuraciones intactos. Registrada con `register_gangevent1.py` (evidencia en `runs/`, `validation-summary.json`). Con '
                   'esto CGroupEventHandler y CGangWars quedan COMPLETAS. Límites: `ComputeResponseLeaderEnterExit` solo con ProcessPed==true; peds, grupos y vehículos '
                   'falsos con pools falsos temporales; objetos devueltos sin `delete` (solo destructor); colaboradores doblados sin crédito.'}
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
assert begin is not None and events.get('begin')==1 and begin.get('functions')==len(EXPECT) and layout is None
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
