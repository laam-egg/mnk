#ifndef ServerForFrontend_INCLUDED
#define ServerForFrontend_INCLUDED

#include <httplib.h>
#include <thread>

class ServerForFrontend {
private:
    httplib::Server m_server;
    std::thread m_serverThread;
    std::string m_host;
    int m_port;

public:
    ServerForFrontend();

    void startListening();

    std::string getServerAddress() const;

    void stopListening();

    ~ServerForFrontend();
};

#endif // ServerForFrontend_INCLUDED
