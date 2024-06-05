#include "shared.h"
#include "Agent.h"

#include <utility>      // std::pair
#include <vector>
#include <string>
#include <cstring>

extern "C" int getMove(
    char const* const flattenedBoard,
    int N,
    char const teamRole,
    int const lineLengthToWin,
    bool const moveFirst,
    float const analyzeTimeInSeconds,
    int* const outNumberOfSimulationsPerformed,
    int* const outMoveRow,
    int* const outMoveCol,
    char* const outErrorMessage
) {
    if (flattenedBoard == nullptr) {
        strcpy(outErrorMessage, "The flattened board is null.");
        return EXIT_FAILURE;
    }

    std::vector<std::vector<std::string>> refereeBoard(N, std::vector<std::string>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            refereeBoard[i][j] = flattenedBoard[i * N + j];
        }
    }

    try {
        Agent agent{};
        std::pair<int, int> move = agent.getMove(
            State(refereeBoard, std::string(1, teamRole), lineLengthToWin, moveFirst),
            analyzeTimeInSeconds,
            outNumberOfSimulationsPerformed
        );
        *outMoveRow = move.first - 1;
        *outMoveCol = move.second - 1;
    } catch (std::exception const& e) {
        strcpy(outErrorMessage, e.what());
        return EXIT_FAILURE;
    }

    strcpy(outErrorMessage, "");
    return EXIT_SUCCESS;
}
