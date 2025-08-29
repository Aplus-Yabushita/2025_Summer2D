#include "UIItemList.h"

#include "../../Direct2DLib.h"

#include "../../Page/PageManager.h"

#include "../../Font/FontPool.h"

#include "../../Input/Input.h"

#include "../../DataBase/ItemImage/ItemImagePool.h"
#include "../../DataBase/ItemData/ItemDataPool.h"

#include "../../Algorithm.h"

void ItemDrawList::Init(float XMin, float YMin, float XMax, float YMax, int CellSize, const CharacterData* pCharacterData) {
	cellSize = CellSize;
	cellSizeTotal = cellSize + 2;
	XMinPos = XMin;
	YMinPos = YMin;

	xcell = static_cast<int>(static_cast<int>(XMax - XMin) - 24) / cellSizeTotal;
	ycell = static_cast<int>(static_cast<int>(YMax - YMin) + cellSizeTotal) / cellSizeTotal;

	ButtonParam buttonParam;
	buttonParam.xmin = static_cast<int>(static_cast<int>(XMinPos) + xcell * cellSizeTotal + 4);
	buttonParam.ymin = static_cast<int>(YMinPos);
	buttonParam.xmax = static_cast<int>(static_cast<int>(XMinPos) + xcell * cellSizeTotal + 24);
	buttonParam.ymax = static_cast<int>(static_cast<int>(YMinPos) + ycell * cellSizeTotal - cellSizeTotal);
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.7f, 0.7f);
	m_ScrollBar.Init(buttonParam);

	m_pCharacterData = const_cast<CharacterData*>(pCharacterData);
}

void ItemDrawList::Update() {
}

