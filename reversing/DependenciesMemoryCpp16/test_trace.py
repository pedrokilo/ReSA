"""Pruebas offline del analizador; no ejecuta juego ni compila."""
import copy
import run_local as a
begin={'owner_thread':71,'debug_armed':{'dr7':0x415},'candidate_prefixes':[{'body':0x10001000}]*4}
event={'entry':0x824257,'eip':0x824257,'args':[33],'owner_thread':71,'scope_serial':1,'sequence':0,'dr7':0x415,'dr6':1,'esp':0x123400,'return_address':0x10002000,'stack_words_valid':True,'context_preserved':True,'caller_origin_valid':True}
def check(e):a.raw_trace(begin,e,[(0x824257,[33])],1,0x10002000,0)
check([event]);rejected=0
mutations={'entry':0x824269,'eip':0x824269,'args':[34],'owner_thread':72,'scope_serial':2,'sequence':1,'dr7':0,'dr6':0,'esp':1,'return_address':0x72F42A,'stack_words_valid':False,'context_preserved':False,'caller_origin_valid':False}
for key,value in mutations.items():
    bad=copy.deepcopy(event);bad[key]=value
    try:check([bad])
    except RuntimeError:rejected+=1
    else:raise AssertionError(key)
for events in ([],[event,event]):
    try:check(events)
    except RuntimeError:rejected+=1
    else:raise AssertionError('count')
assert rejected==15
print('1 valid raw trace; 15 invalid controls rejected; no runtime validation')
