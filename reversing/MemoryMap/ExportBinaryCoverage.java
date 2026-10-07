// @category ReSA
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.address.AddressRangeIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.program.model.address.Address;
import java.io.File;
import java.io.PrintWriter;

public class ExportBinaryCoverage extends GhidraScript {
    public void run() throws Exception {
        File directory=new File(getScriptArgs()[0]);directory.mkdirs();
        try(PrintWriter out=new PrintWriter(new File(directory,"function_ranges.csv"),"UTF-8")) {
            out.println("function_va,start_va,end_va");
            FunctionIterator functions=currentProgram.getFunctionManager().getFunctions(true);
            while(functions.hasNext()) {
                monitor.checkCancelled();Function f=functions.next();if(f.isExternal())continue;
                AddressRangeIterator ranges=f.getBody().getAddressRanges();
                while(ranges.hasNext()) {
                    AddressRange range=ranges.next();
                    out.printf("0x%08x,0x%08x,0x%08x%n",f.getEntryPoint().getOffset(),range.getMinAddress().getOffset(),range.getMaxAddress().getOffset());
                }
            }
        }
        try(PrintWriter code=new PrintWriter(new File(directory,"instructions.csv"),"UTF-8");
            PrintWriter calls=new PrintWriter(new File(directory,"calls.csv"),"UTF-8")) {
            code.println("va,length,function_va");calls.println("caller_va,instruction_va,target_va,kind");
            InstructionIterator instructions=currentProgram.getListing().getInstructions(true);
            while(instructions.hasNext()) {
                monitor.checkCancelled();Instruction i=instructions.next();
                Function f=currentProgram.getFunctionManager().getFunctionContaining(i.getAddress());
                String owner=f==null?"":String.format("0x%08x",f.getEntryPoint().getOffset());
                code.printf("0x%08x,%d,%s%n",i.getAddress().getOffset(),i.getLength(),owner);
                if(i.getFlowType().isCall()) {
                    Address[] targets=i.getFlows();
                    if(targets.length==0)calls.printf("%s,0x%08x,,indirect_unresolved%n",owner,i.getAddress().getOffset());
                    for(Address target:targets)calls.printf("%s,0x%08x,0x%08x,direct%n",owner,i.getAddress().getOffset(),target.getOffset());
                }
            }
        }
        println("Exported function body ranges, instruction coverage and static call sites.");
    }
}
