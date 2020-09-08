//
// Created by service on 4/12/16.
//

#ifndef MASTERPROJECT_MOCKZMQBASE_H
#define MASTERPROJECT_MOCKZMQBASE_H

#include <gmock/gmock.h>
#include "IzmqBase.h"
#include <core/ppcLogger.h>
#include <Poco/Types.h>

class MockzmqBase : public ZMQ::IzmqBase{
 public:

	MOCK_METHOD2(setup_que, void(ppcLogger *, int));

	// Returns true if successfull with the result containing the string
	// Returns false otherwise and does not touch the result string
	MOCK_METHOD1(recv_string, bool (std::string *));

	// Sends a null terminated string
    MOCK_METHOD1(send_xmlString, bool(
            const char *));

	//Sets the maximum number of messages that can be queued for send.
	//This helps to prevent data from becoming stale
	MOCK_METHOD1(setMaxSendBuffer, void (Poco::UInt16 uiNumMsgs));


};

#endif //MASTERPROJECT_MOCKZMQBASE_H
