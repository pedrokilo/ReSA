"""Auditor offline adicional; no ejecuta juego ni sustituye admisión completa del banco."""
import argparse
import collections
import json
from pathlib import Path
M,R,F=0x824257,0x824269,0x82413F
IDLE=0xFFFF0FF0
NAMES=('CMemoryMgr::Malloc-SizeHint','CMemoryMgr::Realloc-MemorySizeHint','CMemoryMgr::MallocAlign-SizeAlignHint','CMemoryMgr::FreeAlign')
SIZES=(0,1,2,3,4,7,8,15,16,17,31,32,33,63,64,65,127,128,129,255,256,257,511,512,513,1023,1024,1025,2047,2048,2049,4096)
ALIGNS=(4,8,16,32,64,128,256)
def require(value,label):
    if not value:raise ValueError(label)
def trace(begin,events,plan,serial,gate,kind=-1,side='calibration'):
    require(len(events)==len(plan),'Cantidad física de eventos')
    for i,(event,(entry,args)) in enumerate(zip(events,plan)):
        slot={M:0,R:1,F:2}[entry]
        require(event['entry']==event['eip']==entry and event['args']==args,'Entrada/argumentos físicos')
        require(event['sequence']==i and event['scope_serial']==serial and event['owner_thread']==begin['owner_thread'],'Identidad de captura')
        require(event['dr6']==IDLE|1<<slot and event['dr7']==begin['debug_armed']['dr7'],'Causa DR exacta')
        require(event['esp']>0 and event['esp']%4==0 and all(event.get(k) is True for k in ('stack_words_valid','context_preserved','caller_origin_valid')),'Pila/contexto')
        ret=event['return_address']
        if i:valid=ret=={M:0x824286,F:0x824299}.get(entry)
        elif side=='calibration':valid=ret==gate
        elif side=='original':valid=ret==(0x72F42A,0x72F44F,0x72F4D1,gate)[kind]
        else:
            body=begin['candidate_prefixes'][kind]['body'];valid=ret==gate or body<=ret<body+1024
        require(valid,'Procedencia de retorno')
def audit(path,require_slots=False):
    rows=[json.loads(line) for line in Path(path).read_text(encoding='utf-8').splitlines()]
    begins=[r for r in rows if r['event']=='begin'];require(len(begins)==1,'Inicio único');begin=begins[0]
    require(all(begin.get(k) is True for k in ('observer_ready','calibration_passed','calibration_resources_restored')),'Calibración admitida')
    cal=begin['calibration'];require([c['name'] for c in cal]==['malloc33','free_malloc33','realloc_null17','realloc_owned_zero','realloc_owned65','reject_malloc17_expected18','outside_scope','other_thread'],'Calibraciones exactas')
    for i,c in enumerate(cal[:5]):
        args=c['input'];plan=([(M,[33])] if i==0 else [(F,[args[0]])] if i==1 else [(R,[0,17]),(M,[17])] if i==2 else [(R,[args[0],0]),(F,[args[0]])] if i==3 else [(R,[args[0],65])])
        require(c['passed'] is True and c['aborted'] is False and c['scope_serial']==i+1,'Calibración positiva')
        if i in (1,3,4):require(args[0]>0,'Bloque real')
        trace(begin,c['trace'],plan,i+1,c['gate_return'])
    neg=cal[5];require(neg['passed'] is True and neg['aborted'] is True and neg['allocator_instruction_executed'] is False and neg['expected_size']==18 and neg['actual_size']==17,'Aborto negativo obligatorio')
    trace(begin,neg['trace'],[(M,[17])],6,neg['gate_return'])
    require(cal[6]['passed'] is True and cal[6]['owned_hits_delta']>=2 and cal[7]['passed'] is True and cal[7]['owner_trace_count']==0,'Aislamiento')
    seen=collections.defaultdict(set);pairs=set();with_slots=0
    for row in rows:
        require(row['event'] not in ('error','first_chance_av'),'Evento de fallo')
        if row['event']!='comparison':continue
        kind=NAMES.index(row['function']);case=row['case'];pair=row['pair']
        require(pair not in pairs and case not in seen[kind],'Pareja duplicada');pairs.add(pair);seen[kind].add(case)
        require(row.get('equal') is True and row.get('accepted') is True and isinstance(row['original'],dict) and row['original']==row['candidate'],'Pareja nula/diferente')
        size=SIZES[case%32];align=ALIGNS[case//32%7];old=0 if case//32%8==0 else SIZES[1+case//8%31]
        for side in ('original','candidate'):
            result=row[side];evidence=row[side+'_allocator_evidence'];pointer=evidence['input_pointer'];base=evidence['allocation_base']
            require(result['payload_size']==size and result['old_size']==(old if kind==1 else 0) and result['hint']==(case*2654435761)&0xFFFFFFFF,'Fixture real')
            require(all(result.get(k) is True for k in ('argument_after_matches','stack_restored','nonvolatile_preserved','allocator_arguments_exact','allocator_callers_valid','observer_scope_ok','debug_readback_ok','resources_balanced','floating_state_restored')),'ABI/observador')
            if kind==0:plan=[(M,[size])];expected_slot=size
            elif kind==1:
                require(bool(pointer)==bool(old),'Identidad realloc');plan=[(R,[pointer,size])]
                if not pointer:plan.append((M,[size]))
                elif not size:plan.append((F,[pointer]))
                expected_slot=pointer
            elif kind==2:plan=[(M,[size+align])];expected_slot=size
            else:
                require(base>0 and pointer%align==0 and base+4<=pointer<=base+align,'Base/alineamiento');plan=[(F,[base])];expected_slot=base
            trace(begin,evidence['trace'],plan,1000+pair*2+(side=='candidate'),evidence['gate_return'],kind,side)
            if 'argument_after' in evidence:require(evidence['argument_after']==expected_slot,'Slot físico crudo');with_slots+=1
            elif require_slots:raise ValueError('Falta argument_after crudo; no probar slot mediante booleano únicamente')
    require(pairs==set(range(8192)) and all(seen[k]==set(range(2048)) for k in range(4)),'Cobertura 8192')
    finals=[r for r in rows if r['event']=='complete'];require(len(finals)==1,'Final único');final=finals[0]
    require(final['pairs']==8192 and final['debug_final']==begin['debug_saved'] and final['foreign_hits']==0 and all(final.get(k) is True for k in ('state_restored','debug_regs_restored','veh_removed','worker_closed','observer_context_preserved','floating_state_restored')),'Restauración exacta')
    return {'functions':4,'pairs':8192,'differences':0,'null_results':0,'raw_slot_sides':with_slots,'state_restored':True,'additional_audit_only':True}
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('path');parser.add_argument('--require-slots',action='store_true');args=parser.parse_args()
    print(json.dumps(audit(args.path,args.require_slots),indent=2))
