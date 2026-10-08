"""Synthetic oracle contract checks. No compilation, game, or runtime approval."""
import copy,json,math,struct
from pathlib import Path
import oracle as o

def row_for(bank,i):
    f=o.fixture(bank,i);row=dict(f,event='comparison',function=o.KEYS[bank],bank=bank,case=i,pair=sum(o.EXPECTED[o.KEYS[k]] for k in range(bank))+i,equal=True)
    row.pop('camera');row.pop('point');row['camera_bits']=list(map(o.bits,f['camera']));row['point_bits']=list(map(o.bits,f['point']))
    pad=[((bank*50000+i)*73+k*29+(k>>3))&255 for k in range(0x134)]
    for k in range(0x118,0x120):pad[k]&=1
    pad[0x10c:0x10e]=struct.pack('<h',f['old_duration']);pad[0x110]=(i*17+3)&255;row['initial_pad']=pad.copy()
    d=[o.f32(c-p) for c,p in zip(f['camera'],f['point'])];sq=[o.f32(x*x) for x in d]
    total=(o.f32(sq[1]+sq[0]) if bank==2 else o.f32(o.f32(sq[2]+sq[1])+sq[0])) if bank else 0.
    mag=o.f32(math.sqrt(total));row['model_sum_bits']=o.bits(total);row['model_magnitude_bits']=o.bits(mag)
    trace=[];duration=f['old_duration'];freq=pad[0x110]
    if f['gates']==1:
        if bank==1 and mag<70:
            if not f['request_frequency']:duration=freq=0
            elif f['request_duration']>duration:duration=f['request_duration'];freq=f['request_frequency']
        if bank==2:
            trace=[dict(event='find_player_vehicle',player=-1,remote=False,role=f['vehicle_role'],arguments_valid=True)]*(2 if f['vehicle_role'] else 1)
            if f['vehicle_role']!=2 and total<4900 and duration<100:duration=100;freq=int(o.f32(o.f32(70.-mag)+30.))&255
    pad[0x10c:0x10e]=struct.pack('<h',duration);pad[0x110]=freq;row['model_pad']=pad;row['model_trace']=trace
    camera=[0]*0xD78;matrix=[0]*0x54;pointer=0x12345000
    camera[4:16]=struct.pack('<fff',*f['camera']);matrix[0x30:0x3c]=struct.pack('<fff',*f['camera']);camera[0x14:0x18]=(pointer if f['mode'] else 0).to_bytes(4,'little')
    vehicle=[(i*37)&255]*0x5A0;vehicle[0x590:0x594]=struct.pack('<i',6 if f['vehicle_role']==2 else 0)
    glob=[camera,[0]*33,[0]*33,[0]*0x268];glob[1][16]=int(bool(f['gates']&1));glob[2][16]=int(bool(f['gates']&2))
    row.update(initial_camera=camera,initial_matrix=matrix,initial_vehicle=vehicle,initial_globals=glob,matrix_address=pointer)
    result=dict(gate=True,helper_patch_exact=True,collaborator_valid=True,pad=pad,camera=camera,matrix=matrix,vehicle=vehicle,globals=glob,trace=trace,guards=True,fp=dict(control=0x20000,exceptions=0,mxcsr=0x1F80))
    row['original']=copy.deepcopy(result);row['candidate']=copy.deepcopy(result)
    return row

def main():
    count=0
    for bank,key in enumerate(o.KEYS):
        for i in range(o.EXPECTED[key]):o.expected_row(row_for(bank,i));count+=1
    failures=[]
    checks={
      'equal_routes_wrong_pad':lambda r:(r['original']['pad'].__setitem__(0,0),r['candidate']['pad'].__setitem__(0,0)),
      'wrong_native_trace_count':lambda r:r['original']['trace'].append(dict(r['model_trace'][0])),
      'wrong_float_boundary_bits':lambda r:r.__setitem__('model_sum_bits',r['model_sum_bits']+1),
      'readonly_camera_changed':lambda r:r['candidate']['camera'].__setitem__(0,1),
      'wrong_FP_precision':lambda r:r['candidate']['fp'].__setitem__('control',0),
      'missing_patch_proof':lambda r:r['candidate'].__setitem__('helper_patch_exact',False),
      'damaged_guard':lambda r:r['candidate'].__setitem__('guards',False),
    }
    for name,mutate in checks.items():
        row=row_for(2,32);mutate(row)
        try:o.expected_row(row)
        except AssertionError:failures.append(name)
        else:raise RuntimeError('Synthetic rejection missing: '+name)
    result=dict(synthetic_only=True,no_runtime_approval=True,accepted_fixture_rows=count,rejected_mutations=failures,python_syntax_checked=True)
    (Path(__file__).resolve().parent/'prepare-checks.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
if __name__=='__main__':main()
