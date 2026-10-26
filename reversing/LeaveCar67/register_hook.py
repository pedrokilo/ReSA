"""Registra CTaskSimpleGangDriveBy::SetupAnim (0x00627B20, helper de animacion sin simbolo confirmado) con la corrida APROBADA de LeaveCar67.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-leavecar67/<corrida>"

PLANTILLA: la de Helpers66A adaptada a un hook Simple. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'LeaveCar67'
FUNCS = (
    # (id del banco, direccion del EXE, clase del hook, nombre del hook, fuente, tipo de hook)
    ('CTaskSimpleGangDriveBy::SetupAnim', '0x00627B20', 'CTaskSimpleGangDriveBy', 'SetupAnim', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-3 (HEAD 1ca520153 + TaskSimpleGangDriveBy.cpp/.h). CTaskSimpleGangDriveBy::SetupAnim 0x627B20 (thiscall, ret 4; ayudante de animacion de la tarea del '
              'tiroteo desde coche que llaman MakeAbortable 0x62D290 y ProcessPed 0x62D3B0; solo se lee AL: test al,al): CAnimManager::GetAnimBlockName 0x4D3A30, GetAnimationBlock(const char*) 0x4D3940, '
              'AddAnimBlockRef 0x4D3FB0 y BlendAnimation 0x4D4610, CStreaming::RequestModel 0x4087E0, CAnimBlendAssociation::SetDeleteCallback 0x4CEBC0 y SetCurrentTime 0x4CEA80, CPed::IsPlayer 0x5DF8F0 y rand() '
              'del EXE 0x821B1E sustituidos por falsos identicos en ambos lados; ms_aAnimAssocGroups (0xB4EA34) apunta a un arreglo falso de 256 grupos durante la llamada. Los callbacks 0x4D1390 y 0x621BE0 se '
              'pasan por direccion y no se ejecutan. Bloques del EXE con indice negativo (.rdata) para el desplazamiento aritmetico. La declaracion nueva es bool SetupAnim(CPed*) en TaskSimpleGangDriveBy.h; '
              'MakeAbortable y ProcessPed la llaman en C++ en lugar de por direccion.')
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

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN, SOURCE, KIND in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            evidence = {'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                        'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']}
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco LeaveCar67 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; ECX/EDX solo registrados y EAX comparado donde un llamador lo lee. '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
