#include "ItemData.h"

#include "../../json.hpp"
#include <fstream>

void ItemData::Load(const WCHAR* szFileName, size_t ID) {
	std::ifstream File(szFileName);
	const auto& JsonData = nlohmann::json::parse(File);
	Name = StringToWString(JsonData["Name"], CP_UTF8);
	Info = StringToWString(JsonData["Info"], CP_UTF8);
	ImagePath = StringToWString(JsonData["ImagePath"], CP_UTF8);
	Stock = JsonData["StockCount"];
	SellValue = JsonData["SellValue"];
	File.close();

	m_ID = ID;
}
