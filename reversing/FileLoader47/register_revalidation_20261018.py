r"""Revalida CFileLoader::LoadPickup (0x005B47B0) con la corrida APROBADA y aislada (FILELOADER47_ONLY) de FileLoader47 del 18/10/2026.

Uso (SOLO tras una corrida aprobada):  python register_revalidation_20261018.py <carpeta-de-evidencia>
Escribe reversing/FileLoader47/validation-summary-reval-20261018.json (archivo APARTE; validation-summary.json sigue siendo la evidencia de las otras 46 funciones)
y REEMPLAZA la evidencia vigente de la fila de LoadPickup en reversing/validation-registry.json, conservando la anterior en 'previous_evidence'. approvals.json no tiene entrada. Idempotente.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'FileLoader47'
FUNCS = (
    ('CFileLoader::LoadPickup', '0x005B47B0', 'CFileLoader', 'LoadPickup'),
)
SCOPE_NOTE = ('REVALIDACIÓN 18/10/2026, corrida aislada con FILELOADER47_ONLY=CFileLoader::LoadPickup (256 pares). Sonda compilada desde el worktree D:/Original/gta-wt-b29 (HEAD 29305c78d; ver artifacts/build-manifest.json); fuentes idénticas al árbol principal. '
              'Cambios del banco: pPickup se parchea también en Body(&CPickups::GenerateNewOne_ExeAbi) (GenerateNewOne es ahora un envoltorio) y l2dfx también en Body(&CFileLoader::Load2dEffect) (ahora C++ enganchado). '
              'Las otras 46 funciones del banco NO se han repetido con las fuentes actuales (FileLoader.cpp cambió): conservan la evidencia anterior y quedan pendientes de la corrida completa (11968 pares).')
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
    ppf_all = dict(s['pairs_per_function'])
    assert all(f[0] in ppf_all for f in FUNCS), ppf_all
    ppf = {f[0]: int(ppf_all[f[0]]) for f in FUNCS}
    assert ppf == {'CFileLoader::LoadPickup': 256} and set(ppf_all) == set(ppf), ppf_all
    tested = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))['sources']
    assert json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))['binaries']['asi'] == s['executed_probe_sha256']
    ev = dict(passed=True, pairs=sum(ppf.values()), differences=0, registered_pairs=sum(ppf.values()), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=SCOPE, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              bank_pairs=int(s['pairs']), bank_functions=len(ppf_all),
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary-reval-20261018.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN in FUNCS:
        r = next(r for r in data['functions'] if r['hook']['class'] == CLS and r['hook']['fn_name'] == FN)
        if r['evidence']['path'] != out.relative_to(ROOT).as_posix():
            r['previous_evidence'] = dict(evidence=r['evidence'], review_reference=r['review_reference'])
        r['evidence'] = {'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
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
