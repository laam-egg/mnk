# mnk

## Introduction

A C++ implementation of the Monte Carlo Tree Search
algorithm to solve [m,n,k-games](https://en.wikipedia.org/wiki/M,n,k-game).

This app currently only accepts games with `m = n`.

The app will communicate with [this referee server](https://github.com/nnphuc/UET_AICaroGame).
Specifically tested to work properly with [commit f0bf3d6 of the referee server](https://github.com/nnphuc/UET_AICaroGame/commit/f0bf3d63c7b132a9fb10627199a8f124d97ee24e).

If you want to test the algorithm without relying
on the referee server, you may want to only read
the code of the class Agent (files `include/Agent.h`
and `src/Agent.cpp`).

## Platforms

Windows, Linux.

This app has not been tested on MacOS yet, but it should
compile and run without problems.

## How to Run

### Configure

Your system must have CMake installed. A build tool
such as Ninja is recommended.

First, `cd` to project root and run CMake configure.
In case you use Ninja:

```sh
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -S. -B./build -G Ninja
```

Replace `CMAKE_C_COMPILER` path with your desired
compiler if necessary.

### Compile

After configuration, you no longer need to
run CMake configure again. Whenever you want
to compile the project (e.g. after code changes),
simply run the following command:

```sh
cmake --build ./build --config Debug --target all
```

and the compiled executable file should be in
`<project_root>/bin`.

### Run

Be aware of environment variables! The program
can read environment variables from a `.env` file
in the `bin` folder.

You can copy content of the `bin/.env.example` file
to your newly created `bin/.env` file, then tweak
the parameters. Save the `.env` file and run the
executable.

```sh
./bin/main
```

On Linux and MacOS, you can also specify the environment
variables directly on the command line like this:

```sh
APP_MCTS_SECONDS_PER_MOVE=15.0 APP_ROOM_ID=2 ./bin/main
```

which will override the environment variables declared
in `.env`.

On Windows you may use the `set` command to achieve
the same thing.

## License

[The Unlicense](./LICENSE.txt).
