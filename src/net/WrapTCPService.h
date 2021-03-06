#ifndef BRYNET_NET_WRAPTCPSERVICE_H_
#define BRYNET_NET_WRAPTCPSERVICE_H_

#include <string>
#include <cstdint>

#include "TCPService.h"
#include "NonCopyable.h"

namespace brynet
{
    namespace net
    {
        class WrapServer;

        /*  以智能指针对象为网络会话标识的网络服务   */
        class TCPSession : public NonCopyable
        {
        public:
            typedef std::shared_ptr<TCPSession>     PTR;
            typedef std::weak_ptr<TCPSession>       WEAK_PTR;

            typedef std::function<void(TCPSession::PTR&)>   CLOSE_CALLBACK;
            typedef std::function<size_t(TCPSession::PTR&, const char*, size_t)>   DATA_CALLBACK;

        public:
            const std::any&         getUD() const;
            void                    setUD(std::any ud);

            const std::string&      getIP() const;
            TcpService::SESSION_TYPE    getSocketID() const;

            void                    send(const char* buffer, size_t len, DataSocket::PACKED_SENDED_CALLBACK callback = nullptr) const;
            void                    send(DataSocket::PACKET_PTR packet, DataSocket::PACKED_SENDED_CALLBACK callback = nullptr) const;

            void                    postShutdown() const;
            void                    postClose() const;

            void                    setCloseCallback(CLOSE_CALLBACK callback);
            void                    setDataCallback(DATA_CALLBACK callback);

        private:
            TCPSession() noexcept;
            virtual ~TCPSession() noexcept;

            void                    setSocketID(TcpService::SESSION_TYPE id);
            void                    setIP(const std::string& ip);

            void                    setService(TcpService::PTR& service);

            CLOSE_CALLBACK&         getCloseCallback();
            DATA_CALLBACK&          getDataCallback();

            static  PTR             Create();

        private:
            TcpService::PTR             mService;
            TcpService::SESSION_TYPE    mSocketID;
            std::string                 mIP;
            std::any                    mUD;

            CLOSE_CALLBACK              mCloseCallback;
            DATA_CALLBACK               mDataCallback;

            friend class WrapServer;
        };

        class WrapServer : public NonCopyable
        {
        public:
            typedef std::shared_ptr<WrapServer> PTR;
            typedef std::weak_ptr<WrapServer>   WEAK_PTR;

            typedef std::function<void(TCPSession::PTR&)>   SESSION_ENTER_CALLBACK;

            WrapServer() noexcept;
            virtual ~WrapServer() noexcept;

            TcpService::PTR&        getService();
            void                    startWorkThread(size_t threadNum, TcpService::FRAME_CALLBACK callback = nullptr);
            void                    addSession(sock fd, 
                                                const SESSION_ENTER_CALLBACK& userEnterCallback, 
                                                bool isUseSSL, 
                                                size_t maxRecvBufferSize, 
                                                bool forceSameThreadLoop = false);

        private:
            TcpService::PTR         mTCPService;
        };
    }
}

#endif