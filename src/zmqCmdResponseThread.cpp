///////////////////////////////////////////////////////////
//  IOCmdHandler.cpp
//  Implementation of the Class IOCmdHandler
//  Created on:      06-Mar-2013 2:25:49 PM
//  Original author: gmorehead
///////////////////////////////////////////////////////////

#include "../include/zmqCmdResponseThread.h"

namespace ZMQ {
	zmqCmdResponseThread::zmqCmdResponseThread(std::string &name, zmq::context_t &zmq_context, std::string &zmq_url)
			: ppcRunnable(name.c_str(), 50), zmqBase(zmq_context, ZMQ_REP, zmq_url) {
	}


	zmqCmdResponseThread::~zmqCmdResponseThread() {

	}


	void zmqCmdResponseThread::run_thread() {

		setup_que(_logRun, 500);    // 500mS timeout on wait for cmd.

		while (!_done)
		{
			process();
		}
	}

	void zmqCmdResponseThread::process() {

		try        // Handle the message ques.
		{
			string res_str;

			if (true == recv_string(&res_str))
			{
				_reply_required = 1;

				_logRun->log(LG_DEBUG, "Received %s", res_str.c_str());

				process_new_string(res_str);

				if (_reply_required != 0)
				{
					_logRun->log(LG_ERR, "%s : Failed to send response on que, send a FAILED message", __FILE__);
					send_string_reponse("FAILED");
				}
			}
			else
			{
				_logRun->log(LG_VERBOSE, "Completed iteration for Thread %s (zmq errno = %d)", _instance_name.c_str(),
						  zmq_errno());
			}
		}
		catch (Poco::Exception &e)
		{
			string response(_instance_name + " [zmqCRT] : " + e.displayText());

			_logRun->log(LG_ERR, e.displayText());

			if (_reply_required > 0)
				send_string_reponse(response.c_str());
		}
		catch (std::exception &e)
		{
			string response(_instance_name + " [zmqCRT] : FAILED : Unable to handle command : " + e.what());

			_logRun->log(LG_ERR, response.c_str());

			if (_reply_required > 0)
				send_string_reponse(response.c_str());
		}

	}


	bool zmqCmdResponseThread::send_string_reponse(const char *response_str) {
		if (_reply_required != 1)
			throw Poco::LogicException("send_string_reponse() : _reply_required not set!");

		_reply_required = 0;

    return send_xmlString(response_str);
	}

	bool zmqCmdResponseThread::send_binary_response(void *bin_obj, int size) {
		if (_reply_required != 1)
			throw Poco::LogicException("send_binary_response() : _reply_required not set!");

		_reply_required = 0;

		return _zmq_socket->send(bin_obj, size);
	}

	bool zmqCmdResponseThread::send(zmq::message_t &msg) {
		if (_reply_required != 1)
			throw Poco::LogicException("send_binary_response() : _reply_required not set!");

		_reply_required = 0;

		return _zmq_socket->send(msg);
	}

}