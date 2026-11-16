"""Registra las 33 funciones del lote 194A con la corrida APROBADA de Fn194A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn205a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn194A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Ademas pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones registradas (NO ejecuta sync_docs ni hooks2md). Argumentos opcionales tras la carpeta: nombres de funciones EXCLUIDAS (no se registran).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn194A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente): se construye desde las filas de hooks.csv del lote (direccion -> clase, nombre) y la fuente de cada clase
_ROWS = {
    0x683440: ('CTaskComplexWaitForPed', 'CreateNextSubTask', 'Tasks/TaskTypes/TaskComplexWaitForPed.cpp'),
    0x683450: ('CTaskComplexWaitForPed', 'CreateFirstSubTask', 'Tasks/TaskTypes/TaskComplexWaitForPed.cpp'),
    0x6834E0: ('CTaskComplexWaitForPed', 'ControlSubTask', 'Tasks/TaskTypes/TaskComplexWaitForPed.cpp'),
    0x669FB0: ('CTaskComplexWanderGang', 'CanJoinGang', 'Tasks/TaskTypes/TaskComplexWanderGang.cpp'),
    0x669FF0: ('CTaskComplexWander', 'GetDistSqOfClosestPathNodeToPed', 'Tasks/TaskTypes/TaskComplexWander.cpp'),
    0x66F4A0: ('CTaskComplexWander', 'ScalarDeletingDestructor', 'Tasks/TaskTypes/TaskComplexWander.cpp'),
    0x673EF0: ('CTaskComplexWander', 'GetWanderTaskByPedTypeId', 'Tasks/TaskTypes/TaskComplexWander.cpp'),
    0x4C5CB0: ('CModelInfo', 'AtomicModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C5D10: ('CModelInfo', 'DamageAtomicModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C5D70: ('CModelInfo', 'LodAtomicModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C5DD0: ('CModelInfo', 'TimeModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C5E30: ('CModelInfo', 'LodTimeModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C5E90: ('CModelInfo', 'WeaponModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C5EF0: ('CModelInfo', 'ClumpModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C5F50: ('CModelInfo', 'VehicleModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C67D0: ('CModelInfo', 'PedModelInfoStoreConstructor', 'Models/ModelInfo.cpp'),
    0x4C6200: ('CBaseModelInfo', 'ShutdownThunk', 'Models/BaseModelInfo.cpp'),
    0x4C6210: ('CAtomicModelInfo', 'ScalarDeletingDestructor', 'Models/AtomicModelInfo.cpp'),
    0x4C6230: ('CDamageAtomicModelInfo', 'ScalarDeletingDestructor', 'Models/DamageAtomicModelInfo.cpp'),
    0x4C6250: ('CLodAtomicModelInfo', 'ScalarDeletingDestructor', 'Models/LodAtomicModelInfo.cpp'),
    0x4C6270: ('CTimeModelInfo', 'ScalarDeletingDestructor', 'Models/TimeModelInfo.cpp'),
    0x4C6290: ('CLodTimeModelInfo', 'ScalarDeletingDestructor', 'Models/LodTimeModelInfo.cpp'),
    0x4C62B0: ('CClumpModelInfo', 'ScalarDeletingDestructor', 'Models/ClumpModelInfo.cpp'),
    0x4C62D0: ('CWeaponModelInfo', 'ScalarDeletingDestructor', 'Models/WeaponModelInfo.cpp'),
    0x4C6350: ('CPedModelInfo', 'ScalarDeletingDestructor', 'Models/PedModelInfo.cpp'),
    0x4C62F0: ('CPedModelInfo', 'DestructorBody', 'Models/PedModelInfo.cpp'),
    0x4C5790: ('CWeaponModelInfo', 'DestructorBody', 'Models/WeaponModelInfo.cpp'),
    0x4C6B10: ('CModelInfoAccelerator', 'FreeModelInfoIds', 'ModelInfoAccelerator.cpp'),
    0x4C6B70: ('CModelInfoAccelerator', 'GetModelInfoIdFile', 'ModelInfoAccelerator.cpp'),
    0x4C6BD0: ('CModelInfoAccelerator', 'EndOfLoadPhase', 'ModelInfoAccelerator.cpp'),
    0x66CA40: ('CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>', 'Constructor', 'Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    0x66CB90: ('CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>', 'ScalarDeletingDestructor', 'Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    0x66CBB0: ('CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>', 'Destructor', 'Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
}
FUNCS = tuple((_o.NAMES[i], '0x%08X' % a, _ROWS[a][0], _ROWS[a][1], 'source/game_sa/' + _ROWS[a][2]) for i, a in enumerate(_o.ENTRIES))
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# todas S (RH_ScopedInstall / RH_ScopedNamedInstall / RH_ScopedNamedGlobalInstall)
VIRTUAL = set()


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def update_hooks_csv(funcs):
    """validated = 1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas"""
    path = ROOT / 'docs' / 'hooks.csv'
    raw = path.read_bytes().decode('utf-8')
    crlf = '\r\n' in raw
    lines = raw.replace('\r\n', '\n').split('\n')
    done = set()
    for n, line in enumerate(lines):
        f = line.split(',')
        if len(f) == 7:
            for ID, ADDR, CLS, FN, SOURCE in funcs:
                if f[0] == CLS and f[1] == FN and (f[2] == ADDR or f[2] == '0x00000000' and ID in VIRTUAL):
                    f[5] = '1'
                    lines[n] = ','.join(f)
                    done.add(ID)
    missing = [ID for ID, *_ in funcs if ID not in done]
    if missing:
        raise SystemExit('Filas de hooks.csv sin encontrar: %s' % missing)
    text = '\n'.join(lines)
    path.write_bytes((text.replace('\n', '\r\n') if crlf else text).encode('utf-8'))


def main(src, excluded=()):
    global FUNCS
    FUNCS = tuple(f for f in FUNCS if f[0] not in excluded)
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
    assert {f[0] for f in FUNCS} <= set(ppf), ppf
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn194A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    update_hooks_csv(FUNCS)
    print('ok', len(FUNCS), 'funciones registradas')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    main(sys.argv[1], tuple(sys.argv[2:]))
