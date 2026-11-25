"""FnD3DX9C: ejecuta gta_sa_standalone.exe (sin gta-sa.exe) en una carpeta de juego propia con el espia de archivos inyectado antes de los callbacks TLS, captura la ventana
a los tiempos pedidos y, en cada uno, vuelca los modulos cargados del proceso (EnumProcessModulesEx, 32 bits) y cuenta los accesos a d3dx9*/d3dcompiler* que ve el espia.
Con --bloquear usa espia_bloqueo.dll y hace que no existan (STATUS_OBJECT_NAME_NOT_FOUND) los archivos cuyo nombre contenga esas subcadenas (p. ej. d3dx9_;d3dcompiler_4):
simula un Windows sin el Runtime de DirectX de 2010 sin tocar System32.
Toma D:\\Original\\gta_lock (espera si lo tiene otro, p. ej. el usuario; no mata nada ajeno) y mata solo su propio proceso.
Uso: python -I correr_d3dx.py <etiqueta> <carpeta_juego> <exe> [limite_s=130] [capturas=20,60,120] [--bloquear=sub;sub] [--] [argumentos del exe...]
Deja en ejecuciones\\<etiqueta>-<hora>\\: espia.log, resumen.txt, modulos_tNNN.txt, capturas cap_tNNN.png, logs del juego."""
import os, sys, time, subprocess, shutil, ctypes
from ctypes import wintypes as W
ctypes.windll.user32.SetProcessDPIAware()
aqui = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(aqui, '..', 'FnSinOriginal'))
sys.path.insert(0, os.path.join(aqui, '..', 'FnFase8'))
import espiar, captura

ps = ctypes.WinDLL('psapi', use_last_error=True)
ps.EnumProcessModulesEx.argtypes = [W.HANDLE, ctypes.c_void_p, W.DWORD, ctypes.POINTER(W.DWORD), W.DWORD]
ps.GetModuleFileNameExW.argtypes = [W.HANDLE, ctypes.c_void_p, W.LPWSTR, W.DWORD]


