#pragma once

#include "../PageParent.h"
#include "../../UI/Button/UIButton.h"
#include "../../UI/ItemList/UIItemList.h"
#include "../../UI/ItemList/UIItemColumn.h"

enum class ShopMode {
	Buy,
	Sell,
};

class Shop :public PageParent {
	std::vector<ButtonParts> m_Button;
	bool m_IsEndPage = false;
	ItemDrawList m_ItemDrawList;
	ItemDrawList m_ItemSellList;
	ItemBuyColumn m_ItemBuyColumn;
	CharacterData SellData;
	BuyItemData BuyData;
	ShopMode m_ShopMode{};
public:
	Shop(void) noexcept {}
	Shop(const Shop&) {}
	Shop(Shop&&) noexcept {}
	Shop& operator=(const Shop&) {}
	Shop& operator=(Shop&&) noexcept {}
	virtual ~Shop(void) noexcept {}
private:
	void SetBuyMode();
	void SetSellMode();
	void Sell();
	void Buy();
	void Reset();
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEndPage() override { return m_IsEndPage; }
	void Dispose()  override;
};