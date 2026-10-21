"""Registra CCarCtrl::ThisVehicleShouldTryNotToTurn (0x00421FE0), FindPathDirection (0x00422090), SwitchBetweenPhysicsAndGhost (0x004222A0), FindSpeedMultiplier (0x004224E0),
StopCarIfNodesAreInvalid (0x00422590), FindPercDependingOnDistToLink (0x00422620) y FindIntersection2Lines (0x004226F0) con la corrida APROBADA de CarCtrlRest63.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63/20261020-195518-439585"

PLANTILLA: la de WaterLevel62 adaptada a siete hooks Simple (cinco RH_ScopedInstall y dos RH_ScopedNamedGlobalInstall hacia X87Math en CCarCtrl::InjectHooks). Idempotente. NO toca docs/
(las filas de docs/hooks.csv, si faltan, las anade el coordinador): `CCarCtrl,<funcion>,<direccion>,1,0,1,S` para las siete.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'CarCtrlRest63'
SRC_CC = 'source/game_sa/CarCtrl.cpp'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CCarCtrl::ThisVehicleShouldTryNotToTurn', '0x00421FE0', 'CCarCtrl', 'ThisVehicleShouldTryNotToTurn', SRC_CC, 'S'),
    ('CCarCtrl::FindPathDirection', '0x00422090', 'CCarCtrl', 'FindPathDirection', SRC_CC, 'S'),
    ('CCarCtrl::SwitchBetweenPhysicsAndGhost', '0x004222A0', 'CCarCtrl', 'SwitchBetweenPhysicsAndGhost', SRC_CC, 'S'),
    ('CCarCtrl::FindSpeedMultiplier', '0x004224E0', 'CCarCtrl', 'FindSpeedMultiplier', SRC_CC, 'S'),
    ('CCarCtrl::StopCarIfNodesAreInvalid', '0x00422590', 'CCarCtrl', 'StopCarIfNodesAreInvalid', SRC_CC, 'S'),
    ('CCarCtrl::FindPercDependingOnDistToLink', '0x00422620', 'CCarCtrl', 'FindPercDependingOnDistToLink', SRC_CC, 'S'),
    ('CCarCtrl::FindIntersection2Lines', '0x004226F0', 'CCarCtrl', 'FindIntersection2Lines', SRC_CC, 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wh-4 (HEAD df26d6fe7 + fuentes del lote 63 parte C). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): '
              'CPathNode::GetPosition (0x420A10) en FindPathDirection (ocho llamadas en orden) y CColStore::HasCollisionLoaded (0x410CE0), CDamageManager::SetWheelStatus (0x6C21A0), CAutomobile::PlaceOnRoadProperly (0x6AF420) y '
              'CBike::PlaceOnRoadProperly (0x6BEEB0) en SwitchBetweenPhysicsAndGhost. FindSpeedMultiplier y FindIntersection2Lines: aritmetica x87 de 24 bits en X87Math (Cc63*); sus fixtures excluyen a1 enorme/infinito en FindSpeedMultiplier '
              '(el bucle de +-2pi del EXE no terminaria). Ningun llamador del EXE lee EAX/ECX/EDX de salida salvo EDX tras FindIntersection2Lines (0x4287C6, DealWithBend_Racing), que el banco compara; el AL de bool y ST0 de float si se comparan.')
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
