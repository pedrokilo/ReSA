# Ejecuta prueba_marcadores.py (wv-8/reversing/FnBIS1, sin modificarlo) pero reservando YO D:\Original\gta_lock (mkdir atomico, espera su turno) y pasando -SinCandado a jugar.ps1,
# para no competir con las demas pruebas que esperan a que el candado desaparezca y lanzan a la vez. Uso: python -I humo_marcadores_g5b.py <args> <etiqueta> (env AGUA_ASI)
import os, sys, time, shutil
aqui = r'D:\Original\wv-8\reversing\FnBIS1'
lock = r'D:\Original\gta_lock'
while True:
    try: os.mkdir(lock); break
    except FileExistsError: time.sleep(3)
open(os.path.join(lock, 'owner_datos_g5b_humo'), 'w').write('datos-g5b marcadores')
try:
    os.chdir(aqui); sys.path.insert(0, aqui)
    code = open(os.path.join(aqui, 'prueba_marcadores.py'), encoding='utf-8').read()
    code = code.replace("exec(compile(src, 'prueba_marcadores(ropero)', 'exec')", "src = src.replace(\"'-Auto','240'\", \"'-Auto','240','-SinCandado'\").replace(\" or os.path.exists('D:/Original/gta_lock')\", '')\nexec(compile(src, 'prueba_marcadores(ropero)', 'exec')")
    code = code.replace("aqui = os.path.dirname(os.path.abspath(__file__))", "aqui = r'%s'" % aqui, 1)
    sys.argv = [os.path.join(aqui, 'prueba_marcadores.py')] + sys.argv[1:]
    exec(compile(code, 'prueba_marcadores_g5b', 'exec'), {'__name__': '__main__', '__file__': sys.argv[0]})
finally:
    shutil.rmtree(lock, ignore_errors=True)
