"""Lote PLAN1: cuenta el avance de la sustitucion (raices abiertas 788 -> 0).
Compara la base (docs/ExeIndependiente/raices.csv) con una medicion nueva de IND0A (dependencias_codigo.csv regenerado con
scan_obj.py + analizar.py sobre el ASI actual). Una raiz esta CERRADA cuando su direccion ya no aparece como destino de codigo
sin hook en la medicion nueva (la fuente ya no la escribe a mano, o ya tiene hook en hooks.csv).
Uso: python -I avance.py --repo <raiz> --datos <carpeta datos de IND0A nueva> [--hooks docs/hooks.csv]"""
import argparse, collections, csv
from pathlib import Path


def rc(p):
    with open(p, encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


ap = argparse.ArgumentParser()
ap.add_argument('--repo', required=True)
ap.add_argument('--datos', required=True)
a = ap.parse_args()
repo = Path(a.repo)
base = [r for r in rc(repo / 'docs/ExeIndependiente/raices.csv') if r['confianza'] != 'ruido']
ahora = {int(r['direccion'], 16) for r in rc(Path(a.datos) / 'dependencias_codigo.csv')}
H = {int(r['address'], 16) for r in rc(repo / 'docs/hooks.csv')}
tot = collections.Counter()
ab = collections.Counter()
por_lote = collections.defaultdict(lambda: [0, 0])
for r in base:
    v = int(r['direccion'], 16)
    cat = r['categoria']
    tot[cat] += 1
    abierta = v in ahora and v not in H
    ab[cat] += abierta
    por_lote[r['lote']][0] += 1
    por_lote[r['lote']][1] += abierta
print('raices abiertas / base: %d / %d' % (sum(ab.values()), sum(tot.values())))
for c in sorted(tot):
    print('  %-32s %4d / %4d' % (c, ab[c], tot[c]))
print('por lote (abiertas / total):')
for l in sorted(por_lote, key=lambda x: (len(x), x)):
    print('  %-18s %3d / %3d' % (l, por_lote[l][1], por_lote[l][0]))
