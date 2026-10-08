from pathlib import Path
import re
p=Path('source/game_sa/GenericGameStorage.cpp');s=p.read_text(encoding='utf8')
start=s.index('    assert(slot < MAX_SAVEGAME_SLOTS);',s.index('void CGenericGameStorage::MakeValidSaveName'));end=s.index('\n}',start)
s=s[:start]+'''    char path[MAX_PATH];
    path[0] = '\\0';
    std::sprintf(path, "%s%i", C_PcSave::DefaultPCSaveFileName, slot + 1);
    if (std::strlen(path) >= 257) path[257] = '\\0';
    std::strcat(path, ".b");
    std::strcpy(ms_SaveFileName, path);
    for (auto* ch = ms_SaveFileName; *ch && *ch != '\\n'; ++ch) {
        if (*ch == '?') *ch = ' ';
    }'''+s[end:]
s=s.replace('strcpy_s(ms_LoadFileName, fileName);','std::strcpy(ms_LoadFileName, fileName);').replace('s_PcSaveHelper.GenerateGameFilename(slot, ms_LoadFileNameWithPath);','std::sprintf(ms_LoadFileNameWithPath, "%s%i%s", C_PcSave::DefaultPCSaveFileName, slot + 1, ".b");')
s=s.replace('    return CGenericGameStorage::ms_SlotSaveDate[slot];','    return CGenericGameStorage::ms_SlotSaveDate.data()[slot];')
for name in ['GetCurrentVersionNumber','MakeValidSaveName','CloseFile','OpenFileForWriting','OpenFileForReading','GetSavedGameDateAndTime','InitRadioStationPositionList','RestoreForStartLoad']:
 s=re.sub(r'(RH_Scoped\w*Install\('+name+r', 0x[0-9A-F]+), \{ \.reversed = false \}',r'\1',s)
p.write_text(s,encoding='utf8')
