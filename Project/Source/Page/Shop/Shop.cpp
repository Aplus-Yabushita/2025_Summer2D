#include "Shop.h"

#include "../../Direct2DLib.h"

#include "../../Page/PageManager.h"

#include "../../Font/FontPool.h"

#include "../../Input/Input.h"

#include "../../DataBase/ItemData/ItemDataPool.h"

void Shop::SetBuyMode() {
	Reset();
	m_Button.at(3).ChangeStatus(ButtonStatus::Enable);
	m_Button.at(4).ChangeStatus(ButtonStatus::Disable);

	m_Button.at(2).ChangeStatus(ButtonStatus::Inactive);
	m_Button.at(5).ChangeStatus(ButtonStatus::Enable);
	m_ItemDrawList.SetDragEnable(true);
	m_ItemSellList.SetDragEnable(false);

	m_ShopMode = ShopMode::Buy;
}

void Shop::SetSellMode() {
	Reset();
	m_Button.at(3).ChangeStatus(ButtonStatus::Disable);
	m_Button.at(4).ChangeStatus(ButtonStatus::Enable);

	m_Button.at(2).ChangeStatus(ButtonStatus::Enable);
	m_Button.at(5).ChangeStatus(ButtonStatus::Inactive);
	m_ItemDrawList.SetDragEnable(false);
	m_ItemSellList.SetDragEnable(false);

	m_ShopMode = ShopMode::Sell;
}

void Shop::Sell() {
	//売るアイテムがある場合
	if (SellData.GetCellItemList().size() == 0) { return; }
	//総額を計算しつつアイテムを削除
	size_t MoneyTotal = 0;
	for (size_t loop = 0, Max = SellData.GetCellItemList().size(); loop < Max; ++loop) {
		auto& s = SellData.GetCellItemList().at(loop);
		MoneyTotal += static_cast<size_t>(ItemDataPool::Instance()->GetByID(s.ID)->GetSellValue()) * s.Count;//金額を取得
		SellData.SubItem(s.offset, s.Count);
		--loop;
		--Max;
	}
	//総額をプレイヤーに渡す
	PlayerData::Instance()->GetMyCharacterData()->AddItem(0, ItemDataPool::Instance()->Find(L"Yellar"), MoneyTotal);
}

void Shop::Buy() {
	//商品リストのクリックされたアイテムを購入
	BuyData.BuyCart(PlayerData::Instance()->GetMyCharacterData().get());
}

void Shop::Reset() {
	switch (m_ShopMode) {
	case ShopMode::Buy:
		//商品リストをクリアにする
		for (size_t loop = 0, Max = SellData.GetCellItemList().size(); loop < Max; ++loop) {
			auto& s = SellData.GetCellItemList().at(loop);
			SellData.SubItem(s.offset, s.Count);
			--loop;
			--Max;
		}
		break;
	case ShopMode::Sell:
		//出してるけど売ってない商品を戻す
		for (size_t loop = 0, Max = SellData.GetCellItemList().size(); loop < Max; ++loop) {
			auto& s = SellData.GetCellItemList().at(loop);
			PlayerData::Instance()->GetMyCharacterData()->AddItem(0, s.ID, s.Count);
			SellData.SubItem(s.offset, s.Count);
			--loop;
			--Max;
		}
		break;
	default:
		break;
	}
}

