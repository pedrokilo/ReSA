"""Inventario del EXE canónico de ReSA, enlazado con el catálogo sin cambiarlo."""
import argparse,base64,csv,gzip,hashlib,json,re,sys
from collections import defaultdict,Counter
from pathlib import Path
sys.dont_write_bytecode=True
from pe_inventory import PE,CANONICAL,hx,rtti_map,write_csv

def rows(path):
    with path.open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root',type=Path,default=Path(__file__).resolve().parents[1])
    p.add_argument('--exe',type=Path,default=Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    p.add_argument('--export',type=Path);p.add_argument('--output',type=Path);p.add_argument('--template',type=Path)
    a=p.parse_args(); root=a.root.resolve(); export=a.export or root/'build/binary-map';out=a.output or root/'docs/MemoryMap'
    pe=PE(a.exe); out.mkdir(parents=True,exist_ok=True)
    log=(export/'ghidra-analysis.log').read_text(encoding='utf-8',errors='replace')
    if not re.search(r'ANALYSIS SUCCEEDED|Analysis succeeded',log,re.I): raise ValueError('Falta terminación aprobada del análisis global de Ghidra')
    if re.search(r'analysis timed out|analysis timeout occurred',log,re.I): raise ValueError('Análisis global incompleto por timeout')
    hooks=rows(root/'docs/MemoryMap/functions.csv')
    references=defaultdict(list)
    for h in hooks:
        fixed=bool(h['va']); addresses=[h['va']] if fixed else h['source_address_candidates'].split(';')
        for address in addresses:
            if address and pe.code(int(address,16)):
                references[int(address,16)].append(dict(name=h['name'],owner=h['class'],
                    match='catalogue_exact_address' if fixed else 'source_registration_candidate',evidence_status=h['evidence_status']))
    symbols={int(r['va'],16):r['name'] for r in rows(export/'ghidra_symbols.csv') if r['source']!='DEFAULT'}
    classes,rtti_tables,rtti_slots=rtti_map(pe,symbols)
    tables=[];slots=[]
    for t in rows(root/'docs/MemoryMap/vtables.csv'):
        try: count=int(t['slots_declared'],0)
        except ValueError: continue
        address=int(t['va'],16);valid=0
        for index in range(min(count,512)):
            values=pe.words(address+4*index,1)
            if values and pe.code(values[0]):
                valid+=1
                slots.append(dict(owner=t['owner'],vtable_va=hx(address),slot=index,function_va=hx(values[0]),function_rva=hx(values[0]-pe.base),status='exe_slot_at_source_vtable'))
        tables.append(dict(owner=t['owner'],va=hx(address),rva=hx(address-pe.base),declared_slots=count,verified_code_pointers=valid,
            file=t['file'],line=t['line'],status='source_table_read_from_exe'))
    virtual_owners=defaultdict(set)
    for s in slots+rtti_slots: virtual_owners[int(s['function_va'],16)].add(s['owner'])
    functions=[]
    for r in rows(export/'ghidra_functions.csv'):
        address=int(r['va'],16);section=pe.section(address)
        if not section or pe.offset(address) is None:continue
        functions.append(dict(r,section=section['name'],executable_section=section['executable'],
            catalogue_names=';'.join(dict.fromkeys(x['name'] for x in references[address])),
            virtual_owners=';'.join(sorted(virtual_owners[address])),evidence_status=';'.join(sorted({x['evidence_status'] for x in references[address]})),
            status='ghidra_function_entry'))
    function_index={int(f['va'],16):f for f in functions}
    seeds={int(s,16) for s in (export/'seeds.txt').read_text().splitlines() if s}
    additional=[]
    for address in sorted((set(virtual_owners)|set(references)|seeds)-set(function_index)):
        if not pe.code(address):continue
        additional.append(dict(va=hx(address),rva=hx(address-pe.base),name='unresolved_entry_'+hx(address)[2:],body_bytes='',thunk='unknown',calling_convention='unknown',source='binary_pointer_or_source_reference',section=pe.section(address)['name'],executable_section=True,
            catalogue_names=';'.join(dict.fromkeys(x['name'] for x in references[address])),virtual_owners=';'.join(sorted(virtual_owners[address])),
            evidence_status=';'.join(sorted({x['evidence_status'] for x in references[address]})),status='entry_candidate_without_ghidra_function'))
    calls=rows(export/'calls.csv');ranges=rows(export/'function_ranges.csv')
    for candidate in additional:
        address=int(candidate['va'],16)
        containing=sorted({r['function_va'] for r in ranges if int(r['start_va'],16)<=address<=int(r['end_va'],16)})
        candidate['containing_function_va']=';'.join(containing)
    counts=Counter(c['caller_va'] for c in calls)
    incoming=Counter(c['target_va'] for c in calls if c['target_va'])
    for f in functions+additional:f.update(call_sites=counts[f['va']],direct_call_sites_to_entry=incoming[f['va']])
    # Account for every byte backed by the file in executable sections, including gaps.
    coverage={s['name']:bytearray(s['raw_size']) for s in pe.sections if s['executable']}
    for r in rows(export/'instructions.csv'):
        va=int(r['va'],16);s=pe.section(va)
        if s and s['name'] in coverage:
            offset=va-s['va'];length=min(int(r['length']),s['raw_size']-offset)
            if length>0:coverage[s['name']][offset:offset+length]=bytes([1])*length
    for r in ranges:
        low=int(r['start_va'],16);high=int(r['end_va'],16)
        for s in pe.sections:
            if s['name'] not in coverage:continue
            begin=max(low,s['va'])-s['va'];end=min(high+1,s['va']+s['raw_size'])-s['va']
            if end>begin:
                data=coverage[s['name']]
                for i in range(begin,end):data[i]|=2
    sections=[];gaps=[]
    for s in pe.sections:
        row=dict(s,va=hx(s['va']),rva=hx(s['rva']),ghidra_function_entries=sum(f['section']==s['name'] for f in functions))
        if s['name'] in coverage:
            data=coverage[s['name']]; row.update(instruction_bytes=sum(bool(v&1) for v in data),function_body_bytes=sum(bool(v&2) for v in data),bytes_without_function=sum(not(v&2) for v in data))
            start=None
            for i in range(len(data)+1):
                unassigned=i<len(data) and not(data[i]&2)
                if unassigned and start is None:start=i
                elif not unassigned and start is not None:
                    gaps.append(dict(section=s['name'],va=hx(s['va']+start),rva=hx(s['rva']+start),end_va=hx(s['va']+i-1),bytes=i-start,decoded_instruction_bytes=sum(bool(v&1) for v in data[start:i]),status='executable_bytes_without_function_body'));start=None
        sections.append(row)
    summary=dict(exe_sha256=CANONICAL,image_base=hx(pe.base),exe_bytes=len(pe.data),analysis_global_completed=True,
        ghidra_function_entries=len(functions),additional_entry_candidates=len(additional),combined_code_addresses=len(functions)+len(additional),
        named_by_catalogue_or_source=sum(bool(f['catalogue_names']) for f in functions),without_catalogue_names=sum(not f['catalogue_names'] for f in functions),
        source_vtables_read=len(tables),source_vtable_slots_verified=len(slots),rtti_types=len(classes),rtti_vtables=len(rtti_tables),
        call_sites=len(calls),indirect_call_sites=sum(c['kind']=='indirect_unresolved' for c in calls),
        executable_file_bytes=sum(len(x) for x in coverage.values()),instruction_bytes=sum(s.get('instruction_bytes',0) for s in sections),
        function_body_bytes=sum(s.get('function_body_bytes',0) for s in sections),bytes_without_function=sum(s.get('bytes_without_function',0) for s in sections),
        unassigned_ranges=len(gaps),exact_total_functions='unknown',exact_total_game_classes='unknown',
        ghidra_version='12.1.4',candidate_seeds=len(seeds),
        analysis_log_sha256=hashlib.sha256((export/'ghidra-analysis.log').read_bytes()).hexdigest(),
        ghidra_functions_sha256=hashlib.sha256((export/'ghidra_functions.csv').read_bytes()).hexdigest())
    seed_review=rows(export/'seed-resolution.csv') if (export/'seed-resolution.csv').is_file() else []
    summary['targeted_seed_functions_added']=sum(r['result']=='created_function' for r in seed_review)
    files={'binary_functions':functions,'binary_entry_candidates':additional,'binary_vtables':tables,'binary_slots':slots,
           'binary_rtti_types':[dict(c,bases=';'.join(c['bases']),vtables=';'.join(c['vtables']),locators=';'.join(c['locators'])) for c in classes],
           'binary_calls':calls,'binary_function_ranges':ranges,'binary_unassigned_ranges':gaps}
    for name,entries in files.items():
        if entries:write_csv(out/(name+'.csv'),entries,list(entries[0]))
    if seed_review:write_csv(out/'binary_seed_resolution.csv',seed_review,list(seed_review[0]))
    (out/'binary_summary.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    (out/'binary_sections.json').write_text(json.dumps(sections,indent=2)+'\n',encoding='utf-8')
    # Keep the source map and binary map as views of the same dataset.
    packed=re.search(r"atob\('([^']+)'\)",(out/'index.html').read_text(encoding='utf-8'))[1]
    dataset=json.loads(gzip.decompress(base64.b64decode(packed)))
    dataset.update(binary_summary=summary,binary_functions=functions,binary_candidates=additional,binary_vtables=tables,
        binary_slots=slots,binary_rtti=classes,binary_calls=calls,binary_sections=sections,binary_gaps=gaps)
    dataset['summary'].update(binary_inventory_available=True,binary_verified=True)
    (out/'summary.json').write_text(json.dumps(dataset['summary'],ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    template=a.template or root/'reversing/MemoryMap/viewer.template.html'
    packed=base64.b64encode(gzip.compress(json.dumps(dataset,ensure_ascii=False,separators=(',',':')).encode(),mtime=0)).decode()
    (out/'index.html').write_text(template.read_text(encoding='utf-8').replace('__DATA__',packed),encoding='utf-8')
    print(json.dumps(summary,indent=2))
if __name__=='__main__':main()
