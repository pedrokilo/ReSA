import csv,sys
from exe_lib import *
lo=int(sys.argv[1],16); hi=int(sys.argv[2],16)
rows=[r for r in csv.DictReader(open('../FnLIB2A/exe_funcs.csv')) if lo<=int(r['va'],16)<=hi]
for r in rows:
    va=int(r['va'],16); sz=int(r['size'])
    ins=disasm(va,va+sz)
    callers=[]
    print(f"{r['va']} {sz:5} calls={r['calls']} str={r['strings'][:30]}")
