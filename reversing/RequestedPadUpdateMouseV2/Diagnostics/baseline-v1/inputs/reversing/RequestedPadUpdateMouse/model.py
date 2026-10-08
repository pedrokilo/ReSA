"""Independent native UpdateMouse field/trace model; no candidate import."""
import struct
COUNT=9216
STATES=[(0,0,0,[0]*8),(1,-1,0,[0]*8),(-1,1,0,[0]*8),(0,0,1,[0]*8),(0,0,-1,[0]*8),(0,0,0,[128,0,0,0,0,1,127,255]),(0,0,0,[0,255,0,0,0,128,0,0]),(0,0,0,[0,0,129,0,0,0,0,0]),(0,0,0,[0,0,0,128,255,0,0,0]),(2147483647,-2147483647,0,[1,127,128,129,255,0,0,0]),(16777217,-16777217,2147483647,[255]*8),(-16777219,16777219,-2147483648,[127]*8)]
def fixture(i):
 assert 0<=i<COUNT
 return {'index':i,'profile':i%12,'route':(i//12)%6,'flags':(i//72)%8,'poison':((i//576)%4)*85,'receiver':(i//2304)%2,'clock':4294967295 if (i//4608)%2 else 0}
def expected(f,begin):
 assert f==fixture(f['index']);poison=f['poison'];route=f['route'];profile=f['profile'];flags=f['flags']
 mouse=bytearray((poison+j*17)%256 for j in range(60))
 for index in range(3):
  mouse[index*20+3]=(poison//85)%2;mouse[index*20+4]=(poison//85+1)%2
  for offset in (8,12,16):mouse[index*20+offset:index*20+offset+4]=struct.pack('<f',float(poison))
 pads=[bytearray([205]*16+[poison]*308+[205]*16) for _ in range(2)];device=begin['mouse_address'] if route in (2,3) or route==0 and profile%2 else 0;trace=[['foreground',route!=0]]
 if route!=0:
  if device==0:
   if not flags&1:trace.append(['exclusive',bool(profile%2)])
   trace.append(['init',not bool(flags&1) and bool(profile%2)]);device=0 if route==1 else begin['mouse_address']
  if device:
   state=STATES[profile];status=-2147467259 if route==2 else -2147024891 if route==5 else 1 if route==4 else 0;trace.append(['read',status,list(struct.pack('<lll8B',*state[:3],*state[3])),True])
   if status>=0:
    prior=mouse[20:40];temp=bytearray(mouse[:20]);x,y,z,buttons=state
    x=-x if not flags&1 and flags&2 else x;y=-y if not flags&1 and flags&4 else y
    temp[0]=buttons[0]&128;temp[1]=buttons[1]&128;temp[2]=buttons[2]&128;temp[3]=int(z>0);temp[4]=int(z<0);temp[5]=buttons[3]&128;temp[6]=buttons[4]&128
    temp[8:12]=struct.pack('<f',float(z));temp[12:16]=struct.pack('<f',float(x));temp[16:20]=struct.pack('<f',float(y));mouse[:20]=temp;mouse[20:40]=temp;mouse[40:60]=prior
    if any(temp[:7]) or x!=0 or y!=0:pads[f['receiver']][16+288:16+292]=struct.pack('<I',f['clock'])
 return {'mouse':list(mouse),'mouse_guard_before':begin['mouse_guard_before'],'mouse_guard_after':begin['mouse_guard_after'],'pads':[list(p) for p in pads],'opaque':[167]*48,'device':device,'trace':trace}
if __name__=='__main__':
 import collections,itertools
 begin={'mouse_address':123456,'mouse_guard_before':[1]*16,'mouse_guard_after':[2]*24};seen=set();traces=collections.Counter()
 for i in range(COUNT):
  f=fixture(i);key=tuple(f[k] for k in ('route','profile','flags','poison','receiver','clock'));assert key not in seen;seen.add(key);out=expected(f,begin);assert len(out['mouse'])==60 and all(len(p)==340 for p in out['pads']);traces.update(t[0] for t in out['trace'])
 assert len(seen)==COUNT and seen==set(itertools.product(range(6),range(12),range(8),(0,85,170,255),range(2),(0,4294967295)))
 print('independent finite corpus',COUNT,'trace totals',dict(traces))
