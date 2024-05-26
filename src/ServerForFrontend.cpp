#include "ServerForFrontend.h"
#include <string>

struct RunConfiguration {
    httplib::Server* m_server;
    std::string m_host;
    int m_port;
};

void RUN_SERVER(void* arg) {
    RunConfiguration* config = reinterpret_cast<RunConfiguration*>(arg);
    config->m_server->listen(config->m_host, config->m_port);
    delete config;
}

ServerForFrontend::ServerForFrontend()
    : m_server{}, m_serverThread{}, m_host{"localhost"}, m_port{1234}
{
    m_server.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello, World!", "text/plain");
    });
}

ServerForFrontend::~ServerForFrontend() {
    stopListening();
}

std::string ServerForFrontend::getServerAddress() const {
    return std::string("http://") + m_host + std::string(":") + std::to_string(m_port);
}

void ServerForFrontend::startListening() {
    if (m_server.is_running()) {
        return;
    }
    RunConfiguration* configPtr = new RunConfiguration{ &m_server, m_host, m_port };
    m_serverThread = std::thread(RUN_SERVER, reinterpret_cast<void*>(configPtr));
}

void ServerForFrontend::stopListening() {
    if (m_server.is_running()) {
        m_server.stop();
        if (true == m_serverThread.joinable()) {
            m_serverThread.join();
        }
    }

    m_serverThread = std::thread{};
}
