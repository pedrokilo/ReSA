"""Control positivo del espia: abre un archivo llamado gta-sa.exe (un señuelo de texto creado aqui, NO el del usuario) desde un proceso x86 espiado y comprueba que el espia lo ve.
Sin esto, un log sin coincidencias no probaria nada."""
import os, sys, tempfile, ctypes
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import espiar
d = os.path.join(aqui, 'control_tmp'); os.makedirs(d, exist_ok=True)
senuelo = os.path.join(d, 'gta-sa.exe'); open(senuelo, 'w').write('senuelo de prueba del espia, no es un ejecutable\n')
log = os.path.join(d, 'control_positivo.log')
pid, h, ht = espiar.lanzar(r'C:\Windows\SysWOW64\cmd.exe', '/c type "%s" > NUL & if exist "%s" echo existe' % (senuelo, senuelo.replace('gta-sa', 'gta_sa')), d, os.path.join(aqui, 'espia_archivos.dll'), log)
ctypes.windll.kernel32.WaitForSingleObject(h, 15000)
t = open(log, errors='replace').read()
vistos = [l for l in t.splitlines() if 'gta-sa.exe' in l.lower() or 'gta_sa.exe' in l.lower()]
print('lineas del espia con gta-sa.exe / gta_sa.exe:', len(vistos)); [print('  ', l) for l in vistos]
print('CONTROL POSITIVO', 'OK' if vistos else 'FALLA'); sys.exit(0 if vistos else 1)
