#include "ItemImagePool.h"

#include <vector>

const ItemImagePool* SingletonBase<ItemImagePool>::m_Singleton = nullptr;

ItemImagePool::ItemImagePool() {
	std::vector<WIN32_FIND_DATA> FileList;
	GetFileNamesInDirectory("Data/Item/icon/*", &FileList);
	for (const auto& d : FileList) {
		std::string p = d.cFileName;
		if (p.find(".png") != std::string::npos) {
			auto* pPtr = m_List.AddBack();
			pPtr->first = L"";
			pPtr->second = std::make_unique<Direct2DBitMap>();
		}
	}
	size_t count = 0;
	for (const auto& d : FileList) {
		std::string p = d.cFileName;
		if (p.find(".png") != std::string::npos) {
			m_List.m_item.at(count).first = StringToWString(p);
			m_List.m_item.at(count).second->Load(StringToWString("Data/Item/icon/" + p).c_str());
			++count;
		}
	}
	FileList.clear();
}
