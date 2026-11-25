import sys,re; sys.path.insert(0,'.')
from g import *
from summ import funcend
def cstr(va):
    b=rd(va,80)
    if len(b)>1 and b[1]==0 and b[0]!=0: return 'W:'+b.decode('utf-16le',errors='ignore').split('\x00')[0]
    return 'A:'+b.split(b'\0')[0].decode('latin1')
def strs(lo,hi=None):
    hi=hi or funcend(lo); out=[]
    for i in allins():
        if i.address<lo: continue
        if i.address>=hi: break
        for m in re.findall(r'0x1003[3-7][0-9a-f]{3}',i.op_str):
            v=int(m,16)
            if 0x10033000<=v<0x10037000:
                s=cstr(v)
                if len(s)>3 and s[2:].isprintable(): out.append((hex(v),s))
    return out
if __name__=='__main__':
    for a in sys.argv[1:]:
        print(a,strs(int(a,16)))
