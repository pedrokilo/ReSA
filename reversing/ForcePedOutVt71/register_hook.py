"""Registra las cuatro funciones enganchadas de CTaskSimpleCarForcePedOut (cuerpo del destructor 0x647790 S; Clone 0x649EE0, MakeAbortable 0x647780 y ProcessPed 0x6477F0 V) con la corrida APROBADA de ForcePedOutVt71.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-forcepedoutvt71/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/ForcePedOutVt71/validation-summary.json y anade las entradas a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador:
"CTaskSimpleCarForcePedOut,Destructor,0x00647790,1,0,1,S" y tres filas V con direccion 0x00000000 para Clone, MakeAbortable y ProcessPed).
El destructor con borrado (ranura 0) no tiene hook: su evidencia queda en el banco, no se registra.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'ForcePedOutVt71'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CTaskSimpleCarForcePedOut::Destructor', '0x00647790', 'CTaskSimpleCarForcePedOut', 'Destructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp', 'S'),
    ('CTaskSimpleCarForcePedOut::Clone', '0x00649EE0', 'CTaskSimpleCarForcePedOut', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp', 'V'),
    ('CTaskSimpleCarForcePedOut::MakeAbortable', '0x00647780', 'CTaskSimpleCarForcePedOut', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp', 'V'),
    ('CTaskSimpleCarForcePedOut::ProcessPed', '0x006477F0', 'CTaskSimpleCarForcePedOut', 'ProcessPed', 'source/game_sa/Tasks/TaskTypes/TaskSimpleCarForcePedOut.cpp', 'V'),
)
EXTRA_LIVE = {'CTaskSimpleCarForcePedOut::DeletingDestructor'}   # ruta viva sin hook (ranura 0)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-2 (HEAD de main + TaskSimpleCarForcePedOut.h/.cpp, InjectHooksMain.cpp y una linea de TaskComplexLeaveCar.cpp, source.patch). Virtuales de CTaskSimpleCarForcePedOut (vtable 0x86EE94): cuerpo del destructor 0x647790 '
              '(limpia la referencia al vehiculo con CleanUpOldReference 0x571A00; hook S), Clone 0x649EE0 (CTask::operator new, comprobacion de nulo, constructor 0x647710), MakeAbortable 0x647780 (devuelve false) y ProcessPed 0x6477F0 '
              '(conductor/pasajeros con IsDriver 0x6D1C40, ComputeOppositeDoorFlag 0x64E610, ComputePassengerIndexFromCarDoor 0x64F1E0 y PositionPedOutOfCollision 0x5E0820); hooks V llamados por la ranura de la vtable del EXE. Colaboradores con hook sustituidos por falsos '
              'identicos en la direccion del EXE y en el cuerpo de la DLL; CTask::~CTask 0x61A3A0 (en linea en la DLL) solo en el EXE con el mismo efecto. Se compara el objeto completo, AL/EAX, la traza de colaboradores y las compuertas de ABI/SEH/restauracion. '
              'El banco incluye ademas la ruta viva del destructor con borrado (0x64BF50 del EXE contra el del compilador en la DLL, con banderas 0/1 y bits altos): sin hook, sin registro. Los bits 1 y 2 de las banderas del destructor con borrado de la DLL (array y ::operator delete global) quedan fuera: ningun llamador los pasa.')
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
    assert set(ppf) == {f[0] for f in FUNCS} | EXTRA_LIVE, ppf
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
                       'scope': ('Reconstruida en C++ (CTaskSimpleCarForcePedOut, tarea de sacar al peatón del coche a la fuerza): ' + FN + ' sin llamadas al EXE para esta función. '
                                 'El banco sustituye por falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL) RegisterReference y compara la secuencia de llamadas con sus argumentos, el objeto completo '
                                 'construido (relleno incluido), los cambios en las arenas del caso y el retorno. Banco ForcePedOutVt71 con ' + str(int(s['pairs'])) + ' pares (' + str(int(ppf[ID])) + ' propios) y cero diferencias; '
                                 'EAX/ECX/EDX de salida: ver README del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
