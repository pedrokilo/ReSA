"""Registra las funciones del lote 102A (constructor de FxPrtMult_c, dos de CSimpleTransform, SquaredMagnitudeExe, destructor de CAESound y cuatro de RenderWare) con la corrida APROBADA de Fn102A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn102a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/ (comparisons.jsonl.gz, o .xz si el gz supera 90 MB: GitHub rechaza ficheros de mas de 100 MB), escribe reversing/Fn102A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, lzma, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn102A'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('FxPrtMult_c::Constructor7f', '0x004AB290', 'FxPrtMult_c', 'Constructor7f', 'source/game_sa/Fx/FxPrtMult.cpp'),
    ('CSimpleTransform::UpdateMatrix', '0x0054F1B0', 'CSimpleTransform', 'UpdateMatrix', 'source/game_sa/SimpleTransform.cpp'),
    ('CSimpleTransform::UpdateRwMatrix', '0x0054EF40', 'CSimpleTransform', 'UpdateRwMatrix', 'source/game_sa/SimpleTransform.cpp'),
    ('CVector::SquaredMagnitudeExe', '0x00406DA0', 'CVector', 'SquaredMagnitudeExe', 'source/game_sa/Core/Vector.cpp'),
    ('CAESound::Destructor', '0x004EF660', 'CAESound', 'Destructor', 'source/game_sa/Audio/AESound.cpp'),
    ('RpWorld::RpClumpForAllAtomics', '0x00749B70', 'RpWorld', 'RpClumpForAllAtomics', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpGeometryForAllMaterials', '0x0074C790', 'RpWorld', 'RpGeometryForAllMaterials', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::RpAtomicSetFrame', '0x0074BF20', 'RpWorld', 'RpAtomicSetFrame', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
    ('RpWorld::AtomicDefaultRenderCallBack', '0x007491C0', 'RpWorld', 'AtomicDefaultRenderCallBack', 'source/game_sa/RenderWare/rw/rpworld.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en un worktree sobre main e9a8cb895 con los cambios del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y, cuando tienen hook RH, en su cuerpo del ASI: '
              'CMatrix::SetTranslate 0x59AF40 y CMatrix::SetRotateZOnly 0x59B020 (UpdateMatrix), CEntity::CleanUpOldReference 0x571A00 (destructor de CAESound), _rwObjectHasFrameSetFrame 0x804EF0 (RpAtomicSetFrame; el falso deja un byte conocido en +3 del atomico) y RxPipelineExecute 0x805710 (AtomicDefaultRenderCallBack); '
              'los callbacks de RpClumpForAllAtomics y RpGeometryForAllMaterials son funciones del banco. Corren de verdad en ambos lados: el recorrido de la lista y del arreglo, las escrituras, fcos/fsin de UpdateRwMatrix (la reduccion de argumentos grandes la hace la propia x87), RwMatrixUpdate 0x7F18A0, '
              'la suma de cuadrados x87 de SquaredMagnitudeExe (Vector.cpp se compila con /O2 /Oi /arch:IA32 /fp:fast; ECX y EDX se comprueban intactos) y la lectura de la tuberia por defecto del motor en memoria real. '
              'Cada lado coincide ademas con el oraculo independiente fixture_oracle.py (modelo exacto de las listas, racional a 24 bits y mpmath para fsin/fcos). '
              'Fuera de los fixtures: rumbos entre 1e6 y 2^63 (la reduccion con pi de 66 bits de la x87 no se modela), NaN con cargas distintas de 0xFFC00000 en SquaredMagnitudeExe, clumps y geometrias con listas corruptas o arreglos de mas de 8 materiales, y la exactitud de los colaboradores sustituidos.')
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
        if (RUN / 'comparisons.jsonl.gz').stat().st_size > 90 * 1000 * 1000:   # GitHub rechaza ficheros de mas de 100 MB: se guarda como .xz
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn100A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
