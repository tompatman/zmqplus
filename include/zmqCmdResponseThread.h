///////////////////////////////////////////////////////////
//  IOCmdHandler.h
//  Implementation of the Class IOCmdHandler
//  Created on:      06-Mar-2013 2:25:49 PM
//  Original author: gmorehead
///////////////////////////////////////////////////////////

#if !defined(ZMQCMDRESPONSETHREAD)
#define ZMQCMDRESPONSETHREAD

#include "core/ppcRunnable.h"
#include "core/ppcLogger.h"
#include "zmq/zmqBase.h"
#include "Poco/AtomicCounter.h"

using Poco::AtomicCounter;

namespace ZMQ {
	class zmqCmdResponseThread : public ppcRunnable, private zmqBase {

	public:
		zmqCmdResponseThread(std::string &name, zmq::context_t &zmq_context, std::string &zmq_url);

		virtual ~zmqCmdResponseThread();

		// Callback to process the received string
		// * This function must call send_string_reponse or send_binary_response!!
		// * This function should throw a Poco based exception on any failure
		virtual void process_new_string(const string recv_string) = 0;

		bool send_string_reponse(const char *response_str);

		bool send_binary_response(void *bin_obj, int size);

		bool send(zmq::message_t &msg);

		// Helper functions

	private:
		AtomicCounter _reply_required;

		void run_thread();

		void process();

	};
}
#endif // !defined(ZMQCMDRESPONSETHREAD)
