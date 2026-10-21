"""Registra CWorldScan::ScanWorld (0x0072CAE0) y SetExtraRectangleToScan (0x0072D5E0) con la corrida APROBADA de WorldScan64.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-worldscan64/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de Misc61 adaptada a dos hooks Simple. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/WorldScan64/validation-summary.json y anade las
entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador):
`CWorldScan,ScanWorld,0x0072CAE0,1,0,1,S` y `CWorldScan,SetExtraRectangleToScan,0x0072D5E0,1,0,1,S`.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'WorldScan64'
SRC_FILE = 'source/game_sa/Renderer.cpp'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CWorldScan::ScanWorld', '0x0072CAE0', 'CWorldScan', 'ScanWorld', SRC_FILE, 'S'),
    ('CWorldScan::SetExtraRectangleToScan', '0x0072D5E0', 'CWorldScan', 'SetExtraRectangleToScan', SRC_FILE, 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wg-4 (HEAD df26d6fe7 + fuente del lote 64). CGeneral::GetATanOfXY (0x53CC70) se sustituye por un falso identico en ambos lados '
              '(parche en la entrada del EXE y en el cuerpo de la DLL; no se valida su cuerpo): atan2 de double redondeado a float, registra EN ORDEN sus argumentos. La devolucion de llamada '
              'scanFunction es un falso del banco que registra (x, y). Las celdas extra (0xC81598/0xC815A8/0xC815B8) se fijan por caso y se restauran. floor/ceil/_ftol2 del CRT (0x8219F0, 0x823820, 0x821B40) '
              'se ejecutan reales en ambos lados. Alcance: 2 a 5 puntos finitos (sin NaN/infinitos, |coord| < 700), a lo sumo 4 celdas extra (el EXE no comprueba el limite). No se prueban los caminos en que el EXE '
              'desreferencia sin comprobar (n < 2, mas de 5 puntos, NaN en las coordenadas). Ningun llamador del EXE lee EAX/ECX/EDX de ScanWorld (0x554E94, 0x55555E, 0x555664, 0x555C9B, 0x6E6E8C) y '
              'SetExtraRectangleToScan no tiene llamadores (codigo muerto): solo se registran.')
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
            if KIND == 'V':
                evidence['original_address'] = ADDR   # los hooks virtuales se registran con direccion 0x00000000 y tipo V
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR if KIND == 'S' else '0x00000000', 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco WorldScan64 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (EAX de los HRESULT si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
