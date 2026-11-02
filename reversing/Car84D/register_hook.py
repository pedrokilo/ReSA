"""Registra CCarCtrl::DragCarToPoint (0x42EC90, lote 84D) con la corrida APROBADA de Car84D.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-car84d/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Car84D/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv
`CCarCtrl,DragCarToPoint,0x0042EC90,1,0,1,S` la anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Car84D'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CCarCtrl::DragCarToPoint', '0x0042EC90', 'CCarCtrl', 'DragCarToPoint', 'source/game_sa/CarCtrl.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el arbol principal (HEAD 89afb3901 + cambios del lote 84D; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos '
              'en la direccion del EXE y en el cuerpo de la DLL (con su hook RH apagado; no se valida su cuerpo): CCollision::ProcessLineSphere 0x412AA0, CCollision::IsStoredPolyStillValidVerticalLine 0x414D70, '
              'CWorld::ProcessVerticalLine 0x5674E0, CrossProduct 0x59C730 y CVehicle::UpdateLightingFromStoredPolys 0x6D0CC0 (el banco compara todos sus argumentos EN ORDEN, con el byte +0x3E1 y las banderas del vehiculo en ese instante, '
              'y escribe los valores del caso) y el virtual GetHeightAboveRoad (vtabla +0xD4) con una vtabla falsa compartida. Corren de verdad la matriz del vehiculo, su informacion de modelo y su caja de colision (arenas enlazadas '
              'desde CModelInfo::ms_modelInfoPtrs, restauradas) y las constantes de .rdata. El banco compara los eventos, las banderas y el byte +0x3E1 del vehiculo, la entidad y los suelos guardados, el volante, las dos matrices y las '
              'banderas de las dos entidades, sin otros cambios en las arenas; cada lado coincide ademas con el oraculo independiente fixture_oracle.py. Mutaciones del estado tras un evento (9 tipos). Fuera de los fixtures: vehiculo sin '
              'matriz (el EXE falla), suelos infinitos o enormes con medio largo finito distinto de cero y medio largo pequeno distinto de cero (alli el cos de la x87 y el de la DLL difieren por la precision del angulo), '
              'NaN con cargas distintas (las fixtures usan un unico patron), indice de modelo negativo o fuera de CModelInfo::ms_modelInfoPtrs y bytes altos de los booleanos pasados a ProcessVerticalLine.')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE (sus colaboradores ProcessLineSphere, IsStoredPolyStillValidVerticalLine, ProcessVerticalLine, CrossProduct y UpdateLightingFromStoredPolys son falsos identicos en ambos lados). Banco Car84D con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
