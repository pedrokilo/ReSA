"""Independent integer/byte oracle; never relies on equal/model flags for acceptance."""
if not __debug__:raise RuntimeError("Strict bank requires Python assertions enabled")
KEYS=('CCustomBuildingRenderer::Initialise','CCustomBuildingRenderer::Shutdown',
      'CCustomBuildingRenderer::PluginAttach','CRealTimeShadowManager::ReturnRealTimeShadow')
EXPECTED=dict(zip(KEYS,(1024,1024,1024,2048)))
def pointer(value):
    assert isinstance(value,int) and 0<value<2**32
    return list(value.to_bytes(4,'little'))
def expected_row(row):
    key=row['function'];target=KEYS.index(key);i=row['case']
    assert type(i) is int and 0<=i<EXPECTED[key]
    if target<3:
        divisor=4 if target==0 else 2 if target==2 else 1
        wide=1024 if target==0 else 512 if target==2 else 256
        before=[((i//divisor)*17+k*29+(i//wide)*(k%5))&255 for k in range(128)]
        data=before.copy();trace=[];ret=0
        normal=bool(i&1);dn=bool(i&2);plugin=bool(i&1)
        assert row['normal_status'] is normal and row['dn_status'] is dn and row['plugin_status'] is plugin
        def step(role,status):
            trace.append(dict(role=role,status=status,token=before[64+role]))
            data[role]=(data[role]+role+1)&255
        if target==0:
            step(0,normal)
            if normal:step(1,dn);ret=int(dn)
        elif target==1:step(2,False);step(3,False)
        else:step(4,plugin);ret=int(plugin)
        assert row['initial_payload']==before
        assert row['model_payload']==data and row['model_trace']==trace and row['model_al']==ret
        return dict(gate=True,raw_al=ret,payload=data,trace=trace)
    initialized=bool(i&1);shadow_null=not initialized and bool(i&2)
    owner_null=not initialized and bool(i&4);poison=((i>>3)*37)&255
    assert row['initialized'] is initialized and row['shadow_null'] is shadow_null and row['owner_null'] is owner_null and row['poison']==poison
    shadow_pointer=pointer(row['shadow_address']);physical_pointer=pointer(row['physical_address']);pointer(row['manager_address'])
    manager=[poison]*0x54;manager[0]=int(initialized)
    shadow=[poison^0x99]*0x4c;shadow[:4]=[0]*4 if owner_null else physical_pointer
    physical=[poison^0x66]*0x138;physical[0x134:0x138]=[0]*4 if i&8 else shadow_pointer
    assert row['initial_manager']==manager and row['initial_shadow']==shadow and row['initial_physical']==physical
    if initialized:shadow[:4]=[0]*4;physical[0x134:0x138]=[0]*4
    assert row['model_manager']==manager and row['model_shadow']==shadow and row['model_physical']==physical
    return dict(gate=True,manager=manager,shadow=shadow,physical=physical,trace=[],guards=True)
