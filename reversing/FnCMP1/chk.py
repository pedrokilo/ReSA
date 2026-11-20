import sys,struct,importlib.util,os,re
spec=importlib.util.spec_from_file_location('fcmp','D:/Original/wv-int/reversing/FnCMP1/fcmp.py')
F=importlib.util.module_from_spec(spec); spec.loader.exec_module(F)
addr=int(sys.argv[1],16); k=float(sys.argv[2].rstrip('fF')); mb=int(sys.argv[3],0) if len(sys.argv)>3 else 0x3000
kv=struct.unpack('<f',struct.pack('<f',k))[0]
for a,pre,desc,outs,_pv in F.analyze(addr,mb):
    if not pre: continue
    m=re.search(r'\[(0x[0-9a-f]+)\]',pre.op_str)
    if m and 'dword' in pre.op_str:
        try: v=struct.unpack('<f',F.rd(int(m.group(1),16),4))[0]
        except: continue
        if abs(v-kv)<=1e-6*max(1,abs(kv)): print('%x %s %s | %s'%(a,pre.op_str,desc,' '.join(outs)))
