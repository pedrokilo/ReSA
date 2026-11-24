# Prueba de la sustitucion de DirectDraw por D3D9 (docs/RenderWareExe/directdraw-sustitucion.md).
# Lanza UN juego (toma D:\Original\gta_lock, espera a que no haya gta_sa*.exe vivos, solo cierra su propio proceso) y mide POR MEMORIA:
#   - s_OSStatus: DxVer (0xC8CF6C), VRAM.Total (0xC8CF80) y VRAM.Avail (0xC8CF84)
#   - tabla de modos de RenderWare (0xC97C48, 0x14 bytes por entrada: ancho, alto, frecuencia, formato D3D, bandera; numero en 0xC97C40)
#   - lista del menu gVideoModes (0xC920D0, char* por modo, nulo = filtrado) y modo actual (0x8D6220)
#   - modulos cargados en el proceso (EnumProcessModulesEx): si esta ddraw.dll
# y guarda JSON + capturas en reversing/FnFase8/ejecuciones/dd-<etiqueta>/. Opcional --teclas "esc,down,enter" (con pausas) y captura tras cada paso.
# Modos:  orig  gta_sa.exe ORIGINAL puro (sin gta_reversed.asi) en D:\Original\_dd_run\orig (dinput8.dll + redir_docs.asi: "Mis documentos" redirigido a esa carpeta)
#         asi   gta_sa.exe + gta_reversed.asi (--asi ruta) en D:\Original\_dd_run\asi
#         std   gta_sa_standalone.exe (--exe ruta) en D:\Original\_dd_run\std
# Uso: python probar_directdraw.py <etiqueta> <orig|asi|std> [--exe X] [--asi X] [--espera 45] [--teclas "a,b"] [--ini "Clave = valor;..."]
import ctypes, json, os, shutil, struct, subprocess, sys, time
from ctypes import wintypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import captura, importlib.util
k = ctypes.WinDLL('kernel32', use_last_error=True); k.OpenProcess.restype = wintypes.HANDLE
ps = ctypes.WinDLL('psapi', use_last_error=True)
LOCK = r'D:\Original\gta_lock'
BASE = r'D:\Original\_dd_run'
FUENTE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\userfiles'
FMT = {21: 'A8R8G8B8', 22: 'X8R8G8B8', 23: 'R5G6B5', 24: 'X1R5G5B5', 25: 'A1R5G5B5', 20: 'R8G8B8', 35: 'A2R10G10B10'}

