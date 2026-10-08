"""Instala y ejecuta bancos ya compilados, tras revisar íntegramente sus manifiestos."""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME,digest
from batch_probe import archive_link

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('banks',nargs='+')
    args=parser.parse_args()
    checked={};plans=[]
    def checked_digest(path):
        stat=path.stat()
        key=(stat.st_dev,stat.st_ino,stat.st_size,stat.st_mtime_ns)
        if not stat.st_ino:key=(str(path.resolve()),stat.st_size,stat.st_mtime_ns)
        if key not in checked:checked[key]=digest(path)
        return checked[key]
    for name in args.banks:
        if Path(name).name!=name:raise RuntimeError('Banco fuera del alcance')
        bank=HERE.parent/name
        builder=(bank/'build_probe.py').read_text(encoding='utf-8-sig')
        prefix=re.search(r"build\([^,]+,\s*['\"]([^'\"]+)",builder).group(1)
        manifest=json.loads((bank/'artifacts/build-manifest.json').read_text())
        for path,value in manifest['sources'].items():
            if checked_digest(ROOT/path)!=value:raise RuntimeError('Fuente posterior al banco: '+path)
        for path,value in manifest['instrumentation'].items():
            if checked_digest(bank/path)!=value:raise RuntimeError('Instrumento posterior al banco: '+str(bank/path))
        for ext,value in manifest['binaries'].items():
            if checked_digest(bank/'artifacts'/f'gta_reversed-{prefix}-probe.{ext}')!=value:
                raise RuntimeError('Binario archivado distinto: '+name)
        plans.append((bank,prefix,manifest))
    # No instalar ningún archivo hasta que todos los bancos solicitados sean revisables.
    results=[]
    for bank,prefix,manifest in plans:
        for ext in manifest['binaries']:
            archived=bank/'artifacts'/f'gta_reversed-{prefix}-probe.{ext}'
            archive_link(archived,GAME/'_baseline'/archived.name)
        print('RUN '+bank.name,flush=True)
        ran=subprocess.run([sys.executable,str(bank/'run_local.py')],cwd=ROOT)
        results.append(dict(bank=bank.name,passed=ran.returncode==0))
        print('RESULT '+json.dumps(results[-1]),flush=True)
    print('ARCHIVED_RESULT '+json.dumps(results),flush=True)
    if not all(r['passed'] for r in results):raise SystemExit(1)

if __name__=='__main__':main()
