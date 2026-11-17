"""Registra las funciones APROBADAS del lote 208A con la corrida APROBADA de Fn208A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn208a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn208A/validation-summary.json, anade las entradas a reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y a
reversing/CppOnlyMigration/approvals.json, y pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas. Idempotente. NO ejecuta sync_docs / hooks2md.
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn208A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CPhoneInfo::PhoneAnimFinishCallback', '0x00453F40', 'CPhoneInfo', 'PhoneAnimFinishCallback', 'source/game_sa/PhoneInfo.cpp'),
    ('CPhoneInfo::Constructor', '0x004541E0', 'CPhoneInfo', 'Constructor', 'source/game_sa/PhoneInfo.cpp'),
    ('CPickup::IsWeaponSlotExchangeable', '0x00454920', 'CPickup', 'IsWeaponSlotExchangeable', 'source/game_sa/Pickup.cpp'),
    ('SurfaceInfos_c::LoadSurfaceInfos', '0x0055EB90', 'SurfaceInfos_c', 'LoadSurfaceInfos', 'source/game_sa/SurfaceInfos_c.cpp'),
    ('SurfaceInfos_c::LoadSurfaceAudioInfos', '0x0055F2B0', 'SurfaceInfos_c', 'LoadSurfaceAudioInfos', 'source/game_sa/SurfaceInfos_c.cpp'),
    ('CZoneDef::IsPointWithin', '0x0072D850', 'CZoneDef', 'IsPointWithin', 'source/game_sa/CullZones.cpp'),
    ('RpClumpGtaStreamReadAtOnce', '0x0072E3A0', 'RwHelper', 'RpClumpGtaStreamReadAtOnce', 'source/game_sa/RenderWare/rw/rwtexdict.cpp'),
    ('CMemoryHeap::JoinAdjacentFreeBlocks', '0x0072E870', 'CMemoryHeap', 'JoinAdjacentFreeBlocks', 'source/game_sa/Memory/MemoryHeap.cpp'),
    ('CMemoryHeap::Shutdown', '0x0072EC60', 'CMemoryHeap', 'Shutdown', 'source/game_sa/Memory/MemoryHeap.cpp'),
    ('CMemoryHeap::FillInBlockData', '0x0072EC80', 'CMemoryHeap', 'FillInBlockData', 'source/game_sa/Memory/MemoryHeap.cpp'),
    ('CMemoryHeap::Malloc', '0x0072ED80', 'CMemoryHeap', 'Malloc', 'source/game_sa/Memory/MemoryHeap.cpp'),
    ('CMemoryHeap::Init', '0x0072F180', 'CMemoryHeap', 'Init', 'source/game_sa/Memory/MemoryHeap.cpp'),
    ('CMemoryMgr::Init', '0x0072F3B0', 'CMemoryMgr', 'Init', 'source/game_sa/MemoryMgr.cpp'),
    ('UserTracksUninitCopy', '0x004F3940', 'CAEUserRadioTrackManager', 'UserTracksUninitCopy', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp'),
    ('UserTracksLegacyString::Reallocate', '0x004F39C0', 'UserTracksLegacyString', 'Reallocate', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn208A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    # docs/hooks.csv: validated=1 (sexto campo) en las filas de las funciones aprobadas (solo las de ppf)
    csv_path = ROOT / 'docs' / 'hooks.csv'
    raw = csv_path.read_bytes().decode('utf-8')
    eol = '\r\n' if '\r\n' in raw else '\n'
    rows = raw.split(eol)
    approved = {(cls, fn) for ID, ADDR, cls, fn, SOURCE in FUNCS if ID in ppf}
    done = 0
    for n, row in enumerate(rows):
        f = row.split(',')
        if len(f) == 7 and (f[0], f[1]) in approved and f[2] in {x[1] for x in FUNCS}:
            if f[5] != '1':
                f[5] = '1'
                rows[n] = ','.join(f)
            done += 1
    assert done == len(approved), (done, len(approved))
    csv_path.write_bytes(eol.join(rows).encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
