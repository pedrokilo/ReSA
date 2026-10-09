"""Registro historico PARCIAL, POR FUNCION, del banco simple GfxWave2: 52 de 55 funciones (CClothesBuilder 14, CShadows 19, CWaterLevel 19). Re-audita la corrida 20261009-095105-342457 en streaming, rechaza explicitamente CWaterLevel::{AddWaterLevelQuad, RenderFlatWaterRectangle, SplitWaterTriangleAlongXLine} y conserva la evidencia en runs/."""
import csv,gzip,hashlib,json,os,re,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-095105-342457'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-gfxwave2')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
# Bank table (expected pair counts) parsed from the commented table in LiveComparison.hpp.
HPP=(HERE/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECT={}
for m in re.finditer(r'^//\s+\{"(\w+)","([\w\-]+)",0x[0-9A-Fa-f]+,\d+,-?\d+,(\d+)\},',HPP,re.M):EXPECT[m.group(1)+'::'+m.group(2)]=int(m.group(3))
assert len(EXPECT)==55 and sum(EXPECT.values())==27616,(len(EXPECT),sum(EXPECT.values()))
REJECTED=['CWaterLevel::AddWaterLevelQuad','CWaterLevel::RenderFlatWaterRectangle','CWaterLevel::SplitWaterTriangleAlongXLine']
EXPECTED_FAIL={'CWaterLevel::AddWaterLevelQuad':254,'CWaterLevel::RenderFlatWaterRectangle':472,'CWaterLevel::SplitWaterTriangleAlongXLine':258}
assert summary['passed'] is False and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['error']=='Funciones con diferencias: CWaterLevel::AddWaterLevelQuad: 254/1024; CWaterLevel::RenderFlatWaterRectangle: 472/1024; CWaterLevel::SplitWaterTriangleAlongXLine: 258/512'
assert summary['executed_probe_sha256'].startswith('9843870c') and summary['original_exe_sha256'].startswith('72ae59e4')
# Streaming re-audit, function by function.
events={};per={};bad={};fsum={};cases=set();complete=None;begin=None;err=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='error':err=row
        elif ev=='begin':begin=row
        else:assert ev=='waiting_for_player',ev
print('events',events)
# The bench writes 'complete' only when passed; with failures it writes a terminal 'error' event (LiveComparison.hpp line 155), here with no 'reason' (not an abort).
assert complete is None and err is not None and set(err)=={'event','pairs','state_restored'} and err['pairs']==27616 and err['state_restored'] is True
assert events.get('comparison')==27616 and events.get('begin')==1 and events.get('error')==1 and events.get('function_summary')==55 and len(cases)==27616
assert begin['known'] is True and all(x['ok'] is True for x in begin['sites'])
assert per==EXPECT,'pair counts differ from the bank table'
assert set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad==EXPECTED_FAIL,bad
GOOD=[f for f in EXPECT if f not in REJECTED]
assert len(GOOD)==52 and all(fsum[f]['failures']==0 and bad.get(f,0)==0 and per[f]==EXPECT[f] for f in GOOD)
assert [sum(1 for f in GOOD if f.startswith(c+'::')) for c in('CClothesBuilder','CShadows','CWaterLevel')]==[14,19,19]
PAIRS={f:per[f] for f in GOOD};GOODTOTAL=sum(PAIRS.values());ALL=27616
# Mapping against docs/hooks.csv (class + name; must be unique and pending).
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
tbl={m.group(1)+'::'+m.group(2):int(m.group(3),16) for m in re.finditer(r'^//\s+\{"(\w+)","([\w\-]+)",(0x[0-9A-Fa-f]+),',HPP,re.M)}
ADDR={};TYPE={}
for name in EXPECT:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
    if tbl[name]!=int(m[0]['address'],16):print('NOTE address differs bank table vs hooks.csv',name,hex(tbl[name]),m[0]['address'])
log_sha=sha(SRC/'comparisons.jsonl')
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
CHANGED_AFTER_RUN={}
for p,h in manifest['sources'].items():
    if sha(ROOT/p)!=h:CHANGED_AFTER_RUN[p]=dict(tested_sha256=h,current_sha256=sha(ROOT/p),current_mtime=__import__('datetime').datetime.fromtimestamp((ROOT/p).stat().st_mtime).isoformat(timespec='seconds'))
# Only WaterLevel.cpp is allowed to differ (edited in the working tree at 09:57, after the 09:52 run, to fix the rejected functions); the registry keeps the TESTED hash.
assert set(CHANGED_AFTER_RUN)<={'source/game_sa/WaterLevel.cpp'},CHANGED_AFTER_RUN
print('changed after run',CHANGED_AFTER_RUN)
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-gfxwave2-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-gfxwave2-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('PARTIAL, per-function approval. Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the hooks on), 27616 isolated CPU pairs for 55 functions of CClothesBuilder (14), CShadows (19) and CWaterLevel (22). '
 'Only the 52 functions whose pairs are ALL equal and whose pair count equals the bank table are registered (CClothesBuilder 14, CShadows 19, CWaterLevel 19); the run as a whole has passed=false because CWaterLevel::AddWaterLevelQuad (254/1024), RenderFlatWaterRectangle (472/1024) and SplitWaterTriangleAlongXLine (258/512) differ: they are REJECTED, not registered and get no credit. '
 'The run has no natural "complete" event: the bench writes "complete" only when passed, otherwise a terminal "error" event (pairs=27616, state_restored=true, no reason, i.e. the run finished normally); saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (fake objects; drawing replaced by traces of RwIm3D*/RwRaster*/etc. with raw arguments, nothing is drawn) and compares return value, collaborator traces, the bytes of the real fields of the fixtures (padding masked), and the restored globals; x87 at 24 bits, no signalling NaN, foreign threads ignored (Mine()). '
 'Traced stubs and out-of-bench pieces get NO credit (e.g. GetFirstTexture, RwRaster*/RwTextureCreate, CStreaming/CDirectory/CTimer collaborators, CEntity::GetColModel, CPed::GetBonePosition, CCamera::IsSphereVisible, CRealTimeShadow::*, RwIm3DTransform/RenderIndexedPrimitive/End, LittleTest, RwTextureDestroy, CWorldScan::ScanWorld, GetWaterLevelNoWaves, AddWaveToResult, FindPlayerCoors, CanSeeWaterFromCurrArea, HandleBeachToysStuff, FindNearestWaterAndItsFlow and, per case, sibling CWaterLevel drawing functions). '
 'Fx_c::GetFxQuality is a stub fixed at quality 0 WITHOUT trace: the drawOnBuildings-by-FX-quality logic of StoreShadowForPedObject/StoreRealTimeShadow/StoreCarLightShadow and the dist==0 check of RenderExtraPlayerShadows (NOTSA) are not exercised and get no credit. '
 'Masked/controlled noise: CWaterVertex padding (copied by the EXE from stack garbage), CRenPar 2-byte padding, structure padding; AddWaterLevelQuad kind 9 uses two Y levels (the EXE reads uninitialised stack); NumLights fixtures 0..6 (uint32 in the DLL vs signed int in the EXE); MAX_DISTANCE_PED_SHADOWS_SQR (0xC4B6B0) is a variable in the EXE vs constexpr 225 in the DLL; Clothes only coherent raster sizes (w*h<=36, 32 bits), so the DLL AssertTextureLayouts and the SOURCE/DEST pixel-count difference are not exercised. '
 'OUT of the bank and without credit: CClothesBuilder::CreateSkinnedClump (streaming of PLAYER.IMG and real RW objects), CShadows::RenderStoredShadows and CastPlayerShadowSectorList (real world sectors and pool nodes), CWaterLevel::WaterLevelInitialise (water.dat + sscanf), RenderBoatWakes (inlined pass over boats) and SplitWaterTriangleAlongYLine (source does not yet equal the 4 EXE branches). '
 'Only the own part of each function is validated. Source corrected to what the EXE does before the run (uncommitted at the time; see reversing/GfxWave2/notes_Cl.md, notes_Sh.md, notes_Wl.md for addresses): see source_corrections_before_run. '
 'WARNING: source/game_sa/WaterLevel.cpp was modified in the working tree AFTER the approved run (see source_changed_after_run); the registry keeps the hash that was TESTED, so source_match is false for the CWaterLevel entries until that file returns to the tested state or is re-validated. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPEC_TXT={
 'CClothesBuilder::GetAtomicWithName':'source corrected (0x5A47E0 callback): the FIRST match wins and stops RpClumpForAllAtomics (the source kept the last).',
 'CClothesBuilder::AddWeightToBoneVertex':'source corrected: free slot -> weights[i]+=add and weights[i+1]=0; with 8 busy slots and no match the EXE does weights[8]+=add.',
 'CClothesBuilder::StoreBoneArray':'source corrected: forward loop without limit and a single -1 terminator if i<64 (the source filled the whole row with -1).',
 'CClothesBuilder::BuildBoneIndexConversionTable':'source corrected: cmp al,0FFh compares only the low byte (RpHAnimIDGetIndex = -1 -> 0).',
 'CClothesBuilder::CopyTexture':'source corrected: format = (cFormat & 0x6F) << 8 | 4.',
 'CClothesBuilder::PlaceTextureOnTopOfTexture':'source corrected: lock SOURCE first then destination, pixel count = SOURCE width*height, unlock source then destination.',
 'CClothesBuilder::BlendTextures-Dst-Src':'source rewritten (BlendTextures x3): lock/unlock order, pixel count of the first source, 3 channels, _ftol2 conversion and low byte without saturation, write order channel 0,2,1, CTimer::Suspend/Resume; destination base-pointer alpha as stated by the requester (notes_Cl.md records alpha untouched).',
 'CClothesBuilder::BlendTextures-Dst-Src1-Src2':'source rewritten (BlendTextures x3): src2*r3 + src1*r2 + dst*r1; same lock order and conversion as the others; destination base-pointer alpha as stated by the requester.',
 'CClothesBuilder::BlendTextures-Dst-Src1-Src2-Tat':'source rewritten (BlendTextures x3): tattoo term fild((tat-v)*alpha)*0x3B808081 + v; destination base-pointer alpha as stated by the requester.',
 'CClothesBuilder::LoadCdDirectory':'compared against the EXE as is (no source change).',
 'CClothesBuilder::RequestGeometry':'compared against the EXE as is (bHasComplexHierarchy = bit 1 of +0x13).',
 'CClothesBuilder::RequestTexture':'compared against the EXE as is.',
 'CClothesBuilder::DestroySkinArrays':'compared against the EXE as is (two operator delete 0x8214BD, local EntryPatch).',
 'CClothesBuilder::GetTextureFromTxdAndLoadNextTxd':'compared against the EXE as is.',
 'CShadows::RemoveOilInArea':'source corrected: strict comparisons and NaN out (was inclusive CRect::IsPointInside).',
 'CShadows::AffectColourWithLighting':'source corrected: nibbles * 0x3D088889 instead of /30; minimum (A<B)?A:B.',
 'CShadows::StoreShadowForPedObject':'source corrected: !(d<225), full int16 strength, z without adding 0.0f; GetFxQuality stub without trace (no credit for the FX-quality branch).',
 'CShadows::StoreRealTimeShadow':'source corrected: !(d<225); intensity movsx byte*0.01f*int16 strength; GetShadowRwTexture + DrawBorderAroundTexture added; elevation -deg(atan2(-y,-z)); CVector::Normalise added; CalcPedShadowValues argument order; GetBonePosition(CVector*,..) form; GetFxQuality stub without trace.',
 'CShadows::RenderExtraPlayerShadows':'source corrected: m_nLightShadowStrength at 0xB7C4EA, !(dist<radius), direction L*(1/dist), size/position/sign fixes, up.z>0, and the call order FindPlayerCoors/FindPlayerVehicle/GetColModel x5 per light as the EXE; the dist==0 NOTSA check is not exercised.',
 'CShadows::RenderIndicatorShadow':'source corrected: size (frontX > -sideX) ? frontX : -sideX, compared with -sideY as 0x70CCB3 fld [esp+2Ch].',
 'CShadows::StoreShadowForPole':'source corrected: intensity int32 of ((up.z-0.5)*strength)*2 then 2*int/3; position order; id = entity + localId + 0x33.',
 'CShadows::StoreCarLightShadow':'source corrected: vector = position - camera, !(d2<729), dot > -maxViewAngle, fade (dist-18)*0.11111111, speed > 0.4 and call order; GetFxQuality stub without trace.',
 'CShadows::RenderStaticShadows':'source corrected: inner loop from the current shadow, int16 intensity alpha, UV*0.005f, bRendered after the polygon walk.',
 'CShadows::CalcPedShadowValues':'NaN handling declared by the requester; notes_Sh.md lists it as a MATCH (no change) at 0x7076C0.',
 'CShadows::Init':'compared against the EXE as is.','CShadows::Shutdown':'compared against the EXE as is.','CShadows::TidyUpShadows':'compared against the EXE as is.',
 'CShadows::StoreShadowToBeRendered-Texture':'compared against the EXE as is.','CShadows::StoreShadowToBeRendered-Type':'compared against the EXE as is (table 0x707B20).',
 'CShadows::SetRenderModeForShadowType':'compared against the EXE as is (table 0x7074CC).','CShadows::GunShotSetsOilOnFire':'compared against the EXE as is.',
 'CShadows::PrintDebugPoly':'compared against the EXE as is.','CShadows::UpdateStaticShadows':'compared against the EXE as is.',
 'CWaterLevel::AddWaterLevelVertex':'source corrected: saturation with <=/>= (X<=-3000, X>=3000) and parameters {z=0, bigWaves=1.0f, smallWaves=0, flow=0}.',
 'CWaterLevel::AddWaterLevelTriangle':'source corrected: no asserts; writes only verts[3] + 3 flag bits.',
 'CWaterLevel::CalculateWavesOnlyForCoordinate':'source rewritten as an exact __asm transcription of the EXE (sine table 0xBB3E00, constants 0x871FF8-0x872020, glare clamp).',
 'CWaterLevel::ScanThroughBlocks':'source corrected: x*0.002f + 6.0f; callback normalised (BlockHit exe/DLL) in the bank; relies on CCamera::GetFrustumPoints giving the EXE corner order.',
 'CWaterLevel::PreRenderWater':'source corrected (UpdateFlow NOTSA): step = TimeStep*0.001f, |d|<step -> desired, else cur -/+ step.',
 'CWaterLevel::RenderWaterTriangle':'source corrected: split by MAX first when maxX>CameraRangeMaxX (and for Y).',
 'CWaterLevel::RenderWaterRectangle':'source corrected: MaxX priority, SetUpWaterFog(minX,Y2,maxX,Y1) after RenderFlatWaterRectangle in the two out-of-camera paths, SetUpWaterFog argument order (minX,minY,maxX,maxY); sibling RenderFlatWaterRectangle/Split* are traced/isolated, their own mismatch is NOT credited.',
 'CWaterLevel::SplitWaterRectangleAlongYLine':'source corrected: t = (split-Y1)/(Y2-Y1) over Y1,Y2 as they are.',
 'CWaterLevel::RenderFlatWaterRectangle_OneLayer':'source rewritten: flush thresholds 0xFFA/0x7FC with function 0x6E7680, layer scales/offsets/alpha, base UV, TL,TR,BL,BR order and indices {0,1,2,3,1,2}, z+0.0f, colour packing.',
 'CWaterLevel::RenderFlatWaterTriangle_OneLayer':'source rewritten: thresholds 0xFFD/0x7FD, layers, UV (Xi-X1)*scale+baseU, colour packing.',
 'CWaterLevel::RenderWakeSegment':'source rewritten: corners (1-t)*A+t*B and (1-t)*D+t*C, UV, alpha, full x87 fsin z, indices {0,2,1,0,3,2}; uses real time/UV offsets saved in Globals().',
 'CWaterLevel::SetUpWaterFog':'source corrected: returns without registering when big==0 && small==0, gbPlayerIsInsideWaterFog only set to 1, FindPlayerCoors called before GetWaterLevelNoWaves.',
 'CWaterLevel::Shutdown':'compared against the EXE as is.','CWaterLevel::BlockHit':'compared against the EXE as is.','CWaterLevel::MarkQuadsAndPolysToBeRendered':'compared against the EXE as is.',
 'CWaterLevel::GetWaterLevel-':'compared against the EXE as is (overloaded hook name GetWaterLevel-).','CWaterLevel::SetCameraRange':'compared against the EXE as is.',
 'CWaterLevel::RenderFlatWaterTriangle':'compared against the EXE as is.','CWaterLevel::SplitWaterRectangleAlongXLine':'compared against the EXE as is.'}
SPECIFIC={n:'Corpus %d; '%PAIRS[n]+SPEC_TXT.get(n,'compared against the EXE as is (no source change).') for n in GOOD}
CLS_SRC={'CClothesBuilder':['source/game_sa/ClothesBuilder.cpp','source/game_sa/ClothesBuilder.h'],'CShadows':['source/game_sa/Shadows.cpp','source/game_sa/Shadows.h'],'CWaterLevel':['source/game_sa/WaterLevel.cpp','source/game_sa/WaterLevel.h']}
SOURCE_CHANGES={'source/game_sa/ClothesBuilder.cpp':'GetAtomicWithName, AddWeightToBoneVertex, StoreBoneArray, BuildBoneIndexConversionTable, CopyTexture, PlaceTextureOnTopOfTexture, BlendTextures x3 (incl. destination base-pointer alpha as stated), CreateSkinnedClump (no bank, no credit)',
 'source/game_sa/Shadows.cpp':'RemoveOilInArea, AffectColourWithLighting, StoreShadowForPedObject and StoreRealTimeShadow (GetBonePosition(CVector*,..)), RenderExtraPlayerShadows (call order), RenderIndicatorShadow, StoreShadowForPole, StoreCarLightShadow, RenderStaticShadows, CalcPedShadowValues (NaN), RenderStoredShadows (no bank, no credit)',
 'source/game_sa/WaterLevel.cpp (+ .h)':'lerp = (1-t)*a + t*b, SetUpWaterFog order, RenderWaterRectangle (Y order, MaxX priority, SetUpWaterFog), AddWaterLevelVertex/Triangle/Quad, CalculateWavesOnlyForCoordinate, ScanThroughBlocks, PreRenderWater, RenderWaterTriangle, SplitWaterRectangleAlongYLine, OneLayer renderers, RenderWakeSegment, SplitWaterTriangleAlongXLine X1==X3 branch (rejected), WaterLevelInitialise rep stos (no bank)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=ALL,registered_pairs=GOODTOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the approved run; not registered, no credit') for f in REJECTED}
