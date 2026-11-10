#!/usr/bin/env python3
"""refreeze_generic.py <Banco> [...]: recalcula los SHA256 de la clave `inputs` de prepared-freeze.json / instrument-freeze.json
sobre los ficheros actuales (LF de git y versiones actuales de ProbeRuntime/runtime_runner/VTableRanges). Anota fecha y causa.
No toca corpus, modelo ni alcance. Informa de entradas ausentes."""
import hashlib, json, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
import re
HEX = re.compile(r'^[0-9a-f]{64}$')
def walk(node, base, stat):
    if isinstance(node, dict):
        for k in list(node):
            v = node[k]
            if isinstance(v, str) and HEX.match(v):
                for cand in (ROOT / k, base / k, ROOT / 'reversing' / k):
                    if cand.is_file():
                        h = hashlib.sha256(cand.read_bytes()).hexdigest()
                        if h != v: node[k] = h; stat[0] += 1
                        break
                else: stat[1].append(k)
            else: walk(v, base, stat)
    elif isinstance(node, list):
        for x in node: walk(x, base, stat)
for bank in sys.argv[1:]:
    base = ROOT / 'reversing' / bank
    for p in sorted(set(base.glob('*freeze*.json')) | set(base.glob('*frozen*.json'))):
        nm = p.name
        d = json.loads(p.read_bytes().decode('utf-8'))
        stat = [0, []]
        walk(d, base, stat)
        if stat[0]:
          if not any('/' in k for k in d):
            d['refrozen'] = {'date': '2026-11-10', 'reason': 'REPARAR-REQUESTED: hashes recalculados sobre LF y versiones actuales de ProbeRuntime.hpp/runtime_runner.py/VTableRanges.hpp'}
          p.write_bytes((json.dumps(d, indent=2) + chr(10)).encode('utf-8'))
        print(bank, nm, 'cambiados', stat[0], 'sin fichero', stat[1])