def modulos(h):
    arr = (ctypes.c_void_p * 2048)()
    need = W.DWORD()
    if not ps.EnumProcessModulesEx(h, arr, ctypes.sizeof(arr), ctypes.byref(need), 3):
        return ['ERROR EnumProcessModulesEx %d' % ctypes.get_last_error()]
    buf = ctypes.create_unicode_buffer(520)
    out = []
    for i in range(need.value // ctypes.sizeof(ctypes.c_void_p)):
        ps.GetModuleFileNameExW(h, ctypes.c_void_p(arr[i]), buf, 520)
        out.append(buf.value)
    return out


args = sys.argv[1:]
et, carpeta, exe = args[:3]
rest = args[3:]
limite, tiempos, bloquear, extra = 130, [20, 60, 120], '', []
pos = []
while rest:
    a = rest.pop(0)
    if a == '--':
        extra = rest
        break
    if a.startswith('--bloquear='):
        bloquear = a.split('=', 1)[1]
    else:
        pos.append(a)
if pos:
    limite = int(pos[0])
if len(pos) > 1:
    tiempos = [int(x) for x in pos[1].split(',')]
sal = os.path.join(aqui, 'ejecuciones', '%s-%s' % (et, time.strftime('%Y%m%d-%H%M%S')))
os.makedirs(sal)
lock = r'D:\Original\gta_lock'
while True:
    try:
        os.mkdir(lock)
        break
    except FileExistsError:
        time.sleep(5)
open(os.path.join(lock, 'owner_d3dx'), 'w').write(et)
res = open(os.path.join(sal, 'resumen.txt'), 'w', buffering=1)
try:
    def vivos():
        o = subprocess.run(['tasklist', '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout.lower()
        return [n for n in ('gta_sa.exe', 'gta_sa_standalone.exe', 'gta_reversed_exe.exe') if '"%s"' % n in o]
    while vivos():
        time.sleep(10)
    exe_dst = os.path.join(carpeta, 'gta_sa_standalone.exe')
    shutil.copy2(exe, exe_dst)
    exes = [f for f in os.listdir(carpeta) if f.lower().endswith('.exe')]
    res.write('exe origen: %s\nexe en la carpeta de juego: %s\n' % (exe, exes))
    res.write('dlls en la carpeta de juego: %s\n' % [f for f in os.listdir(carpeta) if f.lower().endswith('.dll')])
    assert not [f for f in os.listdir(carpeta) if f.lower() in ('gta-sa.exe', 'gta_sa.exe')], 'la carpeta contiene el exe original'
    # el juego reescribe gta_sa.set al salir (resolucion, etc.): se restaura la copia base para que todas las ejecuciones arranquen igual
    shutil.copy2(os.path.join(aqui, 'gta_sa.set.base'), os.path.join(carpeta, 'userfiles', 'gta_sa.set'))
    shutil.rmtree(os.path.join(carpeta, 'logs'), ignore_errors=True)
    os.makedirs(os.path.join(carpeta, 'logs'))
    line = '--directorio-juego "%s" --salida "%s" --limite %d %s' % (carpeta, sal, limite, ' '.join(extra))
    open(os.path.join(sal, 'comando.txt'), 'w').write('"%s" %s\nbloquear=%r' % (exe_dst, line, bloquear))
    if bloquear:
        os.environ['ESPIA_BLOQUEAR'] = bloquear
        dll_espia = os.path.join(aqui, 'out', 'espia_bloqueo.dll')
    else:
        os.environ.pop('ESPIA_BLOQUEAR', None)
        dll_espia = os.path.join(aqui, 'out', 'espia_bloqueo.dll')   # sin ESPIA_BLOQUEAR solo observa
    t0 = time.time()
    pid, h, ht = espiar.lanzar(exe_dst, line, carpeta, dll_espia, os.path.join(sal, 'espia.log'))
    res.write('pid=%d lanzado con espia tras %.2f s\n' % (pid, time.time() - t0))
    k = ctypes.windll.kernel32
    k.WaitForSingleObject.argtypes = [W.HANDLE, W.DWORD]
    for t in tiempos:
        while time.time() - t0 < t and k.WaitForSingleObject(h, 500) == 0x102:
            pass
        if k.WaitForSingleObject(h, 0) != 0x102:
            c = ctypes.c_ulong()
            k.GetExitCodeProcess(h, ctypes.byref(c))
            res.write('proceso terminado antes de t=%d: 0x%08X\n' % (t, c.value))
            break
        m = modulos(h)
        open(os.path.join(sal, 'modulos_t%03d.txt' % t), 'w').write('\n'.join(m))
        d3 = [x for x in m if 'd3dx9' in os.path.basename(x).lower() or 'd3dcompiler' in os.path.basename(x).lower()]
        try:
            r = captura.capturar(os.path.join(sal, 'cap_t%03d.png' % t), traer=True)
        except Exception as e:
            r = 'ERROR captura %r' % e
        res.write('t=%d vivo, %d modulos, d3dx9*/d3dcompiler* cargados: %s ; captura %s\n' % (t, len(m), d3, r))
    if k.WaitForSingleObject(h, max(1, int((limite + 40 - (time.time() - t0)) * 1000))) == 0x102:
        k.TerminateProcess(h, 1)
        res.write('TIMEOUT externo: mato mi proceso\n')
    c = ctypes.c_ulong()
    k.GetExitCodeProcess(h, ctypes.byref(c))
    res.write('exit=0x%08X duracion=%.1f s\n' % (c.value, time.time() - t0))
    time.sleep(1)
    for f in os.listdir(os.path.join(carpeta, 'logs')):
        shutil.copy(os.path.join(carpeta, 'logs', f), sal)
    # resumen del espia
    n = nb = 0
    ejemplos = []
    for ln in open(os.path.join(sal, 'espia.log'), encoding='latin1'):
        low = ln.lower()
        nombre = low.rstrip().replace('/', chr(92)).split(chr(92))[-1]   # solo el nombre del archivo (la carpeta FnD3DX9C de las salidas no cuenta)
        if 'd3dx9' in nombre or 'd3dcompiler' in nombre:
            n += 1
            if 'bloqueado' in low:
                nb += 1
            if len(ejemplos) < 12:
                ejemplos.append(ln.strip())
    res.write('espia: %d accesos a d3dx9*/d3dcompiler*, %d bloqueados\n' % (n, nb))
    for e in ejemplos:
        res.write('   %s\n' % e)
finally:
    res.close()
    shutil.rmtree(lock, ignore_errors=True)
print(sal)
