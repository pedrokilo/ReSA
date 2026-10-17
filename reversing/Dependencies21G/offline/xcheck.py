import sys
sys.path.insert(0,'..')
import fixture_oracle as o
bad=0;n=0
for line in open('rehearsal.out'):
    p=line.split()
    if p[0]=='F':
        c,base,eaxrel,hexs=int(p[1]),int(p[2]),int(p[3]),p[4]
        f=o.first_texture_fixture(c)
        exp=o.build_first_texture(base,f)
        if exp.hex()!=hexs or o.expected_first_texture(f)!=eaxrel: bad+=1; print('F dif',c, exp.hex()==hexs, o.expected_first_texture(f), eaxrel)
    else:
        c,base,ret,hexs=int(p[1]),int(p[2]),int(p[3]),p[4]
        tr=[int(x) for x in p[5].split(',') if x] if len(p)>5 else []
        f=o.replace_fixture(c)
        arena,rep,trace=o.oracle_replace(base,f)
        if arena.hex()!=hexs or int(rep)!=ret or trace!=tr: bad+=1; print('R dif',c, arena.hex()==hexs, rep, ret, trace==tr); 
        if bad>5: break
    n+=1
print('filas',n,'difs',bad)
