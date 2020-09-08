/*
 * IPublish.h
 *
 *  Created on: Dec 2, 2015
 *      Author: service
 */

#pragma once


#include <syslog.h>
#include "zCmdStruct.h"

namespace ZMQ {

	template<typename msgData>
	class IPublish {

	public:
			virtual ~IPublish() = default;

			void Init()
			{
        Init(30, LOG_INFO, "CmdPublish");
			};

			virtual void Init(const int queueSize, const int logLevel = LOG_INFO, const std::string strLogName = "Publisher") = 0;

		virtual bool Send(const msgData &newData) = 0;

  protected:


      //virtual bool Send(msgData &newData, Poco::UInt64 uiChan) = 0;

	};

}

