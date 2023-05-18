#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

#include "Callable.h"
#include "Signal.h"

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





int main()
{
	Signal<int> signal;

    int id1 = signal.add(Callable<int>([](int){std::cout << 1;}));
    int id2 = signal.add(Callable<int>([](int){std::cout << 2;}));
    int id3 = signal.add(Callable<int>([](int){std::cout << 3;}));
    int id4 = signal.add(Callable<int>([](int){std::cout << 4;}));
    int id5 = signal.add(Callable<int>([](int){std::cout << 5;}));
    int id6 = signal.add(Callable<int>([](int){std::cout << 6;}));


    signal(0);
    std::cout<<std::endl;

    signal.remove(id3);

    signal(0);
    std::cout<<std::endl;

    signal.remove(id5);

    signal(0);
    std::cout<<std::endl;

    signal.remove(id2);

    signal(0);
    std::cout<<std::endl;
	// {
	// 	auto lam = [](int a) {
	// 		std::cout << a << " ";
	// 	};
	// 	Callable<int> c(lam);
	// 	signal.add(c);
	// 	signal.add(c);
	// 	signal.add(c);
	// }

	// {
	// 	auto lam = [](int a) {
	// 		std::cout << a+1 << "! ";
	// 	};
	// 	Callable<int> c(lam);
	// 	const int id = signal.add(std::move(c));
	// 	signal.remove(id);
	// }

	// signal(5);
}