"""Registra las 19 funciones del lote 187A con la corrida APROBADA de Fn187A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn187a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn187A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json, y pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas. Idempotente. NO ejecuta sync_docs ni hooks2md.
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn187A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CEventAttractor::GetEventType', '0x004AF3D0', 'CEventAttractor', 'GetEventType', 'source/game_sa/Events/EventAttractor.cpp'),
    ('CEventAttractor::GetEventPriority', '0x004AF3F0', 'CEventAttractor', 'GetEventPriority', 'source/game_sa/Events/EventAttractor.cpp'),
    ('CEventAttractor::GetLifeTime', '0x004AF3E0', 'CEventAttractor', 'GetLifeTime', 'source/game_sa/Events/EventAttractor.cpp'),
    ('CEventVehicleToSteal::Clone', '0x004B74B0', 'CEventVehicleToSteal', 'Clone', 'source/game_sa/Events/EventVehicleToSteal.cpp'),
    ('CEventVehicleThreat::CloneEditable', '0x004B7520', 'CEventVehicleThreat', 'CloneEditable', 'source/game_sa/Events/EventVehicleThreat.cpp'),
    ('CEventKnockOffBike::Clone', '0x004B7590', 'CEventKnockOffBike', 'Clone', 'source/game_sa/Events/EventKnockOffBike.cpp'),
    ('CEventGunAimedAt::CloneEditable', '0x004B7630', 'CEventGunAimedAt', 'CloneEditable', 'source/game_sa/Events/EventGunAimedAt.cpp'),
    ('CEventInAir::Clone', '0x004B76A0', 'CEventInAir', 'Clone', 'source/game_sa/Events/EventInAir.cpp'),
    ('CPlayerPed::ScalarDeletingDestructor', '0x0060A9A0', 'CPlayerPed', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Ped/PlayerPed.cpp'),
    ('CPlayerPed::Constructor', '0x0060D5B0', 'CPlayerPed', 'Constructor', 'source/game_sa/Entity/Ped/PlayerPed.cpp'),
    ('CPlayerPed::EmptyStub60F8C0', '0x0060F8C0', 'CPlayerPed', 'EmptyStub60F8C0', 'source/game_sa/Entity/Ped/PlayerPed.cpp'),
    ('CTaskSimpleDuckWhileShotsWhizzing::Constructor', '0x00692680', 'CTaskSimpleDuckWhileShotsWhizzing', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp'),
    ('CTaskSimpleDuckWhileShotsWhizzing::GetTaskType', '0x006926E0', 'CTaskSimpleDuckWhileShotsWhizzing', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp'),
    ('CTaskSimpleDuckWhileShotsWhizzing::DestructorBody', '0x006926F0', 'CTaskSimpleDuckWhileShotsWhizzing', 'DestructorBody', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp'),
    ('CTaskSimpleDuckWhileShotsWhizzing::MakeAbortable', '0x00692700', 'CTaskSimpleDuckWhileShotsWhizzing', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp'),
    ('CTaskSimpleDuckWhileShotsWhizzing::Clone', '0x00692D60', 'CTaskSimpleDuckWhileShotsWhizzing', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp'),
    ('CTaskSimpleShakeFist::ScalarDeletingDestructor', '0x00692DD0', 'CTaskSimpleShakeFist', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp'),
    ('CTaskSimpleDuckWhileShotsWhizzing::ScalarDeletingDestructor', '0x00693B80', 'CTaskSimpleDuckWhileShotsWhizzing', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp'),
    ('CTaskSimpleDuckWhileShotsWhizzing::ProcessPed', '0x00694640', 'CTaskSimpleDuckWhileShotsWhizzing', 'ProcessPed', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CEventAttractor::GetEventType', 'CEventAttractor::GetEventPriority', 'CEventAttractor::GetLifeTime', 'CEventVehicleToSteal::Clone', 'CEventVehicleThreat::CloneEditable', 'CEventKnockOffBike::Clone', 'CEventGunAimedAt::CloneEditable', 'CEventInAir::Clone', 'CTaskSimpleDuckWhileShotsWhizzing::GetTaskType', 'CTaskSimpleDuckWhileShotsWhizzing::MakeAbortable', 'CTaskSimpleDuckWhileShotsWhizzing::Clone', 'CTaskSimpleDuckWhileShotsWhizzing::ProcessPed'}


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def mark_validated():
    """pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones de este lote (clase, nombre y direccion, o 0x00000000 si es V); conserva el resto del fichero tal cual"""
    path = ROOT / 'docs/hooks.csv'
    lines = path.read_bytes().decode('utf-8').split('\n')
    wanted = {(c, n, '0x00000000' if i in VIRTUAL else a) for i, a, c, n, _ in FUNCS}
    done = set()
    for k, line in enumerate(lines):
        f = line.split(',')
        if len(f) == 7 and (f[0], f[1], f[2]) in wanted:
            f[5] = '1'
            lines[k] = ','.join(f)
            done.add((f[0], f[1], f[2]))
    if done != wanted:
        raise SystemExit('Filas de docs/hooks.csv sin encontrar: %s' % sorted(wanted - done))
    path.write_bytes('\n'.join(lines).encode('utf-8'))


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn187A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    mark_validated()
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
