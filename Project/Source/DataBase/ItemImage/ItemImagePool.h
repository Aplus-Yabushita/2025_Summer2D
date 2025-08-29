#pragma once

#include "../../Util.h"
#include "../../Algorithm.h"

#include "../../Image/Bitmap.h"

#include <memory>
//
struct BMPPool {
	std::wstring first{};
	std::unique_ptr<Direct2DBitMap> second{};
public:
	BMPPool(void) noexcept {}
	BMPPool(const BMPPool&) {}
	BMPPool(BMPPool&&) noexcept {}
	BMPPool& operator=(const BMPPool&) {}
	BMPPool& operator=(BMPPool&&) noexcept {}
	virtual ~BMPPool(void) noexcept {}
};
class ItemImagePool : public SingletonBase<ItemImagePool> {
private:
	friend class SingletonBase<ItemImagePool>;
private:
	List<BMPPool> m_List;
private:
	ItemImagePool();
	ItemImagePool(const ItemImagePool&) = delete;
	ItemImagePool(ItemImagePool&&) = delete;
	ItemImagePool& operator=(const ItemImagePool&) = delete;
	ItemImagePool& operator=(ItemImagePool&&) = delete;
	virtual ~ItemImagePool() {
		m_List.Clear();
	}
public:
	const Direct2DBitMap* GetImage(std::wstring_view Name) {
		const auto* pPtr = m_List.Get([&](const BMPPool& A) { return Name == A.first; });
		if (pPtr) {
			return pPtr->second.get();
		}
		return nullptr;
	}
};
