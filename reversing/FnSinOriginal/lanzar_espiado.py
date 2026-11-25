"""Lanza un exe x86 con el espia de archivos (espia_archivos.dll) inyectado antes de los callbacks TLS y se desengancha; imprime el PID en la ultima linea.
Uso: python -I lanzar_espiado.py <exe> <cwd> <espia.log> [args_linea...]   (el proceso lanzado sigue vivo; quien llama lo espera y lo cierra)"""
import os, sys
aqui = os.path.dirname(os.path.abspath(__file__)); sys.path.insert(0, aqui)
import espiar
exe, cwd, log = sys.argv[1:4]
args = ' '.join(sys.argv[4:])
pid, h, ht = espiar.lanzar(exe, args, cwd, os.path.join(aqui, 'espia_archivos.dll'), log)
print(pid)
