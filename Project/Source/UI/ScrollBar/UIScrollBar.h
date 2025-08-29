#pragma once

#include "../../Image/Bitmap.h"

#include <string>
#include <functional>

#include "../Button/UIButton.h"

class ScrollBarParts {
	ButtonParam		m_buttonParam{};
	ButtonStatus	m_buttonStatus{};

	bool OnPress = false;
public:
	void ChangeStatus(ButtonStatus buttonStatus) { m_buttonStatus = buttonStatus; }
	void Init(ButtonParam buttonParam);
	void Update(int ScrollOnce, int ScrollMax, float* pScrollNow);
};
