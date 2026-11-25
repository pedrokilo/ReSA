"""Partida larga con la ASI de esta rama (prueba de estabilidad del asignador): lanza jugar.ps1 de FnBIS1 (variante X con la ASI indicada), mantiene el juego activo
(ForegroundApp = 1), lleva al jugador a una serie de puntos del mapa cada pocos segundos (obliga a transmitir zonas nuevas y a reservar/liberar mucha memoria) y anota
cada 10 s el proceso (vivo, memoria, estado del juego, posicion). Termina cuando el juego se cierra solo (jugar.ps1 -Auto N lo cierra a los N segundos) y devuelve 0 si
el proceso vivio todo el plazo sin caer (la salida de jugar la decide el propio lanzador). No toma gta_lock por su cuenta: jugar.ps1 lo toma y lo suelta; espera a que este libre.
No mata procesos ajenos: solo el gta_sa.exe que arranco su propio jugar.ps1 (el PID nuevo tras lanzarlo).
Uso: AGUA_ASI=<asi> python asi_larga.py <etiqueta> <segundos> [<carpeta de FnBIS1>]"""
import ctypes
import json
import os
import struct
import subprocess
import sys
import time
from ctypes import wintypes

et, segundos = sys.argv[1], int(sys.argv[2])
bis1 = sys.argv[3] if len(sys.argv) > 3 else r'D:\Original\wv-8\reversing\FnBIS1'
asi = os.environ['AGUA_ASI']
k = ctypes.WinDLL('kernel32', use_last_error=True)
k.OpenProcess.restype = wintypes.HANDLE


def pids():
    out = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe', '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout
    return {int(l.split('","')[1]) for l in out.splitlines() if 'gta_sa' in l}


