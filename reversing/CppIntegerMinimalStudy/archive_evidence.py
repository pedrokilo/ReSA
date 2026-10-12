"""Archive already audited evidence, including exact source/instrument snapshots."""
import argparse
import gzip
import hashlib
import json
from pathlib import Path
from audit_runtime import audit, sha, need, runner_audit

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('comparisons'); ap.add_argument('--kind', type=int, choices=(4,10), required=True)
    for name in ('manifest','asi','repo','bank','out','runner-summary','game'): ap.add_argument('--'+name, required=True)
    a = ap.parse_args(); manifest = Path(a.manifest); m = json.loads(manifest.read_text(encoding='utf-8'))
    required = {('source/game_sa/RenderWare/D3DResourceSystem.cpp' if a.kind == 4 else 'source/game_sa/CarCtrl.cpp'),'source/CMakeLists.txt','docs/hooks.csv',
                'source/StdInc.h','source/game_sa/CarCtrl.h','source/game_sa/RenderWare/D3DResourceSystem.h',
                'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'}
    need(required <= set(m['sources']), 'missing compiler/catalog manifest dependency')
    need(sha(a.asi) == m['binaries']['asi'], 'ASI hash')
    result = audit(a.comparisons,a.kind)
    result.update(runner_audit(a.runner_summary,a.comparisons,a.kind,sha(a.asi),a.game))
    snapshots = []
    for group, root in (('sources',Path(a.repo)),('instrumentation',Path(a.bank))):
        for relative, digest in m[group].items():
            rel = Path(relative)
            need(not rel.is_absolute() and '..' not in rel.parts, 'unsafe relative path')
            data = (root/rel).read_bytes()
            need(hashlib.sha256(data).hexdigest() == digest, 'snapshot hash: '+relative)
            snapshots.append((Path(group)/rel,data))
    # Do not overwrite an existing evidence archive, including partial archives.
    out = Path(a.out); out.mkdir(parents=True, exist_ok=False)
    for rel,data in snapshots:
        target=out/rel; target.parent.mkdir(parents=True,exist_ok=True); target.write_bytes(data)
    data=Path(a.comparisons).read_bytes()
    (out/'comparisons.jsonl.gz').write_bytes(gzip.compress(data,mtime=0))
    (out/'build-manifest.json').write_bytes(manifest.read_bytes())
    (out/'runner-summary.json').write_bytes(Path(a.runner_summary).read_bytes())
    (out/'executed.asi').write_bytes(Path(a.asi).read_bytes())
    # Archive the independent checker that issued this result.
    checker=Path(__file__).with_name('audit_runtime.py')
    (out/'audit_runtime.py').write_bytes(checker.read_bytes())
    result.update(manifest_sha256=sha(manifest),asi_sha256=sha(a.asi),auditor_sha256=sha(checker))
    (out/'independent-audit.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
    print(json.dumps(result,indent=2))

if __name__=='__main__': main()
