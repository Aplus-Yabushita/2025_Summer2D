#pragma once

#include "../Button/UIButton.h"
#include "../ScrollBar/UIScrollBar.h"

#include "../../CharacterData/PlayerData.h"

class ItemDrawList {
	ScrollBarParts m_ScrollBar;
	float m_ScrollPer = 0.0f;
	//表示パラメーター
	int cellSize = 48;
	int cellSizeTotal = cellSize + 2;
	float XMinPos = 4.0f;
	float YMinPos = 100.0f;
	int xcell = 7;
	int ycell = 8;
	CharacterData* m_pCharacterData{};
	bool OnPress = false;
	const CellItem* pDragItem{};
	bool m_DragEnable = true;
	int m_OnMouseOffset = -1;
public:
	bool HasOnMouseOffset() const { return m_OnMouseOffset != -1; }
	int GetOnMouseOffset() const { return m_OnMouseOffset; }
	void SetDragEnable(bool value) { m_DragEnable = value; }
public:
	void Init(float XMin, float YMin, float XMax, float YMax, int CellSize, const CharacterData* pCharacterData);
	void Update();
	void Draw();
};
