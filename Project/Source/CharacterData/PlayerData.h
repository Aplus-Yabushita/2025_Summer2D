#pragma once

#include "../Util.h"
#include "../Algorithm.h"

#include "CharacterData.h"

#include <memory>

struct SaveParam {
	std::string first;
	int64_t second;
};

class PlayerData : public SingletonBase<PlayerData> {
private:
	friend class SingletonBase<PlayerData>;
private:
	std::unique_ptr<CharacterData> m_MyCharacterData;
	List<SaveParam> m_List;
private:
	PlayerData() {
		m_MyCharacterData = std::make_unique<CharacterData>();
	}
	PlayerData(const PlayerData&) = delete;
	PlayerData(PlayerData&&) = delete;
	PlayerData& operator=(const PlayerData&) = delete;
	PlayerData& operator=(PlayerData&&) = delete;
	virtual ~PlayerData() {
		m_MyCharacterData.reset();
		m_List.Clear();
	}
public:
	const std::unique_ptr<CharacterData>& GetMyCharacterData() { return m_MyCharacterData; }
public:
	bool Load();
	void Save();
};
