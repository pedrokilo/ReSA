"""Registra CScriptsForBrains::StartNewStreamedScriptBrain (0x0046B270) con la corrida APROBADA de ScriptsBrainsStart31.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-scriptsbrainsstart31/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: no se ha ejecutado (el banco aun no se ha corrido). Antes de usarla, rellenar SCOPE_NOTE con lo que la corrida demostro de verdad.
Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/ScriptsBrainsStart31/validation-summary.json y anade la entrada a reversing/validation-registry.json y
reversing/CppOnlyMigration/approvals.json. Idempotente. No toca docs/ ni hooks.csv.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK, ADDR, ID, CLS, FN = 'ScriptsBrainsStart31', '0x0046B270', 'CScriptsForBrains::StartNewStreamedScriptBrain', 'CScriptsForBrains', 'StartNewStreamedScriptBrain'
SOURCE = 'source/game_sa/ScriptsForBrains.cpp'
SCOPE_NOTE = 'Fuentes probadas compiladas desde el worktree D:/Original/wm-1 (HEAD 63d8ac6cb + parches del lote 31; ver artifacts/build-manifest.json), de sólo lectura y eliminado tras el registro. En el árbol principal las fuentes se fusionaron en tres vías con trabajo sin validar del lote 27 en los mismos archivos, por lo que sus hashes pueden diferir de los del manifiesto (normalizados a LF o no). La ruta de Pools.cpp de build_probe.py estaba mal en un primer intento de sonda (corregida a source/game_sa/Pools.cpp); ese intento no es evidencia. Reconstruida en C++ (ScriptsForBrains.cpp) con StartNewStreamedScript y RemoveFromWaitingForScriptBrainArray sustituidos por falsos identicos en ambos lados y pools de peds/objetos falsos; EAX/ECX/EDX solo registrados.'
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main(src):
    SRC = Path(src)
    RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + SRC.name)
    RUN.mkdir(parents=True, exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (RUN / n).exists():
            shutil.copyfile(SRC / n, RUN / n)
    if not (RUN / 'comparisons.jsonl.gz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
    s = json.loads((RUN / 'summary.json').read_text(encoding='utf-8'))
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True
    ppf = dict(s['pairs_per_function'])
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
    if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
        data['functions'].append(dict(
            hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': 'S'},
            evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                      'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']},
            tested_sources=tested, dependency_review=None,
            source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
            scope=SCOPE, review_reference=f'reversing/{BANK}/README.md'))
        rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))

    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    if ADDR not in a:
        a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                   'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                   'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                   'state_restored': True, 'scope': SCOPE_NOTE}
        ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
