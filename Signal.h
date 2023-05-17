#pragma once

#include "Callable.h"

#include <vector>

template<class... Args>
class Signal
{
public:
	using CallableType = Callable<Args...>;

	void add(CallableType callback)
	{
		callbacks_.emplace_back(std::move(callback));
	}

private:
	std::vector<CallableType> callbacks_;
};
