import os, re, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import main
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{m[0]}::{m[1]}': int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src)}
# Revalidacion aislada: FILELOADER47_ONLY="CFileLoader::LoadPickup" ejecuta (y exige) solo esa funcion. La sonda lee la 1.a linea de
# fileloader47-only.txt en la carpeta del juego; aqui se escribe antes de la corrida y se borra siempre al terminar.
ONLY = os.environ.get('FILELOADER47_ONLY', '').strip()
if ONLY:
    if ONLY not in EXPECTED: sys.exit(f'FILELOADER47_ONLY desconocida: {ONLY}')
    EXPECTED = {ONLY: EXPECTED[ONLY]}
if __name__ == '__main__':
    analyze_only = len(sys.argv) == 3 and sys.argv[1] == '--analyze'
    only_file = runtime_runner.GAME/'fileloader47-only.txt'
    if ONLY and not analyze_only: only_file.write_text(ONLY + '\n', encoding='utf-8')
    try:
        code = main(Path(__file__).parent, 'fileloader47', 'CFileLoader (lista 34): lectores dat/ide/ipl, colision, RW y LinkLods' + (f' [solo {ONLY}]' if ONLY else ''), EXPECTED, timeout=300 if ONLY else 1500)
    finally:
        if ONLY and not analyze_only and only_file.exists(): only_file.unlink()
    sys.exit(code)
