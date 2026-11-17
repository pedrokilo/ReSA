"""Registra las 25 funciones del lote 221A con la corrida APROBADA de Fn221A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn218a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn221A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json, y pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas. Idempotente. NO ejecuta sync_docs ni hooks2md.
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn221A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CPedLookAtAttractor::Constructor', '0x005EE960', 'CPedLookAtAttractor', 'Constructor', 'source/game_sa/Attractors/PedLookAtAttractor.cpp'),
    ('CPedLookAtAttractor::ScalarDeletingDestructor', '0x005EE9D0', 'CPedLookAtAttractor', 'ScalarDeletingDestructor', 'source/game_sa/Attractors/PedLookAtAttractor.cpp'),
    ('CPedLookAtAttractor::Destructor', '0x005EEA20', 'CPedLookAtAttractor', 'Destructor', 'source/game_sa/Attractors/PedLookAtAttractor.cpp'),
    ('CPedScriptedAttractor::Constructor', '0x005EEA30', 'CPedScriptedAttractor', 'Constructor', 'source/game_sa/Attractors/PedScriptedAttractor.cpp'),
    ('CPedScriptedAttractor::ScalarDeletingDestructor', '0x005EEAA0', 'CPedScriptedAttractor', 'ScalarDeletingDestructor', 'source/game_sa/Attractors/PedScriptedAttractor.cpp'),
    ('CPedScriptedAttractor::Destructor', '0x005EEAF0', 'CPedScriptedAttractor', 'Destructor', 'source/game_sa/Attractors/PedScriptedAttractor.cpp'),
    ('CPedParkAttractor::Constructor', '0x005EEB00', 'CPedParkAttractor', 'Constructor', 'source/game_sa/Attractors/PedParkAttractor.cpp'),
    ('CPedParkAttractor::ScalarDeletingDestructor', '0x005EEB70', 'CPedParkAttractor', 'ScalarDeletingDestructor', 'source/game_sa/Attractors/PedParkAttractor.cpp'),
    ('CPedParkAttractor::Destructor', '0x005EEBC0', 'CPedParkAttractor', 'Destructor', 'source/game_sa/Attractors/PedParkAttractor.cpp'),
    ('CPedStepAttractor::Constructor', '0x005EEBD0', 'CPedStepAttractor', 'Constructor', 'source/game_sa/Attractors/PedStepAttractor.cpp'),
    ('CPedStepAttractor::ScalarDeletingDestructor', '0x005EEC40', 'CPedStepAttractor', 'ScalarDeletingDestructor', 'source/game_sa/Attractors/PedStepAttractor.cpp'),
    ('CPedStepAttractor::Destructor', '0x005EEC90', 'CPedStepAttractor', 'Destructor', 'source/game_sa/Attractors/PedStepAttractor.cpp'),
    ('CPedAttractorManager::BroadcastArrivalList', '0x005EF920', 'CPedAttractorManager', 'BroadcastArrivalList', 'source/game_sa/PedAttractorManager.cpp'),
    ('CPedAttractorManager::RegisterPedList', '0x005EF980', 'CPedAttractorManager', 'RegisterPedList', 'source/game_sa/PedAttractorManager.cpp'),
    ('CTaskComplexSeekEntityRadiusAngleOffset::ScalarDeletingDestructor', '0x00493870', 'CTaskComplexSeekEntityRadiusAngleOffset', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    ('CTheScripts::IsEntityWithinAnySearchLight', '0x00493960', 'CTheScripts', 'IsEntityWithinAnySearchLight', 'source/game_sa/Scripts/TheScripts.cpp'),
    ('CTaskComplexSeekEntityRadiusAngleOffset::GetSeekPos', '0x00496DC0', 'CTaskComplexSeekEntityRadiusAngleOffset', 'GetSeekPos', 'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    ('CTaskComplexBeCop::Constructor', '0x004993D0', 'CTaskComplexBeCop', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h'),
    ('ExeProtectionStub::MD5Init', '0x00401000', 'ExeProtectionStub', 'MD5Init', 'source/game_sa/ExeProtectionStub.cpp'),
    ('ExeProtectionStub::MD5Update', '0x0040104A', 'ExeProtectionStub', 'MD5Update', 'source/game_sa/ExeProtectionStub.cpp'),
    ('ExeProtectionStub::MD5Final', '0x0040118B', 'ExeProtectionStub', 'MD5Final', 'source/game_sa/ExeProtectionStub.cpp'),
    ('ExeProtectionStub::ResolveKernelImports', '0x00401FBB', 'ExeProtectionStub', 'ResolveKernelImports', 'source/game_sa/ExeProtectionStub.cpp'),
    ('ExeProtectionStub::ResolveLoaderImports', '0x00402370', 'ExeProtectionStub', 'ResolveLoaderImports', 'source/game_sa/ExeProtectionStub.cpp'),
    ('ExeProtectionStub::InitCallbackTable', '0x004024C0', 'ExeProtectionStub', 'InitCallbackTable', 'source/game_sa/ExeProtectionStub.cpp'),
    ('ExeProtectionStub::CheckOsVersionSuite', '0x004028A2', 'ExeProtectionStub', 'CheckOsVersionSuite', 'source/game_sa/ExeProtectionStub.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = set()


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn221A (F221A) con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    mark_validated()
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
