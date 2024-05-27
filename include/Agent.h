#ifndef Agent_INCLUDED
#define Agent_INCLUDED

#include <utility>      // std::pair
#include "state/State.h"

class Agent {
public:
    Agent();
    ~Agent() = default;

    void setSize(int size);
    void setMoveFirst(bool moveFirst);

    void startAnalyzing();
    void stopAnalyzing();

    std::pair<int, int> getMove(State const& state);

private:
    bool m_analyzing;
    int m_size;
    bool m_moveFirst;
    bool m_activeMode; // whether to play actively against the opponent - prioritize making winning moves over blocking the opponent
};

#endif // Agent_INCLUDED
