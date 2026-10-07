// @category ReIV
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import java.io.PrintWriter;
import java.io.File;

public class ExportMemoryInventory extends GhidraScript {
    private String quote(String value) { return "\"" + value.replace("\"", "\"\"") + "\""; }
    public void run() throws Exception {
        File directory = new File(getScriptArgs()[0]);
        directory.mkdirs();
        long base = currentProgram.getImageBase().getOffset();
        int count = 0;
        try (PrintWriter out = new PrintWriter(new File(directory, "ghidra_functions.csv"), "UTF-8")) {
            out.println("rva,va,name,body_bytes,thunk,calling_convention,source");
            FunctionIterator functions = currentProgram.getFunctionManager().getFunctions(true);
            while (functions.hasNext()) {
                monitor.checkCancelled();
                Function f = functions.next();
                if (f.isExternal() || !f.getEntryPoint().isMemoryAddress()) continue;
                long va = f.getEntryPoint().getOffset();
                out.printf("0x%08x,0x%08x,%s,%d,%s,%s,%s%n", va-base,va,
                    quote(f.getName(true)),f.getBody().getNumAddresses(),f.isThunk(),
                    quote(f.getCallingConventionName()),quote(f.getSymbol().getSource().toString()));
                count++;
            }
        }
        try (PrintWriter out = new PrintWriter(new File(directory, "ghidra_symbols.csv"), "UTF-8")) {
            out.println("rva,va,name,type,source");
            SymbolIterator symbols = currentProgram.getSymbolTable().getAllSymbols(true);
            while (symbols.hasNext()) {
                monitor.checkCancelled();
                Symbol s = symbols.next();
                if (!s.getAddress().isMemoryAddress() || s.isExternal()) continue;
                long va = s.getAddress().getOffset();
                out.printf("0x%08x,0x%08x,%s,%s,%s%n",va-base,va,
                    quote(s.getName(true)),quote(s.getSymbolType().toString()),quote(s.getSource().toString()));
            }
        }
        println("Exported " + count + " Ghidra function entries; no function reconstruction or hooks installed.");
    }
}
