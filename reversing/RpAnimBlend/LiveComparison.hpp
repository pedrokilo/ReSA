#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
namespace RpAnimBlendLive {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){log<<r.dump()<<'\n';log.flush();}
inline int Report(int type,char* msg,int*){if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",msg?msg:""}});ExitProcess(91);}
inline bool Select(bool enabled){bool found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()!="RpAnimBlend")return;for(auto& h:c.Items())if(h->Name()=="RpAnimBlendClumpUpdateAnimations"){c.SetItemEnabled(h,enabled);found=h->Hooked()==enabled;}});return found;}
struct Fixture {
    RpClump* clump{};RtAnimAnimation* ownedAnimation{};CAnimBlendClumpData* bd{};bool skin{};CVector velocity{.3f,-.2f,.1f};
    std::vector<std::unique_ptr<CAnimBlendHierarchy>> hierarchies;
    std::vector<int> callbacks;
    static void Callback(CAnimBlendAssociation* a,void* data){static_cast<Fixture*>(data)->callbacks.push_back(a->GetAnimId());}
    Fixture(RpClump* source,bool skinned,bool compressed,int visible,int mode,int extract):skin(skinned){
        if(skin){
            clump=RpClumpClone(source);if(!clump)return;
            auto* hierarchy=GetAnimHierarchyFromClump(clump);
            if(!hierarchy||hierarchy==GetAnimHierarchyFromSkinClump(source))return;
            RpClumpForAllAtomics(clump,[](RpAtomic* atomic,void* h){return RpSkinAtomicSetHAnimHierarchy(atomic,static_cast<RpHAnimHierarchy*>(h));},hierarchy);
            ownedAnimation=RpAnimBlendCreateAnimationForHierarchy(hierarchy);
            if(!ownedAnimation)return;
            RtAnimInterpolatorSetCurrentAnim(hierarchy->currentAnim,ownedAnimation);
            RpAnimBlendClumpInit(clump);bd=RpAnimBlendClumpGetData(clump);
            for(unsigned f=0;f<bd->m_NumFrameData;++f){auto& fd=bd->m_FrameDatas[f];fd.KeyFrame->q=RtQuat{0.f,0.f,0.f,1.f};fd.KeyFrame->t=fd.BonePos;}
        }
        else{
            clump=RpClumpCreate();if(!clump)return;auto* root=RwFrameCreate();RpClumpSetFrame(clump,root);bd=new CAnimBlendClumpData;RpAnimBlendClumpGetData(clump)=bd;bd->SetNumberOfBones(2);
            for(unsigned f=0;f<2;++f){auto* frame=RwFrameCreate();RwFrameAddChild(root,frame);std::memset(&bd->m_FrameDatas[f],0,sizeof(AnimBlendFrameData));bd->m_FrameDatas[f].Frame=frame;bd->m_FrameDatas[f].BoneTag=uint32(-1);bd->m_FrameDatas[f].FramePos=CVector{.1f*float(f),.2f,.3f};}
        }
        bd->m_PedPosition=&velocity;
        for(unsigned f=0;f<bd->m_NumFrameData;++f){auto& fd=bd->m_FrameDatas[f];fd.Flags=0;fd.IsCompressed=compressed;fd.HasVelocity=f==0&&extract;fd.HasZVelocity=extract==2;fd.NeedsKeyFrameUpdate=f==0&&visible==2;if(mode==7){fd.KeyFramesIgnoreNodeOrientation=true;fd.KeyFramesIgnoreNodeTranslation=true;}}
        if(mode==8)return;
        const int count=mode==0?1:mode==6?13:3;
        for(int i=0;i<count;++i){
            auto h=std::make_unique<CAnimBlendHierarchy>();h->m_fTotalTime=1.f;h->m_bIsCompressed=compressed;h->m_bKeepCompressed=compressed;
            if(mode!=7||i!=0){h->m_nSeqCount=uint16(bd->m_NumFrameData);h->m_pSequences=new CAnimBlendSequence[h->m_nSeqCount];
                for(unsigned f=0;f<bd->m_NumFrameData;++f){auto& seq=h->m_pSequences[f];seq.SetNumFrames(3,true,compressed,nullptr);
                    for(int k=0;k<3;++k){const CQuaternion q{0.f,0.f,std::sin(float(k)*.1f),std::cos(float(k)*.1f)};const auto pos=bd->m_FrameDatas[f].BonePos+CVector{float(k)*.05f,float(k)*.1f,float(k)*.02f};
                        if(compressed){auto* key=seq.GetCKeyFrame(k);key->Rot=q;key->Trans=pos;key->DeltaTime=k?.5f:0.f;}
                        else{auto* key=seq.GetUKeyFrame(k);key->Rot=q;key->Trans=pos;key->DeltaTime=k?.5f:0.f;}
                    }
                }
            }
            auto* a=new CAnimBlendAssociation;a->AllocateAnimBlendNodeArray(bd->m_NumFrameData);a->m_NumBlendNodes=uint16(bd->m_NumFrameData);a->m_BlendHier=h.get();a->m_AnimId=(AnimationId)i;a->m_BlendAmount=1.f/float(count);a->m_Speed=.7f+float(i)*.13f;a->m_CurrentTime=mode==5?.99f:.15f;
            a->m_Flags=ANIMATION_IS_PLAYING|ANIMATION_IS_LOOPED|ANIMATION_CAN_EXTRACT_VELOCITY|ANIMATION_CAN_EXTRACT_X_VELOCITY;
            if(mode!=1&&i%2==0)a->m_Flags|=ANIMATION_IS_SYNCRONISED;
            if(mode==2&&i==1)a->m_Flags|=ANIMATION_IS_PARTIAL;
            if(mode==3)a->m_Flags&=~ANIMATION_IS_PLAYING;
            if(mode==4&&i==0){a->m_BlendAmount=.001f;a->m_BlendDelta=-1.f;a->m_Flags|=ANIMATION_IS_BLEND_AUTO_REMOVE;a->SetDeleteCallback(Callback,this);}
            if(mode==5){a->m_Flags&=~ANIMATION_IS_LOOPED;a->m_Flags|=ANIMATION_IS_FINISH_AUTO_REMOVE;a->SetFinishCallback(Callback,this);}
            for(unsigned f=0;f<bd->m_NumFrameData;++f){auto& n=a->m_BlendNodes[f];n.Init();n.m_BlendAssoc=a;n.m_Seq=h->m_pSequences?&h->m_pSequences[f]:nullptr;n.m_KFPrev=mode==5?1:0;n.m_KFCurr=mode==5?2:1;n.m_KFRemainingTime=mode==5?.01f:.35f;if(n.m_Seq){if(compressed)n.CalcDeltasCompressed();else n.CalcDeltas();}}
            bd->m_AnimList.Prepend(&a->m_Link);hierarchies.push_back(std::move(h));
        }
    }
    ~Fixture(){if(clump)RpClumpDestroy(clump);if(ownedAnimation)RtAnimAnimationDestroy(ownedAnimation);}
    json Snapshot(){json numbers=json::array(),exact=json::array();auto vec=[&](const RwV3d& v){numbers.push_back(v.x);numbers.push_back(v.y);numbers.push_back(v.z);};vec(velocity);
        for(unsigned f=0;f<bd->m_NumFrameData;++f){auto& fd=bd->m_FrameDatas[f];exact.push_back(fd.Flags);if(skin){auto* key=fd.KeyFrame;vec(key->q.imag);numbers.push_back(key->q.real);vec(key->t);}else{auto* m=RwFrameGetMatrix(fd.Frame);vec(m->right);vec(m->up);vec(m->at);vec(m->pos);}}
        for(auto* link=bd->m_AnimList.next;link;link=link->next){auto* a=CAnimBlendAssociation::FromLink(link);exact.push_back({a->GetAnimId(),a->m_Flags,(int)a->m_nCallbackType});for(float v:{a->m_BlendAmount,a->m_BlendDelta,a->m_CurrentTime,a->m_Speed,a->m_TimeStep})numbers.push_back(v);for(auto& n:a->GetNodes()){exact.push_back({n.m_KFCurr,n.m_KFPrev});for(float v:{n.m_Theta,n.m_InvSinTheta,n.m_KFRemainingTime})numbers.push_back(v);}}
        return {{"numbers",numbers},{"exact",exact},{"callbacks",callbacks},{"global_matches",StaticRef<CAnimBlendClumpData*>(0xB4EA0C)==bd}};
    }
};
inline bool Pair(RpClump* source,int id,bool skin,bool compressed,int visible,int mode,int extract,float step){
    Write({{"event","case_started"},{"pair",id},{"skin",skin},{"compressed",compressed},{"visible",visible},{"mode",mode},{"extract",extract},{"step",step}});
    Fixture a(source,skin,compressed,visible,mode,extract),b(source,skin,compressed,visible,mode,extract);
    if(!a.bd||!b.bd){Write({{"event","error"},{"reason","clump creation failed"}});return false;}
    auto*& global=StaticRef<CAnimBlendClumpData*>(0xB4EA0C);auto* saved=global;global=nullptr;
    bool independent=true;
    for(unsigned f=0;f<a.bd->m_NumFrameData;++f){
        if(skin){auto* originalFrames=RpAnimBlendClumpGetData(source)->m_FrameDatas;independent&=a.bd->m_FrameDatas[f].KeyFrame!=b.bd->m_FrameDatas[f].KeyFrame&&a.bd->m_FrameDatas[f].KeyFrame!=originalFrames[f].KeyFrame&&b.bd->m_FrameDatas[f].KeyFrame!=originalFrames[f].KeyFrame;}
        else independent&=a.bd->m_FrameDatas[f].Frame!=b.bd->m_FrameDatas[f].Frame;
    }
    if(!independent||a.Snapshot()!=b.Snapshot()){Write({{"event","error"},{"reason","different inputs or shared pose storage"},{"pair",id},{"independent",independent}});global=saved;return false;}
    const auto before=RpAnimBlendProbe::calls;
    if(!Select(false)){Write({{"event","error"},{"reason","original hook"}});global=saved;return false;}
    auto entry=reinterpret_cast<void(__cdecl*)(RpClump*,float,bool)>(0x4D34F0);entry(a.clump,step,visible!=0);const auto original=a.Snapshot();const auto middle=RpAnimBlendProbe::calls;
    global=nullptr;if(!Select(true)){Write({{"event","error"},{"reason","candidate hook"}});global=saved;return false;}entry(b.clump,step,visible!=0);const auto candidate=b.Snapshot();const auto after=RpAnimBlendProbe::calls;global=saved;
    bool equal=original["exact"]==candidate["exact"]&&original["callbacks"]==candidate["callbacks"]&&original["global_matches"]==true&&candidate["global_matches"]==true&&original["numbers"].size()==candidate["numbers"].size()&&middle==before&&after==middle+1;
    double error{};if(equal)for(size_t i=0;i<original["numbers"].size();++i){if(!original["numbers"][i].is_number()||!candidate["numbers"][i].is_number()){equal=false;break;}double x=original["numbers"][i],y=candidate["numbers"][i];equal&=std::isfinite(x)&&std::isfinite(y);error=std::max(error,std::abs(x-y));}equal&=error<=.00001;
    Write({{"event","comparison"},{"pair",id},{"skin",skin},{"compressed",compressed},{"visible",visible},{"mode",mode},{"extract",extract},{"step",step},{"equal",equal},{"max_error",error},{"original",original},{"candidate",candidate},{"original_candidate_calls",middle-before},{"candidate_calls",after-middle},{"independent_inputs",true}});return equal;
}
inline std::vector<uint8> PlayerPose(RpClump* clump){
    std::vector<uint8> bytes;auto* data=RpAnimBlendClumpGetData(clump);
    for(unsigned f=0;f<data->m_NumFrameData;++f){auto* begin=reinterpret_cast<const uint8*>(data->m_FrameDatas[f].KeyFrame);bytes.insert(bytes.end(),begin,begin+sizeof(RpHAnimBlendInterpFrame));}
    return bytes;
}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"rpanimblend-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"rpanimblend-runtime.jsonl",std::ios::trunc);_CrtSetReportHook(Report);Write({{"event","waiting_for_player"}});}
    auto* p=FindPlayerPed();if(!p||!p->GetRpClump()||CCutsceneMgr::IsRunning())return;done=true;Write({{"event","player_ready"}});auto* source=p->GetRpClump();const auto playerBefore=PlayerPose(source);int pair{};bool passed=true;
    for(int skin=0;skin<2&&passed;++skin)for(int compressed=0;compressed<2&&passed;++compressed){passed=Pair(source,pair++,skin,compressed,1,8,0,.016f);}
    for(int skin=0;skin<2&&passed;++skin)for(int compressed=0;compressed<2&&passed;++compressed)for(int extract=0;extract<3&&passed;++extract){if(compressed&&extract==1)continue;for(int visible=0;visible<3&&passed;++visible)for(int mode=0;mode<8&&passed;++mode)for(float step:{0.f,.016f,.2f}){if(!passed)break;if(!skin&&extract&&visible==0)continue;passed=Pair(source,pair++,skin,compressed,visible,mode,extract,step);}}
    if(PlayerPose(source)!=playerBefore){Write({{"event","error"},{"reason","player pose changed"}});passed=false;}
    Select(true);Write(passed?json{{"event","complete"},{"pairs",pair},{"state_restored",true},{"player_pose_unchanged",true}}:json{{"event","error"},{"reason","comparison failed"}});
}
}
