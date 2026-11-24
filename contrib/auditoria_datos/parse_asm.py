import re,sys,json
def parse(asm_path):
    L=open(asm_path,encoding='cp850',errors='replace').read().split('\n')
    sizes={};cur=None
    for i,l in enumerate(L):
        m=re.match(r'^(\?\?\$(?:StaticRef)@\S+) PROC',l)
        if m: cur=m.group(1);continue
        if cur:
            m=re.match(r'\s+mov\s+DWORD PTR _audit_sz\$\[ebp\], (\d+)',l)
            if m: sizes[cur]=int(m.group(1));cur=None
    out=[];srcline=None
    for i,l in enumerate(L):
        m=re.match(r'^; \d+\s+:(.*)',l)
        if m and m.group(1).strip(): srcline=m.group(1).strip()
        m=re.match(r'\s+call\s+(\?\?\$StaticRef@\S+)\s*;\s*(.*)',l)
        if m:
            sym,tn=m.groups()
            addr=None
            for j in range(i-1,max(0,i-6),-1):
                mm=re.match(r'\s+push\s+(\d+)\s*;',L[j])
                if mm: addr=int(mm.group(1));break
                if re.match(r'\s+(call|push\s+ebp|ret)',L[j]):break
            out.append((addr,sizes.get(sym),tn,srcline))
    return out
if __name__=='__main__':
    o=parse(sys.argv[1]);print(len(o),sum(1 for x in o if x[0]),sum(1 for x in o if x[0] and x[1]))
    for x in o[:5]:print(x)
