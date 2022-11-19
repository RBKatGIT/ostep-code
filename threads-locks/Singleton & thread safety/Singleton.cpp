#include <iostream>
#include <thread>
/*
Meyers Singleton, static in block scope will be intialized only once 
Therefor this implementation is thraed safe
https://www.modernescpp.com/index.php/thread-safe-initialization-of-a-singleton 
*/

class MeyerSingleton
{
    MeyerSingleton() = default;
    ~MeyerSingleton() = default;

public:
    MeyerSingleton(const MeyerSingleton&) = delete;
    MeyerSingleton& operator=(const MeyerSingleton&) = delete;

    static MeyerSingleton * getInstance()
    {
        static MeyerSingleton * instance = new MeyerSingleton();
        return instance;
    }
};

/*
Singletom using std::once
*/
class Singleton
{
    static std::once_flag initFlag;
    static Singleton * instance;
    Singleton() = default;
    ~Singleton() = default;

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton * getInstance()
    {
        std::call_once(initFlag,Singleton::initInstance);
        return instance;
    }

    static void initInstance()
    {
        instance = new Singleton();
    }
};


std::once_flag Singleton::initFlag;
Singleton * Singleton::instance;
int main ()
{
    std::cout<<"First : "<<MeyerSingleton::getInstance()<<std::endl;
    std::cout<<"Second : "<<MeyerSingleton::getInstance()<<std::endl;

}