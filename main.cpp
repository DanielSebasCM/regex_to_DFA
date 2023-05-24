#include <iostream>
#include "automata.h"

int main()
{
    Automata a("a(b|c)*d");

    AutomataGraph NFA = a.build();

    std::cout << "NFA:" << std::endl;
    std::cout << NFA.toString() << std::endl;

    AutomataGraph DFA = a.transform();

    std::cout << "DFA:" << std::endl;
    std::cout << DFA.toString() << std::endl;
}