//
// Created by 贝克街的流浪猫 on 17/08/2018.
//

#include "socket_factory_wrapper.h"
#include "jni/jni_utils.h"
#include <rtc_base/asyncpacketsocket.h>

rtc::SocketFactoryWrapper::SocketFactoryWrapper(rtc::Thread *thread, std::string white_private_ip_prefix, int min_port,
                                                int max_port, std::string key)
        : rtc::BasicPacketSocketFactory(thread) {
    this->white_private_ip_prefix = std::move(white_private_ip_prefix);
    this->key = std::move(key);
    this->min_port = min_port;
    this->max_port = max_port;
}

rtc::AsyncPacketSocket *
rtc::SocketFactoryWrapper::CreateUdpSocket(const rtc::SocketAddress &local_address, uint16_t min_port,
                                           uint16_t max_port) {
    if (min_port < this->min_port || max_port > this->max_port) {
        WEBRTC_LOG(key + ": Create udp socket cancelled, port out of range, expect port range is:" +
                   std::to_string(this->min_port) + "->" + std::to_string(this->max_port)
                   + "parameter port range is: " + std::to_string(min_port) + "->" + std::to_string(max_port),
                   LogLevel::INFO);
        return nullptr;
    }
    if (!local_address.IsPrivateIP() || local_address.HostAsURIString().find(this->white_private_ip_prefix) == 0) {
        rtc::AsyncPacketSocket *result = BasicPacketSocketFactory::CreateUdpSocket(local_address, min_port, max_port);
        const auto *address = static_cast<const void *>(result);
        std::stringstream ss;
        ss << address;
        WEBRTC_LOG(key + ": Create udp socket, min port is:" + std::to_string(min_port) + ", max port is: " +
                   std::to_string(max_port) + ", result is: " + result->GetLocalAddress().ToString() + "->" +
                   result->GetRemoteAddress().ToString() + ", new socket address is: " + ss.str(), LogLevel::INFO);

        return result;
    } else {
        WEBRTC_LOG(key + ": Create udp socket cancelled, this ip is not in while list:" + local_address.HostAsURIString(),
                   LogLevel::INFO);
        return nullptr;
    }
}

rtc::AsyncPacketSocket *
rtc::SocketFactoryWrapper::CreateServerTcpSocket(const rtc::SocketAddress &local_address, uint16_t min_port,
                                                 uint16_t max_port, int opts) {
    if (min_port < this->min_port || max_port > this->max_port) {
        WEBRTC_LOG(key + ": Create server tcp socket cancelled, port out of range, expect port range is:" +
                   std::to_string(this->min_port) + "->" + std::to_string(this->max_port)
                   + "parameter port range is: " + std::to_string(min_port) + "->" + std::to_string(max_port),
                   LogLevel::INFO);
        return nullptr;
    }
    if (!local_address.IsPrivateIP() || local_address.HostAsURIString().find(this->white_private_ip_prefix) == 0) {
        rtc::AsyncPacketSocket *result = BasicPacketSocketFactory::CreateServerTcpSocket(local_address, min_port,
                                                                                         max_port,
                                                                                         opts);
        const auto *address = static_cast<const void *>(result);
        std::stringstream ss;
        ss << address;
        WEBRTC_LOG(key + ": Create server tcp socket, min port is:" + std::to_string(min_port) + ", max port is: " +
                   std::to_string(max_port) + ", result is: " + result->GetLocalAddress().ToString() + "->" +
                   result->GetRemoteAddress().ToString() + ", new socket address is: " + ss.str(), LogLevel::INFO);
        return result;
    } else {
        WEBRTC_LOG(key + ": Create server tcp socket cancelled, this ip is not in while list:" + local_address.HostAsURIString(),
                LogLevel::INFO);
        return nullptr;
    }
}

rtc::AsyncPacketSocket *rtc::SocketFactoryWrapper::CreateClientTcpSocket(const rtc::SocketAddress &local_address,
                                                                         const rtc::SocketAddress &remote_address,
                                                                         const rtc::ProxyInfo &proxy_info,
                                                                         const std::string &user_agent, int opts) {
    if (!local_address.IsPrivateIP() || local_address.HostAsURIString().find(this->white_private_ip_prefix) == 0) {
        rtc::AsyncPacketSocket *result = BasicPacketSocketFactory::CreateClientTcpSocket(local_address, remote_address,
                                                                                         proxy_info, user_agent, opts);
        const auto *address = static_cast<const void *>(result);
        std::stringstream ss;
        ss << address;
        WEBRTC_LOG(key + ": Create client tcp socket, " + result->GetLocalAddress().ToString() + "->" +
                   result->GetRemoteAddress().ToString() + ", new socket address is: " + ss.str(), LogLevel::INFO);
        return result;
    } else {
        WEBRTC_LOG(key + ": Create client tcp cancelled, this ip is not in while list:" + local_address.HostAsURIString(),
                   LogLevel::INFO);
        return nullptr;
    }
}

rtc::AsyncPacketSocket *rtc::SocketFactoryWrapper::CreateClientTcpSocket(
        const rtc::SocketAddress &local_address,
        const rtc::SocketAddress &remote_address,
        const rtc::ProxyInfo &proxy_info,
        const std::string &user_agent,
        const rtc::PacketSocketTcpOptions &tcp_options) {
    return CreateClientTcpSocket(local_address, remote_address, proxy_info, user_agent, tcp_options.opts);
}

rtc::AsyncResolverInterface *rtc::SocketFactoryWrapper::CreateAsyncResolver() {
    return BasicPacketSocketFactory::CreateAsyncResolver();
}




