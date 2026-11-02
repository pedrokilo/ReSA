"""Registra CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing (0x435830, lote 84J) con la corrida APROBADA de Car84J.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car84j/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Car84J/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv
`CCarCtrl,SteerAICarWithPhysicsFollowPath_Racing,0x00435830,1,0,1,S` la anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Car84J'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CCarCtrl::SteerAICarWithPhysicsFollowPath_Racing', '0x00435830', 'CCarCtrl', 'SteerAICarWithPhysicsFollowPath_Racing', 'source/game_sa/CarCtrl.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el arbol principal (HEAD 1f02243c3/5b8ca9c2c + cambios del lote 84J; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos '
              'en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado; no se valida su cuerpo): CCarCtrl::StopCarIfNodesAreInvalid 0x422590, SteerAICarWithPhysicsHeadingForTarget 0x433280, '
              'DealWithBend_Racing 0x428040, ClipTargetOrientationToLink 0x422760, FindPercDependingOnDistToLink 0x422620, FindAngleToWeaveThroughTraffic 0x4325C0, CPathFind::DoPathSearch 0x4515D0 y FindLinkBetweenNodes '
              '0x451350, CTrafficLights::ShouldCarStopForBridge 0x49D420 y CCarAI::CarHasReasonToStop 0x41C050 (el banco compara los argumentos y el estado del vehiculo en cada llamada EN ORDEN y escribe los valores del caso). '
              'Corren de verdad, con el hook apagado (bytes del EXE frente al C++ de la DLL), CGeneral::GetATanOfXY 0x53CC70 y CAutoPilot::RemoveOnePathNode 0x41B950; ThePaths (tres areas con ocho tramos, ocho nodos y '
              'cuarenta enlaces de nodo y de navegacion) es el del banco. El banco compara los eventos, el estado final del vehiculo (modelo, tramos, signos, carriles, mision, crucero, banderas, nodos, destino.x, contador y ruta '
              'de nodos, velocidad.x, subtipo, temporizador), la matriz y las cuatro salidas, sin otros cambios en las arenas; cada lado coincide ademas con el oraculo independiente fixture_oracle.py. Mutaciones del estado tras '
              'un evento (17 tipos). Fuera de los fixtures: vehiculo sin matriz, tramos, nodos o areas fuera de las tablas del banco, nodo siguiente de la ruta que no este en la tabla de enlaces del nodo actual (el EXE lo busca '
              'sin limite), signos de la ventana de enlaces para enlaces invalidos (basura de pila en el EXE que no se lee), nodos invalidos en medio de la ruta, rumbo corregido infinito o de mas de 2000 radianes, NaN con '
              'cargas distintas (las fixtures usan un unico patron) y bytes altos de los booleanos de los colaboradores.')
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

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE (sus colaboradores StopCarIfNodesAreInvalid, DoPathSearch, SteerAICarWithPhysicsHeadingForTarget, FindLinkBetweenNodes, DealWithBend_Racing, ClipTargetOrientationToLink, FindPercDependingOnDistToLink, FindAngleToWeaveThroughTraffic, ShouldCarStopForBridge y CarHasReasonToStop son falsos identicos en ambos lados). Banco Car84J con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
