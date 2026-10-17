import json,sys,struct
def ext(h):
    b=bytes.fromhex(h)
    m=int.from_bytes(b[:8],'little'); e=int.from_bytes(b[8:10],'little')
    s=-1 if e>>15 else 1; e&=0x7fff
    if e==0x7fff: return 'nan' if (m<<1)&((1<<64)-1) else ('inf' if s>0 else '-inf')
    if e==0 and m==0: return '-0' if s<0 else '0'
    return repr(s*m*2.0**(e-16383-63))
def show(rec,case):
    for l in open(rec):
        r=json.loads(l)
        if r['event']=='comparison' and r['case']==case:
            for side in ('original','candidate'):
                raw=r[side]['x87']['raw_registers']
                print(side,'fsw=%04x'%r[side]['x87']['fsw'],[ 'T-%d=%s'%(8-k,ext(raw[k*20:(k+1)*20])) for k in range(8)][4:])
            print([e[0] for e in r['original']['trace']])
if __name__=='__main__': show(sys.argv[1],int(sys.argv[2]))
