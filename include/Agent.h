#ifndef Agent_INCLUDED
#define Agent_INCLUDED

#include <utility>      // std::pair
#include <nlohmann/json.hpp>
#include "State.h"

class Agent {
public:
    Agent();
    ~Agent() = default;

    void setSize(int size);
    void setMoveFirst(bool moveFirst);

    void startAnalyzing();
    void stopAnalyzing();

    std::pair<int, int> getMove(JSON const& boardFromReferee);

private:
    bool m_analyzing;
    int m_size;
    bool m_moveFirst;
    bool m_activeMode; // whether to play actively against the opponent - prioritize making winning moves over blocking the opponent
};

#endif // Agent_INCLUDED
