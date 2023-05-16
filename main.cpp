#include <iostream>
#include <functional>
#include <vector>
#include <cassert>

class Connection;

class Object
{
public:
    virtual ~Object();

private:
    friend class Connection;

private:
    std::vector<Connection*> connections_;
};

class Connection
{
public:
    Connection(Object* to) : to_(to)
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
    Object* to_{};
};

template <class To, class... Types>
class ConnectionTemplate : public Connection
{
public:
    ConnectionTemplate(To* obj, void (To::*func)(Types...)) : Connection(obj), func_(func)
    {

    }

    void operator()(Types... types)
    {
        assert(to_ && "Object was deleted!");

        To& obj = *get_to();
        (obj.*func_)(types...);
    }

private:
    inline To* get_to() { return static_cast<To*>(to_); }

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

struct S : public Object
{
    void fun(int f)
    {
        std::cout << f << std::endl;
        // return 123;
    }
};


int main()
{
    auto s = new S();

    ConnectionTemplate con(s, &S::fun);
    con(1);
    delete s;
    con(2);

    std::cout << "fsf";
}