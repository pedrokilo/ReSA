"""Construye y ejecuta el arnes aislado de Dependencies25C (sin juego) y escribe offline/out/comparisons.jsonl.

  python -B offline/build.py [--root <repo>] [--overlay <carpeta>] [--out <carpeta>] [--no-run]

* Compila con cl.exe (x86) y las opciones de la configuracion Debug del proyecto (mkrsp.py -> out/proj.rsp): harness.cpp, exemem.cpp y los ocho fuentes RenderWare reconstruidos
  (source/game_sa/RenderWare/rw/{rpworld,rphanim,rpskin,rpuvanim,rtanim,rtdict,rwplcore,rwcore}.cpp), y los enlaza (exemem.obj primero, base 0x400000, seccion .exemem RWX).
* --overlay: carpeta con copias de fuentes (mismas rutas relativas a la raiz del repositorio) que tienen prioridad sobre las del arbol; lo usan mutations.py y las pruebas con parches.
* Solo lee el arbol: no escribe en source/ ni en build/. Requiere gta-sa.exe original en exebytes.inc (gen_exebytes.py ya genero exebytes.inc; se incluye en el banco).
"""
import argparse
import os
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
RW = ['rpworld', 'rphanim', 'rpskin', 'rpuvanim', 'rtanim', 'rtdict', 'rwplcore', 'rwcore']
RW_DIR = 'source/game_sa/RenderWare/rw'


def toolchain():
    vs = Path(os.environ.get('VS_INSTALL', r'C:\Program Files\Microsoft Visual Studio\18\Community'))
    msvc = sorted((vs / 'VC/Tools/MSVC').iterdir())[-1]
    kits = Path(r'C:\Program Files (x86)\Windows Kits\10')
    sdk = sorted(p.name for p in (kits / 'Include').iterdir() if p.name[0].isdigit())[-1]
    env = dict(os.environ)
    env['INCLUDE'] = ';'.join(str(p) for p in (msvc / 'include', kits / 'Include' / sdk / 'ucrt', kits / 'Include' / sdk / 'um', kits / 'Include' / sdk / 'shared'))
    env['LIB'] = ';'.join(str(p) for p in (msvc / 'lib/x86', kits / 'Lib' / sdk / 'ucrt/x86', kits / 'Lib' / sdk / 'um/x86'))
    return msvc / 'bin/Hostx64/x86', env


def build(root, overlay, out, run=True, log=print):
    out.mkdir(parents=True, exist_ok=True)
    bindir, env = toolchain()
    rsp = out / 'proj.rsp'
    subprocess.run([sys.executable, '-B', str(HERE / 'mkrsp.py'), str(root), str(rsp)], check=True, stdout=subprocess.DEVNULL)

    def compile(src, obj):
        r = subprocess.run([str(bindir / 'cl.exe'), '@' + str(rsp), '/Fo' + str(obj), '/Fd' + str(out / 'vc.pdb'), str(src)], cwd=out, env=env, capture_output=True, text=True, encoding='cp1252', errors='replace')
        if r.returncode:
            raise RuntimeError('cl.exe fallo en %s:\n%s' % (src, '\n'.join(l for l in r.stdout.splitlines() if 'error' in l)[:4000]))

    objs = []
    for name in RW:
        rel = '%s/%s.cpp' % (RW_DIR, name)
        src = (overlay / rel) if overlay and (overlay / rel).exists() else root / rel
        obj = out / (name + '.obj')
        compile(src, obj)
        objs.append(obj)
    compile(HERE / 'exemem.cpp', out / 'exemem.obj')
    compile(HERE / 'harness.cpp', out / 'harness.obj')
    exe = out / 'harness.exe'
    r = subprocess.run([str(bindir / 'link.exe'), '/NOLOGO', '/OUT:' + str(exe), '/BASE:0x400000', '/FIXED', '/DYNAMICBASE:NO', '/INCREMENTAL:NO', '/FORCE:UNRESOLVED', '/DEBUG:NONE',
                        '/SECTION:.exemem,ERW', str(out / 'exemem.obj'), str(out / 'harness.obj')] + [str(o) for o in objs] + ['kernel32.lib', 'user32.lib', 'dbghelp.lib'],
                       cwd=out, env=env, capture_output=True, text=True, encoding='cp1252', errors='replace')
    if not exe.exists():
        raise RuntimeError('link.exe fallo:\n' + r.stdout[-3000:])
    if not run:
        return exe, None
    jsonl = out / 'comparisons.jsonl'
    rc = subprocess.run([str(exe), str(jsonl)], cwd=out, capture_output=True, text=True)
    log(rc.stdout.strip() + ((' | ' + rc.stderr.strip()) if rc.stderr.strip() else ''))
    return exe, jsonl


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default=str(HERE.parents[2]))
    ap.add_argument('--overlay', default=None)
    ap.add_argument('--out', default=str(HERE / 'out'))
    ap.add_argument('--no-run', action='store_true')
    a = ap.parse_args()
    exe, jsonl = build(Path(a.root).resolve(), Path(a.overlay).resolve() if a.overlay else None, Path(a.out).resolve(), not a.no_run)
    print('arnes:', exe)
    if jsonl:
        print('registro:', jsonl)
