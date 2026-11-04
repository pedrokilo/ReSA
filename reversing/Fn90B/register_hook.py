"""Registra las ocho funciones del lote piloto 90B con la corrida APROBADA de Fn90B.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn90b/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn90B/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv
las anade el coordinador al registrar).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn90B'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CNodeAddress::operator==', '0x00420920', 'CNodeAddress', 'operator==', 'source/game_sa/NodeAddress.cpp'),
    ('CNodeAddress::operator!=', '0x00420950', 'CNodeAddress', 'operator!=', 'source/game_sa/NodeAddress.cpp'),
    ('CNodeAddress::operator<', '0x00420980', 'CNodeAddress', 'operator<', 'source/game_sa/NodeAddress.cpp'),
    ('CWorld::AdvanceCurrentScanCode', '0x004072E0', 'CWorld', 'AdvanceCurrentScanCode', 'source/game_sa/World.cpp'),
    ('CTaskSimpleScratchHead::CTaskSimpleScratchHead', '0x0048DF30', 'CTaskSimpleScratchHead', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleScratchHead.cpp'),
    ('CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading', '0x00667E20', 'CTaskSimpleAchieveHeading', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp'),
    ('CColModel::CColModel', '0x0040FB60', 'CColModel', 'Constructor', 'source/game_sa/Collision/ColModel.cpp'),
    ('CEventDeath::CEventDeath', '0x004ADDF0', 'CEventDeath', 'Constructor', 'source/game_sa/Events/EventDeath.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el worktree D:/Original/wl-1 (ver artifacts/build-manifest.json). AdvanceCurrentScanCode: el colaborador CWorld::ClearScanCodes 0x563470 se sustituye por un falso identico '
              'en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado; no se valida su cuerpo). Los cuatro constructores NO sustituyen a sus bases (CTaskSimpleRunAnim 0x61A900 / CTask 0x61A390 en el EXE, '
              'constructores C++ en linea en la DLL, sin direccion propia): corren de verdad en ambos lados y se compara la ventana de 0x40 bytes del objeto (vtable normalizada: el EXE deja la suya; la DLL, la de su clase dentro del ASI) '
              'y las guardas. Las tres comparaciones de CNodeAddress y AdvanceCurrentScanCode no llaman a nadie mas. Cada lado coincide ademas con el oraculo independiente fixture_oracle.py. '
              'Los objetos creados desde el EXE con estos constructores pasan a tener la vtable de la clase de la DLL. Fuera de los fixtures: bool de CEventDeath con valores de byte distintos de 0/1 (los llamadores del EXE apilan 0 o 1), '
              'NaN senalizantes ya cubiertos solo como patrones de bits, y los constructores de las clases base (validados por sus propios bancos).')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE (el único colaborador sustituido es ClearScanCodes en AdvanceCurrentScanCode; los constructores corren con sus bases reales). Banco Fn90B con ' + str(int(s['pairs'])) + ' pares y cero diferencias. Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
