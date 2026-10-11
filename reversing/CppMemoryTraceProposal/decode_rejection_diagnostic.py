"""Decode fixed diagnostic records; never executes the game."""
import json,struct,sys
from pathlib import Path
NAMES=['magic', 'version', 'recordBytes', 'kind', 'sequence', 'thread', 'owner', 'scopeSerial', 'rejectedSerial', 'scopeActive', 'pending', 'count', 'rejectedEip', 'rejectedEsp', 'exceptionCode', 'exceptionFlags', 'exceptionAddress', 'contextFlags', 'eip', 'esp', 'eflags', 'dr0', 'dr1', 'dr2', 'dr3', 'dr6', 'dr7', 'eax', 'ebp', 'returnAddress', 'argument0', 'argument1', 'matchMask', 'filterResult', 'writeErrorPrior', 'missedPrior']
FORMAT=struct.Struct('<36I')
def decode(path):
    data=Path(path).read_bytes()
    if len(data)%FORMAT.size:raise ValueError('Truncated fixed diagnostic record')
    rows=[]
    for offset in range(0,len(data),FORMAT.size):
        row=dict(zip(NAMES,FORMAT.unpack_from(data,offset)))
        if row['magic']!=0x4D52444B or row['version']!=1 or row['recordBytes']!=FORMAT.size:raise ValueError('Invalid record layout')
        mask=row['matchMask'];row['failed_matches']=[name for i,name in enumerate(('owner','rejected','serial','single_step','eip','esp','pending')) if not mask&1<<i]
        rows.append(row)
    return rows
if __name__=='__main__':print(json.dumps(decode(sys.argv[1]),indent=2))
