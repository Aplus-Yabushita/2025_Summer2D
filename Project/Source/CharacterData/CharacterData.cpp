#include "CharacterData.h"

#include "../DataBase/ItemData/ItemDataPool.h"
#include "../Algorithm.h"

void CharacterData::SubItem(size_t Offset, size_t Count) {
	CellItem* SubItem = const_cast<CellItem*>(GetCellItem(Offset));
	if (SubItem) {
		SubItem->Count -= Count;
		if (SubItem->Count <= 0) {
			m_List.Sub([&](const CellItem& A) { return Offset == A.offset; });
		}
	}
}

size_t CharacterData::GetItemTotalCount(size_t ID) const {
	size_t Total = 0;
	for (const auto& c : GetCellItemList()) {
		if (c.ID == ID) {
			Total += c.Count;
		}
	}
	return Total;
}

void CharacterData::SubAnyItem(size_t ID, size_t Count) {
	while (true) {
		const CellItem* pFind = nullptr;
		size_t OffsetBuffer = 0;
		for (const auto& c : GetCellItemList()) {
			if (c.ID == ID) {
				//一番オフセットの高いものをさぐる
				if (c.offset > OffsetBuffer) {
					OffsetBuffer = c.offset;
					pFind = &c;
				}
			}
		}
		if (pFind) {
			size_t UseCount = Mathf::Min(pFind->Count, Count);
			SubItem(pFind->offset, UseCount);
			Count -= UseCount;
			if (Count == 0) {
				break;
			}
		}
		else {
			break;
		}
	}
}

void CharacterData::AddItem(size_t offset, size_t ID, size_t Count) {
	auto AddItemLocal = [this](CellItem* pItem, size_t offset, size_t ID, size_t Count){
		size_t StockMax = static_cast<size_t>(ItemDataPool::Instance()->GetByID(ID)->GetStock());
		pItem->offset = offset;
		pItem->ID = ID;
		pItem->Count += Count;
		if (pItem->Count > StockMax) {
			auto More = pItem->Count - StockMax;
			pItem->Count = StockMax;
			AddItem(offset + 1, ID, More);
		}
	};

	while (true) {
		CellItem* pItem = const_cast<CellItem*>(GetCellItem(offset));
		if (pItem) {
			if (pItem->ID == ID) {
				//このマスならアイテムを追加で置けるぞ
				AddItemLocal(pItem, offset, ID, Count);
				return;
			}
			else {
				//このマスはダメだ、つぎをあたろう
				++offset;
			}
		}
		else {
			//何もないマスまできたぞ
			break;
		}
	}
	CellItem* pItem = m_List.AddBack();
	AddItemLocal(pItem, offset, ID, Count);
}

void CharacterData::SwapItem(size_t offset1, size_t offset2) {
	CellItem* pItem1 = const_cast<CellItem*>(GetCellItem(offset1));
	CellItem* pItem2 = const_cast<CellItem*>(GetCellItem(offset2));
	if (pItem1) {
		pItem1->offset = offset2;
	}
	if (pItem2) {
		pItem2->offset = offset1;
	}
}
