#include "ugraph.h"
#include <stack>
#include <map>
#include <queue>

class Automata
{
private:
    ListGraph graph;
    int start;
    std::set<int> final;
    std::set<char> alphabet;

    std::stack<ListGraph> expressions;
    std::stack<char> operators;

    std::string exprStr;
    ListGraph star(const ListGraph &);
    ListGraph plus(const ListGraph &);
    ListGraph concat(const ListGraph &, const ListGraph &);
    ListGraph orOp(const ListGraph &, const ListGraph &);

    void applyOperator(char);

public:
    Automata(std::string expr)
    {
        exprStr = expr;
    }

    ListGraph build();
    ListGraph transform();
};

ListGraph Automata::build()
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
                operators.push('.');
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
        case '.':
        case '|':
            operators.push(c);
            lastToken = OPERATOR;
            break;
        default:

            if (lastToken == OPERAND)
            {
                operators.push('.');
            }

            lastToken = OPERAND;
            ListGraph g;
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
    final.insert(graph.getEnd());

    return ListGraph(graph);
}

void Automata::applyOperator(char op)
{
    if (op == '*')
    {
        ListGraph g = expressions.top();
        expressions.pop();

        ListGraph newGraph = star(g);
        expressions.push(newGraph);

        // delete &g;
    }
    else if (op == '+')
    {
        ListGraph g = expressions.top();
        expressions.pop();

        ListGraph newGraph = plus(g);
        expressions.push(newGraph);

        // delete &g;
    }
    else if (op == '.')
    {
        ListGraph gRight = expressions.top();
        expressions.pop();
        ListGraph gLeft = expressions.top();
        expressions.pop();

        ListGraph newGraph = concat(gLeft, gRight);
        expressions.push(newGraph);

        // delete &gLeft;
        // delete &gRight;
    }
    else if (op == '|')
    {
        ListGraph gRight = expressions.top();
        expressions.pop();
        ListGraph gLeft = expressions.top();
        expressions.pop();

        ListGraph newGraph = orOp(gLeft, gRight);
        expressions.push(newGraph);

        // delete &gLeft;
        // delete &gRight;
    }
}

ListGraph Automata::transform()
{
    std::map<int, std::set<int>> setsMap;
    std::map<int, std::set<int>> PreSetsMap;
    std::map<int, std::set<std::pair<int, char>>> edges = graph.getEdges();

    std::queue<int> q;
    ListGraph newGraph;
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

            for (auto [v, eClosure] : setsMap)
            {
                if (eClosure == nextSetClosure)
                {
                    exists = true;
                    newGraph.addEdge(current, v, c);
                    break;
                }
            }

            if (!exists)
            {
                int next = newGraph.createVertex();
                newGraph.addEdge(current, next, c);
                setsMap[next] = nextSetClosure;
                PreSetsMap[next] = nextSet;
                q.push(next);
            }

            if (nextSetClosure.find(graph.getEnd()) != nextSetClosure.end())
            {
                final.insert(current);
            }
        }
    }

    graph = newGraph;
    return ListGraph(newGraph);
}

ListGraph Automata::star(const ListGraph &g)
{
    int gStart = g.getStart();
    int gEnd = g.getEnd();

    ListGraph newGraph;
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

ListGraph Automata::plus(const ListGraph &g)
{
    int gStart = g.getStart();
    int gEnd = g.getEnd();

    ListGraph newGraph;
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

ListGraph Automata::concat(const ListGraph &gLeft, const ListGraph &gRight)
{
    int startLeft = gLeft.getStart();
    int endLeft = gLeft.getEnd();
    int startRight = gRight.getStart();
    int endRight = gRight.getEnd();

    ListGraph newGraph;

    int start = newGraph.createVertex();
    newGraph.setStart(start);

    auto [newLStart, newLEnd] = newGraph.connectGraphToVertex(gLeft, start);
    auto [newRStart, newREnd] = newGraph.connectGraphToVertex(gRight, newLEnd);

    newGraph.setEnd(newREnd);

    return newGraph;
}

ListGraph Automata::orOp(const ListGraph &gLeft, const ListGraph &gRight)
{
    int startLeft = gLeft.getStart();
    int endLeft = gLeft.getEnd();
    int startRight = gRight.getStart();
    int endRight = gRight.getEnd();

    ListGraph newGraph;

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
