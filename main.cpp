#include <iostream>
#include <functional>
#include <vector>

class Connection;

class Object
{
public:
    virtual ~Object()
    {

    }

private:
    friend class Connection;
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

protected:
    Object* to_{};
};

template <class To, class Ret, class... Types>
class ConnectionTemplate : public Connection
{
public:
    ConnectionTemplate(To* obj, Ret (To::*func)(Types...)) : Connection(obj), func_(func)
    {

    }

    Ret operator()(Types... types)
    {
        To& obj = *get_to();
        return (obj.*func_)(types...);
    }

private:
    inline To* get_to() { return static_cast<To*>(to_); }

private:
    Ret (To::*func_)(Types...) = nullptr;
};



struct S : public Object
{
    int fun()
    {
        // std::cout << f << std::endl;
        return 123;
    }
};


int main()
{
    S s;
    ConnectionTemplate con(&s, &S::fun);

    con();

    std::cout << "fsf";
}