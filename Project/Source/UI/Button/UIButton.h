#pragma once

#include "../../Image/Bitmap.h"

#include <string>
#include <functional>

enum class ButtonStatus : size_t {
	Enable,
	Disable,
	Inactive,
};

enum class ButtonType {
	Once,
	Repeat,
};

struct ButtonParam {
	int xmin{};
	int ymin{};
	int xmax{};
	int ymax{};
	int textSize{50};
	ButtonType m_ButtonType{ ButtonType::Once };
	std::wstring m_ButtonText;
	std::function<void()> m_PressEvent{};
	D2D1::ColorF m_BaseColor{ D2D1::ColorF::White };
};

class ButtonParts {
	Direct2DBitMap	m_BMP{};
	ButtonParam		m_buttonParam{};
	ButtonStatus	m_buttonStatus{};

	D2D1::ColorF	m_BGColor{ D2D1::ColorF::White };
public:
	ButtonParts() {}
	ButtonParts(const ButtonParts&) {}
	ButtonParts(ButtonParts&&) noexcept {}
	ButtonParts& operator=(const ButtonParts&) {}
	ButtonParts& operator=(ButtonParts&&) noexcept {}
	virtual ~ButtonParts() {
		m_BMP.~Direct2DBitMap();
	}
public:
	const ButtonParam GetNowParam() const { return m_buttonParam; }
public:
	void ChangeParam(ButtonParam buttonParam) { m_buttonParam = buttonParam; }
	void ChangeStatus(ButtonStatus buttonStatus) { m_buttonStatus = buttonStatus; }
	void Init(ButtonParam buttonParam);
	void Update();
	void Draw();
};
