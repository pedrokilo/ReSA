"""Compila varios bancos congelados juntos y ejecuta cada uno en un proceso separado.

El manifiesto de cada banco incluye todos los instrumentos y fuentes del grupo.
Los marcadores habituales de runtime activan sólo el banco solicitado.
"""
import argparse
import hashlib
import json
import os
import re
import runpy
import shutil
import subprocess
import sys
from pathlib import Path

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from probe_build import build
from runtime_runner import GAME,digest

def atomic_copy(source,destination):
    destination.parent.mkdir(parents=True,exist_ok=True)
    temporary=destination.with_name(destination.name+f'.copy-{os.getpid()}')
    try:
        shutil.copy2(source,temporary)
        temporary.replace(destination)
    finally:
        if temporary.exists():temporary.unlink()

def archive_link(source,destination):
    """Comparte el archivo en NTFS cuando están en la misma unidad; sustituye de forma atómica."""
    destination.parent.mkdir(parents=True,exist_ok=True)
    temporary=destination.with_name(destination.name+f'.link-{os.getpid()}')
    try:
        try:os.link(source,temporary)
        except OSError:shutil.copy2(source,temporary)
        temporary.replace(destination)
    finally:
        if temporary.exists():temporary.unlink()

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('banks',nargs='+')
    parser.add_argument('--no-install',action='store_true',help='Compila y archiva sin tocar el juego ni ejecutar.')
    parser.add_argument('--full-source-manifest',action='store_true',help='Huella conservadora de todos los headers y CPP modificados para el replay final.')
    args=parser.parse_args()
    names=args.banks
    if len(names)!=len(set(names)):raise RuntimeError('Banco duplicado')
    plans=[];sources=set();instruments=set()
    for name in names:
        if Path(name).name!=name:raise RuntimeError('Se requiere el nombre de un banco hermano')
        bank=HERE.parent/name
        builder=bank/'build_probe.py';content=builder.read_text(encoding='utf-8-sig')
        matched=re.search(r'build\([^,]+,\s*[\'\"]([^\'\"]+)[\'\"],\s*[\'\"]([^\'\"]+)[\'\"]',content)
        if not matched:raise RuntimeError('Builder no reconocido: '+name)
        prefix,namespace=matched.groups()
        sources.update(runpy.run_path(str(builder))['SOURCES'])
        instruments.update(bank.glob('*.hpp'));instruments.update(bank.glob('*.py'))
        plans.append((bank,prefix,namespace))
    if args.full_source_manifest:
        sources.update(p.relative_to(ROOT).as_posix() for p in (ROOT/'source').rglob('*') if p.is_file() and p.suffix in ('.h','.hpp','.inl','.inc'))
        changed=subprocess.check_output(['git','diff','HEAD','--name-only','--','source'],cwd=ROOT,text=True).splitlines()
        changed+=subprocess.check_output(['git','ls-files','--others','--exclude-standard','--','source'],cwd=ROOT,text=True).splitlines()
        sources.update(p.replace('\\','/') for p in changed if p.endswith('.cpp') and p.replace('\\','/')!='source/game_sa/Game.cpp' and (ROOT/p).is_file())
    group=hashlib.sha256('\n'.join(names).encode()).hexdigest()[:12]
    driver=HERE/'batches'/group;driver.mkdir(parents=True,exist_ok=True)
    header=driver/'LiveComparison.hpp'
    text='#pragma once\n'+''.join(f'#include "../../../{bank.name}/LiveComparison.hpp"\n' for bank,_,_ in plans)
    text+='namespace Pending355BatchLive { inline void Update() {\n'+''.join(f'    {namespace}::Update();\n' for _,_,namespace in plans)+'} }\n'
    # Mantener fecha y hash del driver si se repite el mismo grupo.
    if not header.exists() or header.read_text()!=text:header.write_text(text,encoding='utf-8')
    instruments.update((header,Path(__file__).resolve()))
    instruments.update(HERE.parent/p for p in ('ProbeRuntime.hpp','ProbeSupport.hpp','probe_case.py','runtime_runner.py','probe_build.py','probe_summary.py'))
    instrument_paths={p.relative_to(ROOT).as_posix():p for p in instruments}
    source_paths=sorted(sources)
    os.environ['CL']='/MP6'
    print('BUILD_GROUP '+','.join(names),flush=True)
    # El helper comprueba también que ningún instrumento cambió durante la compilación.
    hashes=build(driver,'pending355-batch-'+group,'Pending355BatchLive',sorted(sources|set(instrument_paths)),install=False)
    archived=json.loads((driver/'artifacts/build-manifest.json').read_text())
    tested_sources={p:archived['sources'][p] for p in source_paths}
    tested_instruments={p:archived['sources'][p] for p in instrument_paths}
    cache=Path.home()/'.codex/cache/gta355-evidence/batch-binaries'/group
    shared={}
    for ext in hashes:
        original=driver/'artifacts'/f'gta_reversed-pending355-batch-{group}-probe.{ext}'
        canonical=cache/f'probe.{ext}'
        atomic_copy(original,canonical)
        if digest(canonical)!=hashes[ext]:raise RuntimeError('Copia compartida distinta')
        shared[ext]=canonical
    for bank,prefix,_ in plans:
        print('ARCHIVE '+bank.name,flush=True)
        artifacts=bank/'artifacts';artifacts.mkdir(exist_ok=True)
        for ext in hashes:
            destination=artifacts/f'gta_reversed-{prefix}-probe.{ext}'
            archive_link(shared[ext],destination)
            if not destination.samefile(shared[ext]) and digest(destination)!=hashes[ext]:raise RuntimeError('Copia de sonda distinta')
            if not args.no_install:archive_link(shared[ext],GAME/'_baseline'/destination.name)
        manifest=dict(binaries=hashes,sources=tested_sources,game_cpp=archived['game_cpp'],
                      instrumentation={os.path.relpath(instrument_paths[p],bank).replace('\\','/'):value for p,value in tested_instruments.items()},
                      batch_driver=header.relative_to(ROOT).as_posix(),batch_banks=names,full_source_manifest=args.full_source_manifest)
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
    if args.no_install:return
    results=[]
    for bank,prefix,_ in plans:
        print('RUN '+bank.name,flush=True)
        ran=subprocess.run([sys.executable,str(bank/'run_local.py')],cwd=ROOT)
        results.append(dict(bank=bank.name,passed=ran.returncode==0))
        print('RESULT '+json.dumps(results[-1]),flush=True)
    print('BATCH_RESULT '+json.dumps(results),flush=True)
    if not all(r['passed'] for r in results):raise SystemExit(1)

if __name__=='__main__':main()
