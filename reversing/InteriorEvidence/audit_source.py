"""Read-only Interior_c source/ABI audit; write a hash-bound report after freeze."""
import argparse, csv, hashlib, json, re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

def digest(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def clean(s):
    return re.sub(r'/\*.*?\*/|//[^\n]*',lambda m:re.sub(r'[^\n]',' ',m[0]),s,flags=re.S)
def body_at(s,start):
    depth=0
    for end in range(start,len(s)):
        if s[end]=='{':depth+=1
        elif s[end]=='}':
            depth-=1
            if depth==0:return s[start:end+1],end+1
    raise ValueError('unclosed body')
def number(s):
    s=s.strip()
    return int(s[:-1],16) if s.lower().endswith('h') else int(s,0)
def params(s):
    if not s.strip() or s.strip()=='void':return []
    return [x.strip().split('=')[0].strip() for x in s.split(',')]
def abi_param(s):
    if '*' in s or '&' in s:return 'pointer:4'
    if re.search(r'\bCVector\b',s):return 'CVector:12'
    return 'scalar:4'
def functions(s):
    result={}
    pattern=r'(?m)^\s*(?:__declspec\(naked\)\s+)?([\w:*<>]+(?:\s*\*)?)\s+Interior_c::(\w+)\s*\(([^)]*)\)\s*\{'
    for m in re.finditer(pattern,s):
        b,e=body_at(s,m.end()-1)
        result.setdefault(m[2],[]).append({'return':m[1].strip(),'parameters':params(m[3]),'body':b,'offset':m.start(),'end':e})
    return result

def audit(output):
    paths={k:ROOT/v for k,v in {'catalogue':'docs/hooks.csv','ranges':'docs/MemoryMap/binary_function_ranges.csv','source':'source/game_sa/Interior/Interior_c.cpp','header':'source/game_sa/Interior/Interior_c.h'}.items()}
    with paths['catalogue'].open(encoding='utf-8-sig',newline='') as f:scope=[r for r in csv.DictReader(f) if r['class']=='Interior_c']
    with paths['ranges'].open(encoding='utf-8-sig',newline='') as f:
        ranges={int(r['function_va'],16):(int(r['start_va'],16),int(r['end_va'],16)) for r in csv.DictReader(f)}
    raw=paths['source'].read_text(encoding='utf-8');src=clean(raw);header=clean(paths['header'].read_text(encoding='utf-8'));defs=functions(src)
    tables={m[1]:[int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]+)',m[2])] for m in re.finditer(r'(\w+Calls)\s*\[\s*\]\s*=\s*\{([^}]+)\}',src)}
    trampolines={m[1]:number(m[2]) for m in re.finditer(r'(InteriorHomeCall\w+)\(\)\s*\{\s*__asm\s*\{\s*push\s+([0-9A-Fa-f]+h)',src)}
    helpers={}
    for m in re.finditer(r'(?m)^\s*(?:static\s+)?(?:__declspec\(naked\)\s+)?[\w:*<>]+(?:\s*\*)?\s+(Interior\w+)\s*\([^)]*\)\s*\{',src):
        if not m[1].startswith('InteriorHomeCall'):helpers[m[1]]=body_at(src,m.end()-1)[0]
    issues=[];records=[]
    if len(scope)!=52:issues.append({'reason':'catalogue_scope_count','actual':len(scope),'expected':52})
    for row in scope:
        name=row['fn_name'];address=int(row['address'],16);record={'function':'Interior_c::'+name,'address':row['address'],'catalogue':row};local=[]
        matches=defs.get(name,[])
        if len(matches)!=1:
            issues.append({'function':name,'reason':'definition_count','actual':len(matches)});records.append(record);continue
        fn=matches[0];body=fn['body'];closure={};pending=[body]
        while pending:
            current=pending.pop()
            for symbol in re.findall(r'\bInterior\w+\b',current):
                if symbol in helpers and symbol not in closure:
                    closure[symbol]=helpers[symbol];pending.append(helpers[symbol])
        record['helper_bodies']={k:hashlib.sha256(v.encode()).hexdigest() for k,v in sorted(closure.items())}
        audit_body=body+'\n'+'\n'.join(closure.values())
        record['body_sha256']=hashlib.sha256(body.encode()).hexdigest();record['source_line']=raw[:fn['offset']].count('\n')+1;record['return']=fn['return'];record['parameter_abi']=[abi_param(x) for x in fn['parameters']]
        declarations=list(re.finditer(r'(?m)^\s*([\w:*<>]+(?:\s*\*)?)\s+'+re.escape(name)+r'\s*\(([^)]*)\)\s*(?:const\s*)?;',header))
        if len(declarations)!=1:local.append({'reason':'header_declaration_count','actual':len(declarations)})
        else:
            decl=declarations[0];record['header_return']=decl[1].strip();record['header_parameter_abi']=[abi_param(x) for x in params(decl[2])]
            if re.sub(r'\s','',fn['return'])!=re.sub(r'\s','',decl[1]):local.append({'reason':'header_return_mismatch'})
            if record['parameter_abi']!=record['header_parameter_abi']:local.append({'reason':'header_argument_abi_mismatch'})
        stack=sum(int(a.split(':')[1]) for a in record['parameter_abi']);record['parameter_stack_bytes']=stack
        start,end=ranges.get(address,(address,address));record['catalogue_binary_range']=[hex(start),hex(end)]
        if name=='Shop_FurnishCeiling' and start==end==address:
            end=address+1
            record['range_resolution']='Zero-sized Ghidra range: exact evidence at 0059A130 is one-byte RET; neighboring FurnishOffice pseudocode alias is not the implementation.'
        record['binary_range']=[hex(start),hex(end)]
        evidence=ROOT/'reversing/InteriorEvidence'/('Interior_'+name+'.txt')
        if not evidence.exists():local.append({'reason':'missing_evidence'})
        else:
            asm=[]
            for line in evidence.read_text(encoding='utf-8').splitlines():
                m=re.match(r'^([0-9A-Fa-f]{8}):\s*(.*)',line)
                if m and start<=int(m[1],16)<end:asm.append((int(m[1],16),m[2]))
            record['evidence']=str(evidence.relative_to(ROOT)).replace('\\','/');record['evidence_sha256']=digest(evidence);record['original_instruction_count']=len(asm)
            rets=[number(m[1]) if m[1] else 0 for _,ins in asm if (m:=re.fullmatch(r'ret(?:\s+([0-9A-Fa-f]+h|0x[0-9A-Fa-f]+|\d+))?',ins))]
            record['native_ret_stack_bytes']=sorted(set(rets));record['native_return_writes']=[ins for _,ins in asm if re.match(r'(?:mov|xor)\s+(?:eax|al),',ins)]
            if not asm:local.append({'reason':'no_exact_asm_in_range'})
            if rets and stack not in rets:local.append({'reason':'native_ret_stack_mismatch','source':stack,'native':sorted(set(rets))})
        externals=set();transfers=[]
        for m in re.finditer(r'reinterpret_cast<[^;{}]+?>\s*\(\s*(0x[0-9A-Fa-f]+)\s*\)',audit_body):
            target=int(m[1],16);externals.add(target)
            if start<=target<end:local.append({'reason':'original_body_function_pointer','target':m[1]})
        for m in re.finditer(r'plugin::Call\w*\s*<([^>]+)>',audit_body):
            for v in re.findall(r'0x[0-9A-Fa-f]+',m[1]):
                target=int(v,16);externals.add(target)
                if start<=target<end:local.append({'reason':'self_original_plugin_forward','target':v})
        for m in re.finditer(r'(?m)^\s*((?:j\w+|call))\s+([^\n]+)',audit_body):
            op,operand=m[1],m[2].strip();target=None
            direct=re.fullmatch(r'(?:0x[0-9A-Fa-f]+|[0-9A-Fa-f]+h)',operand)
            if direct:target=number(operand)
            elif operand in trampolines:target=trampolines[operand]
            else:
                t=re.search(r'\[(\w+Calls)(?:\+(\d+))?\]',operand)
                if t:
                    table=tables.get(t[1]);index=int(t[2] or 0)//4
                    if table is None or index>=len(table):local.append({'reason':'unresolved_call_table','operand':operand})
                    else:target=table[index]
            if target is not None:
                externals.add(target);transfers.append({'instruction':op,'operand':operand,'target':hex(target)})
                if start<=target<end:local.append({'reason':'original_body_transfer','instruction':op,'target':hex(target)})
            elif re.match(r'Interior\w*_[0-9A-Fa-f]{8}$',operand):
                if not re.search(r'(?m)^\s*'+re.escape(operand)+r'\s*:',audit_body):local.append({'reason':'unresolved_local_branch','label':operand})
            elif op!='call' and re.search(r'\[|\be[abcd]x\b|\besi\b|\bedi\b',operand):local.append({'reason':'indirect_branch_requires_manual_audit','operand':operand})
        record['original_address_transfers']=transfers;record['bounded_external_addresses']=[hex(v) for v in sorted(externals)];record['implementation_kind']='locally_relocated_asm' if '__asm' in audit_body and re.search(r'\bret\b',audit_body) else 'cpp_with_helpers';record['issues']=local
        issues.extend({'function':name,**x} for x in local);records.append(record)
    report={'schema':'InteriorSourceAudit/v1','passed':not issues,'scope_class':'Interior_c','scope_functions':len(scope),'input_hashes':{k:{'path':str(v.relative_to(ROOT)).replace('\\','/'),'sha256':digest(v)} for k,v in paths.items()},'functions':records,'issues':issues,'limits':['Static structural/ABI audit; not a runtime validation certificate.','External collaborators and other Interior_c functions are explicitly allowed outside each target original-body range.','C++ semantics, dynamic callback ABI and arbitrary exception behavior remain bounded by differential fixtures/manual ASM review.','No claim of full EXE independence; helpers may call canonical external functions.','Return-write inventory is evidence; register values on every path require runtime/manual analysis.']}
    output.parent.mkdir(parents=True,exist_ok=True)
    with output.open('w',encoding='utf-8',newline='\n') as f:json.dump(report,f,indent=2,ensure_ascii=False);f.write('\n')
    print(json.dumps({'passed':report['passed'],'scope_functions':len(scope),'issues':issues,'output':str(output)}))
    return 0 if report['passed'] else 1
if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,default=ROOT/'reversing/InteriorEvidence/artifacts/source-audit.json');args=parser.parse_args();raise SystemExit(audit(args.output))
