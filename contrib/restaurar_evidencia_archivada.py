#!/usr/bin/env python3
"""Restaura (o verifica) la evidencia pesada archivada fuera del repositorio.

El manifiesto `reversing/CppTwelveLevel6Final/ARCHIVO_MANIFIESTO.csv` lista cada
archivo con su sha256. Modos:
  --verificar            comprueba el archivo local contra el manifiesto (no escribe)
  (por defecto)          copia desde el archivo local a su ruta, verificando sha256
  --desde-git COMMIT     extrae del historial de git (`git show COMMIT:ruta`)
  --mover                con el archivo local: mueve en vez de copiar (mismo volumen)
Filtros: --filtro SUBCADENA (puede repetirse) limita a las rutas que la contienen.
Un archivo cuyo sha256 no coincide NUNCA se deja en su sitio; el codigo de salida es 1.

Ejemplos:
  python contrib/restaurar_evidencia_archivada.py --verificar
  python contrib/restaurar_evidencia_archivada.py --filtro CppTwelveLevel6Final/runs/Anim16
  python contrib/restaurar_evidencia_archivada.py --desde-git a00752a002
"""
import argparse, csv, hashlib, os, shutil, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MANIFIESTO = ROOT / 'reversing/CppTwelveLevel6Final/ARCHIVO_MANIFIESTO.csv'


def sha256(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        while chunk := f.read(8 << 20):
            h.update(chunk)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--archivo', default=str(ROOT.parent / 'evidencia_validacion_archivo'),
                    help='directorio del archivo local (por defecto ../evidencia_validacion_archivo)')
    ap.add_argument('--desde-git', metavar='COMMIT')
    ap.add_argument('--verificar', action='store_true')
    ap.add_argument('--mover', action='store_true')
    ap.add_argument('--filtro', action='append', default=[])
    a = ap.parse_args()
    arch = Path(a.archivo)
    with open(MANIFIESTO, newline='', encoding='utf-8') as f:
        filas = [r for r in csv.DictReader(f)
                 if not a.filtro or any(s in r['ruta_relativa'] for s in a.filtro)]

    def uno(r):
        ruta, esperado, tam = r['ruta_relativa'], r['sha256'], int(r['bytes'])
        dst, src = ROOT / ruta, arch / ruta
        if a.verificar:
            if not src.is_file():
                return ruta, 'FALTA en el archivo'
            return ruta, None if src.stat().st_size == tam and sha256(src) == esperado else 'sha256 NO coincide'
        if dst.is_file() and dst.stat().st_size == tam and sha256(dst) == esperado:
            return ruta, None                      # ya restaurado y correcto
        dst.parent.mkdir(parents=True, exist_ok=True)
        tmp = dst.with_name(dst.name + '.restaurando')
        try:
            if a.desde_git:
                with open(tmp, 'wb') as out:
                    subprocess.run(['git', 'show', f'{a.desde_git}:{ruta}'], cwd=ROOT, stdout=out, check=True)
            elif not src.is_file():
                return ruta, 'FALTA en el archivo'
            else:
                shutil.copyfile(src, tmp)
            if sha256(tmp) != esperado:
                return ruta, 'sha256 NO coincide tras restaurar'
            os.replace(tmp, dst)
            if a.mover and not a.desde_git:
                src.unlink()
            return ruta, None
        except subprocess.CalledProcessError:
            return ruta, f'no existe en {a.desde_git}'
        finally:
            if tmp.exists():
                tmp.unlink()

    with ThreadPoolExecutor(4) as ex:
        res = list(ex.map(uno, filas))
    malos = [(r, e) for r, e in res if e]
    for r, e in malos[:50]:
        print(f'ERROR {r}: {e}', file=sys.stderr)
    print(f'{len(res) - len(malos)}/{len(res)} correctos' + (' (verificacion)' if a.verificar else ''))
    return 1 if malos else 0


if __name__ == '__main__':
    sys.exit(main())
