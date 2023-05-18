#include "Callable.h"
#include "Signal.h"

#include <iostream>
#include <unordered_set>

class IAutoSignal;

class Object
{
    template<class... Args>
	friend class AutoSignal;

public:
	virtual ~Object();

private:
    // mutable or other solution??
	mutable std::unordered_set<IAutoSignal *> connections_;
};

class IAutoSignal
{
	friend class Object;

public:
    virtual ~IAutoSignal() = default;

protected:
	virtual void on_object_deleted(const Object *obj) = 0;
};


Object::~Object()
{
	for (IAutoSignal *c : connections_)
	{
		c->on_object_deleted(this);
	}
}

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
	int connect(Object *obj, void (*func)(Args...))
	{
		const int id = signal_.add(Callable<Args...>(func));
		on_object_added(obj, id);
		return id;
	}

	// Member functions
	template<class Obj>
	int connect(Obj *obj, void (Obj::*func)(Args...))
	{
		const int id = signal_.add(Callable<Args...>(obj, func));
		on_object_added(obj, id);
		return id;
	}

	// Const member functions
	template<class Obj>
	int connect(const Obj *obj, void (Obj::*func)(Args...) const)
	{
		const int id = signal_.add(Callable<Args...>(obj, func));
		on_object_added(obj, id);
		return id;
	}

	// Functors
	template<class F>
	int connect(Object *obj, F functor)
	{
		const int id = signal_.add(Callable<Args...>(std::move(functor)));
		on_object_added(obj, id);
		return id;
	}

	void operator()(Args... args) { signal_(std::forward<Args>(args)...); }

	void operator()(Args... args) const { signal_(std::forward<Args>(args)...); }

protected:
    void on_object_added(const Object *obj, int id)
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

void fuf(int a)
{
	std::cout << a + 4;
}

int main()
{
	AutoSignal<int> signal;

	signal.connect(nullptr, [](int a) { std::cout << a + 1; });

	struct Str : Object
	{
		~Str() { std::cout << "destr"; }
		void fun1(int a) { std::cout << a + 2; }
		void fun2(int a) const { std::cout << a + 3; }

		int val = 55;
	};
	auto s = new Str{};
	signal.connect(s, &Str::fun1);
	signal.connect(s, &Str::fun2);

	signal.connect(nullptr, &fuf);

	signal(0);

	delete s;

	signal(0);
}