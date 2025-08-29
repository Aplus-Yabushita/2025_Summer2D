#include "Direct2DLib.h"

#include "Input/Input.h"


const Direct2DLib* SingletonBase<Direct2DLib>::m_Singleton = nullptr;

LRESULT Direct2DLib::MainWindowProc::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) const {
	switch (uMsg) {
	case WM_CREATE:
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// Other messages not shown...

	case WM_SIZE:
		if (Direct2DLib::Instance()->GetDrawSystem()->IsActive()) {
			Direct2DLib::Instance()->GetDrawSystem()->Resize(m_hwnd);
			InvalidateRect(m_hwnd, nullptr, false);
		}
		return 0;
	case WM_MOUSEMOVE:
		Input::Instance()->SetMousePosition(MAKEPOINTS(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		Input::Instance()->SetMouseWheelRot(GET_WHEEL_DELTA_WPARAM(wParam));
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(m_hwnd, &ps);
		Direct2DLib::Instance()->GetDrawSystem()->Draw(m_hwnd);
		EndPaint(m_hwnd, &ps);
	}
	return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
