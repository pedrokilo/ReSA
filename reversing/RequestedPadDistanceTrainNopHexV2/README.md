# Full lossless Hex transport V2: exact x87 environment restoration

Preparation only, no build/GTA/approval. Original revision2 and rejected timeout attempt RequestedPadDistanceTrainNop/runs/20261008-161553-454998 remain untouched; incomplete evidence grants no partial approval.

Exactly 13952 cases remain: NOP1024, Distance10240, Train2688. oracle.py is byte-identical to revision2; all domains and integer/float/trace arithmetic are unchanged. Every original pad/camera/matrix/vehicle/global span is encoded lowercase hex. Camera is retained both separately and inside globals; no deduplication, masking, truncation or pointer replacement. Exact shape/length/type validation recreates the identical canonical array dictionary before the SAME independent oracle. Duplicate JSON keys and nonfinite constants reject.

HexTransport.hpp is the proven Shake asset with namespace-only adaptation. hex_asset.py is an exact copy of Shake hex_wire.py and supplies strict_loads/exact/byte_string. Exact original assets are preserved in assets and explicitly frozen with provenance. Target-specific strict shapes are in hex_wire.py.

FP() is immediately after target invocation and before any transport allocation. 52-byte snapshots, all hook flags, private guards, full camera and all globals/FP restoration remain unchanged. Target lookup was already cached outside loops; there is no per-case FindSimple search. The current matching compiled ASI/PDB Train RET8 gate remains mandatory.

Private pad/matrix have real object lifetimes; vehicle is a field-only ABI image with no constructor/gameplay/lifecycle credit. Camera borrows only the private live matrix and restores before its lifetime ends. Collaborator implementations, camera/resource ownership, graphics and general integration remain excluded, as in the original preparation.

Builder validates the closed explicit instrument-freeze input list before mutations and rechecks after compile, snapshots complete current source/configuration hashes, and restores WinMain and each bin independently even if another restore fails. Attempt outputs never become compile inputs implicitly. Source freeze is deferred until coordinator serial build; no source/instrument edits between build and replay.

Total replay deadline is 900 seconds instead of the insufficient 300; shared stall/hang detection remains unchanged. Counts and acceptance criteria remain identical. run_negative_baseline.py is fail-closed: it never grants approval, including unexpectedly clean negative results, and only lets shared runner measure natural exit after strict terminal rejection with exact product and all measured restoration/stages.

Coordinator serial commands after all other builds/replays end:

    python -B reversing/RequestedPadDistanceTrainNopHexV2/build_probe.py
    python -B reversing/RequestedPadDistanceTrainNopHexV2/run_negative_baseline.py

Preserve complete negative evidence before the separately authorized Train logic fix; afterward rebuild and run ordinary run_local.py for the positive replay. No registry writes during preparation.


V2 changes ONLY instrumentation restoration: CRT fegetenv/fesetenv cannot preserve C0, confirmed by the prior complete baseline and SDK/local x86 UCRT evidence. ReadX87Environment snapshots all 28 bytes with FNSTENV and immediately FLDENV restores its transient exception-mask effect. Final RestoreX87Environment uses FLDENV of the exact initial image, restores exact initial MXCSR, captures a fresh 28-byte image, and compares every byte without filtering. Terminal records retain both full images and measured equality. The raw original/candidate FP comparison and PC24/RN resets are unchanged; no MXCSR precision flag is masked. Caller register payloads beyond the environment are not credited. Actual compiled/runtime restore still requires coordinator replay.

Prior full Hex baseline runs/20261008-164703-580513 is preserved as rejected:13952 cases, native oracle failures zero, Distance1280 FP-only differences, Train414 differences (pad234, FP318 overlap). FP restore failed C0 status32->288 and no natural exit is proven. None of those rows are approved. Source proposal is separate and remains unapplied during this instrument preparation.
