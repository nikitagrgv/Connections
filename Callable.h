#pragma once

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

	private:
		const Obj *obj_ = nullptr;
		void (Obj::*func_)(Args...) const = nullptr;
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
