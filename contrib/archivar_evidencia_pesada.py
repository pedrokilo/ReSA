#!/usr/bin/env python3
"""Archiva fuera del repositorio los volcados de evidencia pesados.

Mueve (os.replace, mismo volumen) los archivos versionados que cumplen la regla
de `es_pesado` al directorio de archivo conservando la ruta relativa, tras
comprobar que su contenido coincide con el blob de git (HEAD) y calcular su
sha256. Escribe el manifiesto CSV. NO borra nada del indice de git: eso se hace
despues con `git rm --cached` sobre las rutas del manifiesto.

Uso: python contrib/archivar_evidencia_pesada.py [--destino DIR] [--dry-run]
"""
import argparse, csv, hashlib, os, re, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MANIFIESTO = 'reversing/CppTwelveLevel6Final/ARCHIVO_MANIFIESTO.csv'
PESADOS = re.compile(r'\.(jsonl(\.gz|\.xz)?|exe|map|gz|xz|zip|7z)$')
MIN_BYTES = 1_000_000          # los .jsonl pequenos (harness.jsonl...) se quedan
MIN_JSON = 20_000_000          # .json enormes de evidencia (volcados de registro/REA)


def es_pesado(ruta, tam):
    if not ruta.startswith('reversing/'):
        return False
    if PESADOS.search(ruta):
        return tam >= MIN_BYTES
    return ruta.endswith('.json') and tam >= MIN_JSON


def listar_pesados():
    out = subprocess.run(['git', 'ls-tree', '-r', '-l', '-z', 'HEAD'], cwd=ROOT,
                         capture_output=True, check=True).stdout.decode('utf-8')
    for ent in filter(None, out.split('\0')):
        meta, ruta = ent.split('\t', 1)
        _, _, oid, tam = meta.split()
        if es_pesado(ruta, int(tam)):
            yield ruta, int(tam), oid


def hashes(path):
    s256, s1, n = hashlib.sha256(), hashlib.sha1(), os.path.getsize(path)
    s1.update(b'blob %d\0' % n)
    with open(path, 'rb') as f:
        while chunk := f.read(8 << 20):
            s256.update(chunk); s1.update(chunk)
    return s256.hexdigest(), s1.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--destino', default=str(ROOT.parent / 'evidencia_validacion_archivo'))
    ap.add_argument('--dry-run', action='store_true')
    a = ap.parse_args()
    dest = Path(a.destino)
    items = list(listar_pesados())
    print(f'{len(items)} archivos, {sum(t for _, t, _ in items) / 1e9:.2f} GB')
    if a.dry_run:
        return 0

    def trabajo(it):
        ruta, tam, oid = it
        src, dst = ROOT / ruta, dest / ruta
        if src.is_file():
            s256, s1 = hashes(src)
            if s1 != oid:
                raise SystemExit(f'NO COINCIDE con git: {ruta}')
            dst.parent.mkdir(parents=True, exist_ok=True)
            if dst.exists():
                raise SystemExit(f'ya existe en el archivo: {ruta}')
            os.replace(src, dst)
        elif dst.is_file():            # reanudacion: ya movido y verificado antes
            s256, s1 = hashes(dst)
            if s1 != oid:
                raise SystemExit(f'archivo corrupto: {ruta}')
        else:
            raise SystemExit(f'falta en ambos sitios: {ruta}')
        return ruta, tam, s256, oid

    with ThreadPoolExecutor(4) as ex:
        res = sorted(ex.map(trabajo, items))
    with open(ROOT / MANIFIESTO, 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['ruta_relativa', 'bytes', 'sha256', 'git_blob_sha1', 'archivado_en'])
        for ruta, tam, s256, oid in res:
            w.writerow([ruta, tam, s256, oid, 'evidencia_validacion_archivo/' + ruta])
    print('manifiesto escrito:', MANIFIESTO)
    return 0


if __name__ == '__main__':
    sys.exit(main())
