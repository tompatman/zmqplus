/*
 * zmqBase.h
 *
 *  Created on: Mar 21, 2013
 *      Author: service
 */

#ifndef ZMQBASE_H_ASCD_1F5Y
#define ZMQBASE_H_ASCD_1F5Y

#include "zmq.hpp"
#include "core/ppcLogger.h"
#include "IzmqBase.h"

using std::string;

namespace ZMQ {
    class zmqBase : public IzmqBase {
    public:
        zmqBase(zmq::context_t &zmq_context, int zmq_type, std::string &zmq_url);

        virtual ~zmqBase();

        //Implemented virtuals

        // Thows Poco based exepctions on failure.
        void setup_que(ppcLogger *plogger, int time_out_ms = -1);

        // Returns true if successfull with the result containing the string
        // Returns false otherwise and does not touch the result string
        bool recv_string(std::string * result);

        // Sends a null terminated string
        bool send_xmlString(const char *str);

        //Send an XML string that doesn't include a null terminator, including the null terminator
        //renders a garbage character on the recieve side for an unknown reason.
        bool send_string(const char *str);

        //Sets the maximum number of messages that can be queued for send.
        //This helps to prevent data from becoming stale
        void setMaxSendBuffer(Poco::UInt16 uiNumMsgs);

        /**
         * Sets the maximum number of receive messages so that not too much old data can be stored
         */
        void setMaxRecvBuffer(Poco::UInt16 uiNumMsgs);

          protected:
        zmq::context_t &_zmq_context;
        zmq::socket_t *_zmq_socket;
        int _zmq_type;
        std::string _zmq_url;
        char _rcv_str[4096];
        int _timeout;


    private:
        void destroy_que();

    };

}
#endif /* ZMQBASE_H_ASCD_1F5Y */
