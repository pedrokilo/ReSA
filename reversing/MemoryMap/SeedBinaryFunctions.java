// @category ReSA
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import java.nio.file.Files;
import java.nio.file.Path;

public class SeedBinaryFunctions extends GhidraScript {
    public void run() throws Exception {
        int count=0;
        for(String line:Files.readAllLines(Path.of(getScriptArgs()[0]))) {
            monitor.checkCancelled();
            if(line.isBlank())continue;
            Address address=toAddr(Long.parseUnsignedLong(line.replace("0x",""),16));
            currentProgram.getSymbolTable().addExternalEntryPoint(address);
            count++;
        }
        println("Registered "+count+" binary/source candidate entry points for automatic analysis.");
    }
}
