#pragma once

#include <unordered_set>

class IAutoSignal;

class Object
{
    template<class... Args>
	friend class AutoSignal;

public:
	virtual ~Object();

private:
	mutable std::unordered_set<IAutoSignal *> connections_;
};
