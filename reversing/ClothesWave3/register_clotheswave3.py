"""Registro historico de CClothesBuilder::CreateSkinnedClump (0x5A69D0) via el banco simple ClothesWave3. Re-audita en streaming la corrida aprobada, conserva la evidencia en runs/ y anade 1 entrada (dependency_review=null). CClothesBuilder queda completa."""
import csv,gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-105013-683422'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-clotheswave3')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
NAME='CClothesBuilder::CreateSkinnedClump';PAIRS={NAME:256};TOTAL=256
assert summary['pairs_per_function']==PAIRS
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
cls,fn=NAME.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
assert len(m)==1 and m[0]['validated']=='0' and m[0]['address']=='0x005A69D0';ADDR=m[0]['address'];TYPE=m[0]['type']
assert [r['fn_name'] for r in rows if r['class']==cls and r['validated']=='0']==[fn]
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith('29806574') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};unequal=0;cases=set();complete=None;begin=None;fsum={}
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:unequal+=1
        elif ev=='function_summary':fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='begin':begin=row
        else:assert ev=='waiting_for_player',ev
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('begin')==1 and events.get('function_summary')==1 and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert fsum[NAME]['cases']==256 and fsum[NAME]['failures']==0
assert complete['pairs']==TOTAL and complete['state_restored'] is True and begin['known'] is True and all(x['ok'] is True for x in begin['sites']) and len(begin['sites'])==34
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-clotheswave3-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-clotheswave3-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('Simple pair bench inside the full game with a loaded save (original with the class hook off vs candidate with the hook on), 256 isolated CPU pairs for CClothesBuilder::CreateSkinnedClump (0x5A69D0), zero differences, natural completion event with state_restored, saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (fake old/new CPedClothesDesc, fake textures/geometries/skins/frames/atomics/clump, drawing and RenderWare replaced by traces) and compares the return value, the order and raw arguments of all collaborator calls, the new descriptor after the default SetModel calls, flags 0x8D0AA4..AA6 and 0xBC1288, the color of the fake materials (written inline), flags|=2 of the temporary geometry, the parent of the fake clump, the deletes (weights first, indices after) and the hash of the descriptor bytes; x87 at 24 bits, foreign threads ignored (Mine()). '
 'Limits: 34 collaborators are replaced by traced doubles WITHOUT credit (RenderWare, streaming, clothes pools: CDirectory::Init/ReadDirFile, CStats::GetStatValue, PreprocessClothesDesc, RwTexDictionaryForAllTextures, RwTexDictionaryRemoveTexture/RwTextureDestroy, RwTexDictionaryFindNamedTexture, RpMaterialCreate/SetTexture/Destroy, GetFirstAtomic, RpSkin*, RpHAnim*, RwFrame*, RpAtomic*, RpClump*, RpGeometryDestroy, RwTexDictionarySetCurrent, operator delete during the call). '
 'ConstructTextures, ConstructGeometryArray and ConstructGeometryAndSkinArrays are NOT executed in this bench (doubled; they are covered by their earlier validations in GfxWave2). CPedClothesDesc::SetModel and CKeyGen::GetUppercaseKey run for real. The callback address of RwTexDictionaryForAllTextures is not compared, only its behaviour. '
 'Only the own part of the function is validated. Source corrected to what the EXE does before the approved run (uncommitted at the time; see reversing/ClothesWave3/README.md). Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={NAME:'Corpus 256; source corrected: (1) 0x5A6B65-0x5A6B77 the material colour is written inline BEFORE RpMaterialSetTexture; (2) 0x5A6CAD-0x5A6CD0 RpSkinGetNumBones and RpSkinGetSkinToBoneMatrices are called before RpSkinCreate (explicit locals). Domain: old descriptor null or equal (early exit nullptr) or different in fat/muscle/model/texture/NaN/several, default models already applied or to fill, bCutscenePlayer 25%, random texture/geometry masks, muscle and fat stats 0/negative/>1/inf/NaN.'}
SOURCE_CHANGES={'source/game_sa/ClothesBuilder.cpp':'CreateSkinnedClump: material colour written inline before RpMaterialSetTexture (0x5A6B65-0x5A6B77); RpSkinGetNumBones and RpSkinGetSkinToBoneMatrices before RpSkinCreate with explicit locals (0x5A6CAD-0x5A6CD0)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=PAIRS,covered_functions={NAME:True},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,begin_events=1,waiting_for_player_events=events.get('waiting_for_player',0),function_summaries=1,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,out_of_bench_not_credited=['ConstructTextures','ConstructGeometryArray','ConstructGeometryAndSkinArrays'],
 notes='With this function CClothesBuilder has no pending entries in docs/hooks.csv (the other 30 were registered earlier, 14 of them by GfxWave2).')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-clothes-wave3.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
hook={'class':cls,'fn_name':fn,'address':ADDR,'type':TYPE}
assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions'])
data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',NAME],pairs=256,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
 source_provenance='Exact compiled ClothesWave3 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest and equal to the working tree at registration; full 256-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[NAME],review_reference='reversing/ClothesWave3/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.clothes-wave3.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp)),indent=2))
