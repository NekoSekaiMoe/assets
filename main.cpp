#include <iostream>

int main () {
int a = 10;
int *p;

std::cout << "a 的值是：" << a << std::endl;       // 输出 a 的值，即 10
std::cout << "a 的地址是：" << &a << std::endl;    // 输出 a 的地址
    std::cout << "p 存储的地址是：" << p << std::endl;    // 输出 p 存储的地址（即 &a）
    std::cout << "p 指向的值是：" << *p << std::endl;     // 输出 p 指向的值（即 a 的值）


}
