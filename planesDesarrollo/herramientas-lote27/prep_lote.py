import json, shutil, glob, os
from pathlib import Path

ROOT = Path(r'D:\Original\gta-reversed-master')
os.chdir(ROOT)
DEST = ROOT / 'reversing/CppTwelveLevel6Final/runs'
BANKS = ['Dependencies22A', 'Dependencies22B', 'Dependencies22C', 'Dependencies21D', 'Dependencies21E']
for bank in BANKS:
    cands = []
    for root in ('reversing/CppLote1Runs/runs', 'reversing/CppAuditRerun/runs'):
        for d in glob.glob(f'{root}/{bank}-2026*'):
            s = json.load(open(Path(d) / 'summary.json', encoding='utf-8'))
            if s.get('passed') is True and s.get('all_equal') is True:
                cands.append(Path(d))
    best = sorted(cands, key=lambda d: d.name.split('-', 1)[1])[-1]
    target = DEST / best.name
    if not target.exists():
        shutil.copytree(best, target)
    print(bank, '->', target.name)

# retirar los registros antiguos de 21D y 21E para que register_hooks_nuevos los regenere con la corrida nueva
reg_path = ROOT / 'reversing/validation-registry.json'
data = json.loads(reg_path.read_text(encoding='utf-8'))
old = {r['evidence']['path'] for r in data['functions'] if r['evidence']['path'].endswith(('Dependencies21D/validation-summary.json', 'Dependencies21E/validation-summary.json'))}
before = len(data['functions'])
data['functions'] = [r for r in data['functions'] if r['evidence']['path'] not in old]
reg_path.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
print('registro', before, '->', len(data['functions']))
ap_path = ROOT / 'reversing/CppOnlyMigration/approvals.json'
ap = json.loads(ap_path.read_text(encoding='utf-8'))
drop = [k for k, v in ap.items() if v.get('bank') in ('Dependencies21D', 'Dependencies21E')]
for k in drop:
    del ap[k]
ap_path.write_bytes(json.dumps(ap, indent=2, ensure_ascii=False).encode('utf-8'))
print('approvals retiradas', len(drop))
for p in old:
    Path(p).unlink()
    print('borrado', p)
