"""Registra CWaterLevel::FillQuadsAndTrianglesList (0x006E7B30), GetWaterDepth (0x006EA960), RenderHighDetailWaterRectangle (0x006EB810), RenderHighDetailWaterTriangle (0x006EDDC0) y
TestLineAgainstWater (0x006E61B0) con la corrida APROBADA de WaterLevel62.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-waterlevel62/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de Misc61 adaptada a cinco hooks Simple (RH_ScopedGlobalInstall en CWaterLevel::InjectHooks). Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe
reversing/WaterLevel62/validation-summary.json y anade las entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/
(las filas de docs/hooks.csv, si faltan, las anade el coordinador): `CWaterLevel,FillQuadsAndTrianglesList,0x006E7B30,1,0,1,S`, `CWaterLevel,GetWaterDepth,0x006EA960,1,0,1,S`,
`CWaterLevel,RenderHighDetailWaterRectangle,0x006EB810,1,0,1,S`, `CWaterLevel,RenderHighDetailWaterTriangle,0x006EDDC0,1,0,1,S`, `CWaterLevel,TestLineAgainstWater,0x006E61B0,1,0,1,S`.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'WaterLevel62'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CWaterLevel::FillQuadsAndTrianglesList', '0x006E7B30', 'CWaterLevel', 'FillQuadsAndTrianglesList', 'source/game_sa/WaterLevel.cpp', 'S'),
    ('CWaterLevel::GetWaterDepth', '0x006EA960', 'CWaterLevel', 'GetWaterDepth', 'source/game_sa/WaterLevel.cpp', 'S'),
    ('CWaterLevel::RenderHighDetailWaterRectangle', '0x006EB810', 'CWaterLevel', 'RenderHighDetailWaterRectangle', 'source/game_sa/WaterLevel.cpp', 'S'),
    ('CWaterLevel::RenderHighDetailWaterTriangle', '0x006EDDC0', 'CWaterLevel', 'RenderHighDetailWaterTriangle', 'source/game_sa/WaterLevel.cpp', 'S'),
    ('CWaterLevel::TestLineAgainstWater', '0x006E61B0', 'CWaterLevel', 'TestLineAgainstWater', 'source/game_sa/WaterLevel.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/we-4 (HEAD 0ac292a76 + fuentes del lote 62). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL; '
              'no se validan sus cuerpos reales): AddToQuadsAndTrianglesList (0x6E5750, sin reconstruir) en FillQuadsAndTrianglesList; CWaterLevel::GetWaterLevelNoWaves (0x6E8580) y GetGroundLevel (0x6EA8A0, '
              'sin reconstruir) en GetWaterDepth; CCamera::IsSphereVisible (0x420C40), SetUpWaterFog (0x6EA9F0), RenderHighDetailWaterRectangle_OneLayer (0x6E91D0, sin reconstruir), '
              'SplitWaterRectangleAlongXLine (0x6E73A0) y RenderWaterRectangle (0x6EC5D0) en RenderHighDetailWaterRectangle; IsSphereVisible, RenderHighDetailWaterTriangle_OneLayer (0x6E8780, sin reconstruir) y '
              'SplitWaterTriangleAlongXLine (0x6ECF00) en RenderHighDetailWaterTriangle. Las funciones de render NO se prueban contra una GPU: se comparan las llamadas a los colaboradores (que son los que '
              'escriben vertices y llaman a RenderWare) con sus argumentos exactos, en orden. El lerp (0x6E5280) NO se sustituye: la DLL usa el de WaterLevel.h y el EXE el suyo, y sus resultados se comparan '
              'en los argumentos de RenderWaterRectangle; los CRenPar de lerp se generan sin denormales (el SSE del juego vacia los denormales, el x87 no). TestLineAgainstWater y FillQuadsAndTrianglesList se prueban '
              'con un atlas de agua sintetico escrito en las tablas reales del juego (guardadas y restauradas en cada caso). Los indices de vertice 0xFFFF leen una ventana de 20 bytes anterior a m_aVertices '
              '(guardada y restaurada). GetWaterDepth: cuando GetGroundLevel devuelve true sin escribir, el EXE leeria basura de pila (no se prueba). EAX/ECX/EDX solo registrados; el AL de salida si se compara.')
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
                                 'en ambos lados). Banco WaterLevel62 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (el AL de bool si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
