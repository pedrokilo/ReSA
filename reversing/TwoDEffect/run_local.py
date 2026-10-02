import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'C2dEffect::Roadsign_GetNumLinesFromFlags': 256, 'C2dEffect::Roadsign_GetNumLettersFromFlags': 256, 'C2dEffect::Roadsign_GetPaletteIDFromFlags': 256, 'C2dEffect::Shutdown': 512, 'C2dEffect::DestroyAtomic': 64, 'C2dEffect::PluginAttach': 64, 'C2dEffect::RpGeometryGet2dFxCount': 128, 'C2dEffect::RpGeometryGet2dFxAtIndex': 256, 'C2dEffect::t2dEffectPluginConstructor': 64, 'C2dEffect::t2dEffectPluginDestructor': 512, 'C2dEffect::t2dEffectPluginCopyConstructor': 64, 'C2dEffect::Rwt2dEffectPluginDataChunkReadCallBack': 1024, 'C2dEffect::Rwt2dEffectPluginDataChunkWriteCallBack': 32, 'C2dEffect::Rwt2dEffectPluginDataChunkGetSizeCallBack': 32}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'twod', 'C2dEffect', EXPECTED))
