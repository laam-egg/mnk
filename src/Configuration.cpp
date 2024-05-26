#include "Configuration.h"
#include <libenvpp/env.hpp>
#include <dotenv.h>

Configuration& Configuration::getInstance() {
    static Configuration instance = makeInstance();
    return instance;
}

Configuration Configuration::makeInstance() {
    dotenv::init(dotenv::Preserve);

    auto envPrefix = env::prefix("APP");

    auto const frontendServerHostName = envPrefix.register_required_variable<std::string>("FRONTEND_SERVER_HOST");
    auto const frontendServerPort = envPrefix.register_required_variable<unsigned int>("FRONTEND_SERVER_PORT");
    auto const refereeServerAddress = envPrefix.register_required_variable<std::string>("REFEREE_SERVER_ADDRESS");

    auto const roomId = envPrefix.register_required_variable<std::string>("ROOM_ID");
    auto const selfId = envPrefix.register_required_variable<std::string>("SELF_ID");
    auto const otherId = envPrefix.register_required_variable<std::string>("OTHER_ID");
    auto const selfTeamRole = envPrefix.register_required_variable<std::string>("SELF_TEAM_ROLE");

    auto const parsedAndValidatedPrefix = envPrefix.parse_and_validate();

    if (!parsedAndValidatedPrefix.ok()) {
        std::cerr << "ERROR PARSING AND VALIDATING ENVIRONMENT VARIABLES:" << std::endl;
        std::cerr << parsedAndValidatedPrefix.warning_message() << std::endl;
        std::cerr << parsedAndValidatedPrefix.error_message() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return Configuration(
        parsedAndValidatedPrefix.get(frontendServerHostName),
        parsedAndValidatedPrefix.get(frontendServerPort),
        parsedAndValidatedPrefix.get(refereeServerAddress),
        parsedAndValidatedPrefix.get(roomId),
        parsedAndValidatedPrefix.get(selfId),
        parsedAndValidatedPrefix.get(otherId),
        parsedAndValidatedPrefix.get(selfTeamRole)
    );
}
