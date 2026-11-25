import sys,re; sys.path.insert(0,'.')
from g import *
from names import name
from capstone.x86 import *
F=['LSX','LSY','RSX','RSY','L1','L2','R1','R2','Up','Down','Left','Right','Start','Select','Square','Triangle','Cross','Circle','L3','R3','Chat','PedWalk','VehMouseLook','RadioSkip']
PAD={0x10a:'Mode',0x10c:'ShakeDur',0x10e:'DisableCtl',0x110:'ShakeFreq',0x117:'JustOutOfFE',0x118:'bApplyBrakes',0x119:'bDisEnterCar',0x11a:'bDisDuck',0x11b:'bDisFire',0x11c:'bDisFireL1',0x11d:'bDisCycleWpn',0x11e:'bDisJump',0x11f:'bDisVitals',0x120:'LastTimeTouched',0x124:'AvgWeapon',0x128:'AvgEntries',0x12c:'NoShakeBefore',0x130:'NoShakeFreq'}
def padname(d):
    if d in PAD: return PAD[d]
    if 0<=d<0x30 and d%2==0: return 'New.'+F[d//2]
    if 0x30<=d<0x60 and d%2==0: return 'Old.'+F[(d-0x30)//2]
    if 0x78<=d<0xa8 and d%2==0: return 'TempKey.'+F[(d-0x78)//2]
    if 0xa8<=d<0xd8 and d%2==0: return 'TempJoy.'+F[(d-0xa8)//2]
    if 0xd8<=d<0x108 and d%2==0: return 'TempMouse.'+F[(d-0xd8)//2]
    return None
def funcend(lo):
    seen_ret=False
    for i in allins():
        if i.address<lo: continue
        if i.mnemonic=='int3' and seen_ret: return i.address
        if i.mnemonic in('ret',): seen_ret=True
        if i.mnemonic=='jmp' and i.op_str.startswith('eax'): seen_ret=True
    return lo+0x100
def summ(lo,hi=None):
    hi=hi or funcend(lo)
    pads=[];calls=[];gl=[];gm=[];n=0
    for i in allins():
        if i.address<lo: continue
        if i.address>=hi: break
        n+=1
        for op in i.operands:
            if op.type==X86_OP_MEM:
                m=op.mem
                if m.base==0 and m.index==0:
                    d=m.disp&0xffffffff
                    if d<0x10000000: gm.append('%x'%d)
                    else: gl.append('%x'%d)
                elif m.base not in(0,X86_REG_ESP,X86_REG_EBP) and m.index==0 and 0<=m.disp<0x134:
                    nm=padname(m.disp)
                    if nm: pads.append(nm)
                elif m.index!=0 and m.base==0:
                    gl.append('%x[]'%(m.disp&0xffffffff))
        if i.mnemonic=='call':
            calls.append(i.op_str)
        if i.mnemonic in('push','mov') and i.operands and i.operands[-1].type==X86_OP_IMM:
            v=i.operands[-1].imm&0xffffffff
            if 0x400000<=v<0x10000000: gm.append('i%x'%v)
    def dd(l):
        o=[];[o.append(x) for x in l if x not in o];return o
    return "len=%d %s | pad:%s | calls:%s | game:%s | asi:%s"%(hi-lo,hex(n),','.join(dd(pads)),','.join(dd(calls)),','.join(dd(gm)),','.join(dd(gl)))
if __name__=='__main__':
    for a in sys.argv[1:]:
        print(a,summ(int(a,16)))
