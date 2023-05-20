#pragma once

#include "Signal.h"

#include <unordered_map>
#include <unordered_set>

class IAutoSignal
{
	friend class Object;

public:
    virtual ~IAutoSignal() = default;

protected:
	virtual void on_object_deleted(const Object *obj) = 0;
};

// TODO: constness of methods?

template<class... Args>
class AutoSignal : IAutoSignal
{
public:
    ~AutoSignal()
    {
	    for(const auto &it : objects_callbacks_)
        {
            const Object *obj = it.first;
            obj->connections_.erase(this);
        }
    }

	// Functions
	int connect(const Object *obj, void (*func)(Args...))
	{
		const int id = signal_.add(Callable<Args...>(func));
		on_callback_added(obj, id);
		return id;
	}

	// Member functions
	template<class Obj>
	int connect(const Obj *obj, void (Obj::*func)(Args...))
	{
		const int id = signal_.add(Callable<Args...>(obj, func));
		on_callback_added(obj, id);
		return id;
	}

	// Const member functions
	template<class Obj>
	int connect(const Obj *obj, void (Obj::*func)(Args...) const)
	{
		const int id = signal_.add(Callable<Args...>(obj, func));
		on_callback_added(obj, id);
		return id;
	}

	// Functors
	template<class F>
	int connect(const Object *obj, F functor)
	{
		const int id = signal_.add(Callable<Args...>(std::move(functor)));
		on_callback_added(obj, id);
		return id;
	}

	void operator()(Args... args) { signal_(std::forward<Args>(args)...); }

	void operator()(Args... args) const { signal_(std::forward<Args>(args)...); }

protected:
    void on_callback_added(const Object *obj, int id)
    {
        assert(objects_callbacks_[obj].find(id) == objects_callbacks_[obj].end());
        objects_callbacks_[obj].insert(id);
        if (obj != nullptr)
        {
            obj->connections_.insert(this);
        }
    }

	void on_object_deleted(const Object *obj) override
    {
        const auto it = objects_callbacks_.find(obj);
        assert(it != objects_callbacks_.end());
        const CallbacksIds& ids = it->second;
        for (const int& id : ids)
        {
            signal_.remove(id);
        }
        objects_callbacks_.erase(it);
    }

private:
	using CallbacksIds = std::unordered_set<int>;

	std::unordered_map<const Object *, CallbacksIds> objects_callbacks_;
	Signal<Args...> signal_;
};
