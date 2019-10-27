//
// Created by 贝克街的流浪猫 on 17/08/2018.
//

#ifndef RTC_SOCKETFACTORY_H
#define RTC_SOCKETFACTORY_H


#include <p2p/base/basicpacketsocketfactory.h>
#include <list>
#include <iterator>

namespace rtc {
    class SocketFactoryWrapper : public rtc::BasicPacketSocketFactory {
    public:
        SocketFactoryWrapper(rtc::Thread *thread, std::string white_private_ip_prefix, int min_port, int max_port, std::string key);

        rtc::AsyncPacketSocket *CreateUdpSocket(const rtc::SocketAddress &local_address,
                                                uint16_t min_port,
                                                uint16_t max_port) override;

        rtc::AsyncPacketSocket *CreateServerTcpSocket(const rtc::SocketAddress &local_address,
                                                      uint16_t min_port,
                                                      uint16_t max_port,
                                                      int opts) override;

        rtc::AsyncPacketSocket *CreateClientTcpSocket(const rtc::SocketAddress &local_address,
                                                      const rtc::SocketAddress &remote_address,
                                                      const rtc::ProxyInfo &proxy_info,
                                                      const std::string &user_agent,
                                                      int opts) override;

        rtc::AsyncPacketSocket *CreateClientTcpSocket(
                const rtc::SocketAddress &local_address,
                const rtc::SocketAddress &remote_address,
                const rtc::ProxyInfo &proxy_info,
                const std::string &user_agent,
                const rtc::PacketSocketTcpOptions &tcp_options) override;

        rtc::AsyncResolverInterface *CreateAsyncResolver() override;

    private:
        std::string white_private_ip_prefix;
        std::string key;
        int min_port;
        int max_port;

        SocketFactory *socket_factory();
    };
}

#endif //RTC_SOCKETFACTORY_H
