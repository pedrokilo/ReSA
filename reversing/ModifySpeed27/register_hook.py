"""Registra CAutoPilot::ModifySpeed (0x0041B980) con la corrida aprobada de ModifySpeed27 (18/10/2026).

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/ModifySpeed27/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente.
"""
import gzip, hashlib, json, shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC = Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-modifyspeed27\20261018-131101-659386')
BANK, ADDR, ID, CLS, FN = 'ModifySpeed27', '0x0041B980', 'CAutoPilot::ModifySpeed', 'CAutoPilot', 'ModifySpeed'
RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + SRC.name)
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main():
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
        a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': 'source/game_sa/AutoPilot.cpp', 'bank': BANK,
                   'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                   'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                   'state_restored': True,
                   'scope': 'Reconstruida en C++ sin llamadas al EXE (CCarPathLink::OneWayLaneOffset de PathFind.h reproduce 0x44DB00; colaborador CCurves::CalcSpeedScaleFactor 0x43C710 ya en C++). Banco ModifySpeed27 con 4096 pares y cero diferencias; EAX/ECX/EDX sólo registrados porque ningún llamador los lee. Sin cobertura exhaustiva más allá de los fixtures del banco.'}
        ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    main()
