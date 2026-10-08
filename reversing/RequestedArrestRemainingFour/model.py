"""Finite model derived from canonical EXE stores/calls, independent of candidate C++."""
import struct
COUNTS={'Constructor':128,'Destructor':256,'MakeAbortable':384,'CreateFirstSubTask':2048}

def expected(function, fixture, leg):
    i=fixture['index']; before=leg['task_before']; task=bytearray(before); trace=[]
    assert len(before)==36 and before[12:16]==[fixture['poison']]*4 and before[20:32]==[fixture['poison']]*12
    def zero(off):task[off:off+4]=bytes(4)
    ref=bool(fixture['reference']);child=bool(fixture['child'])
    if function=='Constructor':
        zero(4);zero(8);zero(0x20)
        if ref:trace.append(['reference',['target',0],['task',16]])
        returned=['task',0]
    elif function=='Destructor':
        if ref:trace.append(['cleanup',['target',0],['task',16]])
        if child:trace.append(['delete',['child',0],1])
        zero(8);returned=None
    elif function=='MakeAbortable':
        p=['ped',0] if (i//192)%2 else None
        event=['eventArena',0] if (i//48)%2 else None
        result=bool((i//96)%2)
        trace.append(['abort',['child',0],p,(i//16)%3,event,result]);returned=1 if result else None
    else:
        scenario=fixture['scenario'];control=fixture['controls'];null=bool(control&8)
        if not scenario:return {'task':list(task[4:]),'trace':[],'returned':None}
        task[12]=0
        if scenario==1:type_=1000
        elif scenario in (2,3):type_=703
        elif scenario==4:
            type_=1003;melee=bool(control&1);trace.append(['melee',['ped',0x5A0+28*((i//512)%4)],melee])
            if melee:
                shotgun=bool(control&2);trace.append(['has',['ped',0],25,shotgun])
                if shotgun:trace.append(['set',['ped',0],25])
                else:
                    pistol=bool(control&4);trace.append(['has',['ped',0],22,pistol])
                    if not pistol:trace.append(['give',['ped',0],22,10,False])
                    trace.append(['set',['ped',0],22])
        else:
            upside=scenario==6;trace.append(['upside',['vehicle',0],upside])
            if upside:type_=1003
            else:
                side=scenario==7;trace.append(['side',['vehicle',0],side]);type_=1003 if side else 722
        trace.append(['create_sub',['task',0],type_,['ped',0],null]);returned=None if null else ['output',0]
    return {'task':list(task[4:]),'trace':trace,'returned':returned}

def validate_fixture(function, fixture):
    i=fixture['index'];assert 0<=i<COUNTS[function]
    fi=list(COUNTS).index(function)
    poison=(i%64)*4 if fi<2 else (i%16)*17 if fi==2 else (i%4)*85
    assert fixture=={'index':i,'poison':poison,'reference':bool((i//64)%2) if fi<2 else bool((i//4)%2),'child':fi==2 or bool((i//128)%2),'scenario':(i//4)%8,'controls':(i//32)%16}
    # Pointer slots are validated by exact trace roles and per-leg native/DLL vptr checks;
    # all non-pointer seed bytes must retain the declared poison before invocation.
    return True
