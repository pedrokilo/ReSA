"""Prueba del independiente con imagen incrustada vs el actual: arranca un exe en una carpeta de pruebas, captura el cliente completo (800x600) a tiempos fijos
desde el lanzamiento y guarda los logs. Toma D:\Original\gta_lock; mata solo su propio proceso.
Uso: python -I probar_imagen.py <etiqueta> <exe> <carpeta> <argumentos del exe...>   (tiempos: IMG_TIEMPOS="20,60,120,180,240", limite IMG_LIMITE=250)"""
import os, sys, time, subprocess, shutil, ctypes
ctypes.windll.user32.SetProcessDPIAware()
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import captura
et, exe, carpeta = sys.argv[1:4]
args = sys.argv[4:]
tiempos = [int(x) for x in os.environ.get('IMG_TIEMPOS', '20,60,120,180,240').split(',')]
limite = int(os.environ.get('IMG_LIMITE', str(tiempos[-1] + 15)))
sal = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'ejecuciones', '%s-%s' % (et, time.strftime('%Y%m%d-%H%M%S')))
os.makedirs(sal)
lock = r'D:\Original\gta_lock'
while True:
    try: os.mkdir(lock); break
    except FileExistsError: time.sleep(5)
open(os.path.join(lock, 'owner_imagen'), 'w').write(et)
try:
    while subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe'], capture_output=True, text=True).stdout.count('gta_sa.exe') or \
          subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa_standalone.exe'], capture_output=True, text=True).stdout.count('gta_sa_standalone.exe'):
        time.sleep(10)
    shutil.rmtree(os.path.join(carpeta, 'logs'), ignore_errors=True); os.makedirs(os.path.join(carpeta, 'logs'))
    cmd = [exe] + args + ['--salida', sal, '--limite', str(limite), '--directorio-juego', carpeta]
    open(os.path.join(sal, 'comando.txt'), 'w').write(subprocess.list2cmdline(cmd))
    t0 = time.time()
    p = subprocess.Popen(cmd, cwd=carpeta)
    res = open(os.path.join(sal, 'capturas.txt'), 'w')
    for t in tiempos:
        while time.time() - t0 < t and p.poll() is None: time.sleep(0.5)
        if p.poll() is not None: res.write('proceso terminado antes de t=%d: 0x%08X\n' % (t, p.returncode & 0xFFFFFFFF)); break
        r = captura.capturar(os.path.join(sal, 'cap_t%03d.png' % t), traer=True)
        res.write('t=%d %s\n' % (t, r)); res.flush()
    try: p.wait(timeout=limite + 40 - (time.time() - t0) if limite + 40 > time.time() - t0 else 1)
    except subprocess.TimeoutExpired: p.kill(); res.write('TIMEOUT externo, mato mi proceso\n')
    res.write('exit=0x%08X\n' % (p.returncode & 0xFFFFFFFF)); res.close()
    for f in os.listdir(os.path.join(carpeta, 'logs')): shutil.copy(os.path.join(carpeta, 'logs', f), sal)
finally:
    shutil.rmtree(lock, ignore_errors=True)
print(sal)
