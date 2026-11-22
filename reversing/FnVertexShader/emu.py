import sys, struct, random
from unicorn import *
from unicorn.x86_const import *
from exedump import D, pe, base, secs, rd
ENGPTR=0xC97B24
def setup():
    mu=Uc(UC_ARCH_X86,UC_MODE_32)
    top=0xCB1000
    mu.mem_map(base, ((top-base)+0xfff)&~0xfff)
    for (va,vs,rp,rs) in secs:
        mu.mem_write(base+va, D[rp:rp+rs])
    mu.mem_map(0x20000000,0x10000)
    mu.mem_map(0x30000000,0x1000)
    mu.mem_map(0x31000000,0x1000)
    mu.mem_map(0x40000000,0x10000)
    mu.mem_write(ENGPTR, struct.pack("<I",0x20000000))
    mu.mem_write(0x20000000+0xf0, struct.pack("<I",0x30000000))
    mu.mem_write(0x30000000, b"\xc3")
    mu.mem_write(0x31000000, b"\xc3")
    return mu
def cstr(mu,a):
    if a==0: return b"(null)"
    s=b""
    while True:
        c=bytes(mu.mem_read(a,1))
        if c==b"\0": return s
        s+=c; a+=1
def run(mu, d, ver, maxc, table):
    mu.mem_write(0xC94C00, struct.pack("<I",ver))
    mu.mem_write(0xC94C04, struct.pack("<I",maxc))
    mu.mem_write(0xC94AF8, struct.pack("<64I",*table))
    mu.mem_write(0x20000100, bytes(d)); mu.mem_write(0x20000200, b"\0"*8)
    esp=0x40008000
    mu.mem_write(esp, struct.pack("<III",0x31000000,0x20000100,0x20000200))
    mu.reg_write(UC_X86_REG_ESP,esp)
    st={'asm':None,'len':None}
    def hook(uc,addr,size,ud):
        if addr==0x30000000:
            esp=uc.reg_read(UC_X86_REG_ESP)
            args=struct.unpack("<II",bytes(uc.mem_read(esp+4,8)))
            dest,fmt=args
            f=cstr(uc,fmt).decode('latin1')
            ap=esp+12
            out="";i=0
            while i<len(f):
                ch=f[i]
                if ch!='%': out+=ch;i+=1;continue
                i+=1
                while f[i] in 'l': i+=1
                t=f[i];i+=1
                v=struct.unpack("<I",bytes(uc.mem_read(ap,4)))[0];ap+=4
                if t=='d': out+=str(v-(1<<32) if v>=1<<31 else v)
                elif t=='u': out+=str(v)
                elif t=='c':
                    b=v&0xff; out+=chr(b)
                elif t=='s': out+=cstr(uc,v).decode('latin1')
                elif t=='%': out+='%'; ap-=4
                else: raise Exception("fmt "+t)
            data=out.encode('latin1')+b"\0"
            uc.mem_write(dest,data)
            uc.reg_write(UC_X86_REG_EAX,len(data)-1)
            ret=struct.unpack("<I",bytes(uc.mem_read(esp,4)))[0]
            uc.reg_write(UC_X86_REG_ESP,esp+4); uc.reg_write(UC_X86_REG_EIP,ret)
        elif addr==0x7652ae:
            esp=uc.reg_read(UC_X86_REG_ESP)
            buf,ln=struct.unpack("<II",bytes(uc.mem_read(esp+4,8)))
            st['asm']=bytes(uc.mem_read(buf,ln)); 
            ret=struct.unpack("<I",bytes(uc.mem_read(esp,4)))[0]
            uc.reg_write(UC_X86_REG_EAX,0x80004005)
            uc.reg_write(UC_X86_REG_ESP,esp+4+0x1c); uc.reg_write(UC_X86_REG_EIP,ret)
        elif addr==0x31000000:
            uc.emu_stop()
    h1=mu.hook_add(UC_HOOK_CODE,hook,begin=0x30000000,end=0x30000000)
    h2=mu.hook_add(UC_HOOK_CODE,hook,begin=0x7652ae,end=0x7652ae)
    h3=mu.hook_add(UC_HOOK_CODE,hook,begin=0x31000000,end=0x31000000)
    try:
        mu.emu_start(0x75f0b0,0x31000000,count=200000)
    finally:
        mu.hook_del(h1);mu.hook_del(h2);mu.hook_del(h3)
    eax=mu.reg_read(UC_X86_REG_EAX)
    outb=bytes(mu.mem_read(0x20000200,8))
    return st['asm'], outb, eax
TABLE=[0]*64
for i in (1,2,6,7,9): TABLE[i]=2
for i in (3,10): TABLE[i]=4
TABLE[11]=6
def descs(n,seed=1):
    r=random.Random(seed+int(sys.argv[3]) if len(sys.argv)>3 else seed)
    L=[]
    for e in range(0,16):
        for w in range(0,5):
            for nm in (0,1):
                for lights in ((0,0,0),(1,0,0),(2,1,1),(5,6,0),(0,3,0),(0,0,2),(0,5,5),(4,4,4),(1,1,1)):
                    for pre in (0,1):
                        for mod in (0,1):
                            for fog in range(4):
                                for morph in (0,1):
                                  if r.random()<0.12:
                                    L.append((lights[0]|lights[1]<<4, lights[2]|(r.randrange(0,5)<<4), morph|(w<<1)|(pre<<4)|(nm<<5)|(r.randrange(2)<<6)|(mod<<7), fog|(e<<2)))
    for _ in range(n):
        L.append((r.randrange(256),r.randrange(256),r.randrange(256) & ~0, r.randrange(256)))
    return L
if __name__=="__main__":
    out=sys.argv[1]; n=int(sys.argv[2])
    mu=setup()
    L=descs(n)
    f=open(out,"w",newline=chr(10),encoding="latin1"); ref=open(out+".in","w")
    for d in L:
        for ver in (10,20,25):
            ref.write("%02x %02x %02x %02x %d\n"%(d+(ver,)))
            asm,ob,eax=run(mu,d,ver,256,TABLE)
            txt=(asm or b"").decode('latin1')
            f.write("D %02x%02x%02x%02x v%d n=%d out=%s\n"%(d+(ver,len(txt) if asm is not None else -1,ob.hex())))
            f.write(txt+"@@\n")
    f.close(); ref.close()
    print(len(L)*3)
