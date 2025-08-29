#pragma once

#include "../../Util.h"
#include "../../Algorithm.h"

#include "ItemData.h"

#include <memory>
//
class ItemDataPool : public SingletonBase<ItemDataPool> {
private:
	friend class SingletonBase<ItemDataPool>;
private:
	List<std::unique_ptr<ItemData>> m_List;
private:
	ItemDataPool();
	ItemDataPool(const ItemDataPool&) = delete;
	ItemDataPool(ItemDataPool&&) = delete;
	ItemDataPool& operator=(const ItemDataPool&) = delete;
	ItemDataPool& operator=(ItemDataPool&&) = delete;
	virtual ~ItemDataPool() {
		m_List.Clear();
	}
public:
	size_t Find(std::wstring_view Name) const;
	const std::unique_ptr<ItemData>& GetByID(size_t ID) const;
};
