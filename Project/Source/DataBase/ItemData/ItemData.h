#pragma once

#include "../../Util.h"
//
class ItemData {
	std::wstring Name;
	std::wstring Info;
	std::wstring ImagePath;
	int Stock{ 1 };
	int SellValue{ 1 };
	size_t m_ID{};
public:
	ItemData() {}
	virtual ~ItemData() {}
public:
	const std::wstring& GetName() const { return Name; }
	const std::wstring& GetImagePath() const { return ImagePath; }
	int GetStock() const { return Stock; }
	int GetSellValue() const { return SellValue; }
	size_t GetID() const { return m_ID; }
public:
	void Load(const WCHAR* szFileName, size_t ID);
};
