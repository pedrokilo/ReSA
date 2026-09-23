"""One local entry point for the existing GTA comparison runners."""
import argparse
import json
import subprocess
import sys
from pathlib import Path

def main():
    root=Path(__file__).resolve().parent
    cases=json.loads((root/'cases.json').read_text(encoding='utf-8'))
    parser=argparse.ArgumentParser(description='Pruebas locales de gta-reversed, sin IA')
    parser.add_argument('--list',action='store_true')
    parser.add_argument('--case',choices=[c['id'] for c in cases])
    parser.add_argument('--all', action='store_true', help='Ejecutar todos los bancos dentro de GTA, en serie')
    args=parser.parse_args()
    if args.all:
        if args.case or args.list: parser.error('--all no se combina con --case o --list')
        from run_all import main as run_all
        return run_all()
    if not args.case:
        print('Comparaciones automatizadas; no son demostraciones visuales.\n')
        for i,c in enumerate(cases,1):print(f"{i}. {c['name']} [{c['id']}]")
        if args.list:return 0
        value=input('\nNúmero de prueba (Enter para salir): ').strip()
        if not value:return 0
        if not value.isdigit() or not 1<=int(value)<=len(cases):
            print('Opción inválida.');return 2
        selected=cases[int(value)-1]
    else:selected=next(c for c in cases if c['id']==args.case)
    runner=(root/selected['runner']).resolve()
    if not runner.is_relative_to(root) or not runner.is_file():
        print('Ejecutor no disponible.');return 2
    return subprocess.run([sys.executable,str(runner)],cwd=root.parent).returncode

if __name__=='__main__':raise SystemExit(main())
