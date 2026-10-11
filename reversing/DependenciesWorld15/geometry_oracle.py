"""Independent rational x87 oracle; consumes observed binary inputs only."""
import struct
from fractions import Fraction

def fraction_bits(bits):
 value=struct.unpack('<f',struct.pack('<I',bits))[0]
 if not(-float('inf')<value<float('inf')):raise ValueError('Nonfinite input')
 return Fraction(value)
def at(blob,offset):return fraction_bits(struct.unpack_from('<I',bytes(blob),offset)[0])
def round_binary(value,precision,mode):
 if not value:return Fraction(0)
 sign=1 if value>0 else -1;v=abs(value);n,d=v.numerator,v.denominator
 exponent=n.bit_length()-d.bit_length()
 power=Fraction(2**exponent)if exponent>=0 else Fraction(1,2**(-exponent))
 if v<power:exponent-=1
 shift=precision-1-exponent
 if shift>=0:n*=2**shift
 else:d*=2**(-shift)
 q,r=divmod(n,d)
 if mode==0:
  if r*2>d or r*2==d and q%2:q+=1
 elif mode==1:
  if sign<0 and r:q+=1
 elif mode==2:
  if sign>0 and r:q+=1
 elif mode!=3:raise ValueError('Unknown rounding mode')
 result=Fraction(q,2**shift)if shift>=0 else Fraction(q*2**(-shift))
 return sign*result
class X87:
 def __init__(self,cw):
  self.precision={0:24,2:53,3:64}.get((cw>>8)&3)
  if self.precision is None:raise ValueError('Reserved x87 precision')
  self.mode=(cw>>10)&3
 def add(self,a,b):return round_binary(a+b,self.precision,self.mode)
 def sub(self,a,b):return round_binary(a-b,self.precision,self.mode)
 def mul(self,a,b):return round_binary(a*b,self.precision,self.mode)
 def store(self,a):return round_binary(a,24,self.mode)
def world_point(fp,m,p):
 # Native 59C890 order, including rounding only at its output stores.
 x=fp.add(fp.add(fp.add(fp.mul(m[8],p[2]),fp.mul(m[4],p[1])),fp.mul(m[0],p[0])),m[12])
 y=fp.add(fp.add(fp.add(fp.mul(m[9],p[2]),fp.mul(m[1],p[0])),fp.mul(m[5],p[1])),m[13])
 z=fp.add(fp.add(fp.add(fp.mul(m[10],p[2]),fp.mul(m[2],p[0])),fp.mul(m[6],p[1])),m[14])
 return [fp.store(x),fp.store(y),fp.store(z)]
def sphere_hit(fp,g,p,r):
 if fp.sub(g[2],r)>p[2]or fp.add(r,g[7])<p[2]:return False
 dy=fp.store(fp.sub(p[1],g[1]));dx_extended=fp.sub(p[0],g[0]);dx=fp.store(dx_extended)
 a=fp.add(fp.mul(dx_extended,g[3]),fp.mul(dy,g[4]))
 if a<-r or a>fp.add(r,g[8]):return False
 bb=fp.add(fp.mul(dx,g[5]),fp.mul(dy,g[6]))
 return not(bb<-r or bb>fp.add(r,g[9]))
def verify(v):
 garage=v.get('garage_bytes',[]);spheres=v.get('sphere_bytes',[]);matrix=v.get('matrix_bytes',[]);pos=v.get('entity_position_bits',[])
 if len(garage)!=0xD8 or len(pos)!=3 or len(spheres)!=20*v['sphere_count']or v['descriptor_count']not in (0,v['sphere_count']):raise ValueError('Missing complete geometry inputs')
 descriptor=v.get('descriptor_bytes',[]);before=v.get('descriptor_before_bytes',[])
 if len(descriptor)!=0x30 or len(before)!=0x30 or descriptor[2:]!=before[2:]or struct.unpack_from('<h',bytes(descriptor),0)[0]!=v['descriptor_int16']or v['descriptor_int16']!=v['descriptor_count']:raise ValueError('Descriptor/guard/int16 contract')
 fp=X87(v['x87_control']);g=[at(garage,i*4)for i in range(14)];p=[fraction_bits(x)for x in pos];radius=fraction_bits(v['standard_radius_bits'])
 broad=6
 for i,(limit,coord,lower)in enumerate(((g[10],p[0],True),(g[11],p[0],False),(g[12],p[1],True),(g[13],p[1],False),(g[2],p[2],True),(g[7],p[2],False))):
  outside=fp.sub(limit,radius)>coord if lower else fp.add(radius,limit)<coord
  if outside:broad=i;break
 first=-1
 if broad==6 and v['descriptor_count']:
  if len(matrix)!=0x48:raise ValueError('Missing real materialized matrix')
  m=[at(matrix,i*4)if i%4!=3 else Fraction(0)for i in range(16)]
  for i in range(v['descriptor_count']):
   center=[at(spheres,i*20+j*4)for j in range(3)];r=at(spheres,i*20+12)
   if sphere_hit(fp,g,world_point(fp,m,center),r):first=i;break
 expected=first>=0
 if v['return_bool']is not expected or v['first_sphere_hit']!=first:raise ValueError('Independent geometry disagreement')
 kind=v['fixture_kind']
 if kind<6 and broad!=kind or kind>=6 and broad!=6 or kind==9 and v['descriptor_count']!=0 or kind!=9 and v['descriptor_count']!=v['sphere_count']:raise ValueError('Claimed branch not reached')
 return broad,first
