#pragma once

#include "../PageParent.h"
#include "../../UI/Button/UIButton.h"
#include "../../UI/ItemList/UIItemList.h"

class Inventory :public PageParent {
	std::vector<ButtonParts> m_Button;
	bool m_IsEndPage = false;
	ItemDrawList m_ItemDrawList;
public:
	Inventory(void) noexcept {}
	Inventory(const Inventory&) {}
	Inventory(Inventory&&) noexcept {}
	Inventory& operator=(const Inventory&) {}
	Inventory& operator=(Inventory&&) noexcept {}
	virtual ~Inventory(void) noexcept {}
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEndPage() override { return m_IsEndPage; }
	void Dispose()  override;
};