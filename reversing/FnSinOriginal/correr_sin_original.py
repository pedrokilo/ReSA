"""Ejecuta gta_sa_standalone.exe SIN gta-sa.exe/gta_sa.exe en la carpeta de juego y SIN --imagen-externa/--ruta-original, con el espia de archivos
(espia_archivos.dll) inyectado antes de los callbacks TLS. Toma D:\\Original\\gta_lock (espera; no mata nada ajeno) y mata solo su propio proceso.
Uso: python -I correr_sin_original.py <etiqueta> <carpeta_juego> [limite_s=330] [capturas="20,60,120,180,240,300"] [exe_args_extra...]
Deja en ejecuciones\\<etiqueta>-<hora>\\: espia.log (todos los accesos), fase8_arranque.log, capturas, resumen.txt."""
import os, sys, time, subprocess, shutil, ctypes
ctypes.windll.user32.SetProcessDPIAware()
aqui = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, aqui)
sys.path.insert(0, os.path.join(aqui, '..', 'FnFase8'))
import espiar, captura
et, carpeta = sys.argv[1:3]
limite = int(sys.argv[3]) if len(sys.argv) > 3 else 330
tiempos = [int(x) for x in (sys.argv[4] if len(sys.argv) > 4 else '20,60,120,180,240,300').split(',')]
extra = sys.argv[5:]
sal = os.path.join(aqui, 'ejecuciones', '%s-%s' % (et, time.strftime('%Y%m%d-%H%M%S')))
os.makedirs(sal)
lock = r'D:\Original\gta_lock'
while True:
    try: os.mkdir(lock); break
    except FileExistsError: time.sleep(5)
open(os.path.join(lock, 'owner_sin_original'), 'w').write(et)
res = open(os.path.join(sal, 'resumen.txt'), 'w', buffering=1)
try:
    def vivos():
        o = subprocess.run(['tasklist', '/FO', 'CSV', '/NH'], capture_output=True, text=True).stdout.lower()
        return [n for n in ('gta_sa.exe', 'gta_sa_standalone.exe', 'gta_reversed_exe.exe') if '"%s"' % n in o]
    while vivos(): time.sleep(10)
    exe = os.path.join(carpeta, 'gta_sa_standalone.exe')
    exes = [f for f in os.listdir(carpeta) if f.lower().endswith('.exe')]
    res.write('exe en la carpeta de juego: %s\n' % exes)
    assert not [f for f in os.listdir(carpeta) if f.lower() in ('gta-sa.exe', 'gta_sa.exe')], 'la carpeta contiene el exe original'
    shutil.rmtree(os.path.join(carpeta, 'logs'), ignore_errors=True); os.makedirs(os.path.join(carpeta, 'logs'))
    args = '--directorio-juego "%s" --salida "%s" --limite %d %s' % (carpeta, sal, limite, ' '.join(extra))
    open(os.path.join(sal, 'comando.txt'), 'w').write('"%s" %s' % (exe, args))
    t0 = time.time()
    pid, h, ht = espiar.lanzar(exe, args, carpeta, os.path.join(aqui, 'espia_archivos.dll'), os.path.join(sal, 'espia.log'))
    res.write('pid=%d lanzado con espia (inyeccion antes del TLS) tras %.2f s\n' % (pid, time.time() - t0))
    k = ctypes.windll.kernel32
    for t in tiempos:
        while time.time() - t0 < t and k.WaitForSingleObject(h, 500) == 0x102: pass
        if k.WaitForSingleObject(h, 0) != 0x102:
            c = ctypes.c_ulong(); k.GetExitCodeProcess(h, ctypes.byref(c)); res.write('proceso terminado antes de t=%d: 0x%08X\n' % (t, c.value)); break
        try: r = captura.capturar(os.path.join(sal, 'cap_t%03d.png' % t), traer=True)
        except Exception as e: r = 'ERROR captura %r' % e
        res.write('t=%d vivo %s\n' % (t, r))
    if k.WaitForSingleObject(h, max(1, int((limite + 40 - (time.time() - t0)) * 1000))) == 0x102:
        k.TerminateProcess(h, 1); res.write('TIMEOUT externo: mato mi proceso\n')
    c = ctypes.c_ulong(); k.GetExitCodeProcess(h, ctypes.byref(c))
    res.write('exit=0x%08X duracion=%.1f s\n' % (c.value, time.time() - t0))
    time.sleep(1)
    for f in os.listdir(os.path.join(carpeta, 'logs')): shutil.copy(os.path.join(carpeta, 'logs', f), sal)
finally:
    res.close()
    shutil.rmtree(lock, ignore_errors=True)
print(sal)
