"""Registra las 28 funciones del lote 204A con la corrida APROBADA de Fn204A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn204a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn204A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas (sin sync_docs ni hooks2md; el resto de docs/ no se toca).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn204A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CTaskComplexAvoidEntity::SetUpIK', '0x0066AB40', 'CTaskComplexAvoidEntity', 'SetUpIK', 'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'),
    ('CTaskComplexAvoidEntity::QuitIK', '0x0066AD10', 'CTaskComplexAvoidEntity', 'QuitIK', 'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'),
    ('CTaskComplexAvoidEntity::MakeAbortable', '0x0066AD40', 'CTaskComplexAvoidEntity', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'),
    ('CTaskComplexAvoidEntity::CreateNextSubTask', '0x0066AD70', 'CTaskComplexAvoidEntity', 'CreateNextSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'),
    ('CTaskComplexAvoidEntity::ControlSubTask', '0x0066ADE0', 'CTaskComplexAvoidEntity', 'ControlSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'),
    ('CTaskComplexAvoidEntity::Clone', '0x0066D0C0', 'CTaskComplexAvoidEntity', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'),
    ('CTaskComplexAvoidEntity::CreateFirstSubTask', '0x00672530', 'CTaskComplexAvoidEntity', 'CreateFirstSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'),
    ('CTaskInteriorSitInChair::ScalarDeletingDestructor', '0x00676D10', 'CTaskInteriorSitInChair', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp'),
    ('CTaskInteriorSitInChair::ProcessPed', '0x00676D30', 'CTaskInteriorSitInChair', 'ProcessPed', 'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp'),
    ('CTaskInteriorLieInBed::ScalarDeletingDestructor', '0x006772C0', 'CTaskInteriorLieInBed', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp'),
    ('CVehicleScanner::DestructorBody', '0x00605FB0', 'CVehicleScanner', 'Destructor', 'source/game_sa/VehicleScanner.cpp'),
    ('CPedScanner::DestructorBody', '0x00606000', 'CPedScanner', 'Destructor', 'source/game_sa/PedScanner.cpp'),
    ('CObjectScanner::ConstructorExe', '0x00606010', 'CObjectScanner', 'Constructor', 'source/game_sa/ObjectScanner.cpp'),
    ('CObjectScanner::DestructorBody', '0x00606050', 'CObjectScanner', 'Destructor', 'source/game_sa/ObjectScanner.cpp'),
    ('CEventScanner::ScanForPedPotentialCollisionEvents', '0x00606580', 'CEventScanner', 'ScanForPedPotentialCollisionEvents', 'source/game_sa/Events/EventScanner.cpp'),
    ('CPedAcquaintanceScanner::CreateAcquaintanceEvent', '0x00606BA0', 'CPedAcquaintanceScanner', 'CreateAcquaintanceEvent', 'source/game_sa/Events/EventScanner.cpp'),
    ('CEventDeath::DestructorBody', '0x004ADE90', 'CEventDeath', 'Destructor', 'source/game_sa/Events/EventDeath.cpp'),
    ('CEventPotentialGetRunOver::AffectsPed', '0x004AE2D0', 'CEventPotentialGetRunOver', 'AffectsPed', 'source/game_sa/Events/EventPotentialGetRunOver.cpp'),
    ('CEventDeadPed::ScalarDeletingDestructor', '0x004B5E00', 'CEventDeadPed', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventDeadPed.cpp'),
    ('CEventGroupEvent::ScalarDeletingDestructor', '0x004B5E50', 'CEventGroupEvent', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventGroupEvent.cpp'),
    ('CEventPotentialGetRunOver::ScalarDeletingDestructor', '0x004B5EA0', 'CEventPotentialGetRunOver', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventPotentialGetRunOver.cpp'),
    ('CEventPotentialWalkIntoVehicle::ScalarDeletingDestructor', '0x004B5EF0', 'CEventPotentialWalkIntoVehicle', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventPotentialWalkIntoVehicle.cpp'),
    ('CEventPotentialWalkIntoObject::ScalarDeletingDestructor', '0x004B5F40', 'CEventPotentialWalkIntoObject', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventPotentialWalkIntoObject.cpp'),
    ('CEventPotentialWalkIntoPed::ScalarDeletingDestructor', '0x004B5F90', 'CEventPotentialWalkIntoPed', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventPotentialWalkIntoPed.cpp'),
    ('CEventVehicleHitAndRun::ScalarDeletingDestructor', '0x004B5FE0', 'CEventVehicleHitAndRun', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventVehicleHitAndRun.cpp'),
    ('CEventPedEnteredMyVehicle::ScalarDeletingDestructor', '0x004B6030', 'CEventPedEnteredMyVehicle', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventPedEnteredMyVehicle.cpp'),
    ('CEventChatPartner::ScalarDeletingDestructor', '0x004B60D0', 'CEventChatPartner', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventChatPartner.cpp'),
    ('CEventSexyPed::ScalarDeletingDestructor', '0x004B6120', 'CEventSexyPed', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventSexyPed.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CTaskComplexAvoidEntity::MakeAbortable', 'CTaskComplexAvoidEntity::CreateNextSubTask', 'CTaskComplexAvoidEntity::ControlSubTask', 'CTaskComplexAvoidEntity::Clone',
           'CTaskComplexAvoidEntity::CreateFirstSubTask', 'CEventPotentialGetRunOver::AffectsPed'}


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()



def mark_hooks_csv():
    """validated=1 (sexto campo) en las filas de docs/hooks.csv de las 28 funciones (clase, nombre y direccion iguales); conserva los finales de linea"""
    path = ROOT / 'docs/hooks.csv'
    raw = path.read_bytes().decode('utf-8')
    wanted = {(cls, fn, '0x00000000' if _id in VIRTUAL else addr) for _id, addr, cls, fn, _src in FUNCS}
    done = set()
    out = []
    for line in raw.split(chr(10)):
        body = line.rstrip(chr(13))
        tail = line[len(body):]
        fields = body.split(',')
        if len(fields) >= 7 and (fields[0], fields[1], fields[2]) in wanted:
            done.add((fields[0], fields[1], fields[2]))
            fields[5] = '1'
            line = ','.join(fields) + tail
        out.append(line)
    if done != wanted:
        raise SystemExit('Filas de hooks.csv no encontradas: %s' % sorted(wanted - done))
    path.write_bytes(chr(10).join(out).encode('utf-8'))


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn204A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    mark_hooks_csv()
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
