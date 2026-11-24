# Control con el ASI sobre el EXE original: lanza el juego con jugar.ps1 de wv-8\FnBIS1 (candado propio, con reintentos si otro agente lo gana)
# y mide por memoria con sonda_traf.py / nubes_matriz.py (direcciones reales del EXE).
# Uso: python control_traf.py <etiqueta> <archivo.args> [--asi ruta.asi] [--espera S] [--auto S] [--sonda "args"] [--script nubes_matriz.py]
import subprocess, sys, os, time
aqui = os.path.dirname(os.path.abspath(__file__))
FN = r'D:\Original\wv-8\reversing\FnBIS1'
et = sys.argv[1]; args = sys.argv[2]
asi = sys.argv[sys.argv.index('--asi') + 1] if '--asi' in sys.argv else r'D:\Original\wv-int\bin\Debug\gta_reversed.asi'
esp = sys.argv[sys.argv.index('--espera') + 1] if '--espera' in sys.argv else '45'
auto = sys.argv[sys.argv.index('--auto') + 1] if '--auto' in sys.argv else '200'
SCRIPT = sys.argv[sys.argv.index('--script') + 1] if '--script' in sys.argv else 'sonda_traf.py'
EXTRA = sys.argv[sys.argv.index('--sonda') + 1].split() if '--sonda' in sys.argv else []
MARCA = 'traf-id=%s-%d' % (et, os.getpid())
sal = os.path.join(aqui, 'ejecuciones', '%s-%s' % (et, time.strftime('%Y%m%d-%H%M%S'))); os.makedirs(sal, exist_ok=True)
def mi_pid(marca):
    r = subprocess.run(['powershell', '-NoProfile', '-Command', "Get-CimInstance Win32_Process -Filter \"Name='gta_sa.exe'\" | Where-Object { $_.CommandLine -like '*%s*' } | Select-Object -First 1 -ExpandProperty ProcessId" % marca], capture_output=True, text=True).stdout.strip()
    return int(r) if r.isdigit() else None
def hay_juego():
    o = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe', '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout
    return 'gta_sa.exe' in o
ARGS_PROPIO = os.path.join(sal, 'args.txt')
open(ARGS_PROPIO, 'w').write(open(args).read().strip() + ' --' + MARCA)
args = ARGS_PROPIO
lan = None
MIPID = None
for intento in range(200):
    t = time.time()
    while (os.path.exists('D:/Original/gta_lock') or os.path.exists(os.path.join(FN, 'respaldo_pendiente')) or hay_juego()) and time.time() - t < 7200: time.sleep(3)
    lan = subprocess.Popen(['pwsh', '-NoProfile', '-File', os.path.join(FN, 'jugar.ps1'), '-Variante', 'X', '-AsiPath', asi, '-Etiqueta', et, '-ArgsFile', args, '-Auto', auto, '-Silencioso', '-NoEnfocar'],
                           cwd=FN, stdout=open(os.path.join(sal, 'jugar.log'), 'w'), stderr=subprocess.STDOUT)
    t = time.time()
    while time.time() - t < 90 and lan.poll() is None and not MIPID:
        MIPID = mi_pid(MARCA); time.sleep(1)
    if MIPID: break
    print('intento %d: jugar.ps1 no llego a lanzar el juego (candado ganado por otro); reintento' % (intento + 1), flush=True)
    if lan.poll() is None: lan.kill()
cmd = [sys.executable, '-I', os.path.join(aqui, SCRIPT), sal, '--exe', 'gta_sa', '--pid', str(MIPID)] + ([] if SCRIPT != 'sonda_traf.py' else ['--espera', esp, '--seg', '25', '--salir']) + EXTRA
son = subprocess.run(cmd, capture_output=True, text=True)
print(son.stdout[-3000:], son.stderr[-500:])
try: lan.wait(timeout=int(auto) + 120)
except Exception: pass
print('dir', sal)
