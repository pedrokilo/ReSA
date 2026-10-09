"""Registro historico de CAEAudioHardware::Initialise y tres funciones de CAEFrontendAudioEntity (banco simple AudioFinal1). Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 4 entradas (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-010133-465573'
RESULTS=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-audiofinal1')
SRC=RESULTS/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
PAIRS={'CAEAudioHardware::Initialise':1536,'CAEFrontendAudioEntity::Initialise':512,'CAEFrontendAudioEntity::IsLoadingTuneActive':128,'CAEFrontendAudioEntity::Reset':128}
ADDR={'CAEAudioHardware::Initialise':'0x004D9930','CAEFrontendAudioEntity::Initialise':'0x005B9AB0','CAEFrontendAudioEntity::IsLoadingTuneActive':'0x004DD470','CAEFrontendAudioEntity::Reset':'0x004DD440'}
TOTAL=2304;assert sum(PAIRS.values())==TOTAL
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('3442924d') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
# Independent offline reaudit of the full log.
events={};per={};unequal=0;cases=set();complete=None;coverage={}
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:unequal+=1
        elif ev=='function_summary':
            assert row['failures']==0 and row['cases']==PAIRS[row['function']] and row['coverage']['accepted'] is True,row;coverage[row['function']]=row['coverage']
        elif ev=='complete':complete=row
        elif ev=='stream_thread':assert row['running'] is True and row['parked'] is True
        else:assert ev=='waiting_for_player',ev
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('function_summary')==4 and events.get('stream_thread')==1 and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True
# Earlier rejected attempt (first run), kept only as hashes.
REJECTED_ID='20261009-005347-353159';rej=json.loads((RESULTS/REJECTED_ID/'summary.json').read_text(encoding='utf-8'))
assert rej['passed'] is False and 'CAEAudioHardware::Initialise: 341/1536' in rej['error']
assert sha(SRC/'previous.jsonl')==sha(RESULTS/REJECTED_ID/'comparisons.jsonl')
prev_bad=0
with open(SRC/'previous.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line)
        if row.get('event')=='comparison' and row.get('equal') is not True:
            prev_bad+=1;assert row['function']=='CAEAudioHardware::Initialise' and row['differs']==['arena_used','blocks','recs'],row
assert prev_bad==341
# Build manifest and sources (must be the exact files the probe was built from).
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-audiofinal1-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-audiofinal1-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('Simple pair bench inside the full game (original with the hook off vs candidate with the hook on), 2304 isolated CPU pairs for 4 functions, zero differences, natural completion event with state_restored, saves unchanged, configs restored, per-function coverage thresholds met. '
 'CAEAudioHardware::Initialise runs on a FAKE 0x1100-byte CAEAudioHardware (pseudo-random fill per case, identical in both passes); the real AEAudioHardware is never touched. DirectSoundCreate (EXE IAT 0x85801C target) and DirectSoundCreate8 (DLL import) are patched in dsound.dll only during the call and return a fake COM device (QueryInterface/AddRef/Release, GetCaps, SetCooperativeLevel and GetSpeakerConfig with scripted HRESULTs and capabilities); the real device is never opened. '
 'operator new (EXE 0x82119A, to which the DLL one forwards) is patched only during the call and hands out blocks of a 1 MiB arena pre-filled with 0xCD (other threads are forwarded to the original), so the REAL constructors of the loaders and channels run (EXE vs DLL) and are compared block by block (offset, size, FNV hash; the vtable pointer, dword 0, of the channel blocks of size 0x90 and 0x60098 is masked). '
 'Compared for CAEAudioHardware::Initialise: return value (AL), hash of the whole fake object, arena blocks, arena bytes used, the fade-thread buffer count (0xB610E8), the user-track scan state (0xB6B988) and the trace of every stubbed call with normalised arguments. '
 'Traced stubs, patched at the EXE address and at the DLL body, so these helpers get NO credit: CAEAudioHardware::InitDirectSoundListener, CAEMP3BankLoader::Initialise, CAEMP3TrackLoader::Initialise, CAEStreamingChannel::Initialise, CAEAudioChannel::SetVolume, CAEUserRadioTrackManager::ScanUserTracks and Initialise, CAESmoothFadeThread::Initialise and Start, CAEStreamThread::Initialise and Start, CAEAudioHardware::LoadSoundBank and LoadSound, CAEAudioUtility::GetRandomNumberInRange<int>, CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity and CancelSoundsOfThisEventPlayingForThisEntity; IsForegroundApp parks the real streaming thread during the bench. No audio thread is created or started. '
 'Declared limits: (1) after a stubbed MP3 bank or track loader Initialise returns false the DLL writes an NOTSA_LOG_ERR message (spdlog allocates through operator new) that the EXE does not have; from that point the trace, the block list and the arena usage are frozen (Freeze()), so allocations or stubbed calls made by the DLL after that failure are not detected through those channels, only through the object hash, the return value and the compared globals; (2) the path with m_nUserTracksScanState == IN_PROGRESS (Sleep(100) loop) is not tested, scan states used are OFF, COMPLETE, ERROR, 4, -1 and INT_MAX; (3) DirectSoundCreate is replaced by a fake COM device, so the real DirectSound behaviour is not exercised; (4) the constructors of channels and loaders run for real (EXE vs DLL) but are not validated here and are only compared as a side effect. '
 'Source corrected to what the EXE does before this run: CAEAudioHardware::Initialise uses operator new(0x6E4) plus Constructor() for CAEMP3BankLoader (the EXE only writes 0x14 and 0x66A, 0x4DFB10); CAEFrontendAudioEntity::Initialise writes the fields that were missing (0x80, 0x82=0x1C, 0x7E=-1, 0x84..0x94) and LoadSound takes a signed byte id (movsx). '
 'An earlier run (20261009-005347-353159, probe 17c9d956...) failed with 341 differences in 1536 CAEAudioHardware::Initialise pairs, all of them only in arena_used, blocks and recs (the NOTSA log allocations of limit 1); it is kept rejected as hashes only. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={
 'CAEAudioHardware::Initialise':'Corpus 1536: DirectSoundCreate HRESULT E_FAIL (1/8), S_FALSE (about 1/7) or S_OK; SetCooperativeLevel HRESULT 0x88780078 (1/8), S_FALSE or S_OK; GetCaps E_FAIL (1/4) or S_OK with random DSCAPS; random speaker config; dwFreeHw3DAllBuffers cycling {0,1,0x17,0x18,0x19,0x1F,0x30,0x3F,0x40,0x41,0x1000,0xFFFFFFFF} around the 0x18/0x40 thresholds; listener, bank loader and track loader initialise success 7/8 each; auto-scan byte 0, 0x80 or 1; scan state {0,2,3,4,-1,INT_MAX}; random initial fade count. Coverage in the approved run: 777 cases reach CAEStreamThread::Start (759 stop earlier), 573 with hardware mixing (>= 0x18) and 204 without, ScanUserTracks reached in 529.',
 'CAEFrontendAudioEntity::Initialise':'Corpus 512: fake 0x1EC-byte entity with pseudo-random content and 16 guard bytes (0xCD) after it; LoadSoundBank, LoadSound and GetRandomNumberInRange<int> are traced stubs; the stubbed random value cycles {0,1,2,3,126,127,128,129,200,255,256,300,-1,0x1234}, 182 of 512 with a byte >= 128 (signed LoadSound id). Return value is void and not compared.',
 'CAEFrontendAudioEntity::IsLoadingTuneActive':'Corpus 128: fake entity, AreSoundsOfThisEventPlayingForThisEntity stub returns 0 or non-zero (67 of 128 non-zero); return value (AL) and the whole object are compared.',
 'CAEFrontendAudioEntity::Reset':'Corpus 128: fake entity with pseudo-random content, CancelSoundsOfThisEventPlayingForThisEntity is a traced stub; void, the object and the trace are compared.'}
SOURCE_CHANGES={'source/game_sa/Audio/Hardware/AEAudioHardware.cpp':'Initialise: operator new(0x6E4) + Constructor() for CAEMP3BankLoader','source/game_sa/Audio/Loaders/AEMP3BankLoader.h':'friend class CAEAudioHardware so Initialise can reserve the block and call Constructor()','source/game_sa/Audio/Entities/AEFrontendAudioEntity.cpp':'Initialise writes the missing fields; LoadSound id is signed'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (1.2 GB, not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,function_summaries=4,unequal=0,distinct_cases=len(cases),per_function=per,coverage=coverage),
 source_corrections_before_run=SOURCE_CHANGES,
 rejected_attempts={REJECTED_ID:dict(error=rej['error'],executed_probe_sha256=rej['executed_probe_sha256'],log_sha256=sha(RESULTS/REJECTED_ID/'comparisons.jsonl'),differs_only_in=['arena_used','blocks','recs'],cause='NOTSA_LOG_ERR allocations in the DLL after a failed stubbed MP3 loader Initialise; fixed in LiveComparison.hpp by freezing the trace/arena at that point; log not copied to the repository')})
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-audio-final1.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);assert n==6891,n
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':'S'}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled AudioFinal1 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 2304-row log reaudited offline and preserved gzipped with the original hash. The audio sources were corrected to the EXE before the run (uncommitted at the time); the first attempt (341 bench-noise differences) is kept rejected as hashes.',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/AudioFinal1/README.md'))
temp=registry.with_suffix('.audio-final1.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
