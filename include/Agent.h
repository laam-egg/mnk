#ifndef Agent_INCLUDED
#define Agent_INCLUDED

#include <utility>      // std::pair
#include "state/State.h"

class Agent {
public:
    Agent() = default;
    ~Agent() = default;

    std::pair<int, int> getMove(State const& state, float const& analyzeTimeInSeconds, int* const numberOfSimulationsPerformed);
};

#endif // Agent_INCLUDED
