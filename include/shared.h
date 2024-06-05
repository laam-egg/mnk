#ifndef shared
#define shared

#include "mnks_export.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

extern "C" int MNKS_EXPORT getMove(
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
);

#endif // shared
