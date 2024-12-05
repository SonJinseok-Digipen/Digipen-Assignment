#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include<functional>

void test(int a)
{
    std::cout << "Mother Fucker !! "<<a << '\n';
}

int main()
{
    std::string my_name;
  

    std::function<void(int)>lef;

    lef = test;

    lef(14);



}