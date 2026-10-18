"""Compuertas independientes del banco Dependencies24A (Win::__MainWndProc 0x747EB0, 19 rutas x 1024 pares).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer): hook registrado, cuerpo en el ASI, colaboradores sustituidos, API de Win32 sustituibles por IAT en el EXE y en la DLL,
todas las compuertas de ABI/ruta/x87, fp_difference vacio, registros x87 vacios iguales, cobertura MINIMA POR RUTA medida con la traza de colaboradores (el mismo reparto que el ensayo sin
juego, offline/main.cpp, al 60 %), conjunto EXACTO de colaboradores permitido por ruta y final con restauracion. kCompareDeadX87Registers debe ser true.
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

ROUTE_NAMES = ['WM_DESTROY', 'WM_CLOSE', 'WM_SIZE', 'WM_ACTIVATE', 'WM_SETFOCUS', 'WM_KILLFOCUS', 'WM_ACTIVATEAPP', 'WM_SETCURSOR', 'WM_KEYDOWN/WM_SYSKEYDOWN', 'WM_KEYUP/WM_SYSKEYUP',
               'WM_MOUSEMOVE', 'WM_xBUTTONDOWN', 'WM_xBUTTONUP', 'WM_MOUSEWHEEL', 'WM_TIMER', 'WM_SIZING', 'WM_DEVICECHANGE', 'WM_GRAPHNOTIFY', 'desconocidos']
ROUTES = len(ROUTE_NAMES)
PER_ROUTE = 1024
NAMES = ['Win::__MainWndProc[%s]' % n for n in ROUTE_NAMES]
EXPECTED = {n: PER_ROUTE for n in NAMES}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored')
# Tipos de traza (Core.hpp, enum Ev): 1 RsEventHandler, 2 RsKeyboardEventHandler, 3 GTATranslateKey, 4 GTATranslateShiftKey, 5 PauseAllSounds, 6 ResumeAllSounds, 7 CPad::Clear, 8 SetupBackBufferVertex,
# 9 WinInput::Shutdown, 10 WaitState, 11 VideoPlayer::UpdateWindow, 12 OnGraphNotify, 13 IsStreamingFromDVD, 14 GetDVDDriveLetter, 15 CheckDVD, 16 NoDiskInDriveMessage, 17 SkipCutscene,
# 18 RwD3D9GetCurrentD3DDevice, 19 SetGammaRamp, 20 IMediaControl::GetState, 21 Run, 22 Pause, 23 metodo COM no previsto (trampa), 30 ClipCursor, 31 PostQuitMessage, 32 SetCursor, 33 ShowCursor,
# 34 SetCapture, 35 ReleaseCapture, 36 GetWindowPlacement, 37 SendMessageA, 38 SetTimer, 39 SetFocus, 40 SetWindowPos, 41 DefWindowProcA, 42 OutputDebugStringA.
# Pares MINIMOS por ruta que deben llamar a cada colaborador (60 % de lo medido con el ensayo sin juego, mismas fixtures) y CONJUNTO EXACTO de tipos que cada ruta puede llamar (todo lo demas es 0).
MEASURED = {
    0: {9: 1024, 30: 1024, 31: 1024},
    1: {9: 1024, 30: 1024, 31: 1024},
    2: {1: 618, 35: 157, 36: 157, 37: 71},
    3: {1: 1024, 5: 352, 6: 672, 7: 1024, 33: 672, 38: 352, 41: 1024},
    4: {41: 1024},
    5: {41: 1024},
    6: {7: 1024, 10: 143, 11: 384, 18: 891, 19: 726, 20: 640, 21: 197, 22: 99, 39: 101, 42: 153},
    7: {32: 1024, 33: 1024, 41: 1024},
    8: {2: 512, 3: 1024, 4: 410},
    9: {2: 512, 3: 1024, 4: 402},
    10: {}, 11: {34: 1024}, 12: {35: 1024}, 13: {}, 14: {},
    15: {1: 483, 8: 1024, 40: 1024},
    16: {13: 683, 14: 480, 15: 103, 16: 204, 17: 101, 41: 1024, 42: 204},
    17: {12: 307, 41: 1024},
    18: {41: 1024},
}
MIN = {r: {k: int(v * 0.6) for k, v in d.items()} for r, d in MEASURED.items()}
APIS = ['ReleaseCapture', 'GetWindowPlacement', 'SetTimer', 'ClipCursor', 'PostQuitMessage', 'SetCursor', 'SetCapture', 'DefWindowProcA', 'SetFocus', 'SendMessageA', 'SetWindowPos', 'ShowCursor', 'OutputDebugStringA']


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado o sin colaboradores sustituidos')
    b = begin[0]
    if b['pairs'] != ROUTES * PER_ROUTE or not all(b['bodies'].values()):
        raise RuntimeError('Pares o cuerpos RH inesperados')
    if b.get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if b.get('apis_patchable') is not True or not all(b['api_slots_exe'].get(a, 0) >= 1 and b['api_slots_asi'].get(a, 0) >= 1 for a in APIS):
        raise RuntimeError('Alguna API de Win32 no se puede sustituir por IAT en el EXE y en la DLL: ' + json.dumps([b['api_slots_exe'], b['api_slots_asi']]))
    if b.get('exe_iat_matches_real') is not True:
        raise RuntimeError('La tabla de importacion del EXE no apunta a las API reales (otro modulo las intercepta): ' + json.dumps(b['exe_iat']))
    seen = {k: set() for k in EXPECTED}
    cov = {r: {} for r in range(ROUTES)}
    dead_equal = dead_total = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if case in seen[fn]:
            raise RuntimeError('Caso repetido %s %d' % (fn, case))
        seen[fn].add(case)
        route = NAMES.index(fn)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d' % (fn, case))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        dead_total += 1
        dead_equal += bool(row['dead_registers_equal'])
        if row['dead_registers_equal'] is not True:
            raise RuntimeError('Registros x87 vacios distintos en %s %d' % (fn, case))
        inp = a['input']
        if inp.get('case') != case or inp.get('route') != route:
            raise RuntimeError('Entrada con otro numero de caso o de ruta')
        counts = a['trace']['counts']
        for k in counts:
            cov[route][int(k)] = cov[route].get(int(k), 0) + 1
        if 23 in map(int, counts):
            raise RuntimeError('Llamada a un metodo COM no previsto en %s %d' % (fn, case))
    for fn in EXPECTED:
        if seen[fn] != set(range(PER_ROUTE)):
            raise RuntimeError('Cobertura incompleta de ' + fn)
    short = {}
    for r in range(ROUTES):
        for kind, need in MIN[r].items():
            if cov[r].get(kind, 0) < need:
                short['ruta %d tipo %d' % (r, kind)] = (cov[r].get(kind, 0), need)
        for kind, n in cov[r].items():
            if kind not in MEASURED[r]:
                short['ruta %d tipo %d inesperado' % (r, kind)] = (n, 0)
    if short:
        raise RuntimeError('Cobertura insuficiente o colaboradores inesperados: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    summary.update(coverage_by_route=cov, compare_dead_x87_registers=b.get('compare_dead_x87_registers'), dead_registers_equal_pairs=[dead_equal, dead_total],
                   coverage_method='serialized_inputs_and_collaborator_trace_counts_per_route')
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies24a', manifest['binaries']['asi'], analyze, 'Dependencies24A', timeout=3600))
