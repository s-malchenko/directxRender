#pragma once

#include <queue>

namespace Util
{
	void SleepMs(size_t ms);

	template<class T>
	inline void Trim(std::queue<T> q, size_t maxCount)
	{
		while (q.size() > maxCount)
		{
			q.pop();
		}
	}

	template <class T>
	T PopOrDefault(std::queue<T>& q)
	{
		if (!q.empty())
		{
			auto result = q.front();
			q.pop();
			return result;
		}

		return T();
	}
}
