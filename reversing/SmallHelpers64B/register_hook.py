"""Registra GetPopcycleGroupPedModel (CGangWars, 0x00406E80), FlushWaterRenderBuffer (CWaterLevel, 0x006E7680) y CTaskComplex::Destructor (0x0061A3D0) con la corrida APROBADA de SmallHelpers64B.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-smallhelpers64b/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de WaterLevel62 adaptada a tres hooks Simple (RH_ScopedGlobalInstall). Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe
reversing/SmallHelpers64B/validation-summary.json y anade las entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/
(las filas de docs/hooks.csv, si faltan, las anade el coordinador): `CGangWars,GetPopcycleGroupPedModel,0x00406E80,1,0,1,S`, `CWaterLevel,FlushWaterRenderBuffer,0x006E7680,1,0,1,S`,
`CTaskComplex,Destructor,0x0061A3D0,1,0,1,S`.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'SmallHelpers64B'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('GetPopcycleGroupPedModel', '0x00406E80', 'CGangWars', 'GetPopcycleGroupPedModel', 'source/game_sa/GangWars.cpp', 'S'),
    ('FlushWaterRenderBuffer', '0x006E7680', 'CWaterLevel', 'FlushWaterRenderBuffer', 'source/game_sa/WaterLevel.cpp', 'S'),
    ('CTaskComplex::Destructor', '0x0061A3D0', 'CTaskComplex', 'Destructor', 'source/game_sa/Tasks/Task.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wp-2 (HEAD 1b13a6db4 + fuentes del lote 64B). Funciones sin simbolo confirmado: nombres asignados por el desensamblado. '
              'GetPopcycleGroupPedModel (0x406E80): tablas del juego (m_TranslationArray, m_PedGroups) sustituidas por contenido sintetico en ventanas guardadas y restauradas; sin colaboradores. '
              'FlushWaterRenderBuffer (0x6E7680): RwIm3DTransform (0x7EF450), RwIm3DRenderIndexedPrimitive (0x7EF550) y RwIm3DEnd (0x7EF520) sustituidos por falsos identicos en ambos lados '
              '(no se prueba contra una GPU; se comparan sus argumentos exactos, en orden, y el contador g_nNumIm3dDrawCalls que LittleTest incrementa, que NO se sustituye). '
              'CTaskComplex::Destructor (0x61A3D0): es el cuerpo no virtual de ~CTaskComplex (SEH omitido: el marco solo desenrolla); la subtarea es un objeto falso cuyo destructor virtual con borrado '
              'registra su llamada; ninguna tarea real se construye ni se destruye en el banco. EAX/ECX/EDX solo registrados salvo el EAX devuelto por GetPopcycleGroupPedModel.')
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
