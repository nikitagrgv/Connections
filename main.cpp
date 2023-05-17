#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

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
	ConnectionTemplate(To *obj, void (To::*func)(Types...))
		: Connection(obj)
		, func_(func)
	{}

	void operator()(Types... types)
	{
		assert(to_ && "Object was deleted!");

		To &obj = *get_to();
		(obj.*func_)(types...);
	}

private:
	inline To *get_to() { return static_cast<To *>(to_); }

private:
	void (To::*func_)(Types...) = nullptr;
};


Object::~Object()
{
	for (Connection *c : connections_)
	{
		c->to_ = nullptr;
	}
}

template<class... Args>
class Callable
{
public:
	// Functions
	explicit Callable(void (*func)(Args...))
		: f_{std::make_unique<FunctionBased>(func)}
	{}

	// Member functions
	template<class Obj>
	explicit Callable(Obj *obj, void (Obj::*func)(Args...))
		: f_{std::make_unique<MemberFunctionBased<Obj>>(obj, func)}
	{}

	// Functors
	template<class F>
	explicit Callable(F &&functor)
		: f_{std::make_unique<FunctorBased<F>>(std::forward<F>(functor))}
	{}

	void operator()(Args... args) { f_->operator()(std::forward<Args>(args)...); }

protected:
	class Base
	{
	public:
		virtual ~Base() = default;
		virtual void operator()(Args... args) = 0;
	};

	class FunctionBased : public Base
	{
	public:
		FunctionBased(void (*func)(Args...))
			: func_(func)
		{
			assert(func);
		}

		void operator()(Args... args) override { (*func_)(std::forward<Args>(args)...); }

	private:
		void (*func_)(Args...) = nullptr;
	};

	template<class Obj>
	class MemberFunctionBased : public Base
	{
	public:
		MemberFunctionBased(Obj *obj, void (Obj::*func)(Args...))
			: obj_(obj)
			, func_(func)
		{
			assert(obj);
			assert(func);
		}

		void operator()(Args... args) override { (obj_->*func_)(std::forward<Args>(args)...); }

	private:
		Obj *obj_ = nullptr;
		void (Obj::*func_)(Args...) = nullptr;
	};

	template<class F>
	class FunctorBased : public Base
	{
	public:
		explicit FunctorBased(F &&functor)
			: functor_(std::forward<F>(functor))
		{}

		void operator()(Args... args) override { functor_(std::forward<Args>(args)...); }

	private:
		F functor_;
	};

private:
	std::unique_ptr<Base> f_;
};

void some_func(int v)
{
	std::cout << "some_func " << v;
}

int main()
{
	{
		Callable<int> c(&some_func);
		c(555153);
	}

	{
		auto lamb = [](int a) {
			std::cout << a;
		};
		Callable<int> c(lamb);
		c(123);
	}

	{
		auto lamb = []() {
			std::cout << " ";
		};
		Callable<> c(lamb);
		c();
	}

	{
		struct Some
		{
			void fun(int v) { std::cout << "fun" << v; }
		};
		Some some;
		Callable<int> c(&some, &Some::fun);
		c(555);
	}
}