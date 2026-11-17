"""Registra las 17 funciones del lote 210A con la corrida APROBADA de Fn210A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn210a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn210A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Ademas pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones registradas (NO ejecuta sync_docs ni hooks2md). Argumentos opcionales tras la carpeta: nombres de funciones EXCLUIDAS (no se registran).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn210A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CTaskComplexFollowPatrolRoute::GetTaskType', '0x00670A60', 'CTaskComplexFollowPatrolRoute', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp'),
    ('CTaskComplexFollowPatrolRoute::GetNextSubTaskType', '0x00670A70', 'CTaskComplexFollowPatrolRoute', 'GetNextSubTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp'),
    ('CTaskComplexFollowPatrolRoute::CreateSubTask', '0x00670B00', 'CTaskComplexFollowPatrolRoute', 'CreateSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp'),
    ('CTaskComplexFollowPatrolRoute::CreateNextSubTask', '0x00670D80', 'CTaskComplexFollowPatrolRoute', 'CreateNextSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp'),
    ('CTaskComplexFollowPatrolRoute::CreateFirstSubTask', '0x00670E20', 'CTaskComplexFollowPatrolRoute', 'CreateFirstSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRouteX87.cpp'),
    ('CTaskComplexFollowPatrolRoute::ScalarDeletingDestructor', '0x00672BB0', 'CTaskComplexFollowPatrolRoute', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPatrolRoute.cpp'),
    ('CTaskComplexTurnToFaceEntityOrCoord::ScalarDeletingDestructor', '0x00670830', 'CTaskComplexTurnToFaceEntityOrCoord', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp'),
    ('CTaskComplexGotoDoorAndOpen::ScalarDeletingDestructor', '0x006711F0', 'CTaskComplexGotoDoorAndOpen', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexGotoDoorAndOpen.cpp'),
    ('CTaskSimpleSlideToCoord::ScalarDeletingDestructor', '0x006712C0', 'CTaskSimpleSlideToCoord', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp'),
    ('CTaskComplexInvestigateDisturbance::ScalarDeletingDestructor', '0x006712E0', 'CTaskComplexInvestigateDisturbance', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexInvestigateDisturbance.cpp'),
    ('CTaskComplexSeekEntity<FixedPos>::Clone', '0x00671330', 'CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>', 'Clone', 'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    ('CTaskComplexSeekEntity<FixedPos>::CreateSubTask', '0x00673070', 'CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>', 'CreateSubTask', 'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    ('CTaskComplexSeekEntity<FixedPos>::ControlSubTask', '0x00673540', 'CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>', 'ControlSubTask', 'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    ('CTaskComplexSeekEntity<FixedPos>::GetSeekPos', '0x00673BA0', 'CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>', 'GetSeekPos', 'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'),
    ('CTaskComplexGoToPointAnyMeans::GetTaskType', '0x0066B780', 'CTaskComplexGoToPointAnyMeans', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAnyMeans.cpp'),
    ('CAutomobile::DebugCode', '0x006A2F80', 'CAutomobile', 'DebugCode', 'source/game_sa/Entity/Vehicle/Automobile.cpp'),
    ('CAutomobile::RemoveRefsToVehicle', '0x006A3030', 'CAutomobile', 'RemoveRefsToVehicle', 'source/game_sa/Entity/Vehicle/Automobile.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CTaskComplexGoToPointAnyMeans::GetTaskType'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn210A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    update_hooks_csv(FUNCS)
    print('ok', len(FUNCS), 'funciones registradas')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    main(sys.argv[1], tuple(sys.argv[2:]))
