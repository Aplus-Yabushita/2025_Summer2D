#pragma once

#include "../Util.h"
#include "../Algorithm.h"
//
struct CellItem {
	size_t offset{};
	size_t ID{};
	size_t Count{};
public:
	CellItem() {
		Count = 0;
	}
};
class CharacterData {
	List<CellItem> m_List;
public:
	CharacterData() {
		m_List.Clear();
	}
public:
	const auto& GetCellItemList() const { return m_List.m_item; }
	const CellItem* GetCellItem(size_t Offset) const { return m_List.Get([&](const CellItem& A) { return Offset == A.offset; }); }
public:
	//アイテムを削除
	void SubItem(size_t Offset, size_t Count);
	//そのアイテムの総所持数を得る
	size_t GetItemTotalCount(size_t ID) const;
	//オフセットの高い順から自動でアイテムを削除する
	void SubAnyItem(size_t ID, size_t Count);
	//アイテムをその場に置く(置けない場合はより後のオフセットの場所に置く)
	void AddItem(size_t offset, size_t ID, size_t Count);
	//offset1とoffset2を入れ替える(同じアイテムならoffset1側に最大値分ける)
	void SwapItem(size_t offset1, size_t offset2);
};
