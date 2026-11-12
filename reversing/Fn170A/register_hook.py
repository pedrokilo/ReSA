"""Registra las 22 funciones del lote 170A con la corrida APROBADA de Fn170A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn170a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn170A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn170A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CTaskComplexDriveFireTruck::GetTaskType', '0x00659390', 'CTaskComplexDriveFireTruck', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp'),
    ('CTaskComplexDriveFireTruck::DestructorBody', '0x006593A0', 'CTaskComplexDriveFireTruck', 'DestructorBody', 'source/game_sa/Tasks/TaskTypes/TaskComplexDriveFireTruck.cpp'),
    ('CTaskComplexUseSwatRope::ConstructorNoHeli', '0x00659410', 'CTaskComplexUseSwatRope', 'ConstructorNoHeli', 'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp'),
    ('CTaskComplexUseSwatRope::GetTaskType', '0x00659440', 'CTaskComplexUseSwatRope', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp'),
    ('CTaskComplexUseSwatRope::IsInterruptable', '0x00659450', 'CTaskComplexUseSwatRope', 'IsInterruptable', 'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp'),
    ('CTaskComplexUseSwatRope::AbortUrgently', '0x00659460', 'CTaskComplexUseSwatRope', 'AbortUrgently', 'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp'),
    ('CTaskComplexUseSwatRope::ScalarDeletingDestructor', '0x0065A3C0', 'CTaskComplexUseSwatRope', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexUseSwatRope.cpp'),
    ('CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask', '0x00631990', 'CTaskComplexObserveTrafficLightsAndAchieveHeading', 'CreateSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexObserveTrafficLightsAndAchieveHeading.cpp'),
    ('CTaskSimpleUseGun::Constructor', '0x0061DE60', 'CTaskSimpleUseGun', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp'),
    ('CTaskSimpleUseGun::ScalarDeletingDestructor', '0x00624DA0', 'CTaskSimpleUseGun', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp'),
    ('CTaskSimpleFight::GetTaskType', '0x0061C520', 'CTaskSimpleFight', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp'),
    ('CWeaponInfo::GetBaseComboByName', '0x0061DB30', 'CWeaponInfo', 'GetBaseComboByName', 'source/game_sa/WeaponInfo.cpp'),
    ('CTaskComplexWanderProstitute::ScalarDeletingDestructor', '0x006726D0', 'CTaskComplexWanderProstitute', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp'),
    ('CTaskComplexWanderProstitute::DestructorBody', '0x006726F0', 'CTaskComplexWanderProstitute', 'DestructorCopy', 'source/game_sa/Tasks/TaskTypes/TaskComplexWanderProstitute.cpp'),
    ('CTaskComplexPlayHandSignalAnim::ScalarDeletingDestructor', '0x0061C0D0', 'CTaskComplexPlayHandSignalAnim', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp'),
    ('CWeaponInfo::GetAnimLoopFire', '0x0061C150', 'CWeaponInfo', 'GetAnimLoopFire', 'source/game_sa/WeaponInfo.cpp'),
    ('CPointRoute::Reverse', '0x00667720', 'CPointRoute', 'Reverse', 'source/game_sa/PedGeometryAnalyser.cpp'),
    ('CNodeRoute::Reverse', '0x00667900', 'CNodeRoute', 'Reverse', 'source/game_sa/NodeRoute.cpp'),
    ('CPatrolRoute::Destructor', '0x006677A0', 'CPatrolRoute', 'Destructor', 'source/game_sa/PatrolRoute.cpp'),
    ('CEntitySeekPosCalculator::DestructorCopy-2', '0x00667980', 'CEntitySeekPosCalculator', 'DestructorCopy-2', 'source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp'),
    ('CTaskSimpleGangDriveBy::DestructorBody', '0x006218C0', 'CTaskSimpleGangDriveBy', 'DestructorBody', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp'),
    ('CTaskComplexKillAllThreats::Clone', '0x00623750', 'CTaskComplexKillAllThreats', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# GetTaskType (V, RH_ScopedVMTInstall); el resto S
VIRTUAL = {'CTaskComplexDriveFireTruck::GetTaskType', 'CTaskComplexUseSwatRope::GetTaskType', 'CTaskComplexUseSwatRope::IsInterruptable', 'CTaskComplexUseSwatRope::AbortUrgently', 'CTaskSimpleFight::GetTaskType'}


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main(src):
    SRC = Path(src)
    RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + SRC.name)
    RUN.mkdir(exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (RUN / n).exists():
            shutil.copyfile(SRC / n, RUN / n)
    if not (RUN / 'comparisons.jsonl.gz').exists() and not (RUN / 'comparisons.jsonl.xz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
        # GitHub rechaza ficheros de mas de 100 MB: por encima de ~90 MB se guarda como .xz
        if (RUN / 'comparisons.jsonl.gz').stat().st_size > 90 * 1000 * 1000:
            import lzma
            with open(SRC / 'comparisons.jsonl', 'rb') as f, lzma.open(RUN / 'comparisons.jsonl.xz', 'wb', preset=6) as g:
                shutil.copyfileobj(f, g)
            (RUN / 'comparisons.jsonl.gz').unlink()
    s = json.loads((RUN / 'summary.json').read_text(encoding='utf-8'))
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True
    ppf = dict(s['pairs_per_function'])
    assert set(ppf) == {f[0] for f in FUNCS}, ppf
    tested = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))['sources']
    scope = SCOPE + (' ' + SCOPE_NOTE if SCOPE_NOTE else '')
    ev = dict(passed=True, pairs=int(s['pairs']), differences=0, registered_pairs=int(s['pairs']), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=scope, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    data = registry_store.load_registry(ROOT)
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN, SOURCE in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if ID in VIRTUAL else ADDR, 'type': 'V' if ID in VIRTUAL else 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], **({'original_address': ADDR} if ID in VIRTUAL else {})},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn170A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