OUT_OF_BENCH=['CClothesBuilder::CreateSkinnedClump','CShadows::RenderStoredShadows','CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>','CWaterLevel::WaterLevelInitialise','CWaterLevel::RenderBoatWakes','CWaterLevel::SplitWaterTriangleAlongYLine']
for f in OUT_OF_BENCH:
    c,n=f.split('::');assert len([r for r in rows if r['class']==c and r['fn_name']==n])==1,f
out=dict(source_changed_after_run=CHANGED_AFTER_RUN,passed=False,partial_per_function_approval=True,pairs=ALL,differences=sum(bad.values()),registered_pairs=GOODTOTAL,registered_functions=len(GOOD),functions=PAIRS,covered_functions={k:True for k in GOOD},rejected_functions=rejected,scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=False,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=ALL,complete_events=0,terminal_error_events=1,terminal_error_event=err,begin_events=1,waiting_for_player_events=events.get('waiting_for_player',0),function_summaries=55,unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 out_of_bench_not_credited=OUT_OF_BENCH,
 stubs_without_credit=['Fx_c::GetFxQuality (fixed at quality 0, no trace)'],
 readme_discrepancies='reversing/GfxWave2/README.md said "Sin compilar ni ejecutar todavia" and "Pendiente de re-ejecutar el banco" (pre-run state); both superseded by the approval section added at registration. Earlier runs (previous.jsonl and older run folders) are not preserved. The run has no "complete" event by design of the bench (terminal "error" event when passed=false).')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-gfx-wave2.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name in GOOD:
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources={p:manifest['sources'][p] for p in CLS_SRC[cls]},
     source_provenance='Exact compiled GfxWave2 probe (ASI/PDB hashes in build-manifest.json); the class sources are hashed from the probe build manifest; full 27616-row log reaudited offline in streaming and preserved gzipped with the original hash. PARTIAL approval: per function, only functions with all pairs equal. The three class sources were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/GfxWave2/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.gfx-wave2.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp),registered_pairs=GOODTOTAL),indent=2))
