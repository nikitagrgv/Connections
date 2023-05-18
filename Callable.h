#pragma once

#include <memory>
#include <cassert>

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

	// Const member functions
	template<class Obj>
	explicit Callable(const Obj *obj, void (Obj::*func)(Args...) const)
		: f_{std::make_unique<ConstMemberFunctionBased<Obj>>(obj, func)}
	{}

	// Functors
	template<class F>
	explicit Callable(F functor)
		: f_{std::make_unique<FunctorBased<F>>(std::move(functor))}
	{}

	Callable(const Callable &other)
		: f_(other.f_->clone())
	{}

	Callable(Callable &&other) noexcept
		: f_(std::move(other.f_))
	{}

	Callable &operator=(const Callable &other)
	{
		if (&other != this)
		{
			f_ = other.f_->clone();
		}

		return *this;
	}

	Callable &operator=(Callable &&other) noexcept
	{
		if (&other != this)
		{
			f_ = std::move(other.f_);
		}

		return *this;
	}

	void operator()(Args... args) { f_->operator()(std::forward<Args>(args)...); }

protected:
	class Base
	{
	public:
		virtual ~Base() = default;
		virtual void operator()(Args... args) = 0;

		virtual std::unique_ptr<Base> clone() = 0;
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

		std::unique_ptr<Base> clone() override { return std::make_unique<FunctionBased>(func_); }

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

		std::unique_ptr<Base> clone() override
		{
			return std::make_unique<MemberFunctionBased>(obj_, func_);
		}

	private:
		Obj *obj_ = nullptr;
		void (Obj::*func_)(Args...) = nullptr;
	};

	template<class Obj>
	class ConstMemberFunctionBased : public Base
	{
	public:
		ConstMemberFunctionBased(const Obj *obj, void (Obj::*func)(Args...) const)
			: obj_(obj)
			, func_(func)
		{
			assert(obj);
			assert(func);
		}

		void operator()(Args... args) override { (obj_->*func_)(std::forward<Args>(args)...); }

		std::unique_ptr<Base> clone() override
		{
			return std::make_unique<ConstMemberFunctionBased>(obj_, func_);
		}

	private:
		const Obj *obj_ = nullptr;
		void (Obj::*func_)(Args...) const = nullptr;
	};

	template<class F>
	class FunctorBased : public Base
	{
	public:
		explicit FunctorBased(F functor)
			: functor_(std::move(functor))
		{}

		void operator()(Args... args) override { functor_(std::forward<Args>(args)...); }

		std::unique_ptr<Base> clone() override { return std::make_unique<FunctorBased>(functor_); }

	private:
		F functor_;
	};

private:
	std::unique_ptr<Base> f_;
};
