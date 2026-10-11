#pragma once
namespace DependenciesMemoryCpp16Live {
struct CandidatePrefix {
    uintptr_t body{};
    std::vector<uint8> bytes;
    void Capture(uintptr_t at){body=at;bytes=ReversingProbe::Bytes(reinterpret_cast<void*>(at),64);}
    bool Check()const{return ModuleOf(body)==ModuleOf(uintptr_t(&ModuleMarker))&&bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(body),64);}
    json Metadata()const{return {{"body",body},{"module",uintptr_t(ModuleOf(body))},{"stable_prefix_bytes",bytes},{"identity_with_exe_required",false}};}
};
__declspec(noinline) inline Result ProtectedGate(uintptr_t entry,const std::array<uint32,3>& args,unsigned arity) {
    Result result{};
    __try {result=Gate(entry,args,arity);}
    __except(hardwareTrace.Filter(GetExceptionInformation())) {result.aborted=true;}
    return result;
}
inline json DebugJSON(const DebugSnapshot& d){return {{"dr0",d.dr0},{"dr1",d.dr1},{"dr2",d.dr2},{"dr3",d.dr3},{"dr6",d.dr6},{"dr7",d.dr7}};}
inline std::array<ExpectedEvent,2> Expected(unsigned fn,uint32 size,uint32 align,uint32 input,uint32 base,unsigned& count){
    std::array<ExpectedEvent,2> result{};count=1;
    if(fn==0)result[0]={0x824257,size,0};
    if(fn==1){result[0]={0x824269,input,size};if(!input){result[1]={0x824257,size,0};count=2;}else if(!size){result[1]={0x82413F,input,0};count=2;}}
    if(fn==2)result[0]={0x824257,size+align,0};
    if(fn==3)result[0]={0x82413F,base,0};
    return result;
}
inline bool CallerOrigin(const HardwareEvent& event,unsigned index,unsigned fn,bool candidate,uintptr_t body,bool calibration){
    if(index&&fn==1){
        if(event.entry==0x824257)return event.returnAddress==0x824286;
        if(event.entry==0x82413F)return event.returnAddress==0x824299;
        return false;
    }
    if(calibration)return event.returnAddress==hardwareTrace.expectedGateReturn;
    if(candidate)return event.returnAddress==hardwareTrace.expectedGateReturn
        ||(event.returnAddress>=body&&event.returnAddress<body+1024&&ModuleOf(event.returnAddress)==ModuleOf(uintptr_t(&ModuleMarker)));
    constexpr unsigned callOffsets[]{5,10,12,11};
    return fn==3?event.returnAddress==hardwareTrace.expectedGateReturn:event.returnAddress==fns[fn].address+callOffsets[fn]+5;
}
inline json TraceRows(unsigned fn,bool candidate,uintptr_t body,bool calibration,bool& callers){
    json rows=json::array();callers=true;
    for(unsigned i=0;i<hardwareTrace.Count();++i){
        const auto& e=hardwareTrace.Event(i);const bool caller=CallerOrigin(e,i,fn,candidate,body,calibration);callers&=caller;
        json args=json::array({e.argument0});if(e.entry==0x824269)args.push_back(e.argument1);
        rows.push_back({{"entry",e.entry},{"eip",e.eip},{"esp",e.esp},{"return_address",e.returnAddress},{"args",args},
            {"owner_thread",e.ownerThread},{"scope_serial",e.scopeSerial},{"sequence",e.sequence},{"dr6",e.dr6},{"dr7",e.dr7},
            {"stack_words_valid",e.stackValid},{"context_preserved",e.contextPreserved},{"caller_origin_valid",caller}});
    }
    return rows;
}
inline json NormalizedTrace(unsigned fn){
    json rows=json::array();
    for(unsigned i=0;i<hardwareTrace.Count();++i){const auto& e=hardwareTrace.Event(i);
        if(e.entry==0x824257)rows.push_back(json::array({"malloc",e.argument0}));
        else if(e.entry==0x824269)rows.push_back(json::array({"realloc",e.argument0?"input":"null",e.argument1}));
        else rows.push_back(json::array({"free",fn==3?"base":"input"}));
    }
    return rows;
}
struct ObservedCall {Result abi;bool exact{},callers{},debug{},scope{};json raw,normalized;};
inline ObservedCall Observe(uintptr_t entry,const std::array<uint32,3>& args,unsigned arity,
    const std::array<ExpectedEvent,2>& expected,unsigned count,DWORD serial,unsigned fn,bool candidate,uintptr_t body,bool calibration=false){
    hardwareTrace.Begin(expected,count,serial);
    const auto result=ProtectedGate(entry,args,arity);
    const bool scope=hardwareTrace.End();
    bool callers{};auto raw=TraceRows(fn,candidate,body,calibration,callers);
    auto normalized=NormalizedTrace(fn);
    const bool debug=hardwareTrace.VerifyArmed();
    return {result,scope&&!result.aborted,callers,debug,scope,std::move(raw),std::move(normalized)};
}
inline bool calibrationResourcesRestored=true;
inline json Calibrate(bool& passed){
    json rows=json::array();passed=true;calibrationResourcesRestored=true;DWORD serial=1;
    auto perform=[&](const char* name,uintptr_t entry,std::array<uint32,3> args,unsigned arity,std::array<ExpectedEvent,2> plan,unsigned length,unsigned fn){
        auto seen=Observe(entry,args,arity,plan,length,serial++,fn,false,0,true);
        const bool valid=seen.exact&&seen.callers&&seen.debug&&seen.abi.stack&&seen.abi.nonvolatile;
        passed&=valid;rows.push_back({{"name",name},{"scope_serial",serial-1},{"trace",seen.raw},{"normalized",seen.normalized},
            {"passed",valid},{"aborted",seen.abi.aborted},{"gate_return",hardwareTrace.expectedGateReturn},{"input",args}});
        return seen.abi;
    };
    auto first=perform("malloc33",0x824257,{33,0,0},1,{{{0x824257,33,0},{}}},1,0);
    if(!passed){calibrationResourcesRestored=false;RestoreFloating();return rows;}
    auto* memory=reinterpret_cast<uint8*>(first.eax);passed&=memory!=nullptr;
    if(memory){Fill(memory,33,13);passed&=Matches(memory,33,13);
        const auto released=perform("free_malloc33",0x82413F,{uint32(uintptr_t(memory)),0,0},1,{{{0x82413F,uint32(uintptr_t(memory)),0},{}}},1,3);
        if(!passed){calibrationResourcesRestored=false;RestoreFloating();return rows;}
        if(released.aborted)NativeFree(memory);}
    auto created=perform("realloc_null17",0x824269,{0,17,0},2,{{{0x824269,0,17},{0x824257,17,0}}},2,1);
    if(!passed){calibrationResourcesRestored=false;RestoreFloating();return rows;}
    memory=reinterpret_cast<uint8*>(created.eax);passed&=memory!=nullptr;if(memory)NativeFree(memory);
    memory=NativeMalloc(23);passed&=memory!=nullptr;
    if(memory){const auto released=perform("realloc_owned_zero",0x824269,{uint32(uintptr_t(memory)),0,0},2,
        {{{0x824269,uint32(uintptr_t(memory)),0},{0x82413F,uint32(uintptr_t(memory)),0}}},2,1);
        if(!passed){calibrationResourcesRestored=false;RestoreFloating();return rows;}
        passed&=!released.aborted&&released.eax==0;
        if(released.aborted)NativeFree(memory);}
    memory=NativeMalloc(23);passed&=memory!=nullptr;
    if(memory){Fill(memory,23,19);auto grown=perform("realloc_owned65",0x824269,{uint32(uintptr_t(memory)),65,0},2,
        {{{0x824269,uint32(uintptr_t(memory)),65},{}}},1,1);
        if(!passed){calibrationResourcesRestored=false;RestoreFloating();return rows;}
        auto* output=reinterpret_cast<uint8*>(grown.eax);passed&=output&&Matches(output,23,19);
        // A rejected first entry has not touched input. Unexpected nested calls
        // after realloc began do not justify assuming its ownership state.
        if(grown.aborted&&hardwareTrace.Count()>1)calibrationResourcesRestored=false;
        else NativeFree(output?output:memory);}
    // Mandatory negative: wrong size is rejected BEFORE malloc's first opcode.
    const auto negative=Observe(0x824257,{17,0,0},1,{{{0x824257,18,0},{}}},1,serial++,0,false,0,true);
    const bool negativeOK=negative.abi.aborted&&hardwareTrace.Rejected()&&!negative.exact&&negative.callers&&negative.debug
        &&hardwareTrace.Count()==1&&hardwareTrace.Event(0).argument0==17&&hardwareTrace.Event(0).entry==0x824257;
    if(!negative.abi.aborted&&negative.abi.eax)NativeFree(reinterpret_cast<void*>(negative.abi.eax));
    passed&=negativeOK;
    rows.push_back({{"name","reject_malloc17_expected18"},{"scope_serial",serial-1},{"trace",negative.raw},{"passed",negativeOK},
        {"aborted",negative.abi.aborted},{"expected_size",18},{"actual_size",17},{"allocator_instruction_executed",!negative.abi.aborted},
        {"gate_return",hardwareTrace.expectedGateReturn}});
    const auto before=hardwareTrace.OutsideHits();auto* outside=NativeMalloc(11);passed&=outside!=nullptr;if(outside)NativeFree(outside);
    const bool outsideOK=outside&&hardwareTrace.OutsideHits()>=before+2;
    passed&=outsideOK;rows.push_back({{"name","outside_scope"},{"passed",outsideOK},{"owned_hits_delta",hardwareTrace.OutsideHits()-before}});
    hardwareTrace.Begin({{{0x824257,11,0},{}}},1,serial++);
    const bool worker=hardwareTrace.CrossThreadCalibration();hardwareTrace.End();
    const bool crossOK=worker&&hardwareTrace.Count()==0&&!hardwareTrace.Rejected()&&hardwareTrace.VerifyArmed();
    passed&=crossOK;rows.push_back({{"name","other_thread"},{"passed",crossOK},{"owner_trace_count",hardwareTrace.Count()}});
    RestoreFloating();return rows;
}
} // namespace DependenciesMemoryCpp16Live
