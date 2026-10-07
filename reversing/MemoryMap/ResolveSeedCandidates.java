// @category ReSA
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import java.nio.file.Files;
import java.nio.file.Path;
import java.io.PrintWriter;

public class ResolveSeedCandidates extends GhidraScript {
    public void run() throws Exception {
        try(PrintWriter out=new PrintWriter(getScriptArgs()[1],"UTF-8")) {
            out.println("va,result,function_va,instruction");
            for(String line:Files.readAllLines(Path.of(getScriptArgs()[0]))) {
                monitor.checkCancelled();if(line.isBlank())continue;
                Address address=toAddr(Long.parseUnsignedLong(line.replace("0x",""),16));
                Function containing=getFunctionContaining(address);
                if(containing!=null) {
                    out.printf("%s,inside_existing_body,0x%08x,%n",line,containing.getEntryPoint().getOffset());continue;
                }
                disassemble(address);
                Instruction instruction=getInstructionAt(address);
                Function function=instruction==null?null:createFunction(address,null);
                out.printf("%s,%s,%s,%s%n",line,function==null?"unresolved":"created_function",
                    function==null?"":String.format("0x%08x",function.getEntryPoint().getOffset()),instruction==null?"":instruction.getMnemonicString());
            }
        }
    }
}
