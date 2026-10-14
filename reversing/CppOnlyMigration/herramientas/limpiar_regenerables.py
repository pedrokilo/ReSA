"""Borra los archivos regenerables que dejan los bancos, conservando los del lote en curso.

Política de retención: se conservan las CONSERVAR sondas más recientes (por defecto 0: se borran todas; ~1,7 GB cada
una) y se borra el resto. rerun_hist.py la aplica sola tras cada banco, así D: no vuelve a llenarse.

Uso manual (vista previa por defecto):
    python -B reversing/CppOnlyMigration/herramientas/limpiar_regenerables.py                 # solo muestra
    python -B reversing/CppOnlyMigration/herramientas/limpiar_regenerables.py --ejecutar      # borra (permanente)
    python -B reversing/CppOnlyMigration/herramientas/limpiar_regenerables.py --conservar 20  # otro tamaño de lote

Solo toca lo que build_probe.py y runtime_runner.py regeneran en cada corrida:
  reversing/<banco>/artifacts/gta_reversed-*-probe.pdb   (D:; se omiten uniones y enlaces)
  <clon>/_baseline/*.pdb                                  (copias de sondas de bancos anteriores)
  <clon>/resultados-*/<corrida>                           (informes crudos; la evidencia comprimida ya está en el repo)
  <clon>/*-runtime.jsonl, *-runtime.start, *-harness.jsonl  (trazas crudas)
No toca el juego, scripts/ del clon (asi/pdb activos), userfiles/, gta_sa.exe, los .asi de artifacts, los manifiestos
ni la evidencia comprimida del repo (reversing/*/runs).
"""
import argparse
import os
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CLON = Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')
CONSERVAR = 0


def _es_enlace(p: Path) -> bool:
    return p.is_symlink() or (hasattr(os.path, 'isjunction') and os.path.isjunction(p))


def _tam(p: Path) -> int:
    if p.is_file():
        return p.stat().st_size
    return sum(f.stat().st_size for f in p.rglob('*') if f.is_file())


def _juego_abierto() -> bool:
    salida = subprocess.run(['tasklist', '/FI', 'IMAGENAME eq gta_sa.exe'], capture_output=True, text=True).stdout
    return 'gta_sa.exe' in salida.lower()


def _clon_valido() -> bool:
    return (CLON / 'gta_sa.exe').is_file() and CLON.name.endswith('CVehicle Test')


def _pdb_artifacts():
    for d in (ROOT / 'reversing').iterdir():
        c = d / 'artifacts'
        if c.is_dir() and not _es_enlace(c):
            yield from (p for p in c.glob('gta_reversed-*-probe.pdb') if p.is_file() and not _es_enlace(p))


def _a_borrar(conservar):
    """Elementos regenerables fuera del lote en curso: todo salvo los `conservar` más recientes de cada tipo."""
    def antiguos(items):
        orden = sorted(items, key=lambda p: p.stat().st_mtime, reverse=True)
        return orden[conservar:]

    objetivos = antiguos(list(_pdb_artifacts()))
    if _clon_valido():
        objetivos += antiguos([p for p in (CLON / '_baseline').glob('*.pdb') if p.is_file()])
        corridas = [c for r in CLON.glob('resultados-*') if r.is_dir() for c in r.iterdir() if c.is_dir()]
        objetivos += antiguos(corridas)
        trazas = [f for f in CLON.iterdir() if f.is_file() and f.name.endswith(('-runtime.jsonl', '-runtime.start', '-harness.jsonl'))]
        objetivos += antiguos(trazas)
    return objetivos


def _borrar(objetivos, ejecutar):
    total = 0
    for o in objetivos:
        total += _tam(o)
        if ejecutar:
            shutil.rmtree(o, ignore_errors=True) if o.is_dir() else o.unlink(missing_ok=True)
    return total


def limpiar_banco(banco=None, carpeta_informe=None, conservar=CONSERVAR):
    """Tras copiar la evidencia de un banco: aplica la retención. Devuelve GB liberados. (Los argumentos de banco e
    informe se aceptan por compatibilidad: la política es global, no por banco.)"""
    if _juego_abierto():
        return 0.0
    return round(_borrar(_a_borrar(conservar), True) / 1024 ** 3, 2)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--ejecutar', action='store_true', help='borra de verdad (sin esto solo muestra)')
    ap.add_argument('--conservar', type=int, default=CONSERVAR, help='cuántos elementos recientes conservar (def. 0 = ninguno)')
    a = ap.parse_args()
    if _juego_abierto():
        raise SystemExit('Cierra gta_sa.exe antes de limpiar.')
    objetivos = _a_borrar(a.conservar)
    for o in objetivos:
        print(f'{_tam(o) / 1024 ** 3:6.2f} GB  {o}')
    total = _borrar(objetivos, a.ejecutar)
    print(f'{len(objetivos)} elementos, {total / 1024 ** 3:.2f} GB' + ('' if a.ejecutar else '  (vista previa: no se borró nada; usa --ejecutar)'))


if __name__ == '__main__':
    main()
