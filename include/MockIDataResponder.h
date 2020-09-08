/*
 * MockIDataResponder.h
 *
 *  Created on: Feb 2, 2016
 *      Author: service
 */

#ifndef SIM_MOCKIDATARESPONDER_H_
#define SIM_MOCKIDATARESPONDER_H_

#include "gmock/gmock.h"  // Brings in Google Mock.
#include <stdint.h>
#include "IDataResponder.h"

namespace ZMQ {

class MockIDataResponder
        : public ZMQ::IDataResponder {
public:

    MOCK_METHOD0(buildDataResponse, void());

    MOCK_METHOD0(buildTemplateResponse, void());

    MOCK_METHOD1(buildSetResponse, void(
            const std::string strSetName));

    MOCK_METHOD0(buildFailResponse, void());

};

}


#endif /* SIM_MOCKIDATARESPONDER_H_ */
