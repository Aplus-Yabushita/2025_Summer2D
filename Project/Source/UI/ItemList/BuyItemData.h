#pragma once

#include <vector>

#include "../../CharacterData/CharacterData.h"

struct BuyItem {
	size_t ID{};
	size_t Count{};
};
class BuyItemData {
private:
	std::vector<BuyItem> m_BuyItem;
public:
	BuyItemData() {
		m_BuyItem.clear();
	}
public:
	const std::vector<BuyItem>& GetBuyItemList() const { return m_BuyItem; }
	const BuyItem* GetBuyItem(size_t Num) const {
		return &m_BuyItem.at(Num);
	}
public:
	//アイテムを削除
	void Reset() {
		m_BuyItem.clear();
	}
	//アイテムを追加
	void AddItem(size_t ID) {
		m_BuyItem.emplace_back();
		m_BuyItem.back().ID = ID;
		m_BuyItem.back().Count = 0;
	}
	//アイテムをカートに入れる
	void AddCart(size_t Num, int Count);
	void SubCart(size_t Num, int Count);
	//カートのアイテムを購入
	void BuyCart(CharacterData* pTargetCharacter);
};
