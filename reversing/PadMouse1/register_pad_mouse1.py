"""Registro historico de CPad::UpdateMouse (banco simple PadMouse1). Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade una entrada (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-001153-051520'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-padmouse1')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==9216 and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']=={'CPad::UpdateMouse':9216}
assert summary['executed_probe_sha256'].startswith('90b4665c') and summary['original_exe_sha256'].startswith('72ae59e4') and summary['log_sha256'].startswith('68a56e77')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
# Independent offline reaudit of the full log.
events={};compared=0;equal=0;cases=set();paths=set()
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event')
        if ev=='comparison':
            compared+=1
            cases.add(row['case'])
        events[ev]=events.get(ev,0)+1
        if ev=='complete':complete=row
assert events.get('comparison')==9216 and events.get('complete')==1 and events.get('begin')==1 and len(cases)==9216
assert complete['pairs']==9216 and complete['state_restored'] is True
# every comparison must carry equal=true
bad=0
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line)
        if row.get('event')=='comparison' and row.get('equal') is not True:bad+=1
assert bad==0
# Build manifest and sources.
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-padmouse1-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-padmouse1-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
# Preserve the run (new path only).
assert not RUN.exists();RUN.mkdir(parents=True)
shutil.copy2(SRC/'summary.json',RUN/'summary.json');shutil.copy2(SRC/'RESULTADO.txt',RUN/'RESULTADO.txt')
raw=(SRC/'comparisons.jsonl').read_bytes()
with open(RUN/'comparisons.jsonl.gz','wb') as out:
    with gzip.GzipFile(filename='',mode='wb',fileobj=out,compresslevel=9,mtime=0) as z:z.write(raw)
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(1<<20),b''):digest.update(block)
assert digest.hexdigest()==log_sha
assert sha(RUN/'summary.json')==sha(SRC/'summary.json') and sha(RUN/'RESULTADO.txt')==sha(SRC/'RESULTADO.txt')
storage=dict(uncompressed_sha256=log_sha,uncompressed_bytes=len(raw),compressed_path=rel(RUN/'comparisons.jsonl.gz'),compressed_sha256=sha(RUN/'comparisons.jsonl.gz'),compression='gzip',gzip_mtime=0,lossless_roundtrip_sha256=digest.hexdigest(),original_location=str(SRC/'comparisons.jsonl'))
previous=SRC/'previous.jsonl'
scope=('9216 isolated CPU pairs for CPad::UpdateMouse (EXE 0x0053F3C0), simple pair bench: original (hook off) vs candidate; zero differences, natural completion event with state_restored, saves unchanged, configs restored. '
 'Corpus: 6 paths (no foreground; init leaving the device null; read E_FAIL; read 0; positive HRESULT 1; E_ACCESSDENIED) x 12 DIMOUSESTATE2 states x 8 menu/invert-X/invert-Y combinations x 4 seed patterns x previous device yes/no x 2 clocks. '
 'Limits: no NaN/Inf/INT_MIN; no real COM (device is an opaque token); diMouseInit, GetMouseState, IsForegroundApp and IsVideoModeExclusive are stubbed and traced (calls and arguments only), so those helpers get no credit; CheckForInput and SetTouched run for real; '
 'padding bytes 7/27/47 of CMouseControllerState are NOT compared (MouseBytes() zeroes them: the EXE propagates byte 7 with rep movs, the compiler copies member by member); stubs ignore foreign threads (audio threads call IsForegroundApp); x87 forced to 24-bit precision. '
 'An earlier run (20261009-000614-797633) showed 579 differences in 1752 pairs plus an access violation; all of it was bench noise (padding byte and foreign-thread traces), fixed in LiveComparison.hpp, not a source/EXE difference. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=9216,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (1.2 GB, not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'])
out=dict(passed=True,pairs=9216,differences=0,functions=summary['pairs_per_function'],scope=scope,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=9216,complete_events=1,unequal=0,distinct_cases=9216),
 rejected_attempts={'20261009-000614-797633':'579 differences in 1752 pairs + access violation; bench noise (padding byte 7, foreign audio thread); log not preserved in repository','previous.jsonl':sha(previous) if previous.exists() else None})
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-pad-mouse1.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);assert n==6870
hook={'class':'CPad','fn_name':'UpdateMouse','address':'0x0053F3C0','type':'S'}
assert not any(x['hook']==hook or (x['hook']['class']=='CPad' and x['hook']['fn_name']=='UpdateMouse') for x in data['functions'])
data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['passed'],pairs=9216,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],source_provenance='Exact compiled PadMouse1 probe (ASI/PDB hashes in build-manifest.json); full 9216-row log reaudited offline and preserved gzipped with the original hash; bench noise from the earlier attempt (padding byte 7/27/47, foreign-thread traces) fixed in LiveComparison.hpp before this run.',dependency_review=None,scope=scope,review_reference='reversing/PadMouse1/README.md'))
temp=registry.with_suffix('.pad-mouse1.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
