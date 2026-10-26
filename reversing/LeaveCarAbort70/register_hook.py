"""Registra CTaskComplexLeaveCar::MakeAbortable (0x00641100, hook VIRTUAL: vtable 0x86E828, ranura 6) con la corrida APROBADA de LeaveCarAbort70.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-leavecarabort70/<corrida>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/LeaveCarAbort70/validation-summary.json y anade las entradas a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador:
virtual con direccion 0x00000000 y tipo V: "CTaskComplexLeaveCar,MakeAbortable,0x00000000,1,0,1,V"). La direccion real va en la clave de approvals.json.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'LeaveCarAbort70'
FUNCS = (
    # (id del banco, direccion del EXE, clase del hook, nombre del hook, fuente, tipo de hook)
    ('CTaskComplexLeaveCar::MakeAbortable', '0x00641100', 'CTaskComplexLeaveCar', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp', 'V'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-1 (HEAD 1e45e275d + TaskComplexLeaveCar.cpp, source.patch). CTaskComplexLeaveCar::MakeAbortable 0x641100 (virtual, ranura 6 de la '
              'vtable 0x86E828, thiscall, ret 0xC; AL = resultado): CVehicle::ClearGettingOutFlags 0x6D3060, CVector::Magnitude 0x4082C0, CPed::IsPlayer 0x5DF8F0, CWorld::ProcessVerticalLine 0x5674E0, '
              'CAnimManager::BlendAnimation(id) 0x4D4610, CCarEnterExit::ComputeTargetDoorToExit 0x64F110, y de las dos tareas temporales de la pila los constructores y destructores '
              'de CTaskSimpleCarCloseDoorFromOutside (0x6464F0 / 0x646570) y de CTaskSimpleCarSetPedOut (0x6478B0 / 0x647950, sin hook: el de la DLL tiene un 4o argumento) y sus MakeAbortable 0x64B020 / ProcessPed 0x647D10, '
              'sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL). GetTaskType y MakeAbortable de la subtarea, GetEventType del evento y '
              'GetNumContactWheels del vehiculo (vtable 55) van por vtables falsas de arena; CEventDamage::HasKilledPed (0x4ABCA0) corre real en ambos lados. '
              'La funcion no se probo con subtarea nula en las rutas de prioridad 1 y 2 (el EXE la desreferencia). CreateNextSubTask 0x6419F0, CreateFirstSubTask 0x641FC0 y CreateSubTask 0x641530 siguen en el EXE; '
              'el constructor, el destructor y ControlSubTask de la clase estan en C++ sin hook ni banco.')
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
                        'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], 'original_address': ADDR}
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if KIND == 'V' else ADDR, 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco LeaveCarAbort70 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; ECX/EDX solo registrados y AL comparado (los llamadores virtuales leen AL). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
