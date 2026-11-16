"""Registra las 33 funciones del lote 191A con la corrida APROBADA de Fn191A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn191a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn191A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Marca validated=1 (sexto campo) en las filas de docs/hooks.csv de las 33 funciones aprobadas (no ejecuta sync_docs/hooks2md).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn191A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CTaskComplexGotoDoorAndOpen::GetTaskType', '0x0066BB90', 'CTaskComplexGotoDoorAndOpen', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp'),
    ('CTaskComplexGotoDoorAndOpen::MakeAbortable', '0x0066BC80', 'CTaskComplexGotoDoorAndOpen', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp'),
    ('CTaskComplexGotoDoorAndOpen::GoToEntryExitCenterTask', '0x0066C280', 'CTaskComplexGotoDoorAndOpen', 'GoToEntryExitCenterTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp'),
    ('CTaskSimpleSlideToCoord::Constructor-NoAnim', '0x0066C3E0', 'CTaskSimpleSlideToCoord', 'Constructor-NoAnim', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp'),
    ('CTaskSimpleSlideToCoord::Constructor-Anim', '0x0066C450', 'CTaskSimpleSlideToCoord', 'Constructor-Anim', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp'),
    ('CTaskSimpleSlideToCoord::Destructor', '0x0066C4C0', 'CTaskSimpleSlideToCoord', 'Destructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp'),
    ('Rs::RsCameraShowRaster', '0x00619440', 'Rs', 'RsCameraShowRaster', 'source/app/platform/platform.cpp'),
    ('Rs::RsSetPresetView', '0x00619780', 'Rs', 'RsSetPresetView', 'source/app/platform/preset_view.cpp'),
    ('Rs::RsDestroyPresetViews', '0x006198C0', 'Rs', 'RsDestroyPresetViews', 'source/app/platform/preset_view.cpp'),
    ('CEntryExitManager::PoolDestructorBody', '0x0043EEB0', 'CEntryExitManager', 'PoolDestructorBody', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::GetInSlot', '0x0043EF00', 'CEntryExitManager', 'GetInSlot', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::SetEntryExitFlagWithName', '0x0043EF20', 'CEntryExitManager', 'SetEntryExitFlagWithName', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::SetEntryExitFlagWithIndex', '0x0043EF90', 'CEntryExitManager', 'SetEntryExitFlagWithIndex', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::PoolConstructor', '0x0043F720', 'CEntryExitManager', 'PoolConstructor', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::PoolDestructorBodyCopy', '0x0043F780', 'CEntryExitManager', 'PoolDestructorBodyCopy', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::PoolNew', '0x0043F7D0', 'CEntryExitManager', 'PoolNew', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::PoolScalarDeletingDestructor', '0x0043F950', 'CEntryExitManager', 'PoolScalarDeletingDestructor', 'source/game_sa/EntryExitManager.cpp'),
    ('CEntryExitManager::SetEnabledByName', '0x0043F9B0', 'CEntryExitManager', 'SetEnabledByName', 'source/game_sa/EntryExitManager.cpp'),
    ('CPad::IsDPadRightPressed', '0x004410E0', 'CPad', 'IsDPadRightPressed', 'source/game_sa/Pad.cpp'),
    ('CPhysical::SetVelocity', '0x00441130', 'CPhysical', 'SetVelocity', 'source/game_sa/Entity/Physical.cpp'),
    ('CTaskComplexFallAndGetUp::GetTaskType', '0x006786F0', 'CTaskComplexFallAndGetUp', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp'),
    ('CTaskComplexFallAndGetUp::DestructorBody', '0x00678790', 'CTaskComplexFallAndGetUp', 'DestructorBody', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp'),
    ('CTaskComplexFallAndStayDown::Constructor', '0x006789D0', 'CTaskComplexFallAndStayDown', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::GetTaskType', '0x00678A00', 'CTaskComplexFallAndStayDown', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::Constructor2', '0x00678A10', 'CTaskComplexFallAndStayDown', 'Constructor2', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::Destructor', '0x00678A90', 'CTaskComplexFallAndStayDown', 'Destructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::MakeAbortable', '0x00678AA0', 'CTaskComplexFallAndStayDown', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::CreateFirstSubTask', '0x00678B20', 'CTaskComplexFallAndStayDown', 'CreateFirstSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::ControlSubTask', '0x00678BC0', 'CTaskComplexFallAndStayDown', 'ControlSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::CreateSubTask', '0x00678BD0', 'CTaskComplexFallAndStayDown', 'CreateSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::ScalarDeletingDestructor', '0x0067CBC0', 'CTaskComplexFallAndStayDown', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::Clone', '0x0067C270', 'CTaskComplexFallAndStayDown', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
    ('CTaskComplexFallAndStayDown::CreateNextSubTask', '0x0067CBE0', 'CTaskComplexFallAndStayDown', 'CreateNextSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndStayDown.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CTaskComplexGotoDoorAndOpen::GetTaskType', 'CTaskComplexGotoDoorAndOpen::MakeAbortable', 'CTaskComplexFallAndGetUp::GetTaskType', 'CTaskComplexFallAndStayDown::GetTaskType', 'CTaskComplexFallAndStayDown::MakeAbortable', 'CTaskComplexFallAndStayDown::CreateFirstSubTask', 'CTaskComplexFallAndStayDown::ControlSubTask', 'CTaskComplexFallAndStayDown::Clone', 'CTaskComplexFallAndStayDown::CreateNextSubTask'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn191A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    csvp = ROOT / 'docs/hooks.csv'
    raw = csvp.read_bytes().decode('utf-8')
    lines = raw.split(chr(10))
    done = 0
    for ID, ADDR, CLS, FN, SOURCE in FUNCS:
        prefix = '%s,%s,%s,' % (CLS, FN, '0x00000000' if ID in VIRTUAL else ADDR)
        hit = [i for i, l in enumerate(lines) if l.startswith(prefix)]
        assert len(hit) == 1, (prefix, hit)
        f = lines[hit[0]].split(',')
        assert len(f) == 7, f
        if f[5] != '1':
            f[5] = '1'
            lines[hit[0]] = ','.join(f)
        done += 1
    csvp.write_bytes(chr(10).join(lines).encode('utf-8'))
    print('hooks.csv: %d filas con validated=1' % done)
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
