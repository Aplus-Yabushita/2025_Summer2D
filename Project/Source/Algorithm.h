#pragma once
#define NOMINMAX
#include <algorithm>
#include <vector>
#include <functional>

namespace Mathf {
	template<class T>
	const T Min(T A, T B) {
		return std::min<T>(A, B);
	}

	template<class T>
	const T Max(T A, T B) {
		return std::max<T>(A, B);
	}

	template<class T>
	const T Clamp(T A, T Min, T Max) {
		return std::clamp<T>(A, Min, Max);
	}
}

template<class T>
class List {
public:
	std::vector<T> m_item;
public:
	List()noexcept { m_item.reserve(100); }
	virtual ~List()noexcept { Clear(); }
public:
	const T* Get(const std::function<bool(const T&)>& CheckFunc) const {
		for (auto& l : m_item) {
			if (CheckFunc(l)) {
				return &l;
			}
		}
		return nullptr;
	}
	//アイテムを末尾に追加
	T* AddBack() {
		m_item.emplace_back();
		return &m_item.back();
	}
	//アイテムを削除
	void Sub(const std::function<bool(const T&)>& CheckFunc) {
		for (auto& l : m_item) {
			if (CheckFunc(l)) {
				std::swap(l, m_item.back());
				m_item.pop_back();
				return;
			}
		}
	}
	//アイテムをすべて削除
	void Clear() {
		m_item.clear();
	}
};