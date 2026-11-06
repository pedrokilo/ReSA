"""Registra las funciones del lote 109A (16 funciones de camara, SetPiece, escaneres, pool de rutas y ropa) con la corrida APROBADA de Fn109A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn109a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn109A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn109A'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CSetPiece::GetSpawnCoord1', '0x004997A0', 'CSetPiece', 'GetSpawnCoord1', 'source/game_sa/SetPiece.cpp'),
    ('CSetPiece::GetSpawnCoord2', '0x004997E0', 'CSetPiece', 'GetSpawnCoord2', 'source/game_sa/SetPiece.cpp'),
    ('CSetPiece::GetTargetCoord1', '0x00499820', 'CSetPiece', 'GetTargetCoord1', 'source/game_sa/SetPiece.cpp'),
    ('CSetPiece::GetTargetCoord2', '0x00499860', 'CSetPiece', 'GetTargetCoord2', 'source/game_sa/SetPiece.cpp'),
    ('CCam::ClipBeta', '0x00509C50', 'CCam', 'ClipBeta', 'source/game_sa/Cam.cpp'),
    ('CCam::GetWeaponFirstPersonOn', '0x00509DC0', 'CCam', 'GetWeaponFirstPersonOn', 'source/game_sa/Cam.cpp'),
    ('IsLampPost', '0x00509A30', 'CCam', 'IsLampPost', 'source/game_sa/Cam.cpp'),
    ('VectorsDiffer', '0x00509760', 'Common', 'VectorsDiffer', 'source/game_sa/common.cpp'),
    ('VectorsAreEqual', '0x004DC670', 'Common', 'VectorsAreEqual', 'source/game_sa/common.cpp'),
    ('CEventDamage::HasKilledPed', '0x004ABCA0', 'CEventDamage', 'HasKilledPed', 'source/game_sa/Events/EventDamage.cpp'),
    ('CVehicleScanner::GetClosestVehicleInRange', '0x005FFEE0', 'CVehicleScanner', 'GetClosestVehicleInRange', 'source/game_sa/VehicleScanner.cpp'),
    ('CPedScanner::GetClosestPedInRange', '0x005FFF20', 'CPedScanner', 'GetClosestPedInRange', 'source/game_sa/PedScanner.cpp'),
    ('CPointRoute::operator new', '0x0041B7C0', 'CPointRoute', 'operator new', 'source/game_sa/PedGeometryAnalyser.cpp'),
    ('CPointRoute::operator delete', '0x0041B7D0', 'CPointRoute', 'operator delete', 'source/game_sa/PedGeometryAnalyser.cpp'),
    ('CPedClothesDesc::SetTextureAndModel-hash', '0x005A8050', 'CPedClothesDesc', 'SetTextureAndModel-hash', 'source/game_sa/PedClothesDesc.cpp'),
    ('CPedClothesDesc::SetTextureAndModel-name', '0x005A8080', 'CPedClothesDesc', 'SetTextureAndModel-name', 'source/game_sa/PedClothesDesc.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el worktree wv-1 sobre main b00bfe4d9 con los cambios del lote (ver artifacts/build-manifest.json). CPointRoute::operator new corre CPointRoutePool::New (0x41B5B0) de verdad en ambos lados (original en el EXE con su hook apagado, cuerpo propio en la DLL); SetTextureAndModel corre CClothes::GetTextureDependency (0x5A7EA0) de verdad en ambos lados y, por nombre, CKeyGen::GetUppercaseKey (0x53CF30) es un falso identico en el EXE y en su cuerpo de la DLL. Cada lado coincide ademas con el oraculo independiente fixture_oracle.py. Fuera de los fixtures: punteros no validos, ranuras de arma fuera de -8..14, bytes de bandera no booleanos (solo se compara AL como verdad), borrado doble en el pool y valores de parte fuera de -8..30.')
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn109A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
