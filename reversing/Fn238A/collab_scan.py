import re,subprocess,json,sys
D="/c/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86"
dis=open('desensamblado/dis_proc.txt').read().splitlines()
ins=[]
for l in dis:
    m=re.match(r'([0-9A-F]{8}):\s+(.*)',l)
    if m: ins.append((int(m.group(1),16),m.group(2).strip()))
sites={}
for i,(a,t) in enumerate(ins):
    m=re.match(r'call\s+([0-9A-F]{8})$',t)
    if m:
        T=int(m.group(1),16)
        nxt=ins[i+1][1] if i+1<len(ins) else ''
        m2=re.match(r'add\s+esp,([0-9A-F]+)h?$',nxt)
        pre=[x[1] for x in ins[max(0,i-8):i]]
        usesecx=any(re.search(r'\becx\b',p) and re.match(r'(mov|lea)\s+ecx',p) for p in pre[-5:])
        sites.setdefault(T,[]).append((a,int(m2.group(1),16) if m2 else None,usesecx))
out={}
for T,s in sorted(sites.items()):
    r=subprocess.run(['sh','../_handoff/dis.sh',hex(T),hex(T+0x600)],capture_output=True,text=True).stdout.splitlines()
    rets=set()
    first=None
    for l in r:
        m=re.match(r'([0-9A-F]{8}):\s+(ret.*|jmp\s+[0-9A-F]{8})$',l.strip())
        if m:
            if m.group(2).startswith('ret'):
                rets.add(m.group(2)); 
            elif first is None and not rets: first=m.group(2)
            if rets: break
    out[hex(T)]={'sites':len(s),'cdecl_adds':sorted({x[1] for x in s if x[1] is not None}),'noadd':sum(1 for x in s if x[1] is None),'ecxset':sum(1 for x in s if x[2]),'rets':sorted(rets),'jmp':first}
    print(hex(T),out[hex(T)],flush=True)
json.dump(out,open('collab_scan.json','w'),indent=1)
