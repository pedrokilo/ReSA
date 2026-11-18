"""Tabla de colaboradores de ProcessCommands1700To1799 (0x496E00) y emisor de Collabs.inc para el C++ del banco.

Cada colaborador se identifica por su direccion en el EXE. conv: 'T' thiscall (ECX = this, el callee limpia 4*argc) o 'C' cdecl (el llamador limpia); argc = dwords de pila que anota el falso;
ret: 'i' entero en EAX, 'f' float en ST0, 'p' puntero en EAX, 'v' nada; deref = {argumento: dwords que se anotan de lo apuntado}; real = corre el codigo real (EXE con el hook apagado / cuerpo de la DLL).
Los falsos son identicos en la direccion del EXE y en el cuerpo de la DLL (el hook queda apagado durante el banco); los argumentos de pila los fija el llamador (desensamblado 0x496E00..0x498F10).
"""
# (id, nombre, conv, argc, ret, deref, real, efecto)
C = [
    (0x464080, 'CRunningScript::CollectParameters', 'T', 1, 'v', {}, False, 'CP'),
    (0x464250, 'CRunningScript::CollectNextParameterWithoutIncreasingPC', 'T', 0, 'i', {}, False, ''),
    (0x464370, 'CRunningScript::StoreParameters', 'T', 1, 'v', {}, False, 'SP'),
    (0x4859D0, 'CRunningScript::UpdateCompareFlag', 'T', 1, 'v', {}, False, ''),
    (0x464790, 'CRunningScript::GetPointerToScriptVariable', 'T', 1, 'p', {}, False, ''),
    (0x463D50, 'CRunningScript::ReadTextLabelFromScript', 'T', 2, 'v', {}, False, 'TL'),
    (0x465C20, 'CRunningScript::GivePedScriptedTask', 'T', 3, 'v', {}, False, ''),
    (0x4637E0, 'CMissionCleanup::AddEntityToList', 'T', 2, 'v', {}, False, ''),
    (0x4654B0, 'CMissionCleanup::RemoveEntityFromList', 'T', 2, 'v', {}, False, ''),
    (0x483720, 'CTheScripts::GetNewUniqueScriptThingIndex', 'C', 2, 'i', {}, False, ''),
    (0x4839A0, 'CTheScripts::GetActualScriptThingIndex', 'C', 2, 'i', {}, False, ''),
    (0x493000, 'CTheScripts::AddScriptSearchLight', 'C', 9, 'i', {}, False, ''),
    (0x493160, 'CTheScripts::RemoveScriptSearchLight', 'C', 1, 'v', {}, False, ''),
    (0x493360, 'CTheScripts::MoveSearchLightBetweenTwoPoints', 'C', 8, 'v', {}, False, ''),
    (0x493420, 'CTheScripts::MoveSearchLightToEntity', 'C', 3, 'v', {}, False, ''),
    (0x493480, 'CTheScripts::MoveSearchLightToPointAndStop', 'C', 5, 'v', {}, False, ''),
    (0x4934F0, 'CTheScripts::AttachSearchlightToSearchlightObject', 'C', 7, 'v', {}, False, ''),
    (0x4935A0, 'CTheScripts::AddScriptCheckpoint', 'C', 8, 'i', {}, False, ''),
    (0x4936C0, 'CTheScripts::RemoveScriptCheckpoint', 'C', 1, 'v', {}, False, ''),
    (0x493900, 'CSearchLight::IsSpottedEntity', 'C', 2, 'i', {}, False, ''),
    (0x49CFE0, 'CTagManager::SetAlphaInArea', 'C', 2, 'v', {0: 4}, False, ''),
    (0x49D0B0, 'CTagManager::GetPercentageTaggedInArea', 'C', 1, 'i', {0: 4}, False, ''),
    (0x4C86B0, 'CVehicleModelInfo::GetNumRemaps', 'T', 0, 'i', {}, False, ''),
    (0x4C74D0, 'CLinkedUpgradeList::FindOtherUpgrade', 'T', 1, 'i', {}, False, ''),
    (0x50C260, 'CCamera::StartCooperativeCamMode', 'T', 0, 'v', {}, False, ''),
    (0x5397E0, 'CFireManager::GetScriptFireCoords', 'T', 1, 'p', {}, False, ''),
    (0x5397F0, 'CFireManager::GetNumFiresInRange', 'T', 2, 'i', {0: 3}, False, ''),
    (0x569660, 'CWorld::FindGroundZForCoord', 'C', 2, 'f', {}, False, ''),
    (0x56A490, 'CWorld::GetIsLineOfSightClear', 'C', 9, 'i', {0: 3, 1: 3}, False, ''),
    (0x582870, 'CRadar::GetActualBlipArrayIndex', 'C', 1, 'i', {}, False, ''),
    (0x5839A0, 'CRadar::SetEntityBlip', 'C', 4, 'i', {}, False, ''),
    (0x583CC0, 'CRadar::ChangeBlipScale', 'C', 2, 'v', {}, False, ''),
    (0x5E6370, 'CPed::RemoveWeaponWhenEnteringVehicle', 'T', 1, 'v', {}, False, ''),
    (0x5E6530, 'CPed::ReplaceWeaponForScriptedCutscene', 'T', 0, 'v', {}, False, ''),
    (0x5E6550, 'CPed::RemoveWeaponForScriptedCutscene', 'T', 0, 'v', {}, False, ''),
    (0x5E7E60, 'CPed::AttachPedToBike', 'T', 8, 'v', {}, False, ''),
    (0x5F69C0, 'CPedGroupMembership::IsLeader', 'T', 1, 'i', {}, False, ''),
    (0x5F6A10, 'CPedGroupMembership::IsMember', 'T', 1, 'i', {}, False, ''),
    (0x5F7340, 'CPedGroupIntelligence::SetGroupDecisionMakerType', 'T', 1, 'v', {}, False, ''),
    (0x5F7E80, 'CPedGroups::GetPedsGroup', 'C', 1, 'p', {}, False, ''),
    (0x5FB210, 'CPedGroupMembership::RemoveMember', 'T', 1, 'v', {}, False, ''),
    (0x5FC7E0, 'CPedGroup::Process', 'T', 0, 'v', {}, False, ''),
    (0x600880, 'CDecisionMakerTypesFileLoader::GetGrpDMName', 'C', 2, 'v', {}, False, ''),
    (0x607D30, 'CDecisionMakerTypesFileLoader::LoadDecisionMaker', 'C', 3, 'i', {}, False, ''),
    (0x6094A0, 'CPlayerPed::RemovePlayerPed', 'C', 1, 'v', {}, False, ''),
    (0x618970, 'IKChainManager_c::LookAt', 'T', 11, 'v', {5: 3}, False, ''),
    (0x632D10, 'CTaskComplexSequence::AddTask', 'T', 1, 'v', {}, False, ''),
    (0x61A5A0, 'CTask::operator new', 'C', 1, 'p', {}, False, ''),
    (0x6A6010, 'CAutomobile::GetCarRoll', 'T', 0, 'f', {}, False, ''),
    (0x6D0C00, 'CVehicle::SetRemap', 'T', 1, 'v', {}, False, ''),
    (0x6DF930, 'CVehicle::RemoveVehicleUpgrade', 'T', 1, 'v', {}, False, ''),
    (0x6E3290, 'CVehicle::AddVehicleUpgrade', 'T', 1, 'i', {}, False, ''),
    (0x6F5DB0, 'CTrain::DisableRandomTrains', 'C', 1, 'v', {}, False, ''),
    (0x6F5E20, 'CTrain::SetTrainSpeed', 'C', 2, 'v', {}, False, ''),
    (0x6F5E50, 'CTrain::SetTrainCruiseSpeed', 'C', 2, 'v', {}, False, ''),
    (0x6F5E70, 'CTrain::FindCaboose', 'C', 1, 'p', {}, False, ''),
    (0x6F6A20, 'CTrain::RemoveMissionTrains', 'C', 0, 'v', {}, False, ''),
    (0x6F6AA0, 'CTrain::RemoveAllTrains', 'C', 0, 'v', {}, False, ''),
    (0x6F6B60, 'CTrain::ReleaseMissionTrains', 'C', 0, 'v', {}, False, ''),
    (0x6F7550, 'CTrain::CreateMissionTrain', 'C', 10, 'v', {}, False, 'MT'),
    (0x73AF00, 'CWeapon::FireOneInstantHitRound', 'C', 3, 'v', {0: 3, 1: 3}, False, ''),
    (0x459660, 'CVehicleRecording::SetPlaybackSpeed', 'C', 2, 'v', {}, False, ''),
    (0x459D10, 'CVehicleRecording::SkipForwardInRecording', 'C', 2, 'v', {}, False, ''),
    (0x45A4A0, 'CVehicleRecording::SkipToEndAndStopPlaybackRecordedCar', 'C', 1, 'v', {}, False, ''),
    (0x45A980, 'CVehicleRecording::StartPlaybackRecordedCar', 'C', 4, 'v', {}, False, ''),
    (0x463C70, 'CStuckCarCheck::IsCarInStuckCarArray', 'T', 1, 'i', {}, False, ''),
    (0x407820, 'CStreaming::HasVehicleUpgradeLoaded', 'C', 1, 'i', {}, False, ''),
    (0x408C70, 'CStreaming::RequestVehicleUpgrade', 'C', 2, 'v', {}, False, ''),
    (0x409C90, 'CStreaming::SetMissionDoesntRequireModel', 'C', 1, 'v', {}, False, ''),
    (0x41BDD0, 'CVehicle::SetEngineOn', 'T', 1, 'v', {}, False, ''),
    (0x42F870, 'CCarCtrl::JoinCarWithRoadSystemGotoCoors', 'C', 4, 'i', {1: 3}, False, ''),
    (0x44F8C0, 'CPathFind::FindNthNodeClosestToCoors', 'T', 12, 'p', {}, False, 'NN'),
    (0x450320, 'CPathFind::FindNodeOrientationForCarPlacement', 'T', 1, 'f', {}, False, ''),
    (0x4505E0, 'CPathFind::FindNodeCoorsForScript', 'T', 3, 'p', {}, False, 'NC'),
    (0x536BE0, 'CEntity::GetDistanceFromCentreOfMassToBaseOfModel', 'T', 0, 'f', {}, False, ''),
]
# colaboradores REALES (hooks apagados durante el banco): reservas de pools, aritmetica de CVector y los constructores de las tareas
REAL = [
    (0x4048E0, 'CVehiclePool::GetAtRef'), (0x404910, 'CPedPool::GetAtRef'), (0x4082A0, 'CPedPool::GetAt'), (0x424160, 'CVehiclePool::GetRef'), (0x465040, 'CTheScripts::ObjectPoolGetAt'),
    (0x4082C0, 'CVector::Magnitude'), (0x406DA0, 'CVector::SquaredMagnitudeExe'),
    (0x46AC10, 'CTaskComplexSeekEntityStandard::Constructor'), (0x493730, 'CTaskComplexSeekEntityRadiusAngleOffset::Constructor'),
    (0x492E20, 'CTaskComplexDiveFromAttachedEntityAndGetUp::Constructor'), (0x61A8B0, 'CTaskSimpleRunAnim::Constructor'),
    (0x631460, 'CTaskComplexSitDownThenIdleThenStandUp::Constructor'), (0x634440, 'CTaskSimpleTriggerLookAt::Constructor'),
    (0x63CC30, 'CTaskComplexCarDriveMission::Constructor'), (0x63D6F0, 'CTaskSimpleCarSetTempAction::Constructor'),
    (0x6433E0, 'CTaskComplexDrivePointRoute::Constructor'), (0x668790, 'CTaskComplexGoToPointAiming::Constructor'),
    (0x66B910, 'CTaskComplexTurnToFaceEntityOrCoord::Constructor-Entity'), (0x68B990, 'CTaskComplexArrestPed::Constructor'),
]
BY_ID = {c[0]: c for c in C}
# argumentos de 8 o 16 bits (bool, uint8, uint16): el EXE empuja un dword con basura en los bytes altos; se anota solo la parte baja (arg -> mascara)
MASK = {0x4859D0: {0: 0xFF}, 0x5E7E60: {4: 0xFFFF}, 0x6F5DB0: {0: 0xFF}, 0x56A490: {i: 0xFF for i in range(2, 9)}, 0x42F870: {2: 0xFF, 3: 0xFF}, 0x45A980: {2: 0xFF, 3: 0xFF},
        0x41BDD0: {0: 0xFF}, 0x6F7550: {3: 0xFF, 9: 0xFF}, 0x618970: {6: 0xFF, 10: 0xFF}, 0x44F8C0: {4: 0xFF, 6: 0xFF, 7: 0xFF, 9: 0xFF, 10: 0xFF}, 0x49CFE0: {1: 0xFF}, 0x607D30: {2: 0xFF}}
