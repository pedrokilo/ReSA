"""Registra las funciones del lote 95C (GetFirstTextureCallback, RwTexDictionaryGtaStreamRead1/2, RpClumpGtaStreamRead1/2 y RpClumpGtaCancelStream) con la corrida APROBADA de Fn95C.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn95c/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn95C/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn95C'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('GetFirstTextureCallback', '0x00734930', 'RwHelper', 'GetFirstTextureCallback', 'source/game_sa/RenderWare/rw/rwtexdict.cpp'),
    ('RwTexDictionaryGtaStreamRead1', '0x00731070', 'RwHelper', 'RwTexDictionaryGtaStreamRead1', 'source/game_sa/RenderWare/rw/rwtexdict.cpp'),
    ('RwTexDictionaryGtaStreamRead2', '0x00731150', 'RwHelper', 'RwTexDictionaryGtaStreamRead2', 'source/game_sa/RenderWare/rw/rwtexdict.cpp'),
    ('RpClumpGtaStreamRead1', '0x0072E570', 'RwHelper', 'RpClumpGtaStreamRead1', 'source/game_sa/RenderWare/rw/rwtexdict.cpp'),
    ('RpClumpGtaStreamRead2', '0x0072E620', 'RwHelper', 'RpClumpGtaStreamRead2', 'source/game_sa/RenderWare/rw/rwtexdict.cpp'),
    ('RpClumpGtaCancelStream', '0x0072E700', 'RwHelper', 'RpClumpGtaCancelStream', 'source/game_sa/RenderWare/rw/rwtexdict.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el worktree wt-95 sobre main 329ab605c con los cambios del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y, cuando tienen hook RH en la DLL '
              '(apagado durante el banco), tambien en el cuerpo de la DLL: RwStreamFindChunk 0x7ED2D0, RwStreamRead 0x7EC9D0, RwStreamSkip 0x7ECD00, RwTexDictionaryCreate 0x7F3600, RwTextureGtaStreamRead 0x730E60 (hook), RwTexDictionaryAddTexture 0x7F3980, '
              'RwTexDictionaryForAllTextures 0x7F3730, RwTexDictionaryDestroy 0x7F36A0, _rwFrameListStreamRead 0x807970, _rwFrameListDeinitialize 0x807720 (hook), CMemoryMgr::Malloc 0x72F420 (hook), CMemoryMgr::Free 0x72F430, RpGeometryStreamRead 0x74D190, '
              'RpGeometryDestroy 0x74CCC0, RpAtomicCreate 0x749C50, RpAtomicSetFrame 0x74BF20, RpAtomicSetGeometry 0x749D40, RpAtomicDestroy 0x749DC0, RpClumpCreate 0x74A290, RpClumpAddAtomic 0x74A490 y RpClumpDestroy 0x74A310. '
              'RpClumpGtaStreamRead1/2 y RpClumpGtaCancelStream usan cuatro ayudantes del EXE con convencion de registros propia (0x72E140, 0x72E220, 0x72E270 y 0x72E360) que NO son enganchables: la DLL los lleva como funciones privadas en C++ y su evidencia es la de estas tres funciones '
              '(el lado EXE ejecuta los ayudantes reales). Registran sus argumentos en orden; el stream es una arena cuya posicion (+0xC) avanzan los falsos. Cada lado coincide ademas con el oraculo independiente fixture_oracle.py. '
              'Fuera de los fixtures: longitudes de chunk de TXD 0 o 1 (el EXE usa bytes viejos de la pila) y mayores de 12 (el EXE desborda su marco), longitudes de atomico distintas de 16, indices de frame o de geometria fuera de las arenas, y la exactitud de los colaboradores falseados (se hereda).')
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn95C con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
