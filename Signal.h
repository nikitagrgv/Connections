#pragma once

#include "Callable.h"

#include <vector>

template<class... Args>
class Signal
{
public:
	using CallableType = Callable<Args...>;

	void operator()(Args... args)
	{
		for (CallableType &c : callbacks_)
		{
			// TODO std forward causes use after move?
			// c(std::forward<Args>(args)...);
			c(args...);
		}
	}

	void operator()(Args... args) const
	{
		for (const CallableType &c : callbacks_)
		{
			// TODO std forward causes use after move?
			// c(std::forward<Args>(args)...);
			c(args...);
		}
	}

	void add(CallableType callback)
	{
		callbacks_.emplace_back(std::move(callback));
	}

private:
	std::vector<CallableType> callbacks_;
};
