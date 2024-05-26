#ifndef Application_INCLUDED
#define Application_INCLUDED

#include "ServerForFrontend.h"

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    void startFrontendServer();
    void stopFrontendServer();
    
    ServerForFrontend m_serverForFrontend;
};

#endif // Application_INCLUDED
