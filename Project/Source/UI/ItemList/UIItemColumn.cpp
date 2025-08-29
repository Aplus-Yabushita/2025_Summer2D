#include "UIItemColumn.h"

#include "../../Direct2DLib.h"

#include "../../Page/PageManager.h"

#include "../../Font/FontPool.h"

#include "../../Input/Input.h"

#include "../../DataBase/ItemImage/ItemImagePool.h"
#include "../../DataBase/ItemData/ItemDataPool.h"

void ItemBuyColumn::Init(float XMin, float YMin, float XMax, float YMax, int CellSize, const BuyItemData* pBuyItemData) {
	cellSize = CellSize;
	cellSizeTotal = cellSize + 2;
	XMinPos = XMin;
	YMinPos = YMin;

	XMaxPos = XMax - 24;
	YMaxPos = YMax;

	ycell = static_cast<int>(static_cast<int>(YMax - YMin) + cellSizeTotal) / cellSizeTotal;

	ButtonParam buttonParam;
	buttonParam.xmin = static_cast<int>(XMax - 24 + 4);
	buttonParam.ymin = static_cast<int>(YMinPos);
	buttonParam.xmax = static_cast<int>(XMax - 24 + 24);
	buttonParam.ymax = static_cast<int>(static_cast<int>(YMinPos) + ycell * cellSizeTotal - cellSizeTotal);
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.7f, 0.7f);
	m_ScrollBar.Init(buttonParam);

	m_pBuyItemData = const_cast<BuyItemData*>(pBuyItemData);
}

void ItemBuyColumn::Update() {
}

void ItemBuyColumn::Draw() {
	m_OnMouseOffset = -1;
	bool Trigger = Input::Instance()->GetKeyTrigger(VK_LBUTTON);
	bool Press = Input::Instance()->GetKeyPress(VK_LBUTTON);

	int offsetMax = static_cast<int>(m_pBuyItemData->GetBuyItemList().size() - 1);

	//スクロールバー
	m_ScrollBar.Update(ycell, offsetMax, &m_ScrollPer);

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
		{
			//アイテム枠描画
			Direct2DLib::Instance()->GetDrawSystem()->SetRect(XMinPos, YMinPos + yMin, XMaxPos, YMinPos + yMax, D2D1::ColorF(0.0f, 0.0f, 0.0f, alpha));
			if (NowY > offsetMax) { continue; }
			BuyItem* pItem = const_cast<BuyItem*>(m_pBuyItemData->GetBuyItem(static_cast<size_t>(NowY)));
			{
				bool OnMouse = HitPointToRectangle(
					static_cast<int>(Input::Instance()->GetMousePosition().x), static_cast<int>(Input::Instance()->GetMousePosition().y),
					static_cast<int>(static_cast<int>(XMinPos) + cellSize + 30), static_cast<int>(YMinPos + yMin + 2), static_cast<int>(static_cast<int>(XMinPos) + cellSize + 80), static_cast<int>(YMinPos + (yMin + yMax) / 2 - 2)
				);
				if (OnMouse && Trigger) {
					m_pBuyItemData->AddCart(static_cast<size_t>(NowY), 1);
				}

				D2D1::ColorF BGColor = D2D1::ColorF(0.0f, 1.0f, 0.0f, alpha);
				//選択時
				if (OnMouse) {
					m_OnMouseOffset = NowY;
					if (pItem) {
						BGColor = D2D1::ColorF(0.0f, 0.8f, 0.0f, alpha);
						if (Press) {
							BGColor = D2D1::ColorF(0.0f, 1.0f, 0.0f, alpha);
						}
						//
						HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
						SetCursor(hCursor);
					}
				}
				Direct2DLib::Instance()->GetDrawSystem()->SetRect(
					static_cast<float>(static_cast<int>(XMinPos) + cellSize + 30), static_cast<float>(YMinPos + yMin + 2),
					static_cast<float>(static_cast<int>(XMinPos) + cellSize + 80), static_cast<float>(YMinPos + (yMin + yMax) / 2 - 2),
					BGColor);
			}
			{
				bool OnMouse = HitPointToRectangle(
					static_cast<int>(Input::Instance()->GetMousePosition().x), static_cast<int>(Input::Instance()->GetMousePosition().y),
					static_cast<int>(static_cast<int>(XMinPos) + cellSize + 30), static_cast<int>(YMinPos + (yMin + yMax) / 2 + 2), static_cast<int>(static_cast<int>(XMinPos) + cellSize + 80), static_cast<int>(YMinPos + yMax - 2)
				);
				if (OnMouse && Trigger) {
					m_pBuyItemData->SubCart(static_cast<size_t>(NowY), 1);
				}

				D2D1::ColorF BGColor = D2D1::ColorF(1.0f, 0.0f, 0.0f, alpha);
				//選択時
				if (OnMouse) {
					m_OnMouseOffset = NowY;
					if (pItem) {
						BGColor = D2D1::ColorF(0.8f, 0.0f, 0.0f, alpha);
						if (Press) {
							BGColor = D2D1::ColorF(1.0f, 0.0f, 0.0f, alpha);
						}
						//
						HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
						SetCursor(hCursor);
					}
				}
				Direct2DLib::Instance()->GetDrawSystem()->SetRect(
					static_cast<float>(static_cast<int>(XMinPos) + cellSize + 30), static_cast<float>(YMinPos + (yMin + yMax) / 2 + 2),
					static_cast<float>(static_cast<int>(XMinPos) + cellSize + 80), static_cast<float>(YMinPos + yMax - 2),
					BGColor);
			}
			//アイテム描画
			if (!pItem) { continue; }
			auto& pItemData = ItemDataPool::Instance()->GetByID(pItem->ID);
			Direct2DLib::Instance()->GetDrawSystem()->SetExtendBMP(
				static_cast<float>(XMinPos + 5), static_cast<float>(YMinPos + yMin + 5),
				static_cast<float>(static_cast<int>(XMinPos) + cellSize - 5), static_cast<float>(YMinPos + yMax - 5),
				alpha, *ItemImagePool::Instance()->GetImage(pItemData->GetImagePath()));
			Direct2DLib::Instance()->GetDrawSystem()->SetFont(
				XMinPos, YMinPos + yMin,
				static_cast<float>(static_cast<int>(XMinPos) + cellSize + 10), YMinPos + yMax,
				StringToWString(std::to_string(pItemData->GetSellValue()) + "$").c_str(),
				D2D1::ColorF(D2D1::ColorF::LightGreen, alpha), FontPool::Instance()->GetFont(12, FontCenterX::Right, FontCenterY::Bottom).get());

			Direct2DLib::Instance()->GetDrawSystem()->SetFont(XMinPos, YMinPos + yMin, XMaxPos, YMinPos + yMax,
				StringToWString(std::to_string(pItem->Count)).c_str(),
				D2D1::ColorF(D2D1::ColorF::LightGreen, alpha), FontPool::Instance()->GetFont(12, FontCenterX::Right, FontCenterY::Bottom).get());
		}
	}
}
