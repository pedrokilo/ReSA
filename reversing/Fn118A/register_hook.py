"""Registra las funciones del lote 118A (nueve funciones: eventos, pools y sueltas) con la corrida APROBADA de Fn118A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn118a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn118A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn118A'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CMemoryMgr::Malloc', '0x0072F420', 'CMemoryMgr', 'Malloc-SizeHint', 'source/game_sa/MemoryMgr.cpp'),
    ('CMemoryMgr::Realloc', '0x0072F440', 'CMemoryMgr', 'Realloc-MemorySizeHint', 'source/game_sa/MemoryMgr.cpp'),
    ('CMemoryMgr::Calloc', '0x0072F460', 'CMemoryMgr', 'Calloc-NumSizeHint', 'source/game_sa/MemoryMgr.cpp'),
    ('CMemoryMgr::MallocAlign', '0x0072F4C0', 'CMemoryMgr', 'MallocAlign-SizeAlignHint', 'source/game_sa/MemoryMgr.cpp'),
    ('CMemoryMgr::FreeAlign', '0x0072F4F0', 'CMemoryMgr', 'FreeAlign', 'source/game_sa/MemoryMgr.cpp'),
    ('CDecision::CDecision', '0x006040C0', 'CDecision', 'Constructor', 'source/game_sa/DecisionMakers/Decision.cpp'),
    ('CTaskComplexLeaveCar::ComputeTargetDoor', '0x0063BAB0', 'CTaskComplexLeaveCar', 'ComputeTargetDoor', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp'),
    ('CWaterLevel::CalculateWavesOnlyForCoordinate2', '0x006E8550', 'CWaterLevel', 'CalculateWavesOnlyForCoordinate2', 'source/game_sa/WaterLevel.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en un worktree sobre main e6e53b1125 con los cambios del lote (ver artifacts/build-manifest.json). Los cuatro CMemoryMgr (Malloc, Realloc, MallocAlign, FreeAlign) sustituyen la evidencia anterior de '
              'DependenciesMemory9 (premisa obsoleta: la DLL ya no es un reenvio al CRT; desde 100A es C++ real). Colaboradores sustituidos por falsos en la direccion del EXE: malloc 0x824257, realloc 0x824269, calloc 0x824416 y free 0x82413F del CRT del EXE '
              '(con trampolin para hilos ajenos), y por falsos identicos en la direccion del EXE y en su cuerpo del ASI (tienen hook RH): CCarEnterExit::ComputeTargetDoorToExit 0x64F110 y CWaterLevel::CalculateWavesOnlyForCoordinate2Native 0x6E7210. '
              'Corre de verdad en ambos lados CDecision::SetDefault 0x600530 (EXE: sus bytes; DLL: su cuerpo). Cada lado coincide ademas con el oraculo independiente fixture_oracle.py. '
              'Fuera de los fixtures: base nula o cruda y alineamientos 0 o mayores de 0x400 en MallocAlign, punteros no mapeados en FreeAlign, el comportamiento real del CRT (memoria devuelta por malloc, realloc, calloc y free) y la exactitud de los colaboradores sustituidos.')
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
        entry = dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md')
        # las entradas existentes (p. ej. las cuatro de CMemoryMgr con DependenciesMemory9) se REEMPLAZAN en su sitio; las nuevas se anaden al final
        idx = [i for i, r in enumerate(data['functions']) if r['hook']['class'] == CLS and r['hook']['fn_name'] == FN]
        assert len(idx) <= 1, (CLS, FN, idx)
        if idx:
            data['functions'][idx[0]] = entry
        else:
            data['functions'].append(entry)
        if True:
            a[ADDR] = {'identity': CLS + '::' + FN, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn118A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
