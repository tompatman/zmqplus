/*
 * zmqBase.cpp
 *
 *  Created on: Mar 21, 2013
 *      Author: service
 */

#include "../include/zmqBase.h"

namespace ZMQ {
	zmqBase::zmqBase(zmq::context_t &zmq_context, int zmq_type, std::string &zmq_url)
			: _zmq_context(zmq_context), _zmq_socket(NULL), _zmq_type(zmq_type), _zmq_url(zmq_url) {

	}

	zmqBase::~zmqBase() {
		destroy_que();
	}

	static const char *zmq_que_names[
			ZMQ_XSUB + 1] = {"ZMQ_PAIR", "ZMQ_PUB", "ZMQ_SUB", "ZMQ_REQ", "ZMQ_REP", "ZMQ_DEALER", "ZMQ_ROUTER",
							 "ZMQ_PULL", "ZMQ_PUSH", "ZMQ_XPUB", "ZMQ_XSUB"};

	void zmqBase::setup_que(ppcLogger *plogger, int time_out_ms) {
		_timeout = time_out_ms;
		try
		{
			plogger->log(LG_DEBUG, "Creating %s, connect/bind to URL: %s", zmq_que_names[_zmq_type], _zmq_url.c_str());

			_zmq_socket = new zmq::socket_t(_zmq_context, _zmq_type);

			if (time_out_ms >= 0)
			{
				_zmq_socket->setsockopt(ZMQ_RCVTIMEO, &time_out_ms, sizeof(int));
				_zmq_socket->setsockopt(ZMQ_SNDTIMEO, &time_out_ms, sizeof(int));
			}

			switch (_zmq_type)
			{
				case ZMQ_SUB:
					_zmq_socket->connect(_zmq_url.c_str());
					_zmq_socket->setsockopt(ZMQ_SUBSCRIBE, NULL, 0);  // Subscribe to any message on this que
					break;

				case ZMQ_REQ:
					_zmq_socket->connect(_zmq_url.c_str());
					break;

				case ZMQ_REP:
				case ZMQ_PUB:
					_zmq_socket->bind(_zmq_url.c_str());
					break;

				default:
					throw Poco::PropertyNotSupportedException("ZMQ Type not supported");
			}
		}
		catch (std::exception &e)
		{
			char flt[128];
			sprintf(flt, "Failed ZMQ setup [%s]:[%s] ; %s", zmq_que_names[_zmq_type], _zmq_url.c_str(), e.what());
			plogger->log(LG_ERR, flt);
			throw Poco::RuntimeException(flt);
		}
	}

	void zmqBase::setMaxSendBuffer(Poco::UInt16 uiNumMsgs) {
		int numMsgs = uiNumMsgs;
		if (_zmq_socket == NULL)
		{
			throw Poco::IOException("ZMQ socket not yet created");
		}

		//Mesages above this number will be blocked
		_zmq_socket->setsockopt(ZMQ_SNDHWM, &numMsgs, sizeof(int));
	}

		void zmqBase::setMaxRecvBuffer(Poco::UInt16 uiNumMsgs) {
			int numMsgs = uiNumMsgs;
			if (_zmq_socket == NULL)
			{
				throw Poco::IOException("ZMQ socket not yet created");
			}

			//Mesages above this number will be blocked
			_zmq_socket->setsockopt(ZMQ_RCVHWM, &numMsgs, sizeof(int));
		}

	void zmqBase::destroy_que() {
		if (_zmq_socket != NULL)
		{
			_zmq_socket->close();
			delete _zmq_socket;
		}

	}


	bool zmqBase::recv_string(std::string *result) {
		zmq::message_t request;
		_zmq_socket->setsockopt(ZMQ_RCVTIMEO, &_timeout, sizeof(int));

		if (true == _zmq_socket->recv(&request))    // Waits until the timeout set in socket options
		{

			Poco::UInt64 len = request.size();

			if (len > (sizeof(_rcv_str) - 1))
			{
				throw Poco::InvalidAccessException("ZMQ string is too long.");
			}

			memset(_rcv_str, 0, len + 1);
			memcpy(_rcv_str, request.data(), len);

			*result = _rcv_str;

			return true;
		}

		return false;
	}

    bool zmqBase::send_xmlString(const char *str) {
      zmq::message_t reply(strlen(str));
      memcpy((void *) reply.data(), str, strlen(str));

		return _zmq_socket->send(reply);
	}


    bool zmqBase::send_string(const char *str) {
      zmq::message_t reply(strlen(str) + 1);
      memcpy((void *) reply.data(), str, strlen(str));

      return _zmq_socket->send(reply);
    }
}
