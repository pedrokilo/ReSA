# Observa por memoria un gta_sa_standalone.exe ya lanzado (ejecutar_f8b.ps1) y comprueba si CJ responde a la entrada.
# Uso: python observar_jugador.py <etiqueta> [segundos] [--entrada] [--modo pad|tecla|ambos]
# Escribe en D:/Original/sim_out/mv_<etiqueta>.log y capturas mv_<etiqueta>_*.png. Solo lee/escribe el proceso de su PID (el standalone).
import ctypes, struct, subprocess, sys, time, os
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, aqui)
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
OUT = 'D:/Original/sim_out'
def pid():
    # solo el proceso lanzado por ESTE arbol (la linea de comandos lleva --salida ...\wv-move\...); no toca el de otros agentes
    o = subprocess.run(['powershell', '-NoProfile', '-Command', "Get-CimInstance Win32_Process -Filter \"Name='gta_sa_standalone.exe'\" | ForEach-Object { \"$($_.ProcessId)|$($_.CommandLine)\" }"], capture_output=True, text=True).stdout
    for l in o.splitlines():
        if 'wv-move' in l: return int(l.split('|')[0])
class M:
    def __init__(s, p): s.p = p; s.h = k.OpenProcess(0x38 | 0x400, False, p)
    def rd(s, a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None
    def wr(s, a, d): r = ctypes.c_size_t(); return k.WriteProcessMemory(s.h, ctypes.c_void_p(a), d, len(d), ctypes.byref(r))
    def u32(s, a): b = s.rd(a, 4); return struct.unpack('<I', b)[0] if b else 0
    def i16(s, a): b = s.rd(a, 2); return struct.unpack('<h', b)[0] if b else 0
    def f32(s, a): b = s.rd(a, 4); return struct.unpack('<f', b)[0] if b else float('nan')
def main():
    et = sys.argv[1]; seg = int(sys.argv[2]) if len(sys.argv) > 2 and sys.argv[2].isdigit() else 120
    entrada = '--entrada' in sys.argv
    modo = sys.argv[sys.argv.index('--modo') + 1] if '--modo' in sys.argv else 'pad'
    f = open('%s/mv_%s.log' % (OUT, et), 'w', encoding='utf-8')
    def P(*a):
        s = ' '.join(str(x) for x in a); print(s, flush=True); f.write(s + '\n'); f.flush()
    t = time.time(); p = None
    while time.time() - t < 5400 and not p: p = pid(); time.sleep(1)
    if not p: P('sin proceso'); return 3
    m = M(p); t0 = time.time(); e9 = None; listo = None; hecho = False; cap = 0; ult = 0
    try:
        import captura
    except Exception as e: captura = None; P('sin captura', e)
    fallos = 0
    while time.time() - t0 < seg and fallos < 4:
        fallos = fallos + 1 if pid() != p else 0
        gs = m.u32(0xC8D4C0); fr = m.u32(0xB7CB4C)
        inf = m.u32(0xB7CD74); ped = m.u32(0xB7CD98 + inf * 0x190); mat = m.u32(ped + 0x14) if ped else 0
        pos = (round(m.f32(mat + 0x30), 1), round(m.f32(mat + 0x34), 1), round(m.f32(mat + 0x38), 1)) if mat else None
        sp = (round(m.f32(ped + 0x44), 3), round(m.f32(ped + 0x48), 3)) if ped else None
        cs = m.rd(0xB5F851, 1); cs = cs[0] if cs else -1
        pad = m.rd(0xB73458, 8); sticks = struct.unpack('<4h', pad) if pad else None
        dis = m.i16(0xB73458 + 0x10E) if True else 0
        pst = m.u32(ped + 0x530) if ped else 0
        pdata = m.u32(ped + 0x480) if ped else 0
        tms = m.u32(0xB7CB84)
        veh = m.u32(ped + 0x58C) if ped else 0
        scs = []
        sc = m.u32(0xA8B42C)
        for _ in range(6):
            if not sc: break
            nm = (m.rd(sc + 8, 8) or b'').split(b'\x00')[0].decode('latin1')
            scs.append('%s@%X' % (nm, m.u32(sc + 0x14) - 0xA49960)); sc = m.u32(sc)
        if gs == 9 and e9 is None: e9 = time.time(); P('gGameState=9 a los %.0f s' % (e9 - t0))
        if time.time() - ult >= 2:
            ult = time.time()
            P('t=%.0f gs=%d fr=%d cs=%d pos=%s speed=%s sticks=%s keyW=%s disableCtl=%d pedState=%d veh=%X tms=%d scripts=%s' % (time.time() - t0, gs, fr, cs, pos, sp, sticks, (m.rd(0xB73190 + 0x1B, 1) or b"?")[0:1], dis, pst, veh, tms, ' '.join(scs)))
        libre = (cs == 0 and tms > 120000 and dis == 0 and cs == 0 and veh == 0 and pos is not None)
        if libre and listo is None: listo = time.time(); P('CONTROL LIBRE aparente a los %.0f s (tms=%d) pos=%s' % (time.time() - t0, tms, pos))
        if not libre: listo = None
        if entrada and e9 and not hecho and listo and time.time() - listo > 8:
            hecho = True; P('ENTRADA modo', modo, 'desde', pos)
            if captura: P(captura.capturar('%s/mv_%s_antes.png' % (OUT, et), traer=False))
            if modo in ('tecla', 'ambos') and captura:
                h = captura.buscar(); captura.u.ShowWindow(h, 9); captura.u.SetForegroundWindow(h); time.sleep(1)
                captura.u.keybd_event(0x57, 0x11, 0, 0)  # W (VK 0x57, scancode 0x11)
            for i in range(200):
                if modo in ('pad', 'ambos'): m.wr(0xB73458 + 2, struct.pack('<h', -128))
                time.sleep(0.02)
                if i % 40 == 0:
                    inf = m.u32(0xB7CD74); ped = m.u32(0xB7CD98 + inf * 0x190); mat = m.u32(ped + 0x14) if ped else 0
                    P('  durante: pos', (m.f32(mat + 0x30), m.f32(mat + 0x34)) if mat else None, 'sticks', struct.unpack('<4h', m.rd(0xB73458, 8)))
            if modo in ('tecla', 'ambos') and captura: captura.u.keybd_event(0x57, 0x11, 2, 0)
            if captura: P(captura.capturar('%s/mv_%s_despues.png' % (OUT, et), traer=False))
            P('POS tras entrada', pos, '->', (m.f32(m.u32(m.u32(0xB7CD98 + inf * 0x190) + 0x14) + 0x30), m.f32(m.u32(m.u32(0xB7CD98 + inf * 0x190) + 0x14) + 0x34)))
        time.sleep(0.25)
    P('fin')
sys.exit(main())
