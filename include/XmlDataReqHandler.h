
#pragma once

#include "core/ppcRunnable.h"
#include "zmq/zmqBase.h"

#include "Poco/Mutex.h"
#include "Poco/AtomicCounter.h"
#include "Poco/SAX/SAXParser.h"
#include "ISubscriptionUpdater.h"
#include "TagParser.h"

using Poco::Mutex;
using Poco::AtomicCounter;

/**
 * This is the standard implementation of the data request handler.
 * In simulation mode a different implementation is used to provided simulated data instead of modbus data
 */
namespace ZMQ {

class XmlDataReqHandler : public ppcRunnable {
public:
    XmlDataReqHandler(ZMQ::ISubscriptionUpdater &updater, ZMQ::TagParser &dataHandler, const char *thread_name,
                      int comm_timeout_ms = 5000, int log_level = LG_INFO, int thread_rate = 25);

    XmlDataReqHandler(ISubscriptionUpdater &updater, TagParser &dataHandler, const Poco::UInt16 watchdogCmd_pushpull_port, const char *thread_name,
                      zmq::context_t &context, string local_ip_bind,
                      int comm_timeout_ms = 5000, int log_level = LG_INFO, int thread_rate = 25);

    virtual ~XmlDataReqHandler();


    void process();

private:

    Poco::Int32 _comm_timeout_ms;

    ZMQ::TagParser &_dataHandler;
    Poco::XML::SAXParser _parser;
    ZMQ::ISubscriptionUpdater &_updater;

    void init();
};
}


