#include "Application.h"
#include "Configuration.h"
#include "GameClient.h"
#include <libenvpp/env.hpp>
#include <iostream>

Application::Application()
    : m_serverForFrontend{}
{
    Configuration::getInstance(); // initialize configuration
}

Application::~Application() {
}

void Application::run() {
    startFrontendServer();
    GameClient gameClient;
    gameClient.run();
    stopFrontendServer();
}

void Application::startFrontendServer() {
    m_serverForFrontend.startListening();
    std::cout << std::unitbuf << "Frontend Server is running on " << m_serverForFrontend.getServerAddress() << "\n";
}

void Application::stopFrontendServer() {
    std::cout << std::unitbuf << "Stopping Frontend Server...\n";
    m_serverForFrontend.stopListening();
}
