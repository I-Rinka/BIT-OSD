#include <iostream>
void func1(char c)
{
    printf("%c\n", c);
}
void func2(char c)
{
    printf("%c\n", c + 1);
}
int main(int argc, char const *argv[])
{
    void (*func[2])(char);
    char c;
    std::cin >> c;
    func[0] = func1;
    func[1] = func2;
    func[0](c);
    func[1](c);
    // funcarray[0](c);
    // funcarray[1](c);

    return 0;
}
