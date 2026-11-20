"""Actualiza mapa_funciones.csv con las funciones validadas en LIB3B. Uso: python upd_mapa.py <grupo> <pares> dir:archivo:funcion [...]  (agrega la fila si no existe)"""
import csv, sys
grupo, pares = sys.argv[1], sys.argv[2]
rows = list(csv.DictReader(open('mapa_funciones.csv', encoding='utf-8', newline='')))
fn = rows and list(rows[0].keys())
by = {r['direccion'].lower(): r for r in rows}
ev = 'banco par a par %s casos, 0 diferencias + oraculo Python (o biblioteca 6b compilada aparte donde la funcion es de flujo) + 2 rondas negativas (LIB3B %s)' % (pares, grupo)
for spec in sys.argv[3:]:
    a, f, n = spec.split(':'); k = a.lower()
    if k not in by:
        r = dict.fromkeys(fn, ''); r['direccion'] = a; r['bytes'] = ''; r['lote_plan'] = 'LIB3B'; rows.append(r); by[k] = r
    r = by[k]; r['archivo_fuente'] = f; r['funcion_fuente'] = n; r['estado'] = 'validada contra el EXE (LIB3B)'; r['evidencia'] = ev
rows.sort(key=lambda r: int(r['direccion'], 16))
w = csv.DictWriter(open('mapa_funciones.csv', 'w', encoding='utf-8', newline=''), fieldnames=fn, lineterminator='\n'); w.writeheader(); w.writerows(rows)
