#include "BuyItemData.h"

#include "../../DataBase/ItemData/ItemDataPool.h"

#include "../../Algorithm.h"

void BuyItemData::AddCart(size_t Num, int Count) {
	m_BuyItem.at(Num).Count = Mathf::Max<size_t>(m_BuyItem.at(Num).Count + Count, 0);
}
void BuyItemData::SubCart(size_t Num, int Count) {
	m_BuyItem.at(Num).Count = Mathf::Max<size_t>(m_BuyItem.at(Num).Count - Count, 0);
}

void BuyItemData::BuyCart(CharacterData* pTargetCharacter) {
	size_t TotalMoney = pTargetCharacter->GetItemTotalCount(ItemDataPool::Instance()->Find(L"Yellar"));
	for (auto& b : m_BuyItem) {
		if (b.Count == 0) { continue; }
		auto& pItemData = ItemDataPool::Instance()->GetByID(b.ID);
		//”ƒ‚¦‚é—Ê‚¾‚¯”ƒ‚¤
		int CanBuyCount = static_cast<int>(TotalMoney) / pItemData->GetSellValue();
		if (CanBuyCount == 0) { continue; }
		b.Count = Mathf::Min(b.Count, static_cast<size_t>(CanBuyCount));
		//”ƒ‚Á‚½•ª‚¨‹à‚ðˆø‚­
		size_t TotalValue = static_cast<size_t>(pItemData->GetSellValue()) * b.Count;
		pTargetCharacter->SubAnyItem(ItemDataPool::Instance()->Find(L"Yellar"), TotalValue);
		TotalMoney -= TotalValue;

		pTargetCharacter->AddItem(0, b.ID, b.Count);
		b.Count = 0;
	}
}
