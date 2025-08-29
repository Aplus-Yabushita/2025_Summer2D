#pragma once

#include "../PageParent.h"
#include "../../UI/Button/UIButton.h"

class MainMenu :public PageParent {
	std::vector<ButtonParts> m_Button;
	bool m_IsEndPage = false;
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
	bool IsEndPage() override { return m_IsEndPage; }
	void Dispose()  override;
};
