//
// Created by talexander on 3/8/19.
//

#pragma once

#include <core/ppcRunnable.h>
#include <core/ppcLogger.h>

namespace ZMQ {
class PubSubProxy
        : public ppcRunnable {

public:
    PubSubProxy(const Poco::UInt32 frontEndPort, const Poco::UInt32 backEndPort, const Poco::UInt32 controlPort, const std::string strBindIp, zmq::context_t &context);

    ~PubSubProxy();

    void close();

    void exiting() override;

    void process() override;

private:

    const Poco::UInt32 _frontEndPort, _backEndPort, _controlPort;
    const std::string _strBindIp;

    std::string _strFrontPath;
    std::string _strBackPath;
    std::string _strControlPath;


    zmq::context_t &_zmqContext;
    ppcLogger *_log = nullptr;
    bool _bClosed = false;
};


}