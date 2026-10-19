"""Registra CPickups::GenerateNewOne (0x00456F20) con la corrida APROBADA de PickupsGenNew28.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:\\JUEGOS\\GTA San Andreas - CVehicle Test\\resultados-pickupsgen28\\<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PickupsGenNew28/validation-summary.json y anade las entradas a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador).
Antes de ejecutarlo hay que rellenar SCOPE_NOTE con el desajuste de fuentes probadas que haya (si lo hay), como hizo DecisionAdd27.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PickupsGenNew28'
FUNCS = (
    # (id del banco, direccion, clase, funcion (nombre del hook RH), fuente)
    ('CPickups::GenerateNewOne', '0x00456F20', 'CPickups', 'GenerateNewOne', 'source/game_sa/Pickups.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas desde el worktree D:/Original/gta-wt-b28 (HEAD de6ed4a4d + source.patch de cada banco + tres arreglos posteriores; ver artifacts/build-manifest.json); idénticas al árbol principal en el momento del registro. El hook se llama GenerateNewOne (cuerpo GenerateNewOne_ExeAbi, con la ABI del EXE: argumentos en pila y retirados por el llamador). Colaboradores sustituidos por falsos idénticos: FindTextIndexForString, IsVisible, GiveUsAPickUpObject, CWorld::Remove, CWorld::Add y el destructor virtual; las conversiones float->int64 de CPickup::SetPosn y _ftol2 son las reales. Los bancos históricos que sustituían 0x456F20 con un falso (PickWeap1, CTaskSimpleJetPack, FileLoader47, GangEvent1, GangWarsPending, InteriorLifecycleRuntime) ya no interceptan la llamada de la DLL y deben revalidarse.')
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauracion verificadas. '
         'Evidencia limitada a los fixtures del banco, no integracion universal ni independencia transitiva del EXE. Alcance por funcion en README del banco. '
         'Colaboradores sustituidos por falsos identicos en ambos lados: CPickup::FindTextIndexForString, CPickup::IsVisible, CPickup::GiveUsAPickUpObject, CWorld::Remove, CWorld::Add y el '
         'destructor virtual del objeto; no se valida aqui su cuerpo.')


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
                       'scope': ('Reconstruida en C++ (CPickups::GenerateNewOne_ExeAbi, hook GenerateNewOne): busqueda de hueco (libre, dinero, tiempo limite), relleno de la ranura, '
                                 'coordenadas con la conversion float->int64 del compilador (equivalente al _ftol2 0x821B40 del EXE, que el original sigue ejecutando). Banco PickupsGenNew28 con '
                                 + str(int(s['pairs'])) + ' pares y cero diferencias; solo EAX se compara (ECX/EDX sólo registrados porque ningún llamador los lee). '
                                 'Colaboradores sustituidos por falsos idénticos. Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
