"""Registra las 24 funciones del lote 152A con la corrida APROBADA de Fn152A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn152a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn152A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn152A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('tUserList::AddPedType', '0x00492CB0', 'tUserList', 'AddPedType', 'source/game_sa/Scripts/Scripted2dEffects.cpp'),
    ('CTaskSimpleInAir::GetTaskType', '0x00678D40', 'CTaskSimpleInAir', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp'),
    ('CTaskSimpleInAir::DestructorBody', '0x00678D50', 'CTaskSimpleInAir', 'DestructorBody', 'source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp'),
    ('TestModelIndices', '0x0053ED30', 'ModelIndices', 'TestModelIndices', 'source/game_sa/ModelIndices.cpp'),
    ('CPad::IsLeftShoulder1Pressed', '0x0053EDC0', 'CPad', 'IsLeftShoulder1Pressed', 'source/game_sa/Pad.cpp'),
    ('CPad::IsRightShoulder1Pressed', '0x0053EE20', 'CPad', 'IsRightShoulder1Pressed', 'source/game_sa/Pad.cpp'),
    ('CPad::IsDPadUpPressed', '0x0053EE60', 'CPad', 'IsDPadUpPressed', 'source/game_sa/Pad.cpp'),
    ('CPad::IsTrianglePressed', '0x0053EF40', 'CPad', 'IsTrianglePressed', 'source/game_sa/Pad.cpp'),
    ('CPad::IsCirclePressed', '0x0053EF60', 'CPad', 'IsCirclePressed', 'source/game_sa/Pad.cpp'),
    ('CEventPotentialWalkIntoObject::GetEventType', '0x004AE640', 'CEventPotentialWalkIntoObject', 'GetEventType', 'source/game_sa/Events/EventPotentialWalkIntoObject.cpp'),
    ('CEventPotentialWalkIntoObject::GetLifeTime', '0x004AE650', 'CEventPotentialWalkIntoObject', 'GetLifeTime', 'source/game_sa/Events/EventPotentialWalkIntoObject.cpp'),
    ('CEventPotentialWalkIntoObject::GetEventPriority', '0x004AE660', 'CEventPotentialWalkIntoObject', 'GetEventPriority', 'source/game_sa/Events/EventPotentialWalkIntoObject.cpp'),
    ('CEventPotentialWalkIntoObject::TakesPriorityOver', '0x004AE6D0', 'CEventPotentialWalkIntoObject', 'TakesPriorityOver', 'source/game_sa/Events/EventPotentialWalkIntoObject.cpp'),
    ('CEventKnockOffBike::operator=', '0x004B4AA0', 'CEventKnockOffBike', 'operator=', 'source/game_sa/Events/EventKnockOffBike.cpp'),
    ('CEventKnockOffBike::CopyConstructor', '0x004B4A50', 'CEventKnockOffBike', 'CopyConstructor', 'source/game_sa/Events/EventKnockOffBike.cpp'),
    ('CTaskSimpleCarDrive::ScalarDeletingDestructor', '0x006426E0', 'CTaskSimpleCarDrive', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp'),
    ('CTaskSimpleCarDrive::StartRollingDoor', '0x00642700', 'CTaskSimpleCarDrive', 'StartRollingDoor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarDrive.cpp'),
    ('CBmx::BlowUpCar', '0x006C0560', 'CBmx', 'BlowUpCar', 'source/game_sa/Entity/Vehicle/Bmx.cpp'),
    ('CBmx::ScalarDeletingDestructor', '0x006C0570', 'CBmx', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Vehicle/Bmx.cpp'),
    ('CBmx::DestructorBody', '0x006BF9D0', 'CBmx', 'DestructorBody', 'source/game_sa/Entity/Vehicle/Bmx.cpp'),
    ('CTaskComplexLeaveCarAndDie::DestructorBody', '0x0062F270', 'CTaskComplexLeaveCarAndDie', 'DestructorBody', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp'),
    ('CTaskComplexLeaveCarAndDie::ScalarDeletingDestructor', '0x0062F250', 'CTaskComplexLeaveCarAndDie', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndDie.cpp'),
    ('CTaskComplexLeaveCarAndWander::ScalarDeletingDestructor', '0x006422A0', 'CTaskComplexLeaveCarAndWander', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCarAndWander.cpp'),
    ('IsExtraSunnyWeather', '0x0062F2D0', 'CTaskComplexSunbathe', 'IsExtraSunnyWeather', 'source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# GetTaskType, GetEventType, GetLifeTime, GetEventPriority, TakesPriorityOver y BlowUpCar (V, RH_ScopedVMTInstall); el resto S
VIRTUAL = {'CTaskSimpleInAir::GetTaskType', 'CEventPotentialWalkIntoObject::GetEventType', 'CEventPotentialWalkIntoObject::GetLifeTime', 'CEventPotentialWalkIntoObject::GetEventPriority',
           'CEventPotentialWalkIntoObject::TakesPriorityOver', 'CBmx::BlowUpCar'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn152A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
