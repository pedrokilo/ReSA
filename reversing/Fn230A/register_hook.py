"""Registra las 25 funciones del lote 230A con la corrida APROBADA de Fn230A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn230a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn230A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Ademas pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas (sin sync_docs ni hooks2md).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn230A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
RW = 'source/game_sa/RenderWare/rw/rpworld.cpp'
FE = 'source/game_sa/Fx/FxEmitter.cpp'
FB = 'source/game_sa/Fx/FxEmitterBP.cpp'
LJ = 'source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp'
FUNCS = (
    ('RpWorld::RpWorldPluginAttach', '0x0074FDA0', 'RpWorld', 'RpWorldPluginAttach', RW),
    ('RpWorld::_rpLightTieDestroy', '0x0074FFA0', 'RpWorld', '_rpLightTieDestroy', RW),
    ('RpWorld::_rpTieDestroy', '0x0074FFF0', 'RpWorld', '_rpTieDestroy', RW),
    ('FxEmitterBP_c::DestructorBody', '0x004A18F0', 'FxEmitterBP_c', 'DestructorBody', FB),
    ('FxEmitterBP_c::OperatorNew', '0x004A1900', 'FxEmitterBP_c', 'OperatorNew', FB),
    ('FxEmitter_c::ConstructorBody', '0x004A1920', 'FxEmitter_c', 'ConstructorBody', FE),
    ('FxEmitterBP_c::IsFxInfoPresent', '0x004A24D0', 'FxEmitterBP_c', 'IsFxInfoPresent', FB),
    ('FxEmitter_c::Init', '0x004A2550', 'FxEmitter_c', 'Init', FE),
    ('FxEmitter_c::Reset', '0x004A2570', 'FxEmitter_c', 'Reset', FE),
    ('FxEmitter_c::Update', '0x004A4460', 'FxEmitter_c', 'Update', FE),
    ('FxEmitterBP_c::ScalarDeletingDestructor', '0x004A2B20', 'FxEmitterBP_c', 'ScalarDeletingDestructor', FB),
    ('FxEmitter_c::ScalarDeletingDestructor', '0x004A2B90', 'FxEmitter_c', 'ScalarDeletingDestructor', FE),
    ('FxInfo_c::DestructorBody', '0x004A4940', 'FxInfo_c', 'DestructorBody', 'source/game_sa/Fx/Blueprints/FxInfo.cpp'),
    ('FxInfoManager_c::DestructorBody', '0x004A4950', 'FxInfoManager_c', 'DestructorBody', 'source/game_sa/Fx/FxInfoManager.cpp'),
    ('LibJpegCompressEncode::jpeg_start_compress', '0x005C26B0', 'LibJpegCompressEncode', 'jpeg_start_compress', LJ),
    ('LibJpegCompressEncode::jpeg_write_scanlines', '0x005C2730', 'LibJpegCompressEncode', 'jpeg_write_scanlines', LJ),
    ('LibJpegCompressEncode::jinit_c_coef_controller', '0x005C30D0', 'LibJpegCompressEncode', 'jinit_c_coef_controller', LJ),
    ('LibJpegCompressEncode::grayscale_convert', '0x005C35F0', 'LibJpegCompressEncode', 'grayscale_convert', LJ),
    ('LibJpegCompressEncode::jinit_color_converter', '0x005C3700', 'LibJpegCompressEncode', 'jinit_color_converter', LJ),
    ('LibJpegCompressEncode::jinit_forward_dct', '0x005C4380', 'LibJpegCompressEncode', 'jinit_forward_dct', LJ),
    ('LibJpegCompressEncode::jpeg_make_c_derived_tbl', '0x005C43F0', 'LibJpegCompressEncode', 'jpeg_make_c_derived_tbl', LJ),
    ('LibJpegCompressEncode::jpeg_gen_optimal_table', '0x005C4D50', 'LibJpegCompressEncode', 'jpeg_gen_optimal_table', LJ),
    ('LibJpegCompressEncode::jinit_huff_encoder', '0x005C5210', 'LibJpegCompressEncode', 'jinit_huff_encoder', LJ),
    ('LibJpegCompressEncode::jinit_compress_master', '0x005C5260', 'LibJpegCompressEncode', 'jinit_compress_master', LJ),
    ('LibJpegCompressEncode::jinit_c_main_controller', '0x005C5420', 'LibJpegCompressEncode', 'jinit_c_main_controller', LJ),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'FxEmitter_c::Init', 'FxEmitter_c::Reset', 'FxEmitter_c::Update'}


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
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
    if not (RUN / 'comparisons.jsonl.gz').exists() and not (RUN / 'comparisons.jsonl.xz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
        # GitHub rechaza ficheros de mas de 100 MB: por encima de ~90 MB se guarda como .xz
        if (RUN / 'comparisons.jsonl.gz').stat().st_size > 90 * 1000 * 1000:
            import lzma
            with open(SRC / 'comparisons.jsonl', 'rb') as f, lzma.open(RUN / 'comparisons.jsonl.xz', 'wb', preset=6) as g:
                shutil.copyfileobj(f, g)
            (RUN / 'comparisons.jsonl.gz').unlink()
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
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if ID in VIRTUAL else ADDR, 'type': 'V' if ID in VIRTUAL else 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], **({'original_address': ADDR} if ID in VIRTUAL else {})},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn230A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    # docs/hooks.csv: validated = 1 (sexto campo) para las funciones aprobadas (sin sync_docs ni hooks2md)
    csvp = ROOT / 'docs/hooks.csv'
    raw = csvp.read_bytes().decode('utf-8')
    ls = raw.splitlines(keepends=True)
    wanted = {(f[2], f[3]): f for f in FUNCS}
    changed = 0
    for i, line in enumerate(ls):
        body = line.rstrip(chr(13) + chr(10))
        eol = line[len(body):]
        cols = body.split(',')
        if len(cols) == 7 and (cols[0], cols[1]) in wanted and cols[5] == '0':
            cols[5] = '1'
            ls[i] = ','.join(cols) + eol
            changed += 1
    csvp.write_bytes(''.join(ls).encode('utf-8'))
    print('docs/hooks.csv: filas puestas a validated=1:', changed, 'de', len(wanted))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
