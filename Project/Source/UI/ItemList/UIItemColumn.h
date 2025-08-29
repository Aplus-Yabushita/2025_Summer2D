#pragma once

#include "../Button/UIButton.h"
#include "../ScrollBar/UIScrollBar.h"

#include "../../CharacterData/PlayerData.h"

#include "BuyItemData.h"

class ItemBuyColumn {
	ScrollBarParts m_ScrollBar;
	float m_ScrollPer = 0.0f;
	//表示パラメーター
	int cellSize = 48;
	int cellSizeTotal = cellSize + 2;
	float XMinPos = 4.0f;
	float YMinPos = 100.0f;
	float XMaxPos = 4.0f;
	float YMaxPos = 100.0f;
	int ycell = 8;
	BuyItemData* m_pBuyItemData{};
	int m_OnMouseOffset = -1;
public:
	bool HasOnMouseOffset() const { return m_OnMouseOffset != -1; }
	int GetOnMouseOffset() const { return m_OnMouseOffset; }
public:
	void Init(float XMin, float YMin, float XMax, float YMax, int CellSize, const BuyItemData* pBuyItemData);
	void Update();
	void Draw();
};
