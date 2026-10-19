"""Registra CObject::CObject(int32, bool) (0x005A1D70) con la corrida APROBADA de ObjectCtor28.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):
  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:\\JUEGOS\\GTA San Andreas - CVehicle Test\\resultados-objectctor28\\<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/ObjectCtor28/validation-summary.json y anade las entradas a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv `CObject,Constructor,0x005A1D70,1,0,1,S`,
si falta, la anade el coordinador). Antes de ejecutarlo hay que rellenar SCOPE_NOTE con el desajuste de fuentes probadas que haya (si lo hay), como hizo DecisionMakeDecision27.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'ObjectCtor28'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CObject::CObject', '0x005A1D70', 'CObject', 'Constructor', 'source/game_sa/Entity/Object/Object.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas desde el worktree D:/Original/gta-wt-b28 (HEAD de6ed4a4d + source.patch de cada banco + tres arreglos posteriores; ver artifacts/build-manifest.json); idénticas al árbol principal en el momento del registro salvo los finales de línea de Population.cpp (CRLF en el árbol principal, LF en el worktree; mismo contenido). Population.cpp:1979 ya llama al constructor C++. La corrida valida con los cuatro colaboradores sustituidos; ECX/EDX sólo registrados (EAX = this sí se exige). Los valores de bCreate no canónicos (2/3) ejercitan sólo el byte bajo.')
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Colaboradores sustituidos por falsos idénticos en ambos lados (CPhysical::CPhysical 0x542260, CEntity::SetModelIndex 0x532AE0, CEntity::SetModelIndexNoCreate 0x533700, CObject::Init 0x59F840): '
         'el banco valida el cuerpo propio del constructor (orden de llamadas, argumentos, selección por el byte bajo de bCreate, vptr y m_pDummyObject), no a los colaboradores. '
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
                       'scope': ('Constructor CObject(int32, bool) reconstruido en C++ (envoltura Constructor del hook + constructor de la DLL; la envoltura Population.cpp:1979 ya llama al constructor C++). '
                                 'Banco ObjectCtor28 con ' + str(int(s['pairs'])) + ' pares y cero diferencias, con los cuatro colaboradores sustituidos por falsos idénticos; '
                                 'ECX/EDX sólo registrados porque ningún llamador los lee (EAX = this sí se exige). Sin cobertura exhaustiva más allá de los fixtures del banco.'
                                 + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
