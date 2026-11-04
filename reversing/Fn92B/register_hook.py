"""Registra la funcion del lote 92B (CObjectPotentialCollisionScanner::ScanForObjectPotentialCollisionEvents) con la corrida APROBADA de Fn92B.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn92b/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn92B/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn92B'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CObjectPotentialCollisionScanner::ScanForObjectPotentialCollisionEvents', '0x00606890', 'CObjectPotentialCollisionScanner', 'ScanForObjectPotentialCollisionEvents', 'source/game_sa/Events/EventScanner.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en un worktree sobre main e3c35027a con los cambios del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y, cuando tienen hook RH en la DLL (apagado durante el banco) o cuerpo C++ propio, en su cuerpo del ASI: '
              'CTaskManager::GetSimplestActiveTask 0x6819D0, CObjectScanner::ScanForObjectsInRange 0x5FFF30 (sin hook: la DLL tiene cuerpo C++ propio y recibe el mismo falso), CEntityScanner::Clear 0x5FF9D0, CEntity::GetBoundCentre 0x534250, '
              'CPedGeometryAnalyser::GetIsLineOfSightClear 0x5F5A30 y 0x5F2F00, CEventGroup::Add 0x4AB420, CEntity::RegisterReference 0x571B70 y CEntity::CleanUpOldReference 0x571A00. Registran sus argumentos en orden con el contenido de los vectores y, en Add, los campos definidos del evento (tipo por GetEventType virtual). '
              'NO se sustituyen y corren de verdad en ambos lados: el temporizador en linea, CPed::IsPlayer 0x5DF8F0, CTask::IsGoToTask 0x61A360 y las llamadas virtuales GetTaskType sobre arenas con vtabla propia, el constructor del escaner de objetos (0x606010 frente al C++ de la DLL), '
              'los constructores y destructores de CEventPotentialWalkIntoObject (0x4AE5D0, hook activo; 0x4AE670) y toda la aritmetica x87 del EXE frente al C++ de la DLL, con CTimer::m_snTimeInMilliseconds (0xB7CB84) y ocho ranuras de la tabla de modelos 0xA9B0C8 como memoria real del juego restaurada tras cada lado. '
              'El banco compara los eventos de los falsos con todos sus argumentos y los bytes cambiados de todas las arenas; cada lado coincide ademas con el oraculo independiente fixture_oracle.py (aritmetica racional exacta a 24 bits). '
              'Fuera de los fixtures: ped sin matriz (el EXE la desreferencia solo con matriz nula usa el placement, cubierto), indices de modelo fuera de las ocho ranuras sustituidas, NaN con cargas distintas de 0xFFC00000, NaN senalizantes, tareas con GetTaskType que escriben y valores de retorno de los falsos.')
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
    if not (RUN / 'comparisons.jsonl.gz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
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
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn92B con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
