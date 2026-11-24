# Lanza gta_sa_standalone.exe en la carpeta de pruebas D:\Original\_cargar_run (junctions a los datos; userfiles propio con COPIA del save),
# bajo D:\Original\gta_lock, ejecuta un guion (esperas, teclas, capturas, lecturas de memoria) y mata SOLO su proceso.
# Uso: python correr.py <etiqueta> <exe> <ini> <guion.json>   (el INI se copia a gta-reversed.ini de la carpeta de pruebas)
import ctypes, struct, subprocess, sys, time, json, os, shutil, hashlib
from ctypes import wintypes
sys.path.insert(0, r'D:\Original\wv-8\reversing\FnBIS1')
import captura
RUN = r'D:\Original\_cargar_run'; LOCK = r'D:\Original\gta_lock'
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE; k.CreateRemoteThread.restype = wintypes.HANDLE; k.CreateRemoteThread.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p, ctypes.c_void_p, wintypes.DWORD, ctypes.c_void_p]
u = ctypes.WinDLL('user32', use_last_error=True)
KEYS = {'enter': 0x0D, 'esc': 0x1B, 'down': 0x28, 'up': 0x26, 'left': 0x25, 'right': 0x27, 'space': 0x20, 'tab': 0x09, 'w': 0x57, 'f': 0x46}
class M:
    def __init__(s, p): s.p = p; s.h = k.OpenProcess(0x38 | 0x400 | 0x2 | 0x100000, False, p)
    def rd(s, a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None
    def u32(s, a): b = s.rd(a, 4); return struct.unpack('<I', b)[0] if b else 0
    def i32(s, a): b = s.rd(a, 4); return struct.unpack('<i', b)[0] if b else 0
    def f32(s, a): b = s.rd(a, 4); return struct.unpack('<f', b)[0] if b else float('nan')
    def u8(s, a): b = s.rd(a, 1); return b[0] if b else -1
def estado(m):
    inf = m.u32(0xB7CD74); base = 0xB7CD98 + inf * 0x190
    ped = m.u32(base); mat = m.u32(ped + 0x14) if ped else 0
    pos = (round(m.f32(mat + 0x30), 2), round(m.f32(mat + 0x34), 2), round(m.f32(mat + 0x38), 2)) if mat else None
    return {'gGameState': m.u32(0xC8D4C0), 'frame': m.u32(0xB7CB4C), 'ped': hex(ped), 'pos': pos, 'dinero': m.i32(base + 0xB8) if ped else None,
            'hora': '%02d:%02d' % (m.u8(0xB70153), m.u8(0xB70152)), 'cutscene': m.u8(0xB5F851), 'tiempo_ms': m.u32(0xB7CB84)}
EXT = {0x25, 0x26, 0x27, 0x28}
def tecla(code, down_ms=120):
    sc = u.MapVirtualKeyW(code, 0); ex = 1 if code in EXT else 0
    u.keybd_event(code, sc, 8 | ex, 0); time.sleep(down_ms / 1000)   # 8 = KEYEVENTF_SCANCODE, 1 = EXTENDEDKEY
    u.keybd_event(code, sc, 8 | ex | 2, 0)
def pid_de(p):
    o = subprocess.run(['tasklist', '/FI', 'PID eq %d' % p, '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout
    return 'gta_sa' in o
def vigilante(segundos, getp):
    # Plazo duro de TODA la ejecucion: pasado el tiempo mata solo el proceso propio, suelta el candado y sale (el driver nunca retiene gta_lock).
    import threading
    def fin():
        p = getp()
        print('PLAZO AGOTADO (%d s): se mata el proceso propio y se suelta el candado' % segundos, flush=True)
        if p: subprocess.run(['taskkill', '/F', '/PID', str(p)], capture_output=True)
        shutil.rmtree(LOCK, ignore_errors=True); os._exit(9)
    t = threading.Timer(segundos, fin); t.daemon = True; t.start(); return t
def main():
    et, exe, ini, guion = sys.argv[1:5]
    pasos = json.load(open(guion))
    out = r'D:\Original\sim_out\carga_%s' % et; os.makedirs(out, exist_ok=True)
    PLAZO = int(os.environ.get('CARGA_PLAZO_S', '420')); espera_lock = time.time()
    while True:
        try: os.mkdir(LOCK); break
        except FileExistsError:
            if time.time() - espera_lock > 1800: print('candado ocupado mas de 30 min, abandono'); return
            time.sleep(5)
    estado_p = [None]; vigilante(PLAZO, lambda: estado_p[0])
    open(os.path.join(LOCK, 'owner_carga'), 'w').write('carga')
    p = None; log = []
    try:
        t_otro = time.time()
        while time.time() - t_otro < 1200 and (subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa_standalone.exe', '/NH'], capture_output=True, text=True).stdout.count('gta_sa_standalone') or \
              subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe', '/NH'], capture_output=True, text=True).stdout.count('gta_sa.exe')):
            print('hay otro juego, espero'); time.sleep(10)
        shutil.copy(ini, os.path.join(RUN, 'gta-reversed.ini'))
        for f in os.listdir(os.path.join(RUN, 'logs')): os.remove(os.path.join(RUN, 'logs', f))
        sal = os.path.join(out, 'salida'); os.makedirs(sal, exist_ok=True)
        args = r'--ruta-original "D:\Grand Theft Auto San Andreas\gta-sa.exe" --directorio-juego "%s" --salida "%s" --limite 0' % (RUN, sal)
        if '--censo' in sys.argv: args += ' --censo'
        pr = subprocess.Popen('"%s" %s' % (exe, args), cwd=RUN); p = pr.pid; estado_p[0] = p; m = M(p)
        t0 = time.time(); n = 0
        for paso in pasos:
            if pr.poll() is not None: print('EL PROCESO TERMINO exit=0x%08X' % (pr.returncode & 0xffffffff)); log.append('exit=%08X' % (pr.returncode & 0xffffffff)); break
            a = paso[0]
            if a == 'esperar': time.sleep(paso[1])
            elif a == 'esperar_estado9':
                t = time.time()
                while time.time() - t < paso[1] and m.u32(0xC8D4C0) != 9 and pr.poll() is None: time.sleep(0.5)
            elif a == 'tecla':
                h = captura.buscar()
                if h: u.ShowWindow(h, 9); u.SetForegroundWindow(h)
                time.sleep(0.3); print('primer plano ok:', u.GetForegroundWindow() == h)
                for c in paso[1:]: tecla(KEYS[c] if isinstance(c, str) else c); time.sleep(0.6)
            elif a == 'captura':
                n += 1; r = captura.capturar(os.path.join(out, '%02d_%s.png' % (n, paso[1])), traer=False); print('captura', paso[1], r); log.append(('captura', paso[1], r))
            elif a == 'estado':
                e = estado(m); print(paso[1] if len(paso) > 1 else '', e); log.append((paso[1] if len(paso) > 1 else '', e))
            elif a == 'guardar':  # llama a C_PcSave::SaveSlot(slot0) (0x619060) en un hilo remoto; el juego debe estar en pausa
                h = k.CreateRemoteThread(m.h, None, 0, ctypes.c_void_p(0x619060), ctypes.c_void_p(paso[1]), 0, None)
                if not h: print('CreateRemoteThread fallo', ctypes.get_last_error()); continue
                r = k.WaitForSingleObject(h, 60000); code = wintypes.DWORD(); k.GetExitCodeThread(h, ctypes.byref(code))
                print('guardar slot', paso[1], 'espera', r, 'salida', hex(code.value)); log.append(('guardar', paso[1], r, code.value))
            elif a == 'rmem':  # lectura u32 [dir]
                print('mem', hex(paso[1]), hex(m.u32(paso[1])))
    finally:
        if p and pid_de(p): subprocess.run(['taskkill', '/F', '/PID', str(p)], capture_output=True)
        time.sleep(1)
        for f in os.listdir(os.path.join(RUN, 'logs')): shutil.copy(os.path.join(RUN, 'logs', f), out)
        shutil.copy(os.path.join(RUN, 'gta-reversed.ini'), out)
        shutil.rmtree(LOCK, ignore_errors=True)
    json.dump(log, open(os.path.join(out, 'log.json'), 'w'), indent=1, default=str)
main()