class Mem:
    def __init__(self, pid):
        self.pid = pid
        self.h = k.OpenProcess(0x38 | 0x400, False, pid)

    def rd(self, a, n):
        b = ctypes.create_string_buffer(n)
        r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(self.h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None

    def wr(self, a, d):
        r = ctypes.c_size_t()
        return k.WriteProcessMemory(self.h, ctypes.c_void_p(a), d, len(d), ctypes.byref(r))

    def u32(self, a):
        b = self.rd(a, 4)
        return struct.unpack('<I', b)[0] if b else 0

    def f32(self, a):
        b = self.rd(a, 4)
        return struct.unpack('<f', b)[0] if b else float('nan')


class _PMC(ctypes.Structure):
    _fields_ = [('cb', wintypes.DWORD), ('PageFaultCount', wintypes.DWORD), ('PeakWorkingSetSize', ctypes.c_size_t), ('WorkingSetSize', ctypes.c_size_t),
                ('QuotaPeakPagedPoolUsage', ctypes.c_size_t), ('QuotaPagedPoolUsage', ctypes.c_size_t), ('QuotaPeakNonPagedPoolUsage', ctypes.c_size_t),
                ('QuotaNonPagedPoolUsage', ctypes.c_size_t), ('PagefileUsage', ctypes.c_size_t), ('PeakPagefileUsage', ctypes.c_size_t)]


def memoria_kb(mem):
    """Memoria privada (PagefileUsage) del proceso del juego en KB; -1 si no se puede leer."""
    c = _PMC()
    c.cb = ctypes.sizeof(c)
    if ctypes.windll.psapi.GetProcessMemoryInfo(wintypes.HANDLE(mem.h), ctypes.byref(c), c.cb):
        return c.PagefileUsage // 1024
    return -1


puntos = [(2495, -1665, 60), (1480, -1730, 60), (1960, -2180, 60), (400, -1800, 60), (1380, -910, 80), (2020, 1000, 60), (-2000, 500, 80), (-400, 2200, 120), (-100, -1100, 60), (1000, -1300, 60)]
LOG = os.path.join(os.environ.get('TEMP', '.'), f'jugar_{et}.log')
pid = None
lan = None
for intento in range(60):
    # espera (sin tocar nada ajeno) a que no haya juego ni candado; jugar.ps1 toma el candado y falla si otro se adelanta: se reintenta
    while pids() or os.path.exists(r'D:\Original\gta_lock') or os.path.exists(os.path.join(bis1, 'respaldo_pendiente')):
        time.sleep(3)
    lan = subprocess.Popen(['pwsh', '-NoProfile', '-File', os.path.join(bis1, 'jugar.ps1'), '-Variante', 'X', '-AsiPath', asi, '-Etiqueta', et, '-ArgsFile', 'cal_mal.args',
                            '-Auto', str(segundos), '-Silencioso', '-NoEnfocar'], cwd=bis1, stdout=open(LOG, 'w'), stderr=subprocess.STDOUT)
    t0 = time.time()
    while time.time() - t0 < 120 and pid is None:
        if lan.poll() is not None:
            break
        nuevos = pids()
        if nuevos:
            pid = sorted(nuevos)[0]
        time.sleep(1)
    if pid is not None:
        break
    print('intento', intento, 'sin juego (lanzador salio con', lan.poll(), '): reintento', flush=True)
    if lan.poll() is None:
        break  # el lanzador sigue vivo pero no arranca el juego: no lo mato, aborto
if pid is None:
    print('ERROR sin juego')
    sys.exit(3)
print('pid del juego', pid, flush=True)
m = Mem(pid)
inicio = time.time()
ultimo_tp = 0
ultimo_log = 0
i_tp = 0
caido = False
muestras = []
while lan.poll() is None:
    if pid not in pids():
        # jugar.ps1 -Auto cierra SU juego al terminar el plazo: se da un margen a que el lanzador salga antes de decidir que fue una caida
        fin = time.time()
        while lan.poll() is None and time.time() - fin < 30:
            time.sleep(1)
        caido = time.time() - inicio < segundos - 10
        break
    m.wr(0x8D621C, struct.pack('<I', 1))
    ahora = time.time()
    try:
        inf = m.u32(0xB7CD74)
        ped = m.u32(0xB7CD98 + inf * 0x190)
        mat = m.u32(ped + 0x14) if ped else 0
        estado = m.u32(0xC8D4C0)
        if estado == 9 and mat and ahora - ultimo_tp > 25 and ahora - inicio > 40:
            x, y, z = puntos[i_tp % len(puntos)]
            i_tp += 1
            ultimo_tp = ahora
            m.wr(mat + 0x30, struct.pack('<3f', float(x), float(y), float(z)))
            m.wr(ped + 0x44, struct.pack('<3f', 0.0, 0.0, 0.0))
        if ahora - ultimo_log > 10:
            ultimo_log = ahora
            pos = (m.f32(mat + 0x30), m.f32(mat + 0x34), m.f32(mat + 0x38)) if mat else None
            kb = memoria_kb(m)
            muestras.append((int(ahora - inicio), estado, kb))
            print(f't={int(ahora - inicio):4d}s estado={estado} mem={kb} KB tp={i_tp} pos={pos}', flush=True)
    except Exception as e:
        print('excepcion de lectura', e, flush=True)
    time.sleep(0.25)
lan.wait()
# Una caida durante el CIERRE (jugar.ps1 -Auto cierra con CloseMainWindow) no la ve el bucle: se busca en el registro de eventos de Windows (Application Error 1000 con este PID)
ps = ("Get-WinEvent -FilterHashtable @{LogName='Application'; Id=1000; StartTime=(Get-Date).AddSeconds(-%d)} -ErrorAction SilentlyContinue | "
      "Where-Object { $_.Message -match '(?i)0x%x' } | ForEach-Object { ($_.Message -split [Environment]::NewLine | Select-Object -First 6) -join ' | ' }") % (int(time.time() - inicio) + 90, pid)
crash = subprocess.run(['pwsh', '-NoProfile', '-Command', ps], capture_output=True, text=True).stdout.strip()
print('APPCRASH al cierre o durante la partida:', crash if crash else 'no', flush=True)
caido = caido or bool(crash)
print('jugar.ps1 salio con', lan.returncode, 'tras', int(time.time() - inicio), 's; caida_antes_del_plazo=', caido, flush=True)
sys.exit(1 if caido else 0)
