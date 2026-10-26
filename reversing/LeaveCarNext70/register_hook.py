"""Registra CTaskComplexLeaveCar::CreateNextSubTask (virtual, slot 8 de la vtable 0x86E828, EXE 0x006419F0; hook VIRTUAL) con la corrida APROBADA de LeaveCarNext70.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-leavecarnext70/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/LeaveCarNext70/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador:
"CTaskComplexLeaveCar,CreateNextSubTask,0x00000000,1,0,1,V").
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'LeaveCarNext70'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CTaskComplexLeaveCar::CreateNextSubTask', '0x006419F0', 'CTaskComplexLeaveCar', 'CreateNextSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp', 'V'),
)
# Los hooks virtuales se registran con direccion 0x00000000 y tipo V (docs/hooks.csv: "CTaskComplexLeaveCar,CreateNextSubTask,0x00000000,1,0,1,V"); la direccion real va en evidence.original_address.
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-2 (HEAD 1e45e275d + TaskComplexLeaveCar.cpp, source.patch). CTaskComplexLeaveCar::CreateNextSubTask 0x6419F0 (virtual, thiscall, ret 4; EAX = tarea nueva o nulo): '
              'segun el tipo de la subtarea actual pide a CreateSubTask (0x641530) la siguiente (TASK_FINISHED, SET_PED_OUT, DIE, WAIT_TO_SLOW_DOWN, WAIT_FOR_DOOR_NOT_TO_BE_IN_USE, JUMP_OUT, GET_OUT, FORCE_PED_OUT, '
              'CLOSE_DOOR_FROM_OUTSIDE, SLOW_BE_DRAGGED_OUT, GET_UP_AND_STAND_STILL), reserva puertas y banderas del vehiculo y crea la utilidad CTaskUtilityLineUpPedWithCar. '
              'Sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): CreateSubTask 0x641530 (sin hook en este arbol: se parchea la envoltura de la DLL), '
              'operator new 0x82119A, constructor de CTaskUtilityLineUpPedWithCar 0x64FBB0, CPed::IsPlayer 0x5DF8F0 y SetPedState 0x5E4500, CCarEnterExit::ComputeDoorFlag 0x64E550, ComputeTargetDoorToExit 0x64F110, '
              'IsRoomForPedToLeaveCar 0x6504C0 y CarHasDoorToClose 0x64EE10, CVehicle::SetGettingOutFlags 0x6D3020, ClearGettingOutFlags 0x6D3060, IsPassenger 0x6D1BD0 e IsDriver 0x6D1C40, '
              'CDamageManager::GetDoorStatus_Component 0x6C2250 y SetDoorStatus_Component 0x6C21E0; CVehicle::CanPedStepOutCar se llama por la vtable falsa del vehiculo. Las ayudantes en linea del EXE (0x63BA00, 0x63BAB0, 0x63BAE0, 0x646CF0) corren reales en el EXE y son funciones locales en la DLL. '
              'Fuera de cobertura: en la rama GET_OUT la puerta se limita a 0..255 (CDamageManager de la DLL recibe la puerta como uint8, el EXE como dword; el juego solo usa 0..11); el residuo de registros (EAX/ECX/EDX) no se compara; '
              'los bytes altos del argumento de SetGettingOutFlags que deja el EXE (basura de pila) se enmascaran.')
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
    for ID, ADDR, CLS, FN, SOURCE, HTYPE in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if HTYPE == 'V' else ADDR, 'type': HTYPE},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], 'original_address': ADDR},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (CTaskComplexLeaveCar, tarea de bajarse de un vehiculo): ' + FN + ' sin llamadas al EXE para esta función. '
                                 'El banco sustituye por falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL) los colaboradores con hook en HEAD y compara la secuencia de llamadas con sus argumentos, el objeto reservado '
                                 'construido por el constructor real de CTaskSimpleAchieveHeading, los cambios en las arenas del caso y el retorno. Banco LeaveCarNext70 con ' + str(int(s['pairs'])) + ' pares (' + str(int(ppf[ID])) + ' propios) y cero diferencias; '
                                 'EAX/ECX/EDX de salida: ver README del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
