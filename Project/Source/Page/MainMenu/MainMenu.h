#pragma once

#include "../PageParent.h"
#include "../../UI/Button/UIButton.h"
#include "../../Network/Net.hpp"

struct PlayerMoveData {
	Algorithm::InputControl				m_Input{};
	Algorithm::MoveInfo					m_move{};
	int32_t						m_FreeData[10]{};
};

class MainMenu :public PageParent {
	std::array<PlayerMoveData, 2> Player;
public:
	MainMenu(void) noexcept {}
	MainMenu(const MainMenu&) {}
	MainMenu(MainMenu&&) noexcept {}
	MainMenu& operator=(const MainMenu&) {}
	MainMenu& operator=(MainMenu&&) noexcept {}
	virtual ~MainMenu(void) noexcept {}
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEndPage() override { return false; }
	void Dispose()  override;
};
