"""Strict independent admission gates. No approval before a complete runtime log."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={
    'CMemoryMgr::Malloc-SizeHint':2048,
    'CMemoryMgr::Realloc-MemorySizeHint':2048,
    'CMemoryMgr::MallocAlign-SizeAlignHint':2048,
    'CMemoryMgr::FreeAlign':2048,
}
GATES=('argument_after_matches','stack_restored','nonvolatile_preserved','route_ok','candidate_code_stable','allocator_arguments_exact','allocator_callers_valid','observer_scope_ok','debug_readback_ok',
       'helper_route_ok','fixture_contract','return_contract','alignment_ok',
       'backpointer_ok','resources_balanced','canaries_preserved','floating_state_restored')

from trace_model import expected,validate
import re
KINDS=('Malloc','Realloc','MallocAlign','FreeAlign')
ENTRIES=(0x72F420,0x72F440,0x72F4C0,0x72F4F0)

def must(value,message):
    if not value:raise RuntimeError(message)

def raw_trace(begin,events,plan,serial,gate,fn,side='calibration'):
    must(len(events)==len(plan),'Trace length')
    for i,(event,(entry,args)) in enumerate(zip(events,plan)):
        slot={0x824257:0,0x824269:1,0x82413F:2}[entry]
        must(event['entry']==event['eip']==entry and event['args']==args,'Actual CRT arguments')
        must(event['owner_thread']==begin['owner_thread'] and event['scope_serial']==serial and event['sequence']==i,'Trace ownership/order')
        must(event['dr7']==begin['debug_armed']['dr7'] and event['dr6']==(0xFFFF0FF0 | (1<<slot)),'Hardware reason')
        must(event['esp']>0 and event['esp']%4==0 and all(event.get(k) is True for k in ('stack_words_valid','context_preserved','caller_origin_valid')),'Context/stack proof')
        ret=event['return_address']
        if i:valid=ret==({0x824257:0x824286,0x82413F:0x824299}.get(entry))
        elif side=='calibration':valid=ret==gate
        elif side=='original':valid=ret==((0x72F42A,0x72F44F,0x72F4D1,gate)[fn])
        else:
            body=begin['candidate_prefixes'][fn]['body'];valid=ret==gate or body<=ret<body+1024
        must(valid,'Independent actual return address')

def check_observer(begin,rows):
    must(all(begin.get(k) is True for k in ('registered_hook','helper_routes_ok','observer_ready','calibration_passed','calibration_resources_restored')) and begin.get('crt_entries_modified') is False,'Observer admission')
    must(begin['owner_thread']>0 and begin['probe_module']>0 and begin['probe_module']!=0x400000,'Own ASI')
    saved,armed,readback=(begin[k] for k in ('debug_saved','debug_armed','debug_readback'))
    must(not saved['dr7']&0x20FF and not saved['dr6']&0xE00F,'Debugger conflict')
    expected_debug=dict(saved,dr0=0x824257,dr1=0x824269,dr2=0x82413F,dr6=0xFFFF0FF0,dr7=(saved['dr7']&~0x0FFF00FF)|0x15)
    must(armed==expected_debug and all(readback[k]==armed[k] for k in armed if k!='dr6') and readback['dr6']&~7==armed['dr6']&~7,'Readback debug registers')
    source=(HERE/'OriginalCode.hpp').read_text()
    originals=[[int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]{2})',v)] for v in re.findall(r'native_[0-9A-F]+\{([^}]+)\}',source)]
    must(begin['original_function_bytes']==originals,'Canonical original bodies')
    prefixes=begin['candidate_prefixes'];must(len(prefixes)==4,'Candidate routes')
    for prefix in prefixes:must(prefix['module']==begin['probe_module'] and prefix['body']>=prefix['module'] and len(prefix['stable_prefix_bytes'])==64 and prefix['identity_with_exe_required'] is False,'Candidate module prefix')
    crt=(HERE.parent/'DependenciesMemory9/CrtProof.hpp').read_text()
    helpers=begin['helper_routes'];must(len(helpers)==3 and {h['entry'] for h in helpers}=={0x824257,0x824269,0x82413F},'CRT routes')
    for h in helpers:must(h['route_ok'] is True and h['body']==h['entry'],'Native CRT collaborator')
    # Exact full CRT bytes are independently cross-checked below against immutable source arrays.
    arrays=re.findall(r'(?:native|crt)_([0-9A-Fa-f]+)[^=]*?\{([^}]+)\}',crt)
    for h in helpers:
        matches=[v for address,v in arrays if int(address,16)==h['entry']]
        must(len(matches)==1,'Canonical CRT array absent')
        canonical=[int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]{2})',matches[0])]
        must(h['bytes']==canonical,'Full native CRT bytes')
    cal=begin['calibration'];names=('malloc33','free_malloc33','realloc_null17','realloc_owned_zero','realloc_owned65','reject_malloc17_expected18','outside_scope','other_thread')
    must(tuple(c['name'] for c in cal)==names and all(c.get('passed') is True for c in cal),'Calibration campaign')
    for i,c in enumerate(cal[:5]):
        args=c['input'];plan=([(0x824257,[33])] if i==0 else [(0x82413F,[args[0]])] if i==1 else [(0x824269,[0,17]),(0x824257,[17])] if i==2 else [(0x824269,[args[0],0]),(0x82413F,[args[0]])] if i==3 else [(0x824269,[args[0],65])])
        must(c['aborted'] is False and c['scope_serial']==i+1,'Calibration aborted')
        if i in (1,3,4):must(args[0]>0,'Real calibration allocation')
        raw_trace(begin,c['trace'],plan,i+1,c['gate_return'],0)
    neg=cal[5];must(neg['aborted'] is True and neg['expected_size']==18 and neg['actual_size']==17 and neg['allocator_instruction_executed'] is False,'Mandatory rejected argument')
    raw_trace(begin,neg['trace'],[(0x824257,[17])],6,neg['gate_return'],0)
    must(cal[6]['owned_hits_delta']>=2 and cal[7]['owner_trace_count']==0,'Thread/scope isolation')
    finals=[r for r in rows if r['event']=='complete'];must(len(finals)==1,'Unique completion')
    final=finals[0];must(all(final.get(k) is True for k in ('state_restored','debug_regs_restored','veh_removed','worker_closed','observer_context_preserved','helper_route_ok','floating_state_restored')) and final['debug_final']==saved and final['foreign_hits']==0 and final['outside_scope_hits']>=2,'Observer restoration')

def check_pair(begin,row):
    fn=list(EXPECTED).index(row['function']);case=row['case']
    sizes=(0,1,2,3,4,7,8,15,16,17,31,32,33,63,64,65,127,128,129,255,256,257,511,512,513,1023,1024,1025,2047,2048,2049,4096)
    size=sizes[case%32];align=(4,8,16,32,64,128,256)[case//32%7]
    old=0 if case//32%8==0 else sizes[1+(case//8)%31]
    for side in ('original','candidate'):
        r=row[side];e=row[side+'_allocator_evidence'];pointer=e['input_pointer'];base=e['allocation_base']
        must(r['payload_size']==size and r['old_size']==(old if fn==1 else 0) and r['alignment']==(align if fn>=2 else 0) and r['hint']==case*2654435761&0xFFFFFFFF,'Fixture sequence')
        if fn==1:must(bool(pointer)==bool(old),'Real realloc identity')
        elif fn!=3:must(pointer==0,'Unexpected input')
        fixture=dict(size=size,alignment=align if fn>=2 else 0,input_pointer=pointer,allocation_base=base)
        must(e['argument_after']==(base if fn==3 else pointer if fn==1 else size),'Physical incoming argument after return')
        plan=expected(KINDS[fn],**fixture);serial=1000+row['pair']*2+(side=='candidate')
        raw_trace(begin,e['trace'],plan,serial,e['gate_return'],fn,side)
        normalized=validate(KINDS[fn],fixture,e['trace'],begin['owner_thread'],serial)
        must(r['allocator_trace']==[list(x) for x in normalized],'Trace normalization')

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1:raise RuntimeError('Missing unique begin')
    check_observer(begin[0],rows)
    cases={fn:set() for fn in EXPECTED};pairs=set();realloc_profiles=set();alignments=set();sizes=set()
    for row in rows:
        if row['event'].startswith('first_chance_av'):raise RuntimeError('Invalid access')
        if row['event']!='comparison':continue
        fn=row['function'];case=row['case']
        if row.get('equal') is not True or row.get('accepted') is not True or row['original']!=row['candidate']:raise RuntimeError('Pair not explicitly accepted')
        if row['pair'] in pairs or case in cases[fn]:raise RuntimeError('Duplicate fixture or pair')
        check_pair(begin[0],row)
        pairs.add(row['pair']);cases[fn].add(case)
        for side in ('original','candidate'):
            r=row[side]
            if not isinstance(r,dict) or not all(r.get(k) is True for k in GATES):raise RuntimeError('NULL result or failed runtime gate')
            if r['allocations']!=r['frees']:raise RuntimeError('Unbalanced owned resource')
            size=r['payload_size'];old=r['old_size']
            if fn.endswith('Malloc-SizeHint'):
                if r['requested_size']!=size or len(r['defined_payload'])!=size:raise RuntimeError('Malloc defined extent mismatch')
                sizes.add(size)
            elif fn.endswith('Realloc-MemorySizeHint'):
                kept=min(old,size)
                if r['requested_size']!=size or r['prefix_preserved_bytes']!=kept or len(r['defined_payload'])!=kept:raise RuntimeError('Realloc prefix mismatch')
                if r['return_kind']!=('null_after_free' if old and size==0 else 'owned_block'):raise RuntimeError('Realloc NULL contract mismatch')
                realloc_profiles.add(('null' if old==0 else 'owned','zero' if size==0 else 'shrink' if old>size else 'same' if old==size else 'grow'))
            else:
                align=r['alignment'];alignments.add(align)
                if align<4 or align&(align-1) or r['requested_size']!=size+align:raise RuntimeError('Invalid alignment extent')
                if fn.endswith('MallocAlign-SizeAlignHint') and len(r['defined_payload'])!=size:raise RuntimeError('Aligned payload mismatch')
                if fn.endswith('FreeAlign') and r['return_kind']!='void':raise RuntimeError('Free return incorrectly interpreted')
    if pairs!=set(range(8192)) or any(v!=set(range(2048)) for v in cases.values()):raise RuntimeError('Incomplete coverage')
    if sizes!={0,1,2,3,4,7,8,15,16,17,31,32,33,63,64,65,127,128,129,255,256,257,511,512,513,1023,1024,1025,2047,2048,2049,4096}:raise RuntimeError('Missing size boundaries')
    if alignments!={4,8,16,32,64,128,256}:raise RuntimeError('Missing valid alignments')
    if not {('null','zero'),('null','grow'),('owned','zero'),('owned','shrink'),('owned','same'),('owned','grow')}<=realloc_profiles:raise RuntimeError('Missing legal realloc branches')
    final=[r for r in rows if r['event']=='complete']
    if len(final)!=1 or final[0]['pairs']!=8192 or not all(final[0].get(k) is True for k in ('state_restored','helper_route_ok','floating_state_restored')):raise RuntimeError('Final restoration missing')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciesmemorycpp16',manifest['binaries']['asi'],analyze,'DependenciesMemoryCpp16',timeout=1500))
