r"""Revalida CTaskSimpleJetPack::DropJetPack (0x0067B660) con la corrida APROBADA de CTaskSimpleJetPack del 18/10/2026.

Uso (SOLO tras una corrida aprobada):  python register_revalidation_20261018.py <carpeta-de-evidencia>
  p. ej. python register_revalidation_20261018.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-jetpack/20261018-222926-929586"

Escribe reversing/CTaskSimpleJetPack/validation-summary-reval-20261018.json (archivo APARTE) y REEMPLAZA la evidencia vigente de la fila de DropJetPack en
reversing/validation-registry.json, conservando la anterior en 'previous_evidence'. approvals.json no tiene entrada. Idempotente.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'CTaskSimpleJetPack'
ID, ADDR, CLS, FN = 'CTaskSimpleJetPack::DropJetPack', '0x0067B660', 'CTaskSimpleJetPack', 'DropJetPack'
SCOPE_NOTE = ('REVALIDACIÓN 18/10/2026 (corrida 20261018-222926-929586, 128 pares: 64 con generador real y 64 con generador controlado, 32 generaciones reales por lado). '
              'Sonda compilada desde el worktree D:/Original/gta-wt-b29 (HEAD 29305c78d; fuentes de artifacts/build-manifest.json idénticas al árbol principal). '
              'Cambios del banco: hook de GenerateNewOne alternado por lado (apagado en el original, encendido en el candidato), falso también en Body(&CPickups::GenerateNewOne_ExeAbi), '
              'compuerta gen_route_ok y trazas orig_trace/cand_trace. Las corridas 20261018-222055-435337, 222447-230287 y 222805-633217 fueron fallos del propio banco '
              '(falso sólo en 0x456F20 sin interceptar la DLL; el apagado del hook hacía saltar la entrada de GenerateNewOne_ExeAbi a 0x456F20; compuerta de rutas que exigía una '
              'generación por par cuando 96 pares abortan antes) y no son evidencia.')
SCOPE = ('MakeAbortable es un override virtual controlado; RNG determinista y parcheado temporalmente. Consulta real de colisión por coordenadas y GenerateNewOne real o controlado; '
         'fallo inyectado en la mitad de los casos. Sin validación de vuelo completo con jetpack, render de modelos, recogida de pickups, FX ni audio. ' + SCOPE_NOTE)


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
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True and s['generate_routes_verified'] is True
    assert int(s['pairs']) == 128
    man = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
    assert man['binaries']['asi'] == s['executed_probe_sha256']
    tested = {k.replace('\\', '/'): v for k, v in man['sources'].items()}
    ev = dict(passed=True, pairs=128, differences=0, registered_pairs=128, registered_functions=1, functions={ID: 128},
              covered_functions={ID: True}, scope=SCOPE, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              real_generator_pairs=s['real_generator_pairs'], controlled_generator_pairs=s['controlled_generator_pairs'], candidate_calls=s['candidate_calls'],
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary-reval-20261018.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    r = next(r for r in data['functions'] if r['hook']['class'] == CLS and r['hook']['fn_name'] == FN)
    if r['evidence']['path'] != out.relative_to(ROOT).as_posix():
        r['previous_evidence'] = dict(evidence=r['evidence'], review_reference=r['review_reference'])
    r['evidence'] = {'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                     'pairs': 128, 'original_exe_sha256': s['original_exe_sha256']}
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
