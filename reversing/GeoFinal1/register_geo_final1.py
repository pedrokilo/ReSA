"""Registro historico de ocho funciones de CCollision, CPlayerPed, CPedGeometryAnalyser y CRealTimeShadowManager via el banco simple GeoFinal1. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 8 entradas (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-010416-836844'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-geofinal1')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
PAIRS={'CCollision::SphereVsEntity': 1024, 'CPlayerPed::SetupPlayerPed': 128, 'CPedGeometryAnalyser::CanPedTargetPed': 1024, 'CPedGeometryAnalyser::ComputeClosestSurfacePoint-ped': 512, 'CRealTimeShadowManager::DoShadowThisFrame': 512, 'CRealTimeShadowManager::Exit': 256, 'CRealTimeShadowManager::Init': 128, 'CRealTimeShadowManager::Update': 1024}
ADDR={'CCollision::SphereVsEntity': ('0x0041A5A0', 'S'), 'CPlayerPed::SetupPlayerPed': ('0x0060D790', 'S'), 'CPedGeometryAnalyser::CanPedTargetPed': ('0x005F1C40', 'S'), 'CPedGeometryAnalyser::ComputeClosestSurfacePoint-ped': ('0x005F3B70', 'S'), 'CRealTimeShadowManager::DoShadowThisFrame': ('0x00706BA0', 'S'), 'CRealTimeShadowManager::Exit': ('0x00706A60', 'S'), 'CRealTimeShadowManager::Init': ('0x007067C0', 'S'), 'CRealTimeShadowManager::Update': ('0x00706AB0', 'S')}
TOTAL=4608;assert sum(PAIRS.values())==TOTAL
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('cd1b9841') and summary['original_exe_sha256'].startswith('72ae59e4')
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
assert sha(HERE/'artifacts/gta_reversed-geofinal1-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-geofinal1-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON='Simple pair bench inside the full game (original with hook off vs candidate with hook on, hooked in CGame::Process), 4608 isolated CPU pairs for the 8 functions, zero differences, natural completion event with state_restored, saves unchanged, configs restored. Fake objects with 16-byte guards on each side (compared whole); the return value (AL only; high EAX bytes are stack garbage in the EXE), the ordered trace of outgoing calls with raw arguments, the x87 control word (24-bit precision during the call) and every fixture byte are compared; the hook jump state (JumpsToDll) is checked after State(candidate). Only real globals touched: CWorld::Players (0x320 bytes), PlayerInFocus and the six CTimeCycle shadow arrays (+ index); saved at start, restored after each run and rechecked at the end. Traced stubs, patched at the EXE address, at the DLL symbol and at the DLL body (bodies computed before patching), so these helpers get NO credit: CCamera::IsExtraEntityToIgnore, CEntity::GetColModel, CPlaceable::AllocateMatrix, CSimpleTransform::UpdateMatrix, CCollision::CalculateTrianglePlanes/TestSphereBox/TestSphereTriangle (scripted results), CPed::operator new, CPlayerPed::CPlayerPed, CPlaceable::SetOrientation, CWorld::Add, CPedGeometryAnalyser::CanPedTargetPoint, ComputeEntityBoundingBoxCornersUncached and ComputeClosestSurfacePoint (rect), CRealTimeShadowManager::ReInit and GetRealTimeShadow, CRealTimeShadow::Create/Destroy/Update, CShadowCamera::Create/Destroy/MakeGradientRaster, CShadows::StoreRealTimeShadow and Fx_c::GetFxQuality; the game operator new/delete (0x82119A/0x8214BD) are replaced only during Init and Exit. Only the own part of each function is validated. Floats: no signalling NaN, magnitudes <= 3e14 (x87 keeps the extended exponent in sums of squares, SSE does not). Bench-own bug fixed before the approved run: the EntryPatch loop computed bodies after patching (stub patched with a jump to itself); bodies are now computed first. Excluded: SetupPlayerPed with playerId = -1 (the EXE indexes Players[-1], foreign memory), only ids 0 and 1 with PlayerInFocus 0/1. Historical evidence approved with explicit limits; no current dependency accreditation.'
SPECIFIC={'CCollision::SphereVsEntity': 'Corpus 1024: the EXE ignores its sphere parameter and tests an uninitialised local CColSphere, i.e. 16 stack bytes at [S-0x68, S-0x59]; a naked bridge (CallSve) writes a 16-byte seed there (finite/NaN/infinite) while the real sphere argument is a NaN decoy that must be ignored; the source captures the same 16 bytes in a naked bridge before any push. Entity using collision / IsExtraEntityToIgnore, matrix present or created via AllocateMatrix+UpdateMatrix, model with and without data, 0..3 spheres/boxes, 0..4 triangles with and without face groups, negative counters (0xFFFF, 0x8000, 0xFFFFFFFF); TestSphere* and CalculateTrianglePlanes are scripted traced stubs.', 'CPlayerPed::SetupPlayerPed': 'Corpus 128: ids 0 and 1 with PlayerInFocus 0/1, fake 0x7A4-byte ped; CPed::operator new, the CPlayerPed constructor (DLL call located as the first call after operator new), SetOrientation and CWorld::Add are traced stubs; source uses CWorld::Players[playerId] directly. playerId = -1 not tested.', 'CPedGeometryAnalyser::CanPedTargetPed': 'Corpus 1024: fake peds with and without matrix, fake intelligence (the DUCK secondary task is really read), CanPedTargetPoint stubbed and traced; source rounding corrected (z + 0.75f then -1.0f when crouching).', 'CPedGeometryAnalyser::ComputeClosestSurfacePoint-ped': 'Corpus 512: fake peds with and without matrix; ComputeEntityBoundingBoxCornersUncached and the rect overload of ComputeClosestSurfacePoint are stubs tracing vector, direction bool, corner pointer, point and ped position.', 'CRealTimeShadowManager::DoShadowThisFrame': 'Corpus 512: fake manager (0x54), 16 fake shadows (0x4C) and 16 fake owners (0x140); two Fx_c::GetFxQuality calls counted; source corrected to the EXE.', 'CRealTimeShadowManager::Exit': 'Corpus 256: destroys each non-null shadow without looking at owners; game operator delete replaced only during the call; source corrected to the EXE.', 'CRealTimeShadowManager::Init': 'Corpus 128: arena for the allocations (operator new/delete of the EXE replaced during the call); CRealTimeShadow, CShadowCamera Create/Destroy/MakeGradientRaster, ReInit stubbed.', 'CRealTimeShadowManager::Update': 'Corpus 1024: signed intensity (test values with bit 7: 128, 129, 200, 253, 255 to exercise the movsx), CRealTimeShadow::Update and CShadows::StoreRealTimeShadow stubbed; source corrected to the EXE (signed intensity).'}
SOURCE_CHANGES={'source/game_sa/Collision/Collision.cpp': 'SphereVsEntity implemented from the EXE body with a naked bridge copying 16 uninitialised stack bytes (was NOTSA_UNREACHABLE)', 'source/game_sa/Entity/Ped/PlayerPed.cpp': 'SetupPlayerPed indexes CWorld::Players[playerId] directly', 'source/game_sa/PedGeometryAnalyser.cpp': 'CanPedTargetPed rounding (z+0.75f, then -1.0f when crouching)', 'source/game_sa/RealTimeShadowManager.cpp': 'Exit, Update and DoShadowThisFrame corrected to the EXE'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,function_summaries=len(PAIRS),unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-geo-final1.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');addr,typ=ADDR[name];hook={'class':cls,'fn_name':fn,'address':addr,'type':typ}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled GeoFinal1 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 4608-row log reaudited offline and preserved gzipped with the original hash. Collision.cpp, PlayerPed.cpp, PedGeometryAnalyser.cpp and RealTimeShadowManager.cpp were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/GeoFinal1/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.geo-final1.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
