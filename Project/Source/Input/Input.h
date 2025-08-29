#pragma once

#include "../Util.h"

#include <array>

// キー押し判定(押した瞬間やリピート、離した瞬間などにも対応)
class switchs {
	const float m_RepeatWaitTime = 0.5f;			//リピート時の1～2打間の時間間隔
	const float m_RepeatTime = 0.04f;				//リピート時の2打以降の判定間隔
private:
	bool		m_prevpress{ false };				// 押し続けたかどうかの判定判定
	bool		m_press{ false };					// 押し続けたかどうかの判定判定
	bool		m_repeat{ false };					// 押し続けた際の繰り返し判定
	float		m_repeatcount{ m_RepeatWaitTime };	// 繰り返し判定に使うタイマー
public:
	//コンストラクタ
	switchs(void) noexcept {
		m_repeatcount = m_RepeatWaitTime;
		m_prevpress = false;
		m_press = false;
	}
	switchs(const switchs&) = delete;
	switchs(switchs&&) = delete;
	switchs& operator=(const switchs&) = delete;
	switchs& operator=(switchs&&) = delete;
	//デストラクタ
	virtual ~switchs(void) noexcept {}
public:
	// 更新
	void			Update(bool key) noexcept {
		m_prevpress = m_press;
		//押したと記録
		m_press = key;
		//リピート処理
		m_repeat = trigger();//押した瞬間か、もしくは...
		//押してから一定時間後、一定間隔でtrueに
		if (m_press) {
			m_repeatcount -= 1.0f / 60.0f;
			if (m_repeatcount <= 0.0f) {
				m_repeatcount += m_RepeatTime;
				m_repeat = true;
			}
		}
		else {
			//離したら押してからのカウントをリセット
			m_repeatcount = m_RepeatWaitTime;
		}
	}

	// 押した瞬間
	bool			trigger(void) const noexcept { return m_press && !m_prevpress; }//押していて、尚且つカウントが1のフレーム
	// 押している間
	bool			press(void) const noexcept { return m_press; }
	// 押している間リピート
	bool			repeat(void) const noexcept { return m_repeat; }
	// 離した瞬間
	bool			release_trigger(void) const noexcept { return !m_press && m_prevpress; }//離していて、尚且つカウントが1のフレーム
	// 離している間
	bool			release(void) const noexcept { return !m_press; }
};

//
class Input : public SingletonBase<Input> {
private:
	friend class SingletonBase<Input>;
private:
	POINTS m_MousePoint{};
	std::array<switchs, 256> m_switchs;
	int MouseWheelRot{};
	int MouseWheelRotVel{};
private:
	Input() {}
	Input(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(const Input&) = delete;
	Input& operator=(Input&&) = delete;
	virtual ~Input() {}
public:
	void Update() {
		BYTE keyState[256] = {};
		if (GetKeyboardState(keyState) == 0) {
			return;
		}

		for (size_t i = 0; i < 256; ++i) {
			m_switchs.at(i).Update(keyState[i] & 0x80);
		}
		MouseWheelRotVel = MouseWheelRot;
		MouseWheelRot = 0;
	}
public:
	void SetMousePosition(POINTS value) { m_MousePoint = value; }
	void SetMouseWheelRot(int value) { MouseWheelRot = value; }
public:
	const auto& GetMousePosition() const { return m_MousePoint; }
	const auto& GetMouseWheelRotVel() const { return MouseWheelRotVel; }

	bool GetKeyTrigger(size_t vkey) const { return m_switchs.at(vkey).trigger(); }
	bool GetKeyRepeat(size_t vkey) const { return m_switchs.at(vkey).repeat(); }
	bool GetKeyPress(size_t vkey) const { return m_switchs.at(vkey).press(); }
};
