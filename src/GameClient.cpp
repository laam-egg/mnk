#include "GameClient.h"
#include "Configuration.h"
#include <thread>
#include <chrono>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
using JSON = JSON;

GameClient::GameClient() {
    m_isRunning = true;
}

void GameClient::run() {
    initialize();
    while (isRunning()) {
        waitTillAllowedToMove();
        move();
    }
}

void GameClient::initialize() {
    JSON payload {
        {"room_id", Configuration::getInstance().getRoomId()},
        {"team1_id", Configuration::getInstance().getSelfId()},
        {"team2_id", Configuration::getInstance().getOtherId()},
        {"init", true}
    };

    int remainingRetries = 3;

    while (isRunning()) {
        cpr::Response r = cpr::Post(
            cpr::Url{Configuration::getInstance().getRefereeServerAddress() + "/init"},
            cpr::Body{payload.dump()},
            cpr::Header{{"Content-Type", "application/json"}}
        );

        if (r.status_code == 200) {
            JSON response = JSON::parse(r.text);
            std::cout << "Connection established" << std::endl;
            Configuration::getInstance().setRoomId(response["room_id"]);
            m_matchId = response["match_id"];

            std::string const verboseTeam1Id = response["team1_id"];
            std::string const verboseTeam2Id = response["team2_id"];
            if (verboseTeam1Id.find(Configuration::getInstance().getSelfId()) != std::string::npos) {
                Configuration::getInstance().setSelfId(verboseTeam1Id);
                Configuration::getInstance().setOtherId(verboseTeam2Id);
                m_moveFirst = true;
            } else if (verboseTeam2Id.find(Configuration::getInstance().getSelfId()) != std::string::npos) {
                Configuration::getInstance().setSelfId(verboseTeam2Id);
                Configuration::getInstance().setOtherId(verboseTeam1Id);
                m_moveFirst = false;
            } else {
                throw std::runtime_error("Could not resolve team id");
            }
            m_size = response["size"];
            m_agent.setSize(m_size);
            m_agent.setMoveFirst(m_moveFirst);
            m_agent.startAnalyzing();

            std::cout << "Board Size: " << m_size << std::endl;
            std::cout << "Self team role: " << Configuration::getInstance().getSelfTeamRole() << std::endl;
            std::cout << "Self player ID: " << Configuration::getInstance().getSelfId() << std::endl;
            std::cout << "Other player ID: " << Configuration::getInstance().getOtherId() << std::endl;

            return;
        }

        if (remainingRetries-- == 0) {
            std::cerr << "ERROR INITIALIZING GAME" << std::endl;
            std::cerr << "Status code: " << r.status_code << std::endl;
            std::cerr << "Response: " << r.text << std::endl;
            m_isRunning = false;
            return;
        }
        CONTINUE: std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GameClient::waitTillAllowedToMove() {
    JSON payload {
        {"room_id", Configuration::getInstance().getRoomId()},
        {"team_id", Configuration::getInstance().getSelfId()},
        {"match_id", m_matchId}
    };

    int remainingRetries = 3;

    while (isRunning()) {
        cpr::Response r = cpr::Post(
            cpr::Url{Configuration::getInstance().getRefereeServerAddress()},
            cpr::Body{payload.dump()},
            cpr::Header{{"Content-Type", "application/json"}}
        );

        if (r.status_code == 200) {
            bool isAllowedToMoveNow;
            JSON response = JSON::parse(r.text);
            m_status = response["status"];
            m_board = response["board"];

            if (m_moveFirst) {
                m_selfTime = response["time1"];
                m_otherTime = response["time2"];
                m_selfScore = response["score1"];
                m_otherScore = response["score2"];
            } else {
                m_selfTime = response["time2"];
                m_otherTime = response["time1"];
                m_selfScore = response["score2"];
                m_otherScore = response["score1"];
            }

            if (m_status != nullptr) {
                std::cout << "Game over " << m_status << std::endl;
                m_isRunning = false;
                return;
            }

            bool isSelfTurn = false;
            bool isOtherTurn = false;
            if (Configuration::getInstance().getSelfId() == response["turn"]) {
                isAllowedToMoveNow = true;
                isSelfTurn = true;
            }
            if (Configuration::getInstance().getOtherId() == response["turn"]) {
                isAllowedToMoveNow = false;
                isOtherTurn = true;
            }
            if (isSelfTurn == isOtherTurn || (isSelfTurn && !isAllowedToMoveNow) || (isOtherTurn && isAllowedToMoveNow)) {
                throw std::runtime_error("could not resolve turn");
            }
            if (!isAllowedToMoveNow) goto CONTINUE;
            return;
        }

        if (remainingRetries-- == 0) {
            std::cerr << "ERROR FETCHING GAME INFO" << std::endl;
            std::cerr << "Status code: " << r.status_code << std::endl;
            std::cerr << "Response: " << r.text << std::endl;
            m_isRunning = false;
            return;
        }
        CONTINUE: std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GameClient::move() {
    if (!isRunning()) return;

    State state(m_board, Configuration::getInstance().getSelfTeamRole());
    std::pair<int, int> move = m_agent.getMove(state);
    m_board[move.first-1][move.second-1] = Configuration::getInstance().getSelfTeamRole();
    std::cout << "Self  Move: " << move.first-1 << "," << move.second-1 << std::endl;

    JSON payload = {
        {"room_id", Configuration::getInstance().getRoomId()},
        {"match_id", m_matchId},
        {"status", m_status},
        {"size", m_size},
        {"board", m_board},
        {"turn", Configuration::getInstance().getSelfId()},
        {"move", m_board}
    };

    int remainingRetries = 3;
    while (isRunning()) {
        cpr::Response r = cpr::Post(
            cpr::Url{Configuration::getInstance().getRefereeServerAddress() + "/move"},
            cpr::Body{payload.dump()},
            cpr::Header{{"Content-Type", "application/json"}}
        );

        if (r.status_code == 200) {
            return;
        }

        if (remainingRetries-- == 0) {
            std::cerr << "ERROR MAKING MOVE" << std::endl;
            std::cerr << "Status code: " << r.status_code << std::endl;
            std::cerr << "Response: " << r.text << std::endl;
            m_isRunning = false;
            return;
        }
        CONTINUE: std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
