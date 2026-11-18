"""Registra las 30 funciones del lote 235A con la corrida APROBADA de Fn235A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn235a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn235A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Ademas pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas (sin sync_docs ni hooks2md).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn235A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
JD = 'source/game_sa/Plugins/JPegCompressPlugin/'
FUNCS = (
    ('LibJpegDecompressUpsample::jinit_post_controller', '0x005CCDF0', 'LibJpegDecompressUpsample', 'jinit_post_controller', JD + 'LibJpegDecompressUpsample.cpp'),
    ('LibJpegDecompressUpsample::sep_upsample', '0x005CCE80', 'LibJpegDecompressUpsample', 'sep_upsample', JD + 'LibJpegDecompressUpsample.cpp'),
    ('LibJpegDecompressUpsample::jinit_upsampler', '0x005CD350', 'LibJpegDecompressUpsample', 'jinit_upsampler', JD + 'LibJpegDecompressUpsample.cpp'),
    ('LibJpegError::error_exit', '0x005CD6B0', 'LibJpegError', 'error_exit', JD + 'LibJpegError.cpp'),
    ('LibJpegError::output_message', '0x005CD6D0', 'LibJpegError', 'output_message', JD + 'LibJpegError.cpp'),
    ('LibJpegError::jpeg_std_error', '0x005CD810', 'LibJpegError', 'jpeg_std_error', JD + 'LibJpegError.cpp'),
    ('LibJpegMemory::alloc_small', '0x005CEBC0', 'LibJpegMemory', 'alloc_small', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::alloc_large', '0x005CED10', 'LibJpegMemory', 'alloc_large', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::alloc_sarray', '0x005CEDD0', 'LibJpegMemory', 'alloc_sarray', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::alloc_barray', '0x005CEE80', 'LibJpegMemory', 'alloc_barray', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::request_virt_sarray', '0x005CEF30', 'LibJpegMemory', 'request_virt_sarray', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::request_virt_barray', '0x005CEFA0', 'LibJpegMemory', 'request_virt_barray', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::realize_virt_arrays', '0x005CF010', 'LibJpegMemory', 'realize_virt_arrays', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::free_pool', '0x005CF580', 'LibJpegMemory', 'free_pool', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jinit_memory_mgr', '0x005CF6D0', 'LibJpegMemory', 'jinit_memory_mgr', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_get_small', '0x005CF800', 'LibJpegMemory', 'jpeg_get_small', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_free_small', '0x005CF810', 'LibJpegMemory', 'jpeg_free_small', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_get_large', '0x005CF820', 'LibJpegMemory', 'jpeg_get_large', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_free_large', '0x005CF830', 'LibJpegMemory', 'jpeg_free_large', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_mem_available', '0x005CF840', 'LibJpegMemory', 'jpeg_mem_available', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_open_backing_store', '0x005CF850', 'LibJpegMemory', 'jpeg_open_backing_store', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_mem_init', '0x005CF870', 'LibJpegMemory', 'jpeg_mem_init', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jpeg_mem_term', '0x005CF880', 'LibJpegMemory', 'jpeg_mem_term', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jdiv_round_up', '0x005CF890', 'LibJpegMemory', 'jdiv_round_up', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jround_up', '0x005CF8A0', 'LibJpegMemory', 'jround_up', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jcopy_sample_rows', '0x005CF8C0', 'LibJpegMemory', 'jcopy_sample_rows', JD + 'LibJpegMemory.cpp'),
    ('LibJpegMemory::jzero_far', '0x005CF940', 'LibJpegMemory', 'jzero_far', JD + 'LibJpegMemory.cpp'),
    ('LibJpegCompressApi::jpeg_CreateCompress', '0x005CF960', 'LibJpegCompressApi', 'jpeg_CreateCompress', JD + 'LibJpegCompressApi.cpp'),
    ('LibJpegCompressApi::jpeg_suppress_tables', '0x005CFA50', 'LibJpegCompressApi', 'jpeg_suppress_tables', JD + 'LibJpegCompressApi.cpp'),
    ('LibJpegCompressApi::jpeg_finish_compress', '0x005CFAC0', 'LibJpegCompressApi', 'jpeg_finish_compress', JD + 'LibJpegCompressApi.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S (ninguno V en este lote)
VIRTUAL = set()


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn235A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
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
