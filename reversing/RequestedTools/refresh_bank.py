"""Actualiza la evidencia registrada de un banco antiguo tras repetirlo con una corrida APROBADA.

Uso:  python refresh_bank.py <Banco> <carpeta-de-evidencia> <build-manifest.json>
Copia la corrida a reversing/CppTwelveLevel6Final/runs/<Banco>-<corrida>, actualiza reversing/<Banco>/validation-summary.json y las filas de
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) que apuntan a ese resumen (sha256, tested_sources, pairs). No anade funciones nuevas (para eso, register_hook.py).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
NOTE = 'Revalidado el 2026-10-26 tras reconstruir colaboradores (lote 67 y anteriores); el banco se adapto a ellos (ver su README.md).'


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def dump(obj, crlf):
    text = json.dumps(obj, ensure_ascii=False, indent=2) + '\n'
    return (text.replace('\n', '\r\n') if crlf else text).encode('utf-8')


def main(bank, src, manifest, summary_name='validation-summary.json'):
    SRC = Path(src)
    RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / f'{bank}-{SRC.name}'
    RUN.mkdir(exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (RUN / n).exists():
            shutil.copyfile(SRC / n, RUN / n)
    if not (RUN / 'comparisons.jsonl.gz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
    s = json.loads((RUN / 'summary.json').read_text(encoding='utf-8'))
    # REPARAR-REQUESTED: tolera resumenes de bancos Requested* (comparisons, bank_restored/harness_restored, sin pairs_per_function)
    if 'state_restored' not in s:
        s['state_restored'] = bool(s.get('bank_restored', True)) and bool(s.get('harness_restored', True))
    if 'all_equal' not in s:
        s['all_equal'] = s.get('differences') == 0 and s.get('model_mismatches', 0) == 0
    if 'pairs' not in s:
        s['pairs'] = s.get('comparisons')
    if 'log_sha256' not in s:
        s['log_sha256'] = s.get('comparison_sha256') or sha(SRC / 'comparisons.jsonl')
    _cur = json.loads((ROOT / 'reversing' / bank / summary_name).read_bytes().decode('utf-8')) if (ROOT / 'reversing' / bank / summary_name).exists() else {}
    if isinstance(s.get('counts'), dict) and isinstance(_cur.get('function'), str) and _cur['function'].split('::')[-1] in s['counts']:   # resumen por funcion de un banco multifuncion
        _fn = _cur['function']; s['pairs'] = int(s['counts'][_fn.split('::')[-1]]); s['pairs_per_function'] = {_fn: s['pairs']}
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True
    if 'pairs_per_function' not in s:
        _ex = json.loads((ROOT / 'reversing' / bank / summary_name).read_bytes().decode('utf-8')).get('functions')
        if isinstance(_ex, dict) and len(_ex) == 1:
            s['pairs_per_function'] = {next(iter(_ex)): int(s['pairs'])}
        elif isinstance(_ex, dict) and sum(int(x) for x in _ex.values()) == int(s['pairs']):
            s['pairs_per_function'] = {k: int(x) for k, x in _ex.items()}
        else:
            s['pairs_per_function'] = s.get('functions_pairs') or {s.get('function', bank): int(s['pairs'])}
    ppf = dict(s['pairs_per_function'])
    man = json.loads(Path(manifest).read_text(encoding='utf-8'))
    sources = man.get('sources') or man['tested_sources']

    out = ROOT / 'reversing' / bank / summary_name
    raw = out.read_bytes()
    crlf = b'\r\n' in raw
    v = json.loads(raw.decode('utf-8'))
    v['passed'] = True
    v['pairs'] = int(s['pairs'])
    v['log_sha256'] = s['log_sha256']
    v['state_restored'] = True
    v['all_equal'] = True
    if 'evidence_directory' in v:  # formato antiguo
        v['evidence_directory'] = str(SRC)
        v['pairs_per_function'] = ppf
        v['probe_build_manifest'] = man
        lim = v.get('limitations') or []
        v['limitations'] = lim if NOTE in lim else ((lim + [NOTE]) if isinstance(lim, list) else (str(lim) + ' ' + NOTE))
    else:  # formato de registro por banco
        v['run'] = RUN.relative_to(ROOT).as_posix()
        v['summary_sha256'] = sha(RUN / 'summary.json')
        v['result_txt_sha256'] = sha(RUN / 'RESULTADO.txt')
        v['executed_probe_sha256'] = s['executed_probe_sha256']
        _f = v.get('functions')
        if isinstance(_f, dict) and _f and all(isinstance(x, dict) for x in _f.values()):
            for _k, _x in _f.items():
                _m = [n for n in ppf if n == _k or n.endswith('::' + _k)]
                if _m: _x['pairs'] = int(ppf[_m[0]]); _x['passed'] = True
        elif not isinstance(_f, (int, float)) or isinstance(_f, bool): v['functions'] = ppf   # REPARAR-REQUESTED: conserva el formato entero de los resumenes Requested*
        v['covered_functions'] = {k: True for k in ppf}
        v['registered_pairs'] = int(s['pairs'])
        oldm = v.get('manifest') if isinstance(v.get('manifest'), dict) else {}
        v['manifest'] = {**{k: x for k, x in oldm.items() if k not in ('tested_sources',)}, **{k: man[k] for k in ('head', 'bank_sources') if k in man}, 'tested_sources': sources}
        v.pop('reaudit', None)
        v['comparison_storage'] = dict(compressed_path=(RUN / 'comparisons.jsonl.gz').relative_to(ROOT).as_posix(), compression='gzip', gzip_mtime=0,
                                       compressed_sha256=sha(RUN / 'comparisons.jsonl.gz'), uncompressed_sha256=s['log_sha256'])
        v['scope'] = v.get('scope') or ''
        v['scope'] = v['scope'] if NOTE in v['scope'] else v['scope'] + ' ' + NOTE
    out.write_bytes(dump(v, crlf))

    data = registry_store.load_registry(ROOT)
    n = 0
    for r in data['functions']:
        if r['evidence']['path'] == out.relative_to(ROOT).as_posix():
            r['evidence']['sha256'] = sha(out)
            r['tested_sources'] = sources
            key = r['evidence']['passed_path'][-1]
            if key in ppf:
                r['evidence']['pairs'] = int(ppf[key])
            n += 1
    registry_store.save_registry(ROOT, data)
    print(bank, 'filas actualizadas:', n)


if __name__ == '__main__':
    if len(sys.argv) not in (4, 5):
        raise SystemExit(__doc__)
    main(*sys.argv[1:])
