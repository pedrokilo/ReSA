"""Registra CCarCtrl::SetUpDriverAndPassengersForVehicle (0x004217C0), ScanForPedDanger (0x0042CE40), SetCoordsOfScriptCar (0x004342A0) y FindMaximumSpeedForThisCarInTraffic (0x00434400)
con la corrida APROBADA de CarCtrlRest63B.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlrest63b/20261020-201801-699060"

PLANTILLA: la de WaterLevel62 adaptada a cuatro hooks Simple (RH_ScopedInstall en CCarCtrl::InjectHooks). Idempotente. NO toca docs/
(las filas de docs/hooks.csv, si faltan, las anade el coordinador): `CCarCtrl,<funcion>,<direccion>,1,0,1,S` para las cuatro.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'CarCtrlRest63B'
SRC_CC = 'source/game_sa/CarCtrl.cpp'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CCarCtrl::SetUpDriverAndPassengersForVehicle', '0x004217C0', 'CCarCtrl', 'SetUpDriverAndPassengersForVehicle', SRC_CC, 'S'),
    ('CCarCtrl::ScanForPedDanger', '0x0042CE40', 'CCarCtrl', 'ScanForPedDanger', SRC_CC, 'S'),
    ('CCarCtrl::SetCoordsOfScriptCar', '0x004342A0', 'CCarCtrl', 'SetCoordsOfScriptCar', SRC_CC, 'S'),
    ('CCarCtrl::FindMaximumSpeedForThisCarInTraffic', '0x00434400', 'CCarCtrl', 'FindMaximumSpeedForThisCarInTraffic', SRC_CC, 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wh-4 (HEAD df26d6fe7 + fuentes del lote 63 parte C, tandas 1 y 2). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y, '
              'si tienen hook, en el cuerpo de la DLL; no se validan sus cuerpos reales): CVehicle::SetUpDriver 0x6D1A50, SetupPassenger 0x6D1AA0, CPed::GiveObjectToPedToHold 0x5E4390, UpdateStatEnteringVehicle 0x5E01A0, '
              'CModelInfo::IsCarModel 0x4C5AA0, CAnimManager::GetAnimationBlockIndex(const char*) 0x4D3990 y rand del EXE 0x821B1E (sin hook) en SetUpDriverAndPassengersForVehicle (el GetAnimFileIndex virtual se prueba con un '
              'vtable falso de 48 ranuras que registra la ranura llamada); CWorld::FindGroundZForCoord 0x569660, CEntity::GetDistanceFromCentreOfMassToBaseOfModel 0x536BE0, CStuckCarCheck::ClearStuckFlagForCar 0x463C40, '
              'CTheScripts::ClearSpaceForMissionEntity 0x486B00, CAutomobile/CBike::PlaceOnRoadProperly 0x6AF420/0x6BEEB0, CCarCtrl::JoinCarWithRoadAccordingToMission 0x432CB0 y los virtuales SetIsStatic (ranura 4) y '
              'Teleport (ranura 14) de un vtable falso del vehiculo en SetCoordsOfScriptCar (pool de vehiculos falso en 0xB74494); FindPlayerVehicle 0x56E0D0 y CWorld::ClearScanCodes 0x563470 mas SlowCarDownForPedsSectorList '
              '0x425440 en ScanForPedDanger; ClearScanCodes y SlowCarDownFor{Cars,Peds,Objects}SectorList 0x432420/0x425440/0x42D4F0 en FindMaximumSpeedForThisCarInTraffic. Las tres SlowCarDownFor...SectorList son ahora envolturas '
              'plugin::Call hacia el EXE (las reconstruye otro agente): el banco las valida con esa forma y, tras fusionar, la funcion las llamara directamente en C++. Ningun llamador del EXE lee EAX/ECX/EDX de salida; el ST0 de '
              'FindMaximumSpeedForThisCarInTraffic si se compara. Los bool de pila de los colaboradores se registran con su byte bajo; Teleport recibe el byte arg5 tal cual (como el EXE).')
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
