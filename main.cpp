#include <iostream>
#include "automata.h"

int main()
{
    Automata a("(a|b)*abb(a|b)*");

    AutomataGraph NFA = a.build();

    std::cout << "NFA:" << std::endl;
    std::cout << NFA.toString() << std::endl;

    AutomataGraph DFA = a.transform();

    std::cout << "DFA:" << std::endl;
    std::cout << DFA.toString() << std::endl;

    std::cout << "Start: " << a.getStart() << std::endl;
    std::cout << "Final: ";
    for (auto f : a.getFinal())
    {
        std::cout << f << " ";
    }
    std::cout << std::endl;
}