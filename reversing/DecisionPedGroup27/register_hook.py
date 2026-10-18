r"""Registra CDecisionMakerTypes::MakeDecision-Ped (0x00606E70) y -Group (0x00606F80) con la corrida APROBADA de DecisionPedGroup27 (18/10/2026).

Uso (SOLO tras una corrida aprobada):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-decisionpedgroup27\<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/DecisionPedGroup27/validation-summary.json y REEMPLAZA la evidencia vigente de las dos
filas (ya existentes) en reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json; la evidencia anterior (DependenciesDecisionPed7) se conserva
en el campo 'previous_evidence' / 'previous'. Idempotente.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'DecisionPedGroup27'
FUNCS = (
    ('CDecisionMakerTypes::MakeDecision-Ped', '0x00606E70', 'CDecisionMakerTypes', 'MakeDecision-Ped'),
    ('CDecisionMakerTypes::MakeDecision-Group', '0x00606F80', 'CDecisionMakerTypes', 'MakeDecision-Group'),
)
SCOPE_NOTE = ('Fuentes probadas (Decision.cpp, Decision.h, DecisionMakerTypes.cpp y .h, DecisionMaker.cpp, X87Math) compiladas desde el worktree D:/Original/gta-wt-dm2 (HEAD 7a759750f); '
              'idénticas byte a byte a las del árbol principal en el momento del registro. CDecision::MakeDecision (0x6040D0) corre en C++ (DLL) en el lado candidato.')
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco. ' + SCOPE_NOTE)


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
    ev = dict(passed=True, pairs=int(s['pairs']), differences=0, registered_pairs=int(s['pairs']), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=SCOPE, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN in FUNCS:
        r = next(r for r in data['functions'] if r['hook']['class'] == CLS and r['hook']['fn_name'] == FN)
        if r['evidence']['path'] != out.relative_to(ROOT).as_posix():
            r['previous_evidence'] = dict(evidence=r['evidence'], review_reference=r['review_reference'])
        r['evidence'] = {'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                         'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']}
        r['tested_sources'] = tested
        r['dependency_review'] = None
        r['source_provenance'] = 'Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.'
        r['scope'] = SCOPE
        r['review_reference'] = f'reversing/{BANK}/README.md'
        e = a[ADDR]
        if e['bank'] != BANK:
            e['previous'] = {k: e[k] for k in ('bank', 'run', 'pairs', 'own_pairs', 'audit', 'scope') if k in e}
        e.update({'bank': BANK, 'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                  'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0, 'state_restored': True,
                  'scope': ('Banco DecisionPedGroup27 (5120 pares, 2560 por función, cero diferencias) con CDecision::MakeDecision (0x6040D0) ya en C++ como colaborador del lado candidato; '
                            'EAX/ECX/EDX sólo registrados salvo el EAX de retorno de Group. Sin cobertura exhaustiva más allá de los fixtures del banco. ' + SCOPE_NOTE)})
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
