r"""Revalida con la corrida APROBADA de GangEvent1 del 19/10/2026 (20261019-004051-297047) las 62 funciones registradas del banco (CGangWars y CGroupEventHandler, 62 funciones).

Uso (SOLO tras una corrida aprobada):  python register_revalidation_20261019.py <carpeta-de-evidencia>
  p. ej. python register_revalidation_20261019.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-gangevent1/20261019-004051-297047"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/GangEvent1/validation-summary-reval-20261019.json (archivo APARTE: validation-summary.json
histórico no se sobrescribe) y REEMPLAZA la evidencia vigente de las filas de reversing/validation-registry.json cuya evidencia estaba en este banco; la anterior se
conserva en 'previous_evidence' (si ya había una, queda anidada). approvals.json no se toca. Las funciones se leen del summary de la corrida y se exige que coincidan
exactamente con las del registro. Idempotente.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'GangEvent1'
SCOPE_NOTE = ('REVALIDACIÓN 19/10/2026: re-ejecución completa del banco (17792 pares, 0 diferencias, 62 funciones) con las fuentes actuales (su manifiesto no lista Pickups.cpp/.h; los hashes que registra son los de las fuentes del propio banco). Sonda compilada desde el worktree D:/Original/wq-f (HEAD 98286d43c + fusión de CQuaternion; ver artifacts/build-manifest.json) con las fuentes actuales de Pickups.cpp/.h (CPickups::GenerateNewOne ahora envoltorio de GenerateNewOne_ExeAbi). Los hashes de tested_sources son los del HEAD 98286d43c compilado; el árbol principal tiene además trabajo sin commit del lote 27 (sin validar) que modifica GangWars.h y GroupEventHandler.cpp, por lo que esos archivos no coinciden con el árbol de trabajo actual (el registro los marcará distintos hasta que ese trabajo se confirme o se descarte).')
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
    ppf = {k: int(v) for k, v in s['pairs_per_function'].items()}
    assert sum(ppf.values()) == int(s['pairs']) == 17792 and len(ppf) == 62, (sum(ppf.values()), len(ppf))
    manifest = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
    tested = manifest['sources']
    assert manifest['binaries']['asi'] == s['executed_probe_sha256']
    out = ROOT / 'reversing' / BANK / 'validation-summary-reval-20261019.json'
    outp = out.relative_to(ROOT).as_posix()
    ev = dict(passed=True, pairs=sum(ppf.values()), differences=0, registered_pairs=sum(ppf.values()), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=SCOPE, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              bank_pairs=int(s['pairs']), bank_functions=len(ppf),
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    prefix = 'reversing/' + BANK + '/'
    rows = {r['evidence']['passed_path'][-1]: r for r in data['functions']
            if r['evidence']['path'].startswith(prefix) and r['evidence']['passed_path'][:1] == ['covered_functions']}
    assert set(rows) == set(ppf), (sorted(set(rows) ^ set(ppf)))
    for ID, r in rows.items():
        if r['evidence']['path'] != outp:
            prev = dict(evidence=r['evidence'], review_reference=r['review_reference'])
            if 'previous_evidence' in r:
                prev['previous_evidence'] = r['previous_evidence']
            r['previous_evidence'] = prev
        r['evidence'] = {'path': outp, 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                         'pairs': ppf[ID], 'original_exe_sha256': s['original_exe_sha256']}
        r['tested_sources'] = tested
        r['dependency_review'] = None
        r['source_provenance'] = 'Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.'
        r['scope'] = SCOPE
        r['review_reference'] = f'reversing/{BANK}/README.md'
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
