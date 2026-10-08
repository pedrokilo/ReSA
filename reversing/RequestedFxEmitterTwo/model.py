"""Canonical EXE byte stores/version branches/fallback model; no candidate source import."""
import struct
COUNTS={'Constructor':512,'LoadTextures':5120}
PROFILES=[['NULL']*4,['NULLX']*4,['']*4,['a'*31]*4,['b'*31]*4,['smoke','NULL','','NULLX'],['NULLX','b'*31,'NULL','a'*31],['a'*31,'NULLX','b'*31,'NULL']]
VERSIONS=[100,101,102,-1,2147483647]
FALLBACKS=[0,15,5,10]
def validate_fixture(name,f):
    i=f['index'];assert 0<=i<COUNTS[name];function=name=='LoadTextures';profile=i%8
    expected={'index':i,'poison':(profile%4)*85 if function else i%256,'patterned':not function and bool((i//256)%2),'profile':profile,'version':VERSIONS[(i//8)%5],'success_mask':(i//40)%16,'fallback_mask':FALLBACKS[(i//640)%4],'seeded':bool((i//2560)%2)}
    assert f==expected
def ctor_bytes(poison,patterned):
    b=bytearray((poison+j*37)%256 if patterned else poison for j in range(64));b[4]=0
    for j in [12,16,20,24,32,36,40]:b[j:j+4]=bytes(4)
    return b
def expected(name,f,leg):
    before=leg['before'];assert len(before)==96
    trace=[]
    if name=='Constructor':
        b=ctor_bytes(f['poison'],f['patterned']);returned=['storage',16]
        expectedBefore=[0xCD]*16+[(f['poison']+j*37)%256 if f['patterned'] else f['poison'] for j in range(64)]+[0xCD]*16
        assert before[:16]==expectedBefore[:16] and before[20:]==expectedBefore[20:]
    else:
        b=bytearray(before[16:80]);addresses=leg['texture_addresses'];assert len(set(addresses))==8
        constructed=ctor_bytes(f['poison'],False)
        for off in range(4,64):
            if not 12<=off<28:assert b[off]==constructed[off]
        for j in range(4):assert struct.unpack_from('<I',b,12+4*j)[0]==(addresses[j] if f['seeded'] else 0)
        for j,text in enumerate(PROFILES[f['profile']]):
            if j and (f['version']<=101 or text=='NULL'):continue
            success=bool(f['success_mask']&(1<<j));fallback=bool(f['fallback_mask']&(1<<j))
            trace.append({'ordinal':len(trace),'slot':j,'name':text,'mask':text+'m','bounded':True,'result':['textures',j+4] if success else None})
            if not success:trace.append({'ordinal':len(trace),'slot':j,'name':text,'mask':None,'bounded':True,'result':['textures',j+4] if fallback else None})
            struct.pack_into('<I',b,12+4*j,addresses[j+4] if success or fallback else 0)
        returned=1
    names=bytearray([0xD3]*160)
    for j,text in enumerate(PROFILES[f['profile']]):
        start=16+32*j;names[start:start+32]=bytes([f['poison']]*32);value=text.encode('ascii')+b'\x00';names[start:start+len(value)]=value
    return {'object':list(b[4:]),'guards_before':[0xCD]*16,'guards_after':[0xCD]*16,'names':list(names),'textures':[0xA7]*len(leg['payload']['textures']),'trace':trace,'returned':returned}
