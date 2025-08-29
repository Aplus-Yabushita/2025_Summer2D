#include "PlayerData.h"

#include "../DataBase/ItemData/ItemDataPool.h"
#include "../FileSystem.h"

#include <vector>

const PlayerData* SingletonBase<PlayerData>::m_Singleton = nullptr;

bool PlayerData::Load() {
	m_List.Clear();
	InputFileSystem file;
	if (!file.Open("Save/new.svf")) {
		return false;
	}
	while (!file.IsEof()) {
		char Line[1024] = "";
		char* L = Line;
		file.GetLine(&L);
		std::string ALL = Line;
		if (ALL == "") {
			continue;
		}
		std::vector<std::string> Args;
		while (true) {
			auto Comma = ALL.find(',');
			if (Comma != std::string::npos) {
				Args.emplace_back(ALL.substr(0, Comma));
				ALL = ALL.substr(Comma + 1);
			}
			else {
				Args.emplace_back(ALL);
				break;
			}
		}
		if (Args.size() == 3) {
			m_MyCharacterData->AddItem(std::stoull(Args.at(0)), std::stoull(Args.at(1)), std::stoull(Args.at(2)));
		}
	}
	file.Close();
	return true;
}

void PlayerData::Save() {
	OutputFileSystem file;
	file.Open("Save/new.svf");
	for (const auto& c : m_MyCharacterData->GetCellItemList()) {
		std::string Line = std::to_string(c.offset) + "," + std::to_string(c.ID) + "," + std::to_string(c.Count);
		file.AddLine(Line.c_str());
	}
	file.Close();
}
