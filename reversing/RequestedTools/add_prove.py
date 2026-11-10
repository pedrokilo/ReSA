#!/usr/bin/env python3
"""add_prove.py <Banco> <guion1> [<guion2> ...]: anade a build_probe.py una funcion prove() que, tras compilar la sonda, ejecuta (desde la raiz del repo)
los guiones indicados (rutas relativas a reversing/<Banco>/) y falla si alguno falla. Asi `bench_here.sh` regenera la prueba estatica del ASI compilado."""
import sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
bank, scripts = sys.argv[1], sys.argv[2:]
p = ROOT / 'reversing' / bank / 'build_probe.py'
s = p.read_text(encoding='utf-8')
if 'def prove()' in s: raise SystemExit('ya tiene prove()')
tail = "if __name__=='__main__':main()"
assert tail in s, 'sin cola esperada'
new = f'''def prove():
    # REPARAR-REQUESTED: la prueba estatica del ASI compilado se regenera aqui (antes eran pasos manuales) para que `bench_here.sh` baste.
    import subprocess as _sp, sys as _sys
    for script in {scripts!r}:
        done=_sp.run([_sys.executable,'-B',str(HERE/script)],cwd=ROOT,capture_output=True,text=True)
        (ART/(Path(script).stem+'.out')).write_text(done.stdout+done.stderr,encoding='utf8')
        if done.returncode:raise SystemExit('Fallo '+script+': '+(done.stderr or done.stdout)[-600:])
    print('Prueba estatica regenerada')
if __name__=='__main__':main();prove()'''
p.write_text(s.replace(tail, new), encoding='utf-8', newline='')
print('ok', bank)
