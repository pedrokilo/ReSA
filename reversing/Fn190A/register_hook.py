"""Registra las 25 funciones del lote 190A con la corrida APROBADA de Fn190A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn190a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn190A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn190A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CEventAcquaintancePedHate::GetLifeTime', '0x00420F00', 'CEventAcquaintancePedHate', 'GetLifeTime', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventAcquaintancePedHate::GetSourceEntity', '0x00420F10', 'CEventAcquaintancePedHate', 'GetSourceEntity', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventAcquaintancePedHate::CanBeInterruptedBySameEvent', '0x00420F20', 'CEventAcquaintancePedHate', 'CanBeInterruptedBySameEvent', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventAcquaintancePedHate::DestructorBody', '0x00420F30', 'CEventAcquaintancePedHate', 'DestructorBody', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventAcquaintancePedHate::GetEventType', '0x00420F40', 'CEventAcquaintancePedHate', 'GetEventType', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventAcquaintancePedHate::CloneEditable', '0x00420F50', 'CEventAcquaintancePedHate', 'CloneEditable', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventAcquaintancePedHate::ScalarDeletingDestructor', '0x00420FB0', 'CEventAcquaintancePedHate', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventAcquaintancePedHate::GetEventPriority', '0x00420FD0', 'CEventAcquaintancePedHate', 'GetEventPriority', 'source/game_sa/Events/EventAcquaintancePedHate.cpp'),
    ('CEventGunShot::ScalarDeletingDestructor', '0x004B58E0', 'CEventGunShot', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventGunShot.cpp'),
    ('CEventVehicleCollision::ScalarDeletingDestructor', '0x004B5930', 'CEventVehicleCollision', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventVehicleCollision.cpp'),
    ('CEventPedCollisionWithPed::ScalarDeletingDestructor', '0x004B5980', 'CEventPedCollisionWithPed', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventPedCollisionWithPed.cpp'),
    ('CEventObjectCollision::ScalarDeletingDestructor', '0x004B59D0', 'CEventObjectCollision', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventObjectCollision.cpp'),
    ('CEventBuildingCollision::ScalarDeletingDestructor', '0x004B5A20', 'CEventBuildingCollision', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventBuildingCollision.cpp'),
    ('CEventDraggedOutCar::ScalarDeletingDestructor', '0x004B5A70', 'CEventDraggedOutCar', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventDraggedOutCar.cpp'),
    ('CTaskComplexKillAllThreats::Constructor', '0x00622180', 'CTaskComplexKillAllThreats', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillAllThreats::GetTaskType', '0x006221B0', 'CTaskComplexKillAllThreats', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillAllThreats::Destructor', '0x006221C0', 'CTaskComplexKillAllThreats', 'Destructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillAllThreats::ScanForThreat', '0x006221D0', 'CTaskComplexKillAllThreats', 'ScanForThreat', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillAllThreats::ControlSubTask', '0x00622230', 'CTaskComplexKillAllThreats', 'ControlSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillAllThreats::ScalarDeletingDestructor', '0x006293A0', 'CTaskComplexKillAllThreats', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillAllThreats::CreateNextSubTask', '0x006293C0', 'CTaskComplexKillAllThreats', 'CreateNextSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillAllThreats::CreateFirstSubTask', '0x00629480', 'CTaskComplexKillAllThreats', 'CreateFirstSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillAllThreats.cpp'),
    ('CTaskComplexKillPedGroupOnFoot::ScalarDeletingDestructor', '0x00629550', 'CTaskComplexKillPedGroupOnFoot', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedGroupOnFoot.cpp'),
    ('CStencilShadows::FindForOwner', '0x00710BA0', 'CStencilShadows', 'FindForOwner', 'source/game_sa/StencilShadows.cpp'),
    ('CStencilShadowObject::Create', '0x00711280', 'CStencilShadowObject', 'Create', 'source/game_sa/StencilShadowObject.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CEventAcquaintancePedHate::GetLifeTime', 'CEventAcquaintancePedHate::GetSourceEntity', 'CEventAcquaintancePedHate::CanBeInterruptedBySameEvent', 'CEventAcquaintancePedHate::GetEventType', 'CEventAcquaintancePedHate::CloneEditable', 'CEventAcquaintancePedHate::GetEventPriority', 'CTaskComplexKillAllThreats::GetTaskType', 'CTaskComplexKillAllThreats::ControlSubTask', 'CTaskComplexKillAllThreats::CreateNextSubTask', 'CTaskComplexKillAllThreats::CreateFirstSubTask'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn190A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
