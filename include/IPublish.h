/*
 * IPublish.h
 *
 *  Created on: Dec 2, 2015
 *      Author: service
 */

#pragma once


#include "zCmdStruct.h"

namespace ZMQ {

	template<typename msgData>
	class IPublish {

	public:
			virtual ~IPublish() = default;

			void Init()
			{
				Init(30);
			};

			virtual void Init(int queueSize) = 0;

		virtual bool Send(const msgData &newData) = 0;

  protected:


      //virtual bool Send(msgData &newData, Poco::UInt64 uiChan) = 0;

	};

}

