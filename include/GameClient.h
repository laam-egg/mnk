#ifndef GameClient_INCLUDED
#define GameClient_INCLUDED

#include "Agent.h"
#include <nlohmann/json.hpp>
using JSON = nlohmann::json;

class GameClient {
public:
    GameClient();
    ~GameClient() = default;

    void run();
    void stop();
    inline bool isRunning() const { return m_isRunning; }

private:
    Agent m_agent;
    bool m_isRunning;
    void initialize();
    void waitTillAllowedToMove();
    void move();

    int m_matchId;
    int m_size;

    bool m_moveFirst;
    JSON m_status; // nullptr or string
    JSON m_board; // ?
    float m_selfTime;
    float m_otherTime;
    float m_selfScore;
    float m_otherScore;
};

#endif // GameClient_INCLUDED
