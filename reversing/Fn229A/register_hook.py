"""Registra las 23 funciones del lote 229A con la corrida APROBADA de Fn229A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn218a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn229A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json, y pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas. Idempotente. NO ejecuta sync_docs ni hooks2md.
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn229A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('RpWorld::RpMaterialStreamGetSize', '0x0074E010', 'RpWorld', 'RpMaterialStreamGetSize', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpMaterialListDeinitialize', '0x0074E150', 'RpWorld', '_rpMaterialListDeinitialize', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpMaterialListAppendMaterial', '0x0074E350', 'RpWorld', '_rpMaterialListAppendMaterial', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpMaterialListStreamGetSize', '0x0074E450', 'RpWorld', '_rpMaterialListStreamGetSize', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpMaterialListStreamRead', '0x0074E600', 'RpWorld', '_rpMaterialListStreamRead', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9AddDynamicGeometry', '0x0074E970', 'RpWorld', '_rpD3D9AddDynamicGeometry', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9RemoveDynamicGeometry', '0x0074E9E0', 'RpWorld', '_rpD3D9RemoveDynamicGeometry', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9PluginAttach', '0x0074EA40', 'RpWorld', '_rpD3D9PluginAttach', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9MaterialPipelineOpen', '0x0074EAB0', 'RpWorld', '_rpD3D9MaterialPipelineOpen', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9MaterialPipelineClose', '0x0074EAC0', 'RpWorld', '_rpD3D9MaterialPipelineClose', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9WorldSectorPipelineOpen', '0x0074EAD0', 'RpWorld', '_rpD3D9WorldSectorPipelineOpen', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9WorldSectorPipelineClose', '0x0074EB40', 'RpWorld', '_rpD3D9WorldSectorPipelineClose', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9AtomicPipelineOpen', '0x0074EB80', 'RpWorld', '_rpD3D9AtomicPipelineOpen', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpD3D9AtomicPipelineClose', '0x0074EC00', 'RpWorld', '_rpD3D9AtomicPipelineClose', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpWorldSectorDeinstanceAll', '0x0074ECA0', 'RpWorld', '_rpWorldSectorDeinstanceAll', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpWorldSectorDestroyRecurse', '0x0074ED50', 'RpWorld', '_rpWorldSectorDestroyRecurse', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::_rpSectorDefaultRenderCallBack', '0x0074EEC0', 'RpWorld', '_rpSectorDefaultRenderCallBack', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpWorldUnlock', '0x0074F210', 'RpWorld', 'RpWorldUnlock', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpWorldSectorGetWorld', '0x0074F4E0', 'RpWorld', 'RpWorldSectorGetWorld', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpWorldDestroy', '0x0074F610', 'RpWorld', 'RpWorldDestroy', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpWorldCreate', '0x0074F760', 'RpWorld', 'RpWorldCreate', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpWorldForAllMaterials', '0x0074FBC0', 'RpWorld', 'RpWorldForAllMaterials', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpWorldForAllWorldSectors', '0x0074FC70', 'RpWorld', 'RpWorldForAllWorldSectors', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = set()


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def mark_validated():
    """pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones de este lote (clase, nombre y direccion, o 0x00000000 si es V); conserva el resto del fichero tal cual"""
    path = ROOT / 'docs/hooks.csv'
    lines = path.read_bytes().decode('utf-8').split('\n')
    wanted = {(c, n, '0x00000000' if i in VIRTUAL else a) for i, a, c, n, _ in FUNCS}
    done = set()
    for k, line in enumerate(lines):
        f = line.split(',')
        if len(f) == 7 and (f[0], f[1], f[2]) in wanted:
            f[5] = '1'
            lines[k] = ','.join(f)
            done.add((f[0], f[1], f[2]))
    if done != wanted:
        raise SystemExit('Filas de docs/hooks.csv sin encontrar: %s' % sorted(wanted - done))
    path.write_bytes('\n'.join(lines).encode('utf-8'))


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn229A (F229A) con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    mark_validated()
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