void Shop::Init() {
	m_IsEndPage = false;
	m_Button.clear();
	m_Button.reserve(6);

	ButtonParam buttonParam;
	buttonParam.xmin = 640 - 82 - 4 - 82 - 4;
	buttonParam.ymin = 4;
	buttonParam.xmax = buttonParam.xmin + 82;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.7f, 0.7f);
	buttonParam.m_ButtonText = L"戻る";
	buttonParam.m_PressEvent = [&] {
		m_IsEndPage = true;
		PageParent::SetNextPageID(0);
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);

	buttonParam.xmin = 640 - 82 - 4;
	buttonParam.ymin = 4;
	buttonParam.xmax = buttonParam.xmin + 82;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(1.0f, 0.0f, 0.0f);
	buttonParam.m_ButtonText = L"終了";
	buttonParam.m_PressEvent = [&] {
		PageManager::Instance()->SetEnd();
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);

	buttonParam.xmin = 390;
	buttonParam.ymin = 100;
	buttonParam.xmax = 595;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(0.0f, 0.7f, 0.0f);
	buttonParam.m_ButtonText = L"売る";
	buttonParam.m_PressEvent = [&] {
		Sell();
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);


	buttonParam.xmin = 16;
	buttonParam.ymin = 50;
	buttonParam.xmax = buttonParam.xmin + 82;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.7f, 0.7f);
	buttonParam.m_ButtonText = L"売る";
	buttonParam.m_PressEvent = [&] {
		SetSellMode();
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);


	buttonParam.xmin = 102;
	buttonParam.ymin = 50;
	buttonParam.xmax = buttonParam.xmin + 82;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.7f, 0.7f);
	buttonParam.m_ButtonText = L"買う";
	buttonParam.m_PressEvent = [&] {
		SetBuyMode();
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);


	buttonParam.xmin = 390;
	buttonParam.ymin = 100;
	buttonParam.xmax = 595;
	buttonParam.ymax = buttonParam.ymin + 48;
	buttonParam.textSize = 32;
	buttonParam.m_ButtonType = ButtonType::Once;
	buttonParam.m_BaseColor = D2D1::ColorF(0.7f, 0.35f, 0.0f);
	buttonParam.m_ButtonText = L"買う";
	buttonParam.m_PressEvent = [&] {
		Buy();
		};
	m_Button.emplace_back();
	m_Button.back().Init(buttonParam);
	m_Button.back().ChangeStatus(ButtonStatus::Enable);

	m_ItemDrawList.Init(4.0f, 100.0f, 4.0f + 374.0f, 100.0f + 350.0f, 48, PlayerData::Instance()->GetMyCharacterData().get());

	m_ItemSellList.Init(390.0f, 150.0f, 614.0f, 100.0f + 350.0f, 48, &SellData);
	m_ItemBuyColumn.Init(390.0f, 150.0f, 614.0f, 100.0f + 350.0f, 48, &BuyData);

	BuyData.Reset();
	BuyData.AddItem(0);
	BuyData.AddItem(1);
	BuyData.AddItem(2);

	//デフォルト
	SetSellMode();
}

void Shop::Update() {
	bool Trigger = Input::Instance()->GetKeyTrigger(VK_LBUTTON);
	switch (m_ShopMode) {
	case ShopMode::Buy:
		break;
	case ShopMode::Sell:
		//自分のインベントリから売り場に出す
		if (m_ItemDrawList.HasOnMouseOffset() && Trigger) {
			const CellItem* pItem = PlayerData::Instance()->GetMyCharacterData()->GetCellItem(static_cast<size_t>(m_ItemDrawList.GetOnMouseOffset()));
			if (pItem) {
				SellData.AddItem(0, pItem->ID, pItem->Count);
				PlayerData::Instance()->GetMyCharacterData()->SubItem(pItem->offset, pItem->Count);
			}
		}
		//売り場から自分のインベントリに戻す
		if (m_ItemSellList.HasOnMouseOffset() && Trigger) {
			const CellItem* pItem = SellData.GetCellItem(static_cast<size_t>(m_ItemSellList.GetOnMouseOffset()));
			if (pItem) {
				PlayerData::Instance()->GetMyCharacterData()->AddItem(0, pItem->ID, pItem->Count);
				SellData.SubItem(pItem->offset, pItem->Count);
			}
		}
		break;
	default:
		break;
	}
	for (auto& b : m_Button) {
		b.Update();
	}
	m_ItemDrawList.Update();
	switch (m_ShopMode) {
	case ShopMode::Buy:
		m_ItemBuyColumn.Update();
		break;
	case ShopMode::Sell:
		m_ItemSellList.Update();
		break;
	default:
		break;
	}
}

void Shop::Draw() {
	Direct2DLib::Instance()->GetDrawSystem()->SetClearScreen(D2D1::ColorF(D2D1::ColorF::SkyBlue));

	Direct2DLib::Instance()->GetDrawSystem()->SetFont(
		4.0f, 4.0f,
		256.0f, 256.0f,
		L"ショップ",
		D2D1::ColorF(D2D1::ColorF::Black), FontPool::Instance()->GetFont(25, FontCenterX::Left, FontCenterY::Top).get());

	for (auto& b : m_Button) {
		b.Draw();
	}
	m_ItemDrawList.Draw();
	switch (m_ShopMode) {
	case ShopMode::Buy:
		m_ItemBuyColumn.Draw();
		break;
	case ShopMode::Sell:
		m_ItemSellList.Draw();
		break;
	default:
		break;
	}
}

void Shop::Dispose() {
	Reset();
	m_Button.clear();
}
