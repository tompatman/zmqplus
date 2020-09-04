//
// Created by service on 6/17/16.
//

#ifndef MASTERPROJECT_MOCKPUBLISH_H_H
#define MASTERPROJECT_MOCKPUBLISH_H_H

#include <gmock/gmock.h>
#include "IPublish.h"
#include <Poco/Types.h>

namespace ZMQ {

		template<typename msgData>
		class MockPublish : public ZMQ::IPublish <msgData> {
		public:

				MOCK_METHOD1( Init, void (const Poco::UInt16 queueSize) );

				MOCK_METHOD1_T( Send, bool (const msgData &newData ) );

				MOCK_METHOD2_T( Send, bool (msgData &newData , Poco::UInt16 uiChan));

		};

}
#endif //MASTERPROJECT_MOCKPUBLISH_H_H
