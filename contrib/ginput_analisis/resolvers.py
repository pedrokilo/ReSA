import sys,re; sys.path.insert(0,'.')
from g import *
from names2 import label
ins=allins()
idx={i.address:k for k,i in enumerate(ins)}
prev=True; fs=[]
for i in ins:
    if prev and i.mnemonic!='int3': fs.append(i.address)
    prev=(i.mnemonic=='int3')
fs.sort()
bounds={a:(fs[k+1] if k+1<len(fs) else a+1) for k,a in enumerate(fs)}
def movimm(a):
    i=ins[idx[a]]
    if i.mnemonic=='mov' and i.op_str.startswith('eax, 0x'): return int(i.op_str.split(', ')[1],16)
    if i.mnemonic=='xor' and i.op_str=='eax, eax': return 0
    return None
def resolver(a):
    code=[i for i in ins if a<=i.address<bounds[a]]
    # find 'movsx eax, al' (or movsx eax, cl)
    k=[n for n,i in enumerate(code) if i.mnemonic=='movsx' and i.op_str.startswith('eax, ')]
    if not k: return None
    k=k[-1]
    res={}
    n=k+1; t=1
    while n<len(code):
        i=code[n]
        if i.mnemonic in('dec','sub'): pass
        if i.mnemonic=='je':
            res[t]=movimm(int(i.op_str,16)); t+=1
        elif i.mnemonic=='mov' and i.op_str.startswith('eax, 0x'):
            res[0]=int(i.op_str.split(', ')[1],16); break
        elif i.mnemonic=='ret': break
        n+=1
    return res
if __name__=='__main__':
    for a in fs:
        code=[i for i in ins if a<=i.address<bounds[a]]
        if any(i.mnemonic=='cmp' and '0x3539f633' in i.op_str for i in code):
            r=resolver(a)
            print(hex(a),{t:(hex(v) if v else v) for t,v in (r or {}).items()}, label(r[0]) if r and r.get(0) else '')
