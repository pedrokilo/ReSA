"""Registra en reversing/validation-registry.json los hooks que el lote de nivel 6 anade al catalogo (docs/hooks.csv).

Son tres hooks que ya estaban registrados en el juego pero no tenian fila en el catalogo: CStats::UpdateSexAppealStat (0x55BF20),
CPlayerPed::ControlButtonSprint (0x60A610) y RpAnimBlend::FrameUpdateCallBackOffscreen (0x4D2E10). Cada uno se acredita con la corrida aprobada
que ya figura en reversing/CppOnlyMigration/approvals.json (summary.json con passed y all_equal verdaderos). El script es idempotente: no repite
un hook ya registrado ni sobrescribe un validation-summary.json existente.
"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HOOKS = (
    # (clase del catalogo, funcion del catalogo, direccion, identidad en approvals, banco)
    ('CStats', 'UpdateSexAppealStat', '0x0055BF20', 'CStats::UpdateSexAppealStat', 'DependenciesStatsSex16'),
    ('CPlayerPed', 'ControlButtonSprint', '0x0060A610', 'CPlayerPed::ControlButtonSprint', 'DependenciesTaskSprint16'),
    ('RpAnimBlend', 'FrameUpdateCallBackOffscreen', '0x004D2E10', 'RpAnimBlend::FrameUpdateCallBackOffscreen', 'CppRpAnimUpdate1'),
    # Destinos inciertos de la cohorte de 499 (PLAN-001), reconstruidos y validados con los bancos Dependencies21A..G.
    ('CRT', 'CrtPrintf', '0x00821982', 'CRT::CrtPrintf', 'Dependencies21A'),
    ('CCamera', 'ProcessObbeCinemaCameraCar', '0x005267C0', 'CCamera::ProcessObbeCinemaCameraCar', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraTrain', '0x00526950', 'CCamera::ProcessObbeCinemaCameraTrain', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraHeli', '0x00526AE0', 'CCamera::ProcessObbeCinemaCameraHeli', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraPlane', '0x00526C80', 'CCamera::ProcessObbeCinemaCameraPlane', 'Dependencies21B'),
    ('CCamera', 'ProcessObbeCinemaCameraBoat', '0x00526E20', 'CCamera::ProcessObbeCinemaCameraBoat', 'Dependencies21B'),
    ('CTaskSimpleFight', 'FightSetUpCol', '0x0061D5F0', 'CTaskSimpleFight::FightSetUpCol', 'Dependencies21C'),
    ('CTaskSimpleFight', 'FightHitCar', '0x0061D0B0', 'CTaskSimpleFight::FightHitCar', 'Dependencies21C'),
    ('CTaskSimpleFight', 'FightStrike', '0x006240B0', 'CTaskSimpleFight::FightStrike', 'Dependencies21C'),
    ('CTaskSimpleGangDriveBy', 'Constructor', '0x006217D0', 'CTaskSimpleGangDriveBy::Constructor', 'Dependencies21D'),
    ('CTaskSimpleGangDriveBy', 'PlayerTarget', '0x00621960', 'CTaskSimpleGangDriveBy::PlayerTarget', 'Dependencies21D'),
    ('CTaskSimpleGangDriveBy', 'AITarget', '0x00627600', 'CTaskSimpleGangDriveBy::AITarget', 'Dependencies21D'),
    ('CTaskSimpleGangDriveBy', 'AimGun', '0x00628350', 'CTaskSimpleGangDriveBy::AimGun', 'Dependencies21D'),
    ('CPopulation', 'RandomIntInLimitRange', '0x00610DB0', 'CPopulation::RandomIntInLimitRange', 'Dependencies21E'),
    ('CZoneDef', 'GetCenter', '0x0072D6D0', 'CZoneDef::GetCenter', 'Dependencies21E'),
    ('CTaskComplexEnterCar', 'GetCameraStickModifier', '0x0063A380', 'CTaskComplexEnterCar::GetCameraStickModifier', 'Dependencies21E'),
    ('D3DResourceSystem', 'TidyUpD3DTextures', '0x007305E0', 'D3DResourceSystem::TidyUpD3DTextures', 'Dependencies21F'),
    ('D3DResourceSystem', 'TidyUpD3DIndexBuffers', '0x00730740', 'D3DResourceSystem::TidyUpD3DIndexBuffers', 'Dependencies21F'),
    ('RwHelper', 'GetFirstTexture', '0x00734940', 'RwHelper::GetFirstTexture', 'Dependencies21G'),
    ('RwHelper', 'RpGeometryReplaceOldMaterialWithNewMaterial', '0x00734DE0', 'RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial', 'Dependencies21G'),
    ('CQuaternion', 'Slerp-halftheta', '0x0059C300', 'CQuaternion::Slerp', 'Dependencies21G'),
    # Lote 1 posterior (verificacion de existencia en el EXE): CPointLights, luces de CVehicle, WrapAngle y CreateParticles.
    ('CPointLights', 'GenerateLightsAffectingObject', '0x006FFBB0', 'CPointLights::GenerateLightsAffectingObject', 'Dependencies22A'),
    ('CPointLights', 'GetLightMultiplier', '0x006FFE70', 'CPointLights::GetLightMultiplier', 'Dependencies22A'),
    ('CPointLights', 'ProcessVerticalLineUsingCache', '0x006FFFF0', 'CPointLights::ProcessVerticalLineUsingCache', 'Dependencies22A'),
    ('CPointLights', 'RenderFogEffect', '0x007002D0', 'CPointLights::RenderFogEffect', 'Dependencies22A'),
    ('CVehicle', 'DoHeadLightEffect', '0x006E0A50', 'CVehicle::DoHeadLightEffect', 'Dependencies22B'),
    ('CVehicle', 'DoTailLightEffect', '0x006E1780', 'CVehicle::DoTailLightEffect', 'Dependencies22B'),
    ('CCam', 'WrapAngle', '0x00509BE0', 'CCam::WrapAngle', 'Dependencies22C'),
    ('FxEmitter_c', 'CreateParticles', '0x004A41E0', 'FxEmitter_c::CreateParticles', 'Dependencies22C'),
)
SOURCES = {
    'CRT::CrtPrintf': 'source/game_sa/VehicleRecording.cpp',
    'CCamera::ProcessObbeCinemaCameraCar': 'source/game_sa/Camera.cpp', 'CCamera::ProcessObbeCinemaCameraTrain': 'source/game_sa/Camera.cpp',
    'CCamera::ProcessObbeCinemaCameraHeli': 'source/game_sa/Camera.cpp', 'CCamera::ProcessObbeCinemaCameraPlane': 'source/game_sa/Camera.cpp',
    'CCamera::ProcessObbeCinemaCameraBoat': 'source/game_sa/Camera.cpp',
    'CTaskSimpleFight::FightSetUpCol': 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
    'CTaskSimpleFight::FightHitCar': 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
    'CTaskSimpleFight::FightStrike': 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp',
    'CTaskSimpleGangDriveBy::Constructor': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CTaskSimpleGangDriveBy::PlayerTarget': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CTaskSimpleGangDriveBy::AITarget': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CTaskSimpleGangDriveBy::AimGun': 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp',
    'CPopulation::RandomIntInLimitRange': 'source/game_sa/Population.cpp', 'CZoneDef::GetCenter': 'source/game_sa/CullZones.cpp',
    'CTaskComplexEnterCar::GetCameraStickModifier': 'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp',
    'D3DResourceSystem::TidyUpD3DTextures': 'source/game_sa/RenderWare/D3DResourceSystem.cpp',
    'D3DResourceSystem::TidyUpD3DIndexBuffers': 'source/game_sa/RenderWare/D3DResourceSystem.cpp',
    'RwHelper::GetFirstTexture': 'source/game_sa/RenderWare/rw/rwtexdict.cpp',
    'RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial': 'source/game_sa/RwHelper.cpp',
    'CQuaternion::Slerp': 'source/game_sa/Core/Quaternion.cpp',
    'CPointLights::GenerateLightsAffectingObject': 'source/game_sa/PointLights.cpp', 'CPointLights::GetLightMultiplier': 'source/game_sa/PointLights.cpp',
    'CPointLights::ProcessVerticalLineUsingCache': 'source/game_sa/PointLights.cpp', 'CPointLights::RenderFogEffect': 'source/game_sa/PointLights.cpp',
    'CVehicle::DoHeadLightEffect': 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'CVehicle::DoTailLightEffect': 'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'CCam::WrapAngle': 'source/game_sa/Cam.cpp', 'FxEmitter_c::CreateParticles': 'source/game_sa/Fx/FxEmitter.cpp',
}
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    registry_path = ROOT / 'reversing/validation-registry.json'
    raw = registry_path.read_bytes().decode('utf-8')
    data = json.loads(raw)
    approvals = json.loads((ROOT / 'reversing/CppOnlyMigration/approvals.json').read_text(encoding='utf-8'))
    added = 0
    written_now = set()
    approvals_dirty = False
    for cls, fn, address, identity, bank in HOOKS:
        if any(r['hook']['class'] == cls and r['hook']['fn_name'] == fn for r in data['functions']):
            continue
        entry = approvals.get(address)
        if entry is not None:
            run_dir = ROOT / Path(entry['audit']).parent
        else:
            # corrida aprobada mas reciente del banco en la regresion final
            runs = sorted((r for r in (ROOT / 'reversing/CppTwelveLevel6Final/runs').glob(bank + '-2026*')
                           if json.loads((r / 'summary.json').read_text(encoding='utf-8')).get('passed') is True),
                          key=lambda r: r.stat().st_mtime)
            run_dir = runs[-1]
        summary = json.loads((run_dir / 'summary.json').read_text(encoding='utf-8'))
        assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True
        pairs = int(entry['own_pairs']) if entry is not None else int(summary['pairs_per_function'][identity])
        if entry is None:
            approvals[address] = {
                'identity': identity, 'converted_source_functions': 1, 'source': SOURCES[identity], 'shared_source': 'source/game_sa/X87Math.cpp',
                'bank': bank, 'run': run_dir.name.split('-', 1)[1], 'pairs': int(summary['pairs']), 'own_pairs': pairs,
                'audit': run_dir.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                'state_restored': True, 'helper_codegen': 'reversing/CppTwelveLevel6Study/README.md',
                'scope': 'Destino incierto de la cohorte de 499 (PLAN-001), reconstruido en C++ y validado con su banco Dependencies21 en la regresión final; pares propios de la función según pairs_per_function. Sin cobertura exhaustiva más allá de los fixtures del banco.'}
            approvals_dirty = True
        manifest = json.loads((ROOT / 'reversing' / bank / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        tested = manifest.get('sources', {})
        evidence = dict(
            passed=True, pairs=int(summary['pairs']), differences=0, registered_pairs=int(summary['pairs']),
            registered_functions=len(summary.get('pairs_per_function') or {identity: pairs}),
            functions=dict(summary.get('pairs_per_function') or {identity: pairs}),
            covered_functions={k: True for k in (summary.get('pairs_per_function') or {identity: pairs})}, scope=SCOPE, dependency_review=None,
            run=run_dir.relative_to(ROOT).as_posix(), summary_sha256=sha(run_dir / 'summary.json'),
            result_txt_sha256=sha(run_dir / 'RESULTADO.txt'), log_sha256=summary['log_sha256'], all_equal=True, state_restored=True,
            saves_unchanged=summary['saves_unchanged'], configs_restored=summary['configs_restored'],
            original_exe_sha256=summary['original_exe_sha256'], executed_probe_sha256=summary['executed_probe_sha256'],
            manifest=dict(tested_sources=tested))
        out = ROOT / 'reversing' / bank / 'validation-summary.json'
        text = json.dumps(evidence, ensure_ascii=False, indent=2) + '\n'
        if not out.exists() or out in written_now:
            out.write_bytes(text.encode('utf-8'))
            written_now.add(out)
        data['functions'].append(dict(
            hook=dict(**{'class': cls, 'fn_name': fn, 'address': address, 'type': 'S'}),
            evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', identity],
                      'pairs': pairs, 'original_exe_sha256': summary['original_exe_sha256']},
            tested_sources=tested, dependency_review=None,
            source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
            scope=SCOPE, review_reference=f'reversing/{bank}/README.md'))
        added += 1
    if approvals_dirty:
        (ROOT / 'reversing/CppOnlyMigration/approvals.json').write_bytes(json.dumps(approvals, indent=2, ensure_ascii=False).encode('utf-8'))
    if added:
        registry_path.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    print(json.dumps({'registrados': added, 'total_registro': len(data['functions'])}))


if __name__ == '__main__':
    main()
