#ifndef BRYNET_NET_HTTPSERVER_H_
#define BRYNET_NET_HTTPSERVER_H_

#include <memory>
#include <any>

#include "WrapTCPService.h"
#include "HttpParser.h"
#include "NonCopyable.h"
#include "WebSocketFormat.h"

namespace brynet
{
    namespace net
    {
        class HttpServer;

        class HttpSession : public NonCopyable
        {
        public:
            typedef std::shared_ptr<HttpSession>    PTR;

            typedef std::function < void(const HTTPParser&, HttpSession::PTR) > HTTPPARSER_CALLBACK;
            typedef std::function < void(HttpSession::PTR, WebSocketFormat::WebSocketFrameType opcode, const std::string& payload) > WS_CALLBACK;

            typedef std::function < void(HttpSession::PTR) > CLOSE_CALLBACK;
            typedef std::function < void(HttpSession::PTR, const HTTPParser&) > WS_CONNECTED_CALLBACK;

        public:
            void                    setHttpCallback(HTTPPARSER_CALLBACK callback);
            void                    setCloseCallback(CLOSE_CALLBACK callback);
            void                    setWSCallback(WS_CALLBACK callback);
            void                    setWSConnected(WS_CONNECTED_CALLBACK callback);

            void                    send(const DataSocket::PACKET_PTR& packet, const DataSocket::PACKED_SENDED_CALLBACK& callback = nullptr);
            void                    send(const char* packet, size_t len, const DataSocket::PACKED_SENDED_CALLBACK& callback = nullptr);

            void                    postShutdown() const;
            void                    postClose() const;

            const std::any&         getUD() const;
            void                    setUD(std::any);

        protected:
            virtual ~HttpSession() = default;

        private:
            explicit HttpSession(TCPSession::PTR);

            static  PTR             Create(TCPSession::PTR session);

            TCPSession::PTR&        getSession();

            HTTPPARSER_CALLBACK&    getHttpCallback();
            CLOSE_CALLBACK&         getCloseCallback();
            WS_CALLBACK&            getWSCallback();
            WS_CONNECTED_CALLBACK&  getWSConnectedCallback();

        private:
            TCPSession::PTR         mSession;
            std::any                mUD;
            HTTPPARSER_CALLBACK     mHttpRequestCallback;
            WS_CALLBACK             mWSCallback;
            CLOSE_CALLBACK          mCloseCallback;
            WS_CONNECTED_CALLBACK   mWSConnectedCallback;

            friend class HttpServer;
        };

        class HttpServer : public NonCopyable, public std::enable_shared_from_this<HttpServer>
        {
        public:
            typedef std::shared_ptr<HttpServer> PTR;
            typedef std::function < void(HttpSession::PTR&) > ENTER_CALLBACK;

        public:
            static  PTR             Create();

            void                    startWorkThread(size_t workthreadnum, TcpService::FRAME_CALLBACK callback = nullptr);
            void                    startListen(bool isIPV6, const std::string& ip, int port, const char *certificate = nullptr, const char *privatekey = nullptr);

            WrapServer::PTR         getServer();
            void                    setEnterCallback(ENTER_CALLBACK callback);
            void                    addConnection(sock fd, 
                                                    ENTER_CALLBACK enterCallback,
                                                    HttpSession::HTTPPARSER_CALLBACK responseCallback,
                                                    HttpSession::WS_CALLBACK wsCallback = nullptr,
                                                    HttpSession::CLOSE_CALLBACK closeCallback = nullptr,
                                                    HttpSession::WS_CONNECTED_CALLBACK wsConnectedCallback = nullptr);
        private:
            HttpServer();
            virtual ~HttpServer();

            void                    handleHttp(HttpSession::PTR& httpSession);

        private:
            ENTER_CALLBACK          mOnEnter;
            WrapServer::PTR         mServer;
            ListenThread::PTR       mListenThread;
        };
    }
}

#endif