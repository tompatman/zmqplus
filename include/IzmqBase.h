//
// Created by service on 4/12/16.
//

#ifndef MASTERPROJECT_IZMQBASE_H_H
#define MASTERPROJECT_IZMQBASE_H_H

#include <core/ppcLogger.h>
#include "zmq.hpp"

namespace ZMQ {
    class IzmqBase {
    public:
        virtual ~IzmqBase() { };

        // Thows Poco based exepctions on failure.
        virtual void setup_que(ppcLogger *plogger, int time_out_ms = -1) = 0;

        // Returns true if successfull with the result containing the string
        // Returns false otherwise and does not touch the result string
        virtual bool recv_string(std::string *result) = 0;

        // Sends a null terminated string
        virtual bool send_xmlString(const char *str) = 0;

        virtual bool send_string(const char *str) = 0;

        //Sets the maximum number of messages that can be queued for send.
        //This helps to prevent data from becoming stale
        virtual void setMaxSendBuffer(Poco::UInt16 uiNumMsgs) = 0;

    protected:


    };
}

#endif //MASTERPROJECT_IZMQBASE_H_H