EFFECT = {'': 0, 'CP': 1, 'SP': 2, 'TL': 3, 'MT': 4, 'NC': 5, 'NN': 6}


def emit(path):
    from fixture_common import REGIONS, BIG_REGIONS
    lines = ['// Generado por collabs.py (no editar a mano): tabla de colaboradores, reales y regiones de memoria de Fn238A.']
    lines.append('struct CollabSpec {u32 id; u8 thiscall, argc, ret /*0 i, 1 f, 2 v, 3 p*/, effect; u8 deref[11]; u16 mask[11]; const char* name;};')
    lines.append('inline constexpr CollabSpec kCollabs[] = {')
    for (cid, name, conv, argc, ret, deref, real, eff) in C:
        d = [0] * 11
        for k, v in deref.items():
            d[k] = v
        r = {'i': 0, 'f': 1, 'v': 2, 'p': 3}[ret]
        mk = [0xFFFFFFFF & 0xFFFF] * 11
        mk = [0xFFFF if False else 0 for _ in range(11)]
        for k, v in MASK.get(cid, {}).items():
            mk[k] = v
        lines.append('    {0x%X, %d, %d, %d, %d, {%s}, {%s}, "%s"},' % (cid, 1 if conv == 'T' else 0, argc, r, EFFECT[eff], ','.join(map(str, d)), ','.join(map(str, mk)), name))
    lines.append('};')
    lines.append('struct RealSpec {u32 id; const char* name;};')
    lines.append('inline constexpr RealSpec kReals[] = {')
    for cid, name in REAL:
        lines.append('    {0x%X, "%s"},' % (cid, name))
    lines.append('};')
    lines.append('struct RegionSpec {u32 at; u32 bytes;};')
    lines.append('inline constexpr RegionSpec kRegionTable[] = {')
    for a, n in REGIONS + BIG_REGIONS:
        lines.append('    {0x%X, %d},' % (a, n))
    lines.append('};')
    open(path, 'w', encoding='utf-8', newline='\n').write('\n'.join(lines) + '\n')


if __name__ == '__main__':
    import sys
    from pathlib import Path
    sys.path.insert(0, str(Path(__file__).parent))
    emit(Path(__file__).parent / 'Collabs.inc')
    print('Collabs.inc', len(C), 'falsos', len(REAL), 'reales')
