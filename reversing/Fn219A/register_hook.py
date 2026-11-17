"""Registra las 30 funciones del lote 219A con la corrida APROBADA de Fn219A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn173a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn219A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Ademas pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas (sin sync_docs ni hooks2md).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn219A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('ListItem_c::Destructor', '0x004A8DC0', 'ListItem_c', 'Destructor', 'source/game_sa/Core/ListItem_c.cpp'),
    ('TList_c::ListConstructorExe', '0x004A8DD0', 'TList_c', 'ListConstructorExe', 'source/game_sa/Core/List_c.cpp'),
    ('TList_c::ListDestructorExe', '0x004A8DE0', 'TList_c', 'ListDestructorExe', 'source/game_sa/Core/List_c.cpp'),
    ('TList_c::ListRemoveItemExe', '0x004A8E30', 'TList_c', 'ListRemoveItemExe', 'source/game_sa/Core/List_c.cpp'),
    ('TList_c::ListRemoveHeadExe', '0x004A8E70', 'TList_c', 'ListRemoveHeadExe', 'source/game_sa/Core/List_c.cpp'),
    ('TList_c::ListGetItemOffsetExe', '0x004A9010', 'TList_c', 'ListGetItemOffsetExe', 'source/game_sa/Core/List_c.cpp'),
    ('FxFrustumInfo_c::Constructor', '0x004A9090', 'FxFrustumInfo_c', 'Constructor', 'source/game_sa/Fx/FxFrustumInfo.cpp'),
    ('CPedAttractor::SArrayTidyStride4D', '0x005EC040', 'CPedAttractor', 'SArrayTidyStride4D', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4E', '0x005EC0B0', 'CPedAttractor', 'SArrayTidyStride4E', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4F', '0x005EC180', 'CPedAttractor', 'SArrayTidyStride4F', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4G', '0x005EC1F0', 'CPedAttractor', 'SArrayTidyStride4G', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4H', '0x005EC260', 'CPedAttractor', 'SArrayTidyStride4H', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4I', '0x005EC2D0', 'CPedAttractor', 'SArrayTidyStride4I', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4J', '0x005EC340', 'CPedAttractor', 'SArrayTidyStride4J', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4K', '0x005EC3B0', 'CPedAttractor', 'SArrayTidyStride4K', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayTidyStride4L', '0x005EC3E0', 'CPedAttractor', 'SArrayTidyStride4L', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayFillNReturnEndStride4B', '0x005EC070', 'CPedAttractor', 'SArrayFillNReturnEndStride4B', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayFillNReturnEndStride20', '0x005EC0E0', 'CPedAttractor', 'SArrayFillNReturnEndStride20', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayFillNReturnEndStride4C', '0x005EC110', 'CPedAttractor', 'SArrayFillNReturnEndStride4C', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayFillNReturnEndStride12', '0x005EC140', 'CPedAttractor', 'SArrayFillNReturnEndStride12', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayFillNReturnEndStride4D', '0x005EC1B0', 'CPedAttractor', 'SArrayFillNReturnEndStride4D', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayEraseStride4', '0x005EC220', 'CPedAttractor', 'SArrayEraseStride4', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('LegacyRuntimeError::VectorXlenB', '0x005ECAB0', 'LegacyRuntimeError', 'VectorXlenB', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp'),
    ('LegacyRuntimeError::VectorXlenC', '0x005ECB30', 'LegacyRuntimeError', 'VectorXlenC', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp'),
    ('LegacyRuntimeError::VectorXlenD', '0x005ECBB0', 'LegacyRuntimeError', 'VectorXlenD', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp'),
    ('LegacyRuntimeError::VectorXlenE', '0x005ECC30', 'LegacyRuntimeError', 'VectorXlenE', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp'),
    ('LegacyRuntimeError::VectorXlenF', '0x005ECCB0', 'LegacyRuntimeError', 'VectorXlenF', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp'),
    ('CPedAttractor::SArrayInsertNStride4A', '0x005ECF10', 'CPedAttractor', 'SArrayInsertNStride4A', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayInsertNStride20', '0x005ED1B0', 'CPedAttractor', 'SArrayInsertNStride20', 'source/game_sa/Attractors/PedAttractor.cpp'),
    ('CPedAttractor::SArrayInsertNStride4B', '0x005ED4B0', 'CPedAttractor', 'SArrayInsertNStride4B', 'source/game_sa/Attractors/PedAttractor.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn219A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
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
