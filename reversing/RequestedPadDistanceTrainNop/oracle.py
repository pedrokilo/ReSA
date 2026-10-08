"""Native PC24 finite arithmetic and byte oracle, independent of candidate CVector code."""
import math,struct
if not __debug__:raise RuntimeError('Strict bank requires assertions enabled')
KEYS=('CPad::ProcessPCSpecificStuff','CPad::StartShake_Distance','CPad::StartShake_Train')
EXPECTED=dict(zip(KEYS,(1024,10240,2688)))
DURATIONS=((-32768,-32768),(-32768,-1),(-1,-32768),(-1,0),(0,-1),(0,0),(0,1),(1,0),(1,1),(1,100),(99,100),(100,99),(100,100),(32767,-32768),(-32768,32767),(32767,32767))
TRAIN_DURATIONS=(-32768,-1,0,99,100,101,32767)
FREQUENCIES=(0,1,127,128,255)
ORIGINS=((0.,0.,0.),(13.5,-7.25,4.),(-17.25,11.5,-3.),(.125,-.25,103.5))
def f32(v):return struct.unpack('<f',struct.pack('<f',v))[0]
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def frombits(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def s16(v):return (v+32768)%65536-32768
LO=frombits(bits(70.)-1);HI=frombits(bits(70.)+1)
DELTA=((0.,0.,0.),(LO,0.,0.),(70.,0.,0.),(HI,0.,0.),(-LO,0.,0.),(0.,LO,0.),(0.,70.,0.),(0.,HI,0.),(0.,0.,LO),(0.,0.,70.),(0.,0.,HI),(42.,56.,0.),(42.,frombits(bits(56.)-1),0.),(42.,frombits(bits(56.)+1),0.),(1.,2.,3.),(20.,-30.,40.))
def fixture(bank,i):
    if bank==0:return dict(geometry=0,mode=(i//4)%2,gates=i%4,vehicle_role=0,duration_role=(i//8)%4,old_duration=s16(i*127),request_duration=0,request_frequency=i&255,camera=ORIGINS[(i//8)%4],point=DELTA[i%16])
    n=i;geometry=n%16;n//=16;mode=n%2;n//=2;gates=n%4;n//=4
    if bank==1:
        freq=FREQUENCIES[n%5];n//=5;role=n;old,request=DURATIONS[role];vehicle=0
    else:
        vehicle=n%3;n//=3;role=n;old=TRAIN_DURATIONS[role];request=freq=0
    camera=ORIGINS[role%4];point=tuple(f32(c+d) for c,d in zip(camera,DELTA[geometry]))
    return dict(geometry=geometry,mode=mode,gates=gates,vehicle_role=vehicle,duration_role=role,old_duration=old,request_duration=request,request_frequency=freq,camera=camera,point=point)
def expected_row(row):
    key=row['function'];bank=KEYS.index(key);i=row['case'];assert type(i) is int and 0<=i<EXPECTED[key] and row['bank']==bank
    f=fixture(bank,i)
    for k,v in f.items():
        if k not in ('camera','point'):assert row[k]==v,(k,row[k],v)
    assert row['camera_bits']==list(map(bits,f['camera'])) and row['point_bits']==list(map(bits,f['point']))
    pad=[((bank*50000+i)*73+k*29+(k>>3))&255 for k in range(0x134)]
    for k in range(0x118,0x120):pad[k]&=1
    pad[0x10c:0x10e]=list(struct.pack('<h',f['old_duration']));pad[0x110]=(i*17+3)&255
    assert row['initial_pad']==pad
    total=mag=0.
    if bank:
        difference=[f32(c-p) for c,p in zip(f['camera'],f['point'])]
        square=[f32(d*d) for d in difference]
        total=f32(square[1]+square[0]) if bank==2 else f32(f32(square[2]+square[1])+square[0])
        mag=f32(math.sqrt(total))
    assert row['model_sum_bits']==bits(total) and row['model_magnitude_bits']==bits(mag)
    old=f['old_duration'];duration=old;freq=pad[0x110];trace=[]
    if f['gates']==1:
        if bank==1 and mag<70:
            if not f['request_frequency']:duration=freq=0
            elif f['request_duration']>old:duration=f['request_duration'];freq=f['request_frequency']
        if bank==2:
            call=dict(event='find_player_vehicle',player=-1,remote=False,role=f['vehicle_role'],arguments_valid=True)
            trace=[call]*(2 if f['vehicle_role'] else 1)
            if f['vehicle_role']!=2 and total<4900 and old<100:
                duration=100;freq=int(f32(f32(70.-f32(math.sqrt(total)))+30.))&255
    pad[0x10c:0x10e]=list(struct.pack('<h',duration));pad[0x110]=freq
    assert row['model_pad']==pad and row['model_trace']==trace
    camera=row['initial_camera'];matrix=row['initial_matrix'];vehicle=row['initial_vehicle'];glob=row['initial_globals']
    assert isinstance(camera,list) and len(camera)==0xD78 and isinstance(matrix,list) and len(matrix)==0x54 and isinstance(vehicle,list) and len(vehicle)==0x5A0
    assert len(glob)==4 and list(map(len,glob))==[0xD78,33,33,0x268]
    assert camera==glob[0]
    assert camera[4:16]==list(struct.pack('<fff',*f['camera'])) and matrix[0x30:0x3c]==list(struct.pack('<fff',*f['camera']))
    pointer=row['matrix_address'];assert isinstance(pointer,int) and 0<pointer<2**32
    assert camera[0x14:0x18]==list((pointer if f['mode'] else 0).to_bytes(4,'little'))
    assert glob[1][16]==int(bool(f['gates']&1)) and glob[2][16]==int(bool(f['gates']&2))
    expected_vehicle=[(i*37)&255]*0x5A0;expected_vehicle[0x590:0x594]=list(struct.pack('<i',6 if f['vehicle_role']==2 else 0))
    assert vehicle==expected_vehicle
    expected=dict(gate=True,helper_patch_exact=True,collaborator_valid=True,pad=pad,camera=camera,matrix=matrix,vehicle=vehicle,globals=glob,trace=trace,guards=True)
    for route in (row['original'],row['candidate']):
        assert all(route.get(k)==v for k,v in expected.items())
        fp=route.get('fp');assert isinstance(fp,dict) and all(type(fp.get(k)) is int for k in ('control','exceptions','mxcsr'))
        # _MCW_PC=0x30000, _PC_24=0x20000; round nearest (_MCW_RC=0x300).
        assert fp['control']&0x30300==0x20000 and fp['mxcsr']&0x6000==0
    assert row['original']['fp']==row['candidate']['fp']
    expected['fp']=row['original']['fp']
    return expected
