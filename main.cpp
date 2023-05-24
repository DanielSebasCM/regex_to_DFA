#include <iostream>
#include "automata.h"

int main()
{
    Automata a("a(b|c)*d");

    ListGraph NFA = a.build();

    std::cout << "NFA:" << std::endl;
    std::cout << NFA.toString() << std::endl;

    ListGraph DFA = a.transform();

    std::cout << "DFA:" << std::endl;
    std::cout << DFA.toString() << std::endl;
}