void ItemDrawList::Draw() {
	m_OnMouseOffset = -1;
	bool Trigger = Input::Instance()->GetKeyTrigger(VK_LBUTTON);
	bool Press = Input::Instance()->GetKeyPress(VK_LBUTTON);

	//はんぶんこする
	bool CtrlPress = m_DragEnable && Input::Instance()->GetKeyPress(VK_CONTROL);
	if (m_DragEnable && OnPress && pDragItem) {
		//持っているアイテムが1個だけな場合
		if (pDragItem->Count == 1) {
			CtrlPress = false;
		}
		//置く場所に持っているアイテムと別種のアイテムがある
		for (int y = 0; y < ycell; ++y) {
			int NowY = (static_cast<int>(m_ScrollPer) + y);
			float yMin = -m_ScrollPer * static_cast<float>(cellSizeTotal) + static_cast<float>(NowY * cellSizeTotal);
			float yMax = yMin + static_cast<float>(cellSize);
			for (int x = 0; x < xcell; ++x) {
				size_t Offset = static_cast<size_t>(NowY * xcell + x);
				float xMin = static_cast<float>(x) * static_cast<float>(cellSizeTotal);
				float xMax = xMin + static_cast<float>(cellSize);
				//
				bool OnMouse = HitPointToRectangle(
					static_cast<int>(Input::Instance()->GetMousePosition().x), static_cast<int>(Input::Instance()->GetMousePosition().y),
					static_cast<int>(XMinPos + xMin), static_cast<int>(YMinPos + yMin), static_cast<int>(XMinPos + xMax), static_cast<int>(YMinPos + yMax)
				);
				if (OnMouse) {
					CellItem* pItem = const_cast<CellItem*>(m_pCharacterData->GetCellItem(Offset));
					if (pItem) {
						if (pItem->ID != pDragItem->ID) {
							CtrlPress = false;
						}
					}
				}
			}
		}
	}

	size_t offsetMax = 0;
	for (auto& l : m_pCharacterData->GetCellItemList()) {
		if (offsetMax <= l.offset) {
			offsetMax = l.offset;
		}
	}

	//スクロールバー
	m_ScrollBar.Update(ycell, static_cast<int>(offsetMax) / xcell + 1 + (((static_cast<int>(offsetMax) % xcell) != 0) ? 1 : 0), &m_ScrollPer);

	for (int y = 0; y < ycell; ++y) {
		int NowY = (static_cast<int>(m_ScrollPer) + y);
		float yMin = -m_ScrollPer * static_cast<float>(cellSizeTotal) + static_cast<float>(NowY * cellSizeTotal);
		float yMax = yMin + static_cast<float>(cellSize);

		float alpha = 1.0f;
		if (y == 0) {
			alpha = yMax / static_cast<float>(cellSizeTotal);
		}
		else if (y == ycell - 1) {
			alpha = (static_cast<float>(ycell * cellSizeTotal) - yMax) / static_cast<float>(cellSizeTotal);
		}
		for (int x = 0; x < xcell; ++x) {
			size_t Offset = static_cast<size_t>(NowY * xcell + x);
			float xMin = static_cast<float>(x) * static_cast<float>(cellSizeTotal);
			float xMax = xMin + static_cast<float>(cellSize);
			//
			bool OnMouse = HitPointToRectangle(
				static_cast<int>(Input::Instance()->GetMousePosition().x), static_cast<int>(Input::Instance()->GetMousePosition().y),
				static_cast<int>(XMinPos + xMin), static_cast<int>(YMinPos + yMin), static_cast<int>(XMinPos + xMax), static_cast<int>(YMinPos + yMax)
			);
			if (m_DragEnable) {
				if (!OnPress) {
					if (OnMouse && Trigger) {
						pDragItem = m_pCharacterData->GetCellItem(Offset);
						if (pDragItem) {
							OnPress = true;
						}
					}
				}
				else {
					if (OnMouse && !Press) {
						OnPress = false;
						//分割処理
						if (CtrlPress) {
							size_t Div = pDragItem->Count / 2;
							m_pCharacterData->SubItem(pDragItem->offset, Div);
							m_pCharacterData->AddItem(Offset, pDragItem->ID, Div);
						}
						else {
							CellItem* pItem1 = const_cast<CellItem*>(m_pCharacterData->GetCellItem(Offset));
							CellItem* pItem2 = const_cast<CellItem*>(pDragItem);
							//同じアイテムならまとめる
							if (pItem1 && pItem1->ID == pItem2->ID) {
								int StockMax = ItemDataPool::Instance()->GetByID(pItem1->ID)->GetStock();
								int addto1 = Mathf::Min(StockMax - static_cast<int>(pItem1->Count), static_cast<int>(pItem2->Count));
								m_pCharacterData->SubItem(pItem1->offset, static_cast<size_t>(addto1));
								m_pCharacterData->AddItem(pItem2->offset, pItem2->ID, static_cast<size_t>(addto1));
							}
							//違うアイテム、もしくはアイテムがないならスワップ
							else {
								m_pCharacterData->SwapItem(Offset, pDragItem->offset);
							}
						}
						pDragItem = nullptr;
					}
				}
			}

			D2D1::ColorF BGColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, alpha);
			//選択時
			if (OnMouse) {
				m_OnMouseOffset = static_cast<int>(Offset);
				if (CtrlPress) {
					BGColor = D2D1::ColorF(0.8f, 0.8f, 0.0f, alpha);
				}
				else {
					BGColor = D2D1::ColorF(0.0f, 0.8f, 0.0f, alpha);
				}
				if (!m_DragEnable) {
					if (Press) {
						BGColor = D2D1::ColorF(0.0f, 1.0f, 0.0f, alpha);
					}
				}
				//
				if (!OnPress) {
					HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
					SetCursor(hCursor);
				}
			}
			//押下時
			if (OnPress) {
				HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEALL);
				SetCursor(hCursor);
			}
			//アイテム枠描画
			Direct2DLib::Instance()->GetDrawSystem()->SetRect(XMinPos + xMin, YMinPos + yMin, XMinPos + xMax, YMinPos + yMax, BGColor);
			//アイテム描画
			if (Offset > offsetMax) { continue; }
			CellItem* pItem = const_cast<CellItem*>(m_pCharacterData->GetCellItem(Offset));
			if (!pItem) { continue; }
			Direct2DLib::Instance()->GetDrawSystem()->SetExtendBMP(XMinPos + xMin + 5, YMinPos + yMin + 5, XMinPos + xMax - 5, YMinPos + yMax - 5,
				alpha, *ItemImagePool::Instance()->GetImage(ItemDataPool::Instance()->GetByID(pItem->ID)->GetImagePath()));
			Direct2DLib::Instance()->GetDrawSystem()->SetFont(XMinPos + xMin, YMinPos + yMin, XMinPos + xMax, YMinPos + yMax,
				StringToWString(std::to_string(pItem->Count)).c_str(),
				D2D1::ColorF(D2D1::ColorF::LightGreen, alpha), FontPool::Instance()->GetFont(12, FontCenterX::Right, FontCenterY::Bottom).get());
		}
	}
	//ドラッグ中のアイテム
	if (OnPress && pDragItem) {
		D2D1::ColorF BGColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
		if (CtrlPress) {
			BGColor = D2D1::ColorF(0.8f, 0.8f, 0.0f);
		}
		else {
			BGColor = D2D1::ColorF(0.0f, 0.8f, 0.0f);
		}

		Direct2DLib::Instance()->GetDrawSystem()->SetRect(
			static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().x) - cellSize / 2), static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().y) - cellSize / 2),
			static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().x) + cellSize / 2), static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().y) + cellSize / 2),
			BGColor
		);
		Direct2DLib::Instance()->GetDrawSystem()->SetExtendBMP(
			static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().x) - cellSize / 2), static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().y) - cellSize / 2),
			static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().x) + cellSize / 2), static_cast<float>(static_cast<int>(Input::Instance()->GetMousePosition().y) + cellSize / 2),
			1.0f, *ItemImagePool::Instance()->GetImage(ItemDataPool::Instance()->GetByID(pDragItem->ID)->GetImagePath()));
	}
	//画面外で離した
	if (OnPress && !Press) {
		OnPress = false;
	}
}
