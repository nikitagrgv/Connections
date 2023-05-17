#pragma once

#include "Callable.h"

#include <vector>
#include <unordered_map>

template<class... Args>
class Signal
{
public:
	using CallableType = Callable<Args...>;

	void operator()(Args... args)
	{
		for (auto &c : callbacks_)
		{
			// TODO std forward causes use after move?
			// c(std::forward<Args>(args)...);
			c.second(args...);
		}
	}

	void operator()(Args... args) const
	{
		for (const auto &c : callbacks_)
		{
			// TODO std forward causes use after move?
			// c(std::forward<Args>(args)...);
			c.second(args...);
		}
	}

	int add(CallableType callback)
	{
		const int id = generate_id();
		callbacks_.emplace(std::make_pair(id, std::move(callback)));
		return id;
	}

	void remove(int id)
	{
		callbacks_.erase(id);
	}

private:
	int generate_id() const
	{
		int id = 0;
		while (callbacks_.find(id) != callbacks_.end())
		{
			++id;
		}
		return id;
	}

private:
	std::unordered_map<int, CallableType> callbacks_;
};
