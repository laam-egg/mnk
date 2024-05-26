#ifndef Configuration_INCLUDED
#define Configuration_INCLUDED

#include <iostream>
#include <cstdlib>

class Configuration {
public:
    static Configuration& getInstance();
    ~Configuration() = default;

    unsigned int getFrontendServerPort() const                      { return m_frontendServerPort; }
    std::string const& getFrontendServerHostName() const            { return m_frontendServerHostName; }
    std::string const& getRefereeServerAddress() const              { return m_refereeServerAddress; }
    
    std::string const& getRoomId() const                            { return m_roomId; }
    std::string const& getSelfId() const                            { return m_selfId; }
    std::string const& getOtherId() const                           { return m_otherId; }
    std::string const& getSelfTeamRole() const                      { return m_selfTeamRole; }

    void setRoomId(std::string const& roomId)                       { m_roomId = roomId; }
    void setSelfId(std::string const& selfId)                       { m_selfId = selfId; }
    void setOtherId(std::string const& otherId)                     { m_otherId = otherId; }
    void setSelfTeamRole(std::string const& selfTeamRole)           { m_selfTeamRole = selfTeamRole; }

private:
    // Required variables
    std::string m_frontendServerHostName;
    unsigned int m_frontendServerPort;

    std::string m_refereeServerAddress;

    std::string m_roomId;
    std::string m_selfId;
    std::string m_otherId;
    std::string m_selfTeamRole;

    static Configuration makeInstance();

    Configuration(
        std::string frontendServerHostName,
        unsigned int frontendServerPort,
        std::string refereeServerAddress,
        std::string roomId,
        std::string selfId,
        std::string otherId,
        std::string selfTeamRole
    ) :
        m_frontendServerHostName(frontendServerHostName),
        m_frontendServerPort(frontendServerPort),
        m_refereeServerAddress(refereeServerAddress),
        m_roomId(roomId),
        m_selfId(selfId),
        m_otherId(otherId),
        m_selfTeamRole(selfTeamRole)
    {}
};

#endif // Configuration_INCLUDED
