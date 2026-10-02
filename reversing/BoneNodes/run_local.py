import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'BoneNode_c::Constructor': 64, 'BoneNode_c::Destructor': 64, 'BoneNode_c::Init': 512, 'BoneNode_c::InitLimits': 256, 'BoneNode_c::EulerToQuat': 2048, 'BoneNode_c::QuatToEuler': 2048, 'BoneNode_c::GetIdFromBoneTag': 256, 'BoneNode_c::ClampLimitsCurrent': 512, 'BoneNode_c::ClampLimitsDefault': 512, 'BoneNode_c::Limit': 2048, 'BoneNode_c::BlendKeyframe': 1024, 'BoneNode_c::GetSpeed': 64, 'BoneNode_c::SetSpeed': 64, 'BoneNode_c::SetLimits': 256, 'BoneNode_c::GetLimits': 256, 'BoneNode_c::AddChild': 256, 'BoneNode_c::CalcWldMat': 1024, 'BoneNodeManager_c::Constructor': 16, 'BoneNodeManager_c::Destructor': 16, 'BoneNodeManager_c::Init': 64, 'BoneNodeManager_c::Exit': 64, 'BoneNodeManager_c::Reset': 64, 'BoneNodeManager_c::GetBoneNode': 256, 'BoneNodeManager_c::ReturnBoneNode': 256, 'BoneNodeManager_c::GetNumBoneNodesLeft': 256}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'bones', 'Huesos (BoneNode_c, BoneNodeManager_c)', EXPECTED))
