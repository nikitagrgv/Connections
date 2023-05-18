#include "Callable.h"
#include "Signal.h"

#include <iostream>


class Connection;

class Object
{
public:
	virtual ~Object();

private:
	friend class Connection;

private:
	std::vector<Connection *> connections_;
};

class Connection
{
public:
	Connection(Object *to)
		: to_(to)
	{
		to->connections_.push_back(this);
	}

	virtual ~Connection()
	{
		for (int i = 0, count = to_->connections_.size(); i < count; ++i)
		{
			if (to_->connections_[i] == this)
			{
				// fixme
				to_->connections_.erase(to_->connections_.begin() + i);
				break;
			}
		}
	}

	bool isValid() const { return to_ != nullptr; }

private:
	friend class Object;

protected:
	Object *to_{};
};

template<class To, class... Types>
class ConnectionTemplate : public Connection
{
public:
	ConnectionTemplate(To *obj, Callable<Types...> func)
		: Connection(obj)
		, func_(func)
	{}

	void operator()(Types... types)
	{
		assert(to_ && "Object was deleted!");
		func_(std::forward<Types>(types)...);
	}

private:
	Callable<Types...> func_;
};


Object::~Object()
{
	for (Connection *c : connections_)
	{
		c->to_ = nullptr;
	}
}


template<class... Args>
class AutoSignal
{
public:
	// Functions
	int connect(void (*func)(Args...))
    {
        return signal_.add(Callable<Args...>(func));
    }

	// Member functions
	template<class Obj>
	int connect(Obj *obj, void (Obj::*func)(Args...))
	{
        return signal_.add(Callable<Args...>(obj, func));
    }

	// Const member functions
	template<class Obj>
	int connect(const Obj *obj, void (Obj::*func)(Args...) const)
	{
        return signal_.add(Callable<Args...>(obj, func));
    }

	// Functors
	template<class F>
	int connect(F functor)
	{
        return signal_.add(Callable<Args...>(std::move(functor)));
    }

    void operator()(Args... args)
	{
		signal_(std::forward<Args>(args)...);
	}

	void operator()(Args... args) const
	{
		signal_(std::forward<Args>(args)...);
	}

private:
	Signal<Args...> signal_;
};

void fuf(int a)
{
    std::cout << a + 4;
}

int main()
{
    AutoSignal<int> signal;

    signal.connect([](int a){std::cout << a + 1;});

    struct Str
    {
        void fun1(int a) { std::cout << a + 2; }
        void fun2(int a) const { std::cout << a + 3; }
    };
    Str s;
    signal.connect(&s, &Str::fun1);
    signal.connect(&s, &Str::fun2);

    signal.connect(&fuf);

    signal(0);


}