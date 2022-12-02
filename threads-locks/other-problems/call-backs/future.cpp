
/* 
https://linuxhint.com/callback-function-in-cpp/
Truly asynchronous behaviour within the same thraed
*/
#include <iostream>
#include <future>

std::future<std::string> output;

void principal_function(std::string(*funct_ptr)(int))
{
    output = async(funct_ptr,2);
    std::cout<<"Principal function"<<std::endl;
}


std::string function(int value)
{
    std::cout<<"We have a value "<<value<<std::endl;
    return "I am the outoput";
}

int main()
{
    std::string (*ptr)(int) = &function;
    principal_function(ptr);
    std::string ret = output.get();
    std::cout<<ret<<std::endl;

}