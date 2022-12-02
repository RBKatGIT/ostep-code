/*
https://linuxhint.com/callback-function-in-cpp/
*/
#include <iostream>


void principal_function(void(*funct_ptr)(int))
{
    funct_ptr(2);
}


void function(int value)
{
    std::cout<<"We have a value "<<value<<std::endl;
}

int main()
{
    void (*ptr)(int) = &function;
    principal_function(ptr);

}