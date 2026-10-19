r"""Revalida CPickups::CreateSomeMoney (0x00458970) y CPickups::GenerateNewOne_WeaponType (0x00457380) con la corrida APROBADA de PickWeap1 del 18/10/2026.

Uso (SOLO tras una corrida aprobada):  python register_revalidation_20261018.py <carpeta-de-evidencia>
  p. ej. python register_revalidation_20261018.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-pickweap1/20261018-221822-133087"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PickWeap1/validation-summary-reval-20261018.json (archivo APARTE: validation-summary.json
sigue siendo la evidencia de las otras 58 funciones y su sha256 no debe cambiar) y REEMPLAZA la evidencia vigente de las dos filas de reversing/validation-registry.json;
la anterior se conserva en 'previous_evidence'. approvals.json no se toca (0x00458970 pertenece a PickupsPending; 0x00457380 no tiene entrada). Idempotente.
Las otras 58 funciones NO se actualizan: el summary de la corrida sólo da passed/all_equal globales y pares por función, no resultado por función.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PickWeap1'
FUNCS = (
    ('CPickups::CreateSomeMoney', '0x00458970', 'CPickups', 'CreateSomeMoney'),
    ('CPickups::GenerateNewOne_WeaponType', '0x00457380', 'CPickups', 'GenerateNewOne_WeaponType'),
)
SCOPE_NOTE = ('REVALIDACIÓN 18/10/2026. Sonda compilada desde el worktree D:/Original/gta-wt-b29 (HEAD 29305c78d; ver artifacts/build-manifest.json); fuentes del banco idénticas al árbol principal '
              '(Pickups.cpp/.h, Weapon.cpp) salvo Weapon.h, que sólo difiere en finales de línea (CRLF frente a LF). CPickups::GenerateNewOne es ahora un envoltorio de GenerateNewOne_ExeAbi: '
              'el banco parchea el falso de GenerateNewOne en 0x456F20 y en Body(&CPickups::GenerateNewOne_ExeAbi), y activa el hook de GenerateNewOne sólo en el lado candidato (compuerta gen_route_ok). '
              'La corrida completa ejecutó las 60 funciones del banco (11020 pares, todo igual), pero su summary sólo da el resultado global y los pares por función: sólo CreateSomeMoney (192) y '
              'GenerateNewOne_WeaponType (128) pasan a esta evidencia; las otras 58 conservan la evidencia anterior (corrida 20261009-162447-757920).')
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
    assert ppf == {'CPickups::CreateSomeMoney': 192, 'CPickups::GenerateNewOne_WeaponType': 128}, ppf
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
