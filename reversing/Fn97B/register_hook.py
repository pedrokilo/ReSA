"""Registra las funciones del lote 97A (seis funciones de CMatrix: SetRotateZOnly, UpdateRwMatrix, UpdateRW, destructor y los dos constructores) con la corrida APROBADA de Fn97B.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn97b/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn97B/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn97B'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CMatrix::SetRotateZOnly', '0x0059B020', 'CMatrix', 'SetRotateZOnly', 'source/game_sa/Core/Matrix.cpp'),
    ('CMatrix::UpdateRwMatrix', '0x0059AD70', 'CMatrix', 'UpdateRwMatrix', 'source/game_sa/Core/Matrix.cpp'),
    ('CMatrix::UpdateRW', '0x0059BBB0', 'CMatrix', 'UpdateRW', 'source/game_sa/Core/Matrix.cpp'),
    ('CMatrix::Destructor', '0x0059ACD0', 'CMatrix', 'Destructor', 'source/game_sa/Core/Matrix.cpp'),
    ('CMatrix::ConstructCopy', '0x0059BCF0', 'CMatrix', 'ConstructCopy', 'source/game_sa/Core/Matrix.cpp'),
    ('CMatrix::ConstructAttached', '0x0059C050', 'CMatrix', 'ConstructAttached', 'source/game_sa/Core/Matrix.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en un worktree sobre main dca501f0c con los cambios del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en ambos lados: RwMatrixUpdate 0x7F18A0 (UpdateRwMatrix), UpdateRwMatrix 0x59AD70 con hook RH (UpdateRW), RwMatrixDestroy 0x7F2A20 (Destructor) y Attach 0x59BD10 con hook RH (ConstructAttached); registran sus argumentos en orden. SetRotateZOnly y ConstructCopy no tienen colaboradores (fsin/fcos del x87 reales en ambos lados). Ventana de memoria con punteros a la ventana, nulos y crudos, y la bandera de propiedad como dword (el relleno cuenta). Ademas de bytes escritos y eventos se compara EAX de salida (this) en los constructores. Cada lado coincide con el oraculo independiente fixture_oracle.py (seno/coseno exactos con enteros de 400 bits). Fuera de los fixtures: angulos con modulo entre 2^19 y 2^62 (reduccion del x87 con el pi de 66 bits), bits altos de la bandera de Attach (los llamadores del EXE empujan 0 o 1) y excepciones x87 desenmascaradas.')
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
                       'scope': ('Reconstruida en C++ (colaboradores sustituidos por falsos identicos en ambos lados; ver la nota de alcance). Banco Fn97B con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
