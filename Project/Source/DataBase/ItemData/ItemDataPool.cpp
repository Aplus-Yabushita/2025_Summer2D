#include "ItemDataPool.h"

#include <algorithm>
#include <vector>

const ItemDataPool* SingletonBase<ItemDataPool>::m_Singleton = nullptr;

ItemDataPool::ItemDataPool() {
	std::vector<WIN32_FIND_DATA> FileList;
	GetFileNamesInDirectory("Data/Item/data/*", &FileList);
	std::sort(FileList.begin(), FileList.end(), [&](const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
		std::string pa = a.cFileName;
		std::string pb = b.cFileName;
		return pa < pb;
		});
	for (const auto& d : FileList) {
		std::string p = d.cFileName;
		if (p.find(".json") != std::string::npos) {
			auto* pPtr = m_List.AddBack();
			*pPtr = std::make_unique<ItemData>();
		}
	}
	size_t count = 0;
	for (const auto& d : FileList) {
		std::string p = d.cFileName;
		if (p.find(".json") != std::string::npos) {
			m_List.m_item.at(count)->Load(StringToWString("Data/Item/data/" + p).c_str(), count);
			++count;
		}
	}
	FileList.clear();
}

size_t ItemDataPool::Find(std::wstring_view Name)  const {
	const auto* pPtr = m_List.Get([&](const std::unique_ptr<ItemData>& A) { return Name == A->GetName(); });
	if (pPtr) {
		return (*pPtr)->GetID();
	}
	return SIZE_MAX;
}

const std::unique_ptr<ItemData>& ItemDataPool::GetByID(size_t ID)  const {
	const auto* pPtr = m_List.Get([&](const std::unique_ptr<ItemData>& A) { return ID == A->GetID(); });
	return (*pPtr);
}
