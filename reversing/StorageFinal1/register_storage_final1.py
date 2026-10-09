"""Registro historico de cinco funciones de CGenericGameStorage via el banco simple StorageFinal1. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 5 entradas (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-010650-155833'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-storagefinal1')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
PAIRS={'CGenericGameStorage::SaveDataToWorkBuffer-org': 1620, 'CGenericGameStorage::LoadDataFromWorkBuffer-org': 1620, 'CGenericGameStorage::ReportError': 208, 'CGenericGameStorage::GenericSave': 768, 'CGenericGameStorage::GenericLoad': 768}
ADDR={'CGenericGameStorage::SaveDataToWorkBuffer-org': ('0x005D1270', 'S'), 'CGenericGameStorage::LoadDataFromWorkBuffer-org': ('0x005D1300', 'S'), 'CGenericGameStorage::ReportError': ('0x005D08C0', 'S'), 'CGenericGameStorage::GenericSave': ('0x005D13E0', 'S'), 'CGenericGameStorage::GenericLoad': ('0x005D17B0', 'S')}
TOTAL=4984;assert sum(PAIRS.values())==TOTAL
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('f84a7e7c') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
# Independent offline reaudit of the full log.
events={};per={};unequal=0;cases=set();complete=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True:unequal+=1
        elif ev=='complete':complete=row
        elif ev=='error':raise SystemExit('error event in log')
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('function_summary')==len(PAIRS) and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True
# Build manifest and sources (must be the exact files the probe was built from).
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-storagefinal1-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-storagefinal1-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON="Simple pair bench inside the full game (original with hook off vs candidate with hook on), 4984 isolated CPU pairs for the 5 CGenericGameStorage functions, zero differences, natural completion event with state_restored, saves unchanged (hashes of the real saves checked before and after), configs restored. NO real I/O: GenericSave/GenericLoad run with everything touching files or the world replaced by traced doubles (patched at the EXE address and at the DLL body; the bench aborts with an error event before patching if a DLL body is not found or two overlap): OpenFileForWriting (0x5D0DD0), OpenFileForReading (0x5D0D20), SaveWorkBuffer (0x5D0F80) and LoadWorkBuffer (0x5D10B0) with scripted flush/reload outcomes (success, failure with and without ms_bFailed, virtual file for GenericLoad), CFileMgr::CloseFile (0x5389D0), CPad::UpdatePads (0x541E50), CCheat::ResetCheats (0x438450), CSimpleVariablesSaveStructure::Construct (0x5D1B80) and Extract (0x5D1EA0), ReportError (inside GenericLoad), DoGameSpecificStuffAfterSucessLoad (0x618E90), the 24 block saves and 24 block loads (CTheScripts::Save ... C3dMarkers::LoadUser3dMarkers) and DoScriptSetupAfterPoolsHaveLoaded (0x5D3390); CPools::Load is scripted false/true. These helpers get NO credit; only the own logic of each function is validated. The 'file' is a std::vector and the 'FILE*' an opaque token; the work buffer is allocated by the open double with the game operator new[] and freed by the function under test (otherwise by the bench after noting work_null). The returned value, ordered trace of outgoing calls with raw arguments, x87 control word and all fixture bytes are compared. Excluded/limits: ReportError blocks 25, 26 and >= 28 (the EXE uses an uninitialised buffer); ms_WorkBufferSize kept at 0xC800 in GenericLoad (with a smaller value the EXE reads past the 5-byte header), its effect covered only in LoadDataFromWorkBuffer-org; work-buffer sizes below 0xC800 are sanitised to pos = kBuf to avoid the EXE's own infinite recursion; the NOTSA branch of GenericLoad (save-version check) is not exercised because the Extract double returns the current version, and the NOTSA block 25/26 names in ReportError are left as deviations. In ReportError the EXE formats with sprintf (0x821BB5, replaced) into a local buffer while the DLL emits the same text through spdlog (captured with a temporary logger sink). Historical evidence approved with explicit limits; no current dependency accreditation."
SPECIFIC={'CGenericGameStorage::SaveDataToWorkBuffer-org': 'Corpus 1620: 9 sizes (-1..40000) x 10 positions (0..51200, including those crossing 0xC800) x prior failure x 3 flush modes (success / failure / failure with ms_bFailed) x 3 ms_WorkBufferSize; source corrected: fixed 0xC800 threshold and remainder 0xC800-pos.', 'CGenericGameStorage::LoadDataFromWorkBuffer-org': 'Corpus 1620: same grid with simulated reload; ms_WorkBufferSize in {0xC800, 51000, 40000} to tell the variable threshold from the fixed 0xC800-pos remainder; source corrected: threshold ms_WorkBufferSize and remainder 0xC800-pos.', 'CGenericGameStorage::ReportError': 'Corpus 208: 26 blocks x 8 codes (0,1,2,3,4,255,-1,INT_MAX); no message for codes > 2; blocks 25, 26 and >= 28 excluded.', 'CGenericGameStorage::GenericSave': 'Corpus 768: data totals 0..230000 bytes (crossing several buffers and the padding up to 0x317FC = 202748), headers split at the buffer boundary, open/flush/block/close failures; source corrected: padding loop up to 0x317FC and strncpy of 0x103.', 'CGenericGameStorage::GenericLoad': 'Corpus 768: virtual 28-block file with optional corrupt header, reloads with failure (with and without ms_bFailed), block failure, CPools::Load false/true, close failure; source corrected: calls ReportError(block-1, 2).'}
SOURCE_CHANGES={'source/game_sa/GenericGameStorage.cpp': 'SaveDataToWorkBuffer/LoadDataFromWorkBuffer thresholds and remainders, GenericLoad ReportError(block-1,2), GenericSave padding loop (0x317FC) and strncpy 0x103, ReportError no message for codes > 2'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,function_summaries=len(PAIRS),unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-storage-final1.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');addr,typ=ADDR[name];hook={'class':cls,'fn_name':fn,'address':addr,'type':typ}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled StorageFinal1 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 4984-row log reaudited offline and preserved gzipped with the original hash. GenericGameStorage.cpp was corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/StorageFinal1/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.storage-final1.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
