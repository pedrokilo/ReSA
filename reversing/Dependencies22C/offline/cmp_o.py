import sys
import fixture_oracle as o
bad=0; nanmask=0
for ln in open(sys.argv[1]):
    p=ln.split()
    c=int(p[1]); base=int(p[2]); arena=bytes.fromhex(p[3]); ti=p.index('T'); trace=[int(x,16) for x in p[ti+1:]]
    f=o.create_fixture(c)
    buf,exp_trace,model=o.oracle_create(base,f)
    ign=set()
    if model['final_bits'] is None or (not model['continued'] and model['sum_bits'] is None):
        word=int.from_bytes(arena[0x10:0x14],'little'); assert (word&0x7F800000)==0x7F800000 and (word&0x7FFFFF)!=0,(c,hex(word)); ign=set(range(0x10,0x14)); nanmask+=1
    ok=all(arena[i]==buf[i] for i in range(len(buf)) if i not in ign)
    et=list(exp_trace); tt=list(trace)
    tok=len(et)==len(tt)
    if tok:
        for i,(x,y) in enumerate(zip(et,tt)):
            if x is None:
                if not ((y&0x7F800000)==0x7F800000 and (y&0x7FFFFF)): tok=False
            elif x!=y: tok=False
    if not ok or not tok:
        bad+=1
        if bad<6:
            print('CASO',c,'estado' if not ok else '', 'traza' if not tok else '', model.get('reason'),model.get('path'))
            if not tok: print('  exp',[hex(v) if v is not None else None for v in et][:40]); print('  got',[hex(v) for v in tt][:40])
            if not ok:
                diff=[i for i in range(len(buf)) if arena[i]!=buf[i] and i not in ign][:6]; print('  dif',diff,[ (hex(arena[i]),hex(buf[i])) for i in diff])
print('casos con diferencias frente al oraculo:',bad,'(intensidad NaN en',nanmask,')')
