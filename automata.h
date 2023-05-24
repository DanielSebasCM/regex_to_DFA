#include "agraph.h"
#include <stack>
#include <map>
#include <queue>

#define CONCAT_OPERATOR '.'

class Automata
{
private:
    AutomataGraph graph;
    int start;
    std::set<int> final;
    std::set<char> alphabet;

    std::stack<AutomataGraph> expressions;
    std::stack<char> operators;

    std::string exprStr;
    AutomataGraph star(const AutomataGraph &);
    AutomataGraph plus(const AutomataGraph &);
    AutomataGraph concat(const AutomataGraph &, const AutomataGraph &);
    AutomataGraph orOp(const AutomataGraph &, const AutomataGraph &);

    void applyOperator(char);

public:
    Automata(std::string expr)
    {
        exprStr = expr;
    }

    AutomataGraph build();
    AutomataGraph transform();
    int getStart() const { return start; }
    std::set<int> getFinal() const { return final; }
};

AutomataGraph Automata::build()
{
    enum TokenType
    {
        OPERATOR,
        OPERAND
    };

    TokenType lastToken = OPERATOR;
    for (auto c : exprStr)
    {
        switch (c)
        {
        case '(':

            if (lastToken == OPERAND)
            {
                operators.push(CONCAT_OPERATOR);
            }
            operators.push(c);
            lastToken = OPERATOR;
            break;
        case ')':
            while (!operators.empty())
            {
                char op = operators.top();
                operators.pop();
                if (op == '(')
                    break;
                else
                    applyOperator(op);
            }
            lastToken = OPERAND;
            break;
        case '*':
        case '+':
            applyOperator(c);
            break;
        case CONCAT_OPERATOR:
        case '|':
            operators.push(c);
            lastToken = OPERATOR;
            break;
        default:

            if (lastToken == OPERAND)
            {
                operators.push(CONCAT_OPERATOR);
            }

            lastToken = OPERAND;
            AutomataGraph g;
            int start = g.createVertex();
            int end = g.createVertex();
            g.setStart(start);
            g.setEnd(end);
            g.addEdge(start, end, c);
            expressions.push(g);
            if (c != EPSILON)
                alphabet.insert(c);
            break;
        }
    }

    while (!operators.empty())
    {
        char op = operators.top();
        operators.pop();
        applyOperator(op);
    }

    graph = expressions.top();
    start = graph.getStart();

    return AutomataGraph(graph);
}

void Automata::applyOperator(char op)
{
    if (op == '*')
    {
        AutomataGraph g = expressions.top();
        expressions.pop();

        AutomataGraph newGraph = star(g);
        expressions.push(newGraph);

        // delete &g;
    }
    else if (op == '+')
    {
        AutomataGraph g = expressions.top();
        expressions.pop();

        AutomataGraph newGraph = plus(g);
        expressions.push(newGraph);

        // delete &g;
    }
    else if (op == CONCAT_OPERATOR)
    {
        AutomataGraph gRight = expressions.top();
        expressions.pop();
        AutomataGraph gLeft = expressions.top();
        expressions.pop();

        AutomataGraph newGraph = concat(gLeft, gRight);
        expressions.push(newGraph);

        // delete &gLeft;
        // delete &gRight;
    }
    else if (op == '|')
    {
        AutomataGraph gRight = expressions.top();
        expressions.pop();
        AutomataGraph gLeft = expressions.top();
        expressions.pop();

        AutomataGraph newGraph = orOp(gLeft, gRight);
        expressions.push(newGraph);

        // delete &gLeft;
        // delete &gRight;
    }
}

