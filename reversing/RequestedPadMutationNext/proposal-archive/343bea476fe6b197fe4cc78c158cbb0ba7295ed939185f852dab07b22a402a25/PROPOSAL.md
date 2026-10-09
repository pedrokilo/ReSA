<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7478 con evidencia histórica, 0 con vigencia acreditada y 968 pendientes en 17 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPad mutation selectors: static proposal only

Nine pending catalogue identities have exact fresh REA dossiers and complete primary EXE bodies in evidence/, bound to canonical EXE SHA72ae59e4â€¦. No compilation, execution, functional source edit, accreditation or change to existing banks.

Recommended order:

| Target | Entry | Proposed corpus | Collaborators and limits |
|---|---|---|---|
| SetTouched |53F200|256 complete private CPad poisons Ã—8 uint32 clocks|Only reads B7CB84 and writes pad+120; seed/restore full clock; compare full pad incl padding.|
| StopShaking |53FB50|256 pad poisons Ã—8 int16 args|Native RET4 no payload effects; declared void, never compare accidental EAX.|
| ClearMouseHistory |541BD0|256 independently poisoned three20-byte mouse states|Native clears first7 booleans each; preserve float bits, padding and neighboring bytes; restore all60 bytes.|
| ClearKeyBoardHistory |53F1E0|256 independent three0x270-byte states|Native KeyboardClear53F090 preserves enter+23C; preserve all guards and skipped fields. Audit actual helper bytes; no helper accreditation.|
| Clear |541A70|256 pad/global poison Ã—all4 bool parameter combinations|Five ControllerClear calls, three KeyboardClear calls; direct mouse boolean writes; private pad plus original static keyboard/mouse blocks seeded/restored independently.|
| Initialise |541D90|256 two-global-pad poison/config seeds|Original loops exactly2 global pads, calls Clear(1,1), zeroes Modes and B73400/B73401. Snapshot original global pads/keyboards/mice/bytes plus guards; no startup claim.|
| ReconcileTwoControllersInput |53F530|Axis crossproduct boundaries plus one-hot button truth/pressures and alias cases|Four signed axes, twenty buttons, directional conflicts; native scratch B736C0 size48 and candidate private function-local static scratch must each be located and restored. Return reference ABI/alias must be confirmed in original terminal assembly.|
| Update |541C40|4modes Ã—2 disabled Ã—5 hornindices Ã—input absent/present Ã—4 JustOutFrontEnd values Ã—independently patterned10-element steering history|All3 input states private. Real native reconcile/clear/check helpers and inline candidate helpers need symmetric pure contexts; clock/global native scratch and candidate scratch restored. Valid horn indices0..4 only; record invalid indices separately if added.|
| StartShake |53F920|4 vibration/cutscene combinations Ã—duration less/equal/greater Ã—frequency0/1/127/128/255 Ã—clock/deadline lower/equal/greater including wrap/high-bit boundaries|Only B7CB84 uint32, BA6768 vibration byte and B5F851 cutscene byte; full private pad. Reset x87/MXCSR each leg and restore if existing source performs FP conversion.|

Reconcile proposal: axes[-32768,-128,-1,0,1,128,32767], all49 ordered axis pairs in each of4 fields; ordinary-domain labels distinguish Â±32768/32767 signed-contract stress. Twenty button positions each gets0/1/128/255 in both controllers, plus sixteen left/right/up/down conflict masks; out separate, out==A, out==B aliases with source guard bytes. Source returns out reference; native EAX must be checked against that same private output address. Native scratch is B736C0 immediately after the two global pads; it is not part of a private pad. Candidate static local scratch lives in the DLL, so finding its exact symbol is a prerequisite. Never silently omit global mutation or normalize away scratch differences: report physical global writes and role-specific scratch contents separately, and scope any approved contract explicitly.

Static discrepancies to reproduce before fixes:

* StartShake native treats pad+12C as uint32 deadline, compares unsigned deadline<=clock, assigns wrapping clock+delay bits, and updates duration/frequency only when existing signed ShakeDur<requested time. Current header declares float NoShakeBeforeThis, source uses float comparison/conversion and if(ShakeDur). This is not an authorized source patch; baseline runtime must demonstrate both effects independently.
* Update native moves history9<-8 â€¦1<-0 and preserves history0. Current source rotates ten elements left, changing the whole history. Seed distinct entries, compare all ten bits directly, do not normalize the histories.
* Update native horn index increment uses byte wrapping then resets values>4 to0. Source modulo agrees for valid0..4; invalid starting bytes would constitute separate stress scope.

ProcessPCSpecificStuff53FB40 has REA procedure-lookup failure; primary PE window included and shows a trivial return, but it is not counted among these nine until exact body/ABI identity is independently established. StartShake_Distance and StartShake_Train are deferred because camera matrix/FindPlayerVehicle/type contexts require additional bounded collaborators. ProcessPad/UpdateMouse require OS/input interfaces and are outside this pure proposal.

All future wrappers must snapshot original/candidate entry contents, entire registered vtables and all hook flags before disabling hooks. They must preserve/restore every original static global and any candidate private static scratch, compare whole private objects and guards, bind source/fixture/evidence hashes and demand natural exit0. Helper windows include branches past early returns plus explicit trailing padding; they are primary CPU evidence, not fresh REA identities or additional validation credit.


Terminal ABI verification: Reconcile native53F8F2 loads EAX=[ESP+10] (output argument), copies twelve dwords from B736C0 to EDI=EAX, restores three saved registers and RET0C. Therefore returned pointer identity is exactly the private out arena, including alias cases. Native ControllerClear53EF80–53EFE1, ControllerCheckForInput53EFF0–53F088 and KeyboardClear53F090–53F1D6 contain no calls; both CheckForInput terminal returns and all internal targets are retained in the primary windows. ProcessPCSpecificStuff53FB40 is a one-byte RET.

Additional StartShake static boundary: pad+130 is compared as unsigned byte in the original. The source NoShakeFreq is char, whose signed promotion can change behavior for128..255. Include independent stored-frequency128/255 cases before attributing any deadline or duration mismatch. These observations remain static evidence pending runtime reproduction.
