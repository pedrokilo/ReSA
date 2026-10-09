"""Registro historico de CRope (5) y CMonsterTruck (5) via el banco simple WorldFinal1. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 10 entradas (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-005433-453152'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-worldfinal1')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
PAIRS={'CRope::CreateHookObjectForRope':768,'CRope::PickUpObject':1536,'CRope::ReleasePickedUpObject':384,'CRope::Remove':640,'CRope::Render':1536,'CMonsterTruck::Constructor':512,'CMonsterTruck::BurstTyre':256,'CMonsterTruck::PreRender':1536,'CMonsterTruck::ProcessEntityCollision':3072,'CMonsterTruck::SetUpWheelColModel':128}
ADDR={'CRope::CreateHookObjectForRope':('0x00556070','S'),'CRope::PickUpObject':('0x005569C0','S'),'CRope::ReleasePickedUpObject':('0x00556030','S'),'CRope::Remove':('0x00556780','S'),'CRope::Render':('0x00556800','S'),
 'CMonsterTruck::Constructor':('0x006C8D60','S'),'CMonsterTruck::BurstTyre':('0x00000000','V'),'CMonsterTruck::PreRender':('0x00000000','V'),'CMonsterTruck::ProcessEntityCollision':('0x00000000','V'),'CMonsterTruck::SetUpWheelColModel':('0x00000000','V')}
TOTAL=10368;assert sum(PAIRS.values())==TOTAL
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('29f52f71') and summary['original_exe_sha256'].startswith('72ae59e4')
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
        elif ev=='layout':assert row['ok'] is True
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('function_summary')==10 and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True and complete['layout_ok'] is True
# Build manifest and sources (must be the exact files the probe was built from).
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-worldfinal1-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-worldfinal1-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('Simple pair bench (original with hook off vs candidate with hook on), 10368 isolated CPU pairs for the 10 functions, zero differences, natural completion event with state_restored and layout_ok, saves unchanged, configs restored. '
 'Fake objects with 32-byte guards (rope 0x328 bytes, truck 0x99C bytes, 5 entities of 0xA40 bytes with a fake vtable, 6 matrices, 3 fake model infos/colmodels/collision data); full content (rope and truck byte by byte, the rest by hash), guards, return value, x87 control word and the outgoing call trace with arguments are compared. '
 'Traced stubs, patched at the EXE address and at the DLL body, so these helpers get NO credit: CEntity::RegisterReference, CPhysical::AddToMovingList, CWorld::Add/Remove, CObject::operator new (fixed buffer) and CObject(int32,bool), DefinedState, CCamera::IsSphereVisible, RwRenderStateSet, RwIm3DTransform/RenderIndexedPrimitive/End, CAutomobile::CAutomobile, CMonsterTruck::SetupSuspensionLines, CAutomobile::PreRender, CVehicleModelInfo::GetWheelPosn, CVehicle::SetTransmissionRotation/SetComponentRotation, CCollision::ProcessColModels and CPhysical::AddCollisionRecord (only the own part of each function is validated). '
 'Entities always carry a non-null matrix (a null one would allocate from the pool); fixture NaNs are quiet and floats copied with x87 are finite (no signalling NaN); no pool, file or real operator new/delete is touched. '
 'Excluded: CObject pool full (operator new returns null, EXE and DLL both dereference it); Remove and ReleasePickedUpObject with null m_pAttachedEntity and an attached object (the EXE dereferences null); CreateHookObjectForRope with m_pAttachedEntity != null is only tested as an early exit. '
 'Source corrected to what the EXE does before this run: CRope::PickUpObject (RegisterReference on &m_pRopeAttachObject and position through operator* 0x59C890), CRope::CreateHookObjectForRope (types 0 and >=8 return without creating), CRope::Render (NaN in distance and the table at 0x8CD818), CMonsterTruck::ProcessEntityCollision (jump on NaN and SetEntityReference without clearing the previous one). '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={
 'CRope::CreateHookObjectForRope':'Corpus 768: rope types 0..9 and 255, attached entity null (12% non-null for the early exit), new object storing its position in the placement or in a matrix, finite segment-31 coordinates; source change: types 0 and >=8 exit without creating (jump table 0x556194).',
 'CRope::PickUpObject':'Corpus 1536: target kinds vehicle/object/ped/other, status and flag combinations, prior attached object null/other/same, attached entity with and without matrix, random model height; source change: RegisterReference on &m_pRopeAttachObject and position through operator* (0x59C890).',
 'CRope::ReleasePickedUpObject':'Corpus 384: random entity types/status, attached object present (70%) or null; attached entity never null.',
 'CRope::Remove':'Corpus 640: attached object present/absent, attached entity null only when there is no attached object.',
 'CRope::Render':'Corpus 1536: rope types, distance kinds including a quiet NaN, camera visibility script (first sphere, mirror only, none), m_bMirrorActive 0/1, random TempBufferVertices; source changes: NaN in distance and index table 0x8CD818 (62 and 2 indices). RwIm3D*/RwRenderStateSet are traced stubs.',
 'CMonsterTruck::Constructor':'Corpus 512: model ids 444,406,0,1,19991,-1,0x7FFF x 5 created-by values; CAutomobile constructor and SetupSuspensionLines are stubs; vtable pointer checked as EXE vtable 0x8717D8 or DLL address, then normalised to 0.',
 'CMonsterTruck::BurstTyre':'Corpus 256 via virtual slot 44 (vtable 0x8717D8, 0x6C7D30); random component and physical flag.',
 'CMonsterTruck::PreRender':'Corpus 1536 via virtual slot 17 (0x6C7DE0): truck/dumper model, random car nodes, misc component angle, wheel positions; CAutomobile::PreRender, GetWheelPosn, SetTransmissionRotation and SetComponentRotation are stubs.',
 'CMonsterTruck::ProcessEntityCollision':'Corpus 3072 via virtual slot 22 (0x6C8AE0): entity types, physical/entity flags, collision data with 0/1/2/4/255 lines, ProcessColModels stub returning -2..32 with scripted touch distances incl. NaN and wheel positions incl. NaN/Inf; source changes: jump on NaN and SetEntityReference without clearing the previous reference.',
 'CMonsterTruck::SetUpWheelColModel':'Corpus 128 via virtual slot 43 (0x6C7D20); random collision model pointer.'}
SOURCE_CHANGES={'source/game_sa/Rope.cpp':'PickUpObject, CreateHookObjectForRope and Render corrected to the EXE (see scope)','source/game_sa/Entity/Vehicle/MonsterTruck.cpp':'ProcessEntityCollision corrected to the EXE (see scope)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (1.2 GB, not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,function_summaries=10,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-world-final1.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);assert n==6871
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');addr,typ=ADDR[name];hook={'class':cls,'fn_name':fn,'address':addr,'type':typ}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled WorldFinal1 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 10368-row log reaudited offline and preserved gzipped with the original hash. Rope.cpp/MonsterTruck.cpp were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/WorldFinal1/README.md'))
temp=registry.with_suffix('.world-final1.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