AutomataGraph Automata::transform()
{
    std::map<int, std::set<int>> setsMap;
    std::map<int, std::set<int>> PreSetsMap;
    std::map<int, std::set<std::pair<int, char>>> edges = graph.getEdges();

    std::queue<int> q;
    AutomataGraph newGraph;
    int newStart = newGraph.createVertex();
    newGraph.setStart(newStart);
    q.push(newStart);
    setsMap.insert(std::pair<int, std::set<int>>(newStart, graph.eClosure(newStart)));
    PreSetsMap.insert(std::pair<int, std::set<int>>(newStart, graph.getNeighbours(newStart)));

    while (!q.empty())
    {
        int current = q.front();
        q.pop();
        for (auto c : alphabet)
        {
            std::set<int> nextSet;
            for (auto v : setsMap[current])
            {
                for (auto [to, edge] : edges[v])
                {
                    if (edge == c)
                    {
                        nextSet.insert(to);
                    }
                }
            }

            if (nextSet.empty())
                continue;

            std::set<int> nextSetClosure = graph.eClosure(nextSet);

            bool exists = false;

            int next;
            for (auto [v, eClosure] : setsMap)
            {
                if (eClosure == nextSetClosure)
                {
                    exists = true;
                    newGraph.addEdge(current, v, c);
                    next = v;
                    break;
                }
            }

            if (!exists)
            {
                next = newGraph.createVertex();
                newGraph.addEdge(current, next, c);
                setsMap[next] = nextSetClosure;
                PreSetsMap[next] = nextSet;
                q.push(next);
            }

            if (nextSetClosure.find(graph.getEnd()) != nextSetClosure.end())
            {
                final.insert(next);
            }
        }
    }

    graph = newGraph;
    return AutomataGraph(newGraph);
}

AutomataGraph Automata::star(const AutomataGraph &g)
{
    int gStart = g.getStart();
    int gEnd = g.getEnd();

    AutomataGraph newGraph;
    int start = newGraph.createVertex();
    newGraph.setStart(start);
    int next = newGraph.createVertex();
    newGraph.addEdge(start, next, EPSILON);

    auto [newStart, newEnd] = newGraph.connectGraphToVertex(g, next);
    newGraph.addEdge(newEnd, newStart, EPSILON);

    int end = newGraph.createVertex();
    newGraph.setEnd(end);
    newGraph.addEdge(start, end, EPSILON);
    newGraph.addEdge(newEnd, end, EPSILON);

    return newGraph;
}

AutomataGraph Automata::plus(const AutomataGraph &g)
{
    int gStart = g.getStart();
    int gEnd = g.getEnd();

    AutomataGraph newGraph;
    int start = newGraph.createVertex();
    newGraph.setStart(start);
    int next = newGraph.createVertex();
    newGraph.addEdge(start, next, EPSILON);

    auto [newStart, newEnd] = newGraph.connectGraphToVertex(g, next);
    newGraph.addEdge(newEnd, newStart, EPSILON);

    int end = newGraph.createVertex();
    newGraph.setEnd(end);
    newGraph.addEdge(newEnd, end, EPSILON);

    return newGraph;
}

AutomataGraph Automata::concat(const AutomataGraph &gLeft, const AutomataGraph &gRight)
{
    int startLeft = gLeft.getStart();
    int endLeft = gLeft.getEnd();
    int startRight = gRight.getStart();
    int endRight = gRight.getEnd();

    AutomataGraph newGraph;

    int start = newGraph.createVertex();
    newGraph.setStart(start);

    auto [newLStart, newLEnd] = newGraph.connectGraphToVertex(gLeft, start);
    auto [newRStart, newREnd] = newGraph.connectGraphToVertex(gRight, newLEnd);

    newGraph.setEnd(newREnd);

    return newGraph;
}

AutomataGraph Automata::orOp(const AutomataGraph &gLeft, const AutomataGraph &gRight)
{
    int startLeft = gLeft.getStart();
    int endLeft = gLeft.getEnd();
    int startRight = gRight.getStart();
    int endRight = gRight.getEnd();

    AutomataGraph newGraph;

    int start = newGraph.createVertex();
    newGraph.setStart(start);

    int nextL = newGraph.createVertex();
    int nextR = newGraph.createVertex();

    newGraph.addEdge(start, nextL, EPSILON);
    newGraph.addEdge(start, nextR, EPSILON);

    auto [newLStart, newLEnd] = newGraph.connectGraphToVertex(gLeft, nextL);
    auto [newRStart, newREnd] = newGraph.connectGraphToVertex(gRight, nextR);

    int end = newGraph.createVertex();
    newGraph.setEnd(end);
    newGraph.addEdge(newLEnd, end, EPSILON);
    newGraph.addEdge(newREnd, end, EPSILON);

    return newGraph;
}