class M:
    def __init__(s, p): s.h = k.OpenProcess(0x38 | 0x400 | 0x10, False, p)
    def rd(s, a, n):
        b = ctypes.create_string_buffer(n); r = ctypes.c_size_t()
        return b.raw if k.ReadProcessMemory(s.h, ctypes.c_void_p(a), b, n, ctypes.byref(r)) else None
    def u32(s, a): b = s.rd(a, 4); return struct.unpack('<I', b)[0] if b else 0
    def modulos(s):
        arr = (wintypes.HMODULE * 1024)(); need = wintypes.DWORD()
        ps.EnumProcessModulesEx.argtypes = [wintypes.HANDLE, ctypes.c_void_p, wintypes.DWORD, ctypes.c_void_p, wintypes.DWORD]
        if not ps.EnumProcessModulesEx(s.h, arr, ctypes.sizeof(arr), ctypes.byref(need), 3): return None
        out = []
        for i in range(need.value // ctypes.sizeof(wintypes.HMODULE)):
            nm = ctypes.create_unicode_buffer(520)
            ps.GetModuleFileNameExW.argtypes = [wintypes.HANDLE, wintypes.HMODULE, ctypes.c_wchar_p, wintypes.DWORD]
            ps.GetModuleFileNameExW(s.h, arr[i], nm, 520); out.append(nm.value)
        return out

def medir(m):
    r = {'DxVer': hex(m.u32(0xC8CF6C)), 'VRAM_Total': m.u32(0xC8CF80), 'VRAM_Avail': m.u32(0xC8CF84), 'modo_actual_rw': m.u32(0xC97C18), 'gCurrentVideoMode': m.u32(0x8D6220)}
    n = m.u32(0xC97C40); tab = m.u32(0xC97C48); r['num_modos_rw'] = n
    modos = []
    if tab and 0 < n < 4096:
        b = m.rd(tab, n * 0x14) or b''
        for i in range(len(b) // 0x14):
            w, h, hz, f, fl = struct.unpack_from('<5I', b, i * 0x14); modos.append([w, h, hz, FMT.get(f, f), fl])
    r['tabla_rw'] = modos
    r['formatos_0x884788'] = [m.u32(0x884788 + 4 * i) for i in range(3)]
    lst = m.u32(0xC920D0); r['gVideoModes_ptr'] = hex(lst); menu = []
    if lst and 0 < n < 4096:
        for i in range(n):
            p = m.u32(lst + 4 * i)
            if not p: menu.append(None); continue
            s = m.rd(p, 40) or b''; menu.append(s.split(b'\0')[0].decode('ascii', 'replace'))
    r['lista_menu'] = menu
    r['lista_menu_no_nula'] = [x for x in menu if x]
    return r

def lanzar(modo, a, sal):
    exe = a['exe']; dest = os.path.join(BASE, modo)
    docs = os.path.join(dest, 'docs'); env = os.environ.copy(); env['COMPARAR_DOCS'] = docs
    for x in os.listdir(os.path.join(dest, 'logs')) if os.path.isdir(os.path.join(dest, 'logs')) else []:
        try: os.remove(os.path.join(dest, 'logs', x))
        except OSError: pass
    ini = "[FastLoader]\r\nEnable = true\r\nNoEAX = true\r\nNoNVidia = true\r\nNoLogo = true\r\nNoTitleOrIntro = true\r\nNoCopyright = true\r\nNoFading = true\r\nNoLoadingTune = true\r\nRenderAtAllTimes = true\r\n" + ''.join(x.strip() + '\r\n' for x in a['ini'].split(';') if x.strip())
    ini += "[Misc]\r\nSkipDeviceSelection = true\r\nSaveDirectoryPath = %s\\userfiles\r\nLoadSavesWithMismatchingVersion = true\r\n" % dest
    # copia fresca del ajuste grafico (gta_sa.set) de la copia de pruebas: todos los modos parten de lo mismo
    df = os.path.join(docs, 'GTA San Andreas User Files'); os.makedirs(df, exist_ok=True)
    for f in ('GTASAsf2.b', 'gta_sa.set'):
        shutil.copy(os.path.join(FUENTE, f), df); os.makedirs(os.path.join(dest, 'userfiles'), exist_ok=True); shutil.copy(os.path.join(FUENTE, f), os.path.join(dest, 'userfiles'))
    if modo == 'std':
        shutil.copy(exe, os.path.join(dest, 'gta_sa_standalone.exe'))
        open(os.path.join(dest, 'gta-reversed.ini'), 'w', encoding='ascii').write(ini)
        args = f'--ruta-original "D:\\Grand Theft Auto San Andreas\\gta-sa.exe" --directorio-juego "{dest}" --salida "{sal}" --limite 0 --shims'
        return subprocess.Popen(f'"{os.path.join(dest, "gta_sa_standalone.exe")}" {args}', cwd=dest, env=env)
    sc = os.path.join(dest, 'scripts'); os.makedirs(sc, exist_ok=True)
    if modo == 'asi':
        shutil.copy(a['asi'], os.path.join(sc, 'gta_reversed.asi'))
        open(os.path.join(dest, 'gta-reversed.ini'), 'w', encoding='ascii').write(ini); shutil.copy(os.path.join(dest, 'gta-reversed.ini'), os.path.join(sc, 'gta-reversed.ini'))
    return subprocess.Popen(f'"{os.path.join(dest, "gta_sa.exe")}"', cwd=dest, env=env)

def main():
    et, modo = sys.argv[1], sys.argv[2]
    g = lambda n, d: sys.argv[sys.argv.index(n) + 1] if n in sys.argv else d
    a = {'exe': g('--exe', r'D:\Original\wv-dd\reversing\FnFase8\salida\gta_sa_standalone.exe'), 'asi': g('--asi', ''), 'ini': g('--ini', '')}
    espera = int(g('--espera', '45')); teclas = [t for t in g('--teclas', '').split(',') if t]
    sal = os.path.join(aqui, 'ejecuciones', 'dd-' + et); os.makedirs(sal, exist_ok=True)
    f = open(os.path.join(sal, 'log.txt'), 'w', encoding='utf-8')
    def P(*x):
        s = ' '.join(str(i) for i in x); print(s, flush=True); f.write(s + '\n'); f.flush()
    while True:
        try: os.mkdir(LOCK); break
        except FileExistsError: time.sleep(5)
    open(LOCK + r'\owner_dd', 'w').write('dd-' + et)
    p = None
    try:
        while any(subprocess.run(['tasklist', '/FI', 'IMAGENAME eq ' + n], capture_output=True, text=True).stdout.count(n) for n in ('gta_sa_standalone.exe', 'gta_sa.exe', 'gta_reversed_exe.exe')): time.sleep(10)
        p = lanzar(modo, a, sal); P('lanzado', modo, 'pid', p.pid)
        m = M(p.pid); t0 = time.time(); r = {}
        # espera a que RenderWare haya publicado los modos y el menu tenga su lista
        while time.time() - t0 < espera:
            time.sleep(2)
            if p.poll() is not None: P('EL PROCESO TERMINO con', p.returncode); break
        if p.poll() is None:
            r = medir(m); r['modulos'] = [os.path.basename(x) for x in (m.modulos() or [])]
            r['ddraw_cargada'] = any(x.lower() == 'ddraw.dll' for x in r['modulos'])
            h = captura.buscar(); P('ventana', h)
            if h and teclas:
                sys.path.insert(0, os.path.join(aqui, '..', 'FnComparar')); import teclas as T
                ctypes.WinDLL('user32').keybd_event(0x12, 0, 0, 0); ctypes.WinDLL('user32').keybd_event(0x12, 0, 2, 0)
                captura.u.ShowWindow(h, 9); captura.u.SetForegroundWindow(h); time.sleep(1)
                for i, t in enumerate(teclas):
                    T.tecla(t); time.sleep(1.2)
                    if t.startswith('shot') or i == len(teclas) - 1: pass
                    P('tecla', t, captura.capturar(os.path.join(sal, 'paso%02d_%s.png' % (i, t)), traer=False))
                r['lista_menu_tras_teclas'] = medir(m)['lista_menu_no_nula']
            elif h:
                P(captura.capturar(os.path.join(sal, 'captura.png'), traer=False))
            r['vivo_al_final'] = p.poll() is None
        json.dump(r, open(os.path.join(sal, 'medida.json'), 'w', encoding='utf-8'), indent=1)
        P(json.dumps({k2: v for k2, v in r.items() if k2 not in ('tabla_rw', 'lista_menu', 'modulos')}))
    finally:
        if p and p.poll() is None: p.kill()
        d = os.path.join(BASE, modo, 'logs')
        for x in os.listdir(d) if os.path.isdir(d) else []:
            try: shutil.copy(os.path.join(d, x), os.path.join(sal, x))
            except OSError: pass
        try:
            if os.path.exists(LOCK + r'\owner_dd'): os.remove(LOCK + r'\owner_dd')
            os.rmdir(LOCK)
        except OSError: pass

if __name__ == '__main__':
    main()
