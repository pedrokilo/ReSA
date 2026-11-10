"""refresh_funcsummaries.py <Banco> <id-corrida> : refresca los resumenes POR FUNCION (con `aggregate_parent`) de un banco antiguo tras repetirlo aprobado.
Actualiza aggregate_parent (carpeta runs/ del banco), parent_sha256 (enriched-summary.json), manifest (head/tested_sources/bank_sources/asi/pdb),
strict_preserved_reaudit (pairs, hashes de log y harness) y las filas de reversing/validation-registry (sha256 de la evidencia y tested_sources)."""
import hashlib, json, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store
def sha(p): return hashlib.sha256(Path(p).read_bytes()).hexdigest()
bank, rid = sys.argv[1], sys.argv[2]
RUN = ROOT / 'reversing' / bank / 'runs' / rid
rep = json.loads((RUN / 'enriched-summary.json').read_text(encoding='utf-8'))
man = json.loads((ROOT / 'reversing' / bank / 'artifacts/manifest.json').read_text(encoding='utf-8'))
assert rep['passed'] is True and rep.get('differences', 0) == 0
data = registry_store.load_registry(ROOT)
n = 0
for r in data['functions']:
    p = ROOT / r['evidence']['path']
    if not p.exists() or p.parent == ROOT / 'reversing' / bank: continue
    raw = p.read_bytes(); crlf = b'\r\n' in raw
    d = json.loads(raw.decode('utf-8'))
    if bank not in str(d.get('aggregate_parent', '')): continue
    d['aggregate_parent'] = RUN.relative_to(ROOT).as_posix()
    d['parent_sha256'] = sha(RUN / 'enriched-summary.json')
    d['manifest'] = {**{k: v for k, v in d.get('manifest', {}).items()}, **{k: man[k] for k in ('head', 'tested_sources', 'bank_sources', 'asi', 'pdb', 'compiled_instruments') if k in man}}
    sp = d.get('strict_preserved_reaudit')
    if isinstance(sp, dict):
        for k in ('pairs', 'pairs_per_function', 'log_sha256', 'harness_sha256'):
            if k in rep: sp[k] = rep[k]
        sp['differences'] = 0
    d['refreshed'] = '2026-11-10 REPARAR-REQUESTED: banco repetido y aprobado con el codigo actual (corrida %s)' % rid
    text = json.dumps(d, ensure_ascii=False, indent=2) + '\n'
    p.write_bytes((text.replace('\n', '\r\n') if crlf else text).encode('utf-8'))
    r['evidence']['sha256'] = sha(p)
    r['tested_sources'] = man['tested_sources']
    n += 1
registry_store.save_registry(ROOT, data)
print(bank, 'filas por funcion actualizadas:', n)
