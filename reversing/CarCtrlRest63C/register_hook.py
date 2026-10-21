"""Registra CCarCtrl::PickNextNodeAccordingStrategy (0x00432B10), JoinCarWithRoadSystem (0x0042F5A0) y JoinCarWithRoadSystemGotoCoors (0x0042F870) con la corrida APROBADA de CarCtrlRest63C.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63c/20261020-205957-788299"

PLANTILLA: la de WaterLevel62 adaptada a tres hooks Simple (RH_ScopedInstall en CCarCtrl::InjectHooks). Idempotente. NO toca docs/
(las filas de docs/hooks.csv, si faltan, las anade el coordinador): `CCarCtrl,<funcion>,<direccion>,1,0,1,S` para las tres.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'CarCtrlRest63C'
SRC_CC = 'source/game_sa/CarCtrl.cpp'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CCarCtrl::PickNextNodeAccordingStrategy', '0x00432B10', 'CCarCtrl', 'PickNextNodeAccordingStrategy', SRC_CC, 'S'),
    ('CCarCtrl::JoinCarWithRoadSystem', '0x0042F5A0', 'CCarCtrl', 'JoinCarWithRoadSystem', SRC_CC, 'S'),
    ('CCarCtrl::JoinCarWithRoadSystemGotoCoors', '0x0042F870', 'CCarCtrl', 'JoinCarWithRoadSystemGotoCoors', SRC_CC, 'S'),
)
SCOPE_NOTE = 'Validado en el worktree D:/Original/wh-4 (HEAD df26d6fe7 + fuentes del lote 63 parte C, tandas 1 a 3). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y, si tienen hook activo, en el cuerpo de la DLL; no se validan sus cuerpos reales): FindPlayerCoors 0x56E010, FindPlayerVehicle 0x56E0D0 y las envolturas PickNextNodeToChaseCar 0x426EF0, PickNextNodeToFollowPath 0x427740 y PickNextNodeRandomly 0x42DE80 en PickNextNodeAccordingStrategy (el EXE empuja ademas un dword sobrante, el vehiculo del jugador o el objetivo, que la funcion llamada ignora); CPathFind::FindNodeClosestToCoorsFavourDirection 0x44FCE0 (devolucion por puntero oculto) y FindLinksToGoWithTheseNodes 0x42B470 (envoltura plugin::Call) en JoinCarWithRoadSystem; CPathFind::DoPathSearch 0x4515D0 (19 dwords de pila), RemoveBadStartNode 0x44E4F0 (sin reconstruir; por direccion), CAutoPilot::RemoveOnePathNode 0x41B950, FindLinksToGoWithTheseNodes 0x42B470 y JoinCarWithRoadSystem 0x42F5A0 (con el hook apagado fuera de su propia prueba, cuerpo de la DLL parcheado) en JoinCarWithRoadSystemGotoCoors. Los nodos, enlaces y areas de ThePaths son sinteticos (arenas con guardas; ThePaths.m_pPathNodes/m_pNodeLinks guardados y restaurados). Los bool de pila de DoPathSearch se registran con su byte bajo. JoinCarWithRoadSystem solo se prueba con matriz valida, como el EXE (que la desreferencia sin comprobarla); JoinCarWithRoadSystemGotoCoors con y sin matriz. Ningun llamador del EXE lee EAX/ECX/EDX de salida salvo AL de las dos funciones bool (que el banco compara).'
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
    for ID, ADDR, CLS, FN, SOURCE, KIND in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            evidence = {'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                        'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']}
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco CarCtrlRest63 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (el AL de bool si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
