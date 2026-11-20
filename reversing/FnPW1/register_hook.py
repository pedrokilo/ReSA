"""Registra de nuevo CVehicle::ProcessWheel (0x6D6C00) con la corrida APROBADA de FnPW1 (sustituye la evidencia antigua del banco CVehD, obsoleta tras las correcciones de NaN y x87).

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fnpw1/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/FnPW1/validation-summary.json y REEMPLAZA la entrada de ProcessWheel en
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (docs/hooks.csv ya trae la fila).
Antes de ejecutarlo hay que revisar SCOPE_NOTE.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'FnPW1'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente)
    ('CVehicle::ProcessWheel', '0x006D6C00', 'CVehicle', 'ProcessWheel', 'source/game_sa/Entity/Vehicle/Vehicle.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el worktree wv-int (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con sus hooks RH apagados; '
              'no se validan sus cuerpos): CPhysical::ApplyMoveForce(CVector) 0x5429F0 y CPhysical::ApplyTurnForce 0x542A50 (el producto vectorial corre de verdad en ambos lados y lo calcula el oraculo); la fuerza y el torque aplicados se comparan como los argumentos exactos de esas dos llamadas '
              '(las palabras de float de los eventos con cualquier NaN canonizado: NaN con NaN). Corren de verdad rand() del CRT del EXE (0x821B1E, semilla fijada por lado), CTimer::ms_fTimeStep, '
              'gHandlingDataMgr.fWheelFriction, el CHandlingData, el vehiculo y las estaticas 0xC1CDAC..0xC1CDAF (regiones restauradas). El banco compara el orden y todos los argumentos de los falsos, los sorteos de rand(), '
              'el estado de la rueda y las estaticas; cada lado coincide ademas con el oraculo independiente fixture_oracle.py. Fuera de los fixtures: cargas de NaN, valores de las estaticas AD/AE antes de la llamada '
              '(la funcion los escribe), wheelSpeed (no se lee).')
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
        entry = dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md')
        # la entrada existente (evidencia antigua de CVehD) se REEMPLAZA en su sitio
        idx = [i for i, r in enumerate(data['functions']) if r['hook']['class'] == CLS and r['hook']['fn_name'] == FN]
        assert len(idx) <= 1, (CLS, FN, idx)
        if idx:
            data['functions'][idx[0]] = entry
        else:
            data['functions'].append(entry)
        if True:
            a[ADDR] = {'identity': CLS + '::' + FN, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ con la x87 del EXE (ayudantes Veh83D*). Banco FnPW1 con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
