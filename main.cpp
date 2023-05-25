#include <iostream>
#include "automata.h"

int main()
{
    Automata a("b+a*");

    AutomataGraph NFA = a.build();

    std::cout << "NFA:" << std::endl;
    std::cout << NFA.toString();
    std::cout << "Start: " << NFA.getStart() << std::endl;
    std::cout << "Final: ";
    for (auto f : NFA.getFinal())
    {
        std::cout << f << " ";
    }
    std::cout << std::endl
              << std::endl;

    AutomataGraph DFA = a.transform();

    std::cout << "DFA:" << std::endl;
    std::cout << DFA.toString();

    std::cout << "Start: " << DFA.getStart() << std::endl;
    std::cout << "Final: ";
    for (auto f : DFA.getFinal())
    {
        std::cout << f << " ";
    }
    std::cout << std::endl
              << std::endl;
}