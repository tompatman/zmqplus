/*
 * IDataReqHandler.h
 *
 *  Created on: Jan 8, 2016
 *      Author: service
 */

#pragma once

#include <string>
#include "Poco/Exception.h"
#include <Poco/SAX/Attributes.h>

namespace ZMQ {

	class IDataResponder
	{
		public:
			virtual ~IDataResponder() { };

			virtual void buildDataResponse() = 0;

      virtual void buildDataResponse(const char *request)
      {
        throw Poco::RuntimeException("Not implemented: buildDataResponse(const char *request)");
      }

			virtual void buildDataResponse(const char *request, const Poco::XML::Attributes &attributes)
			{
				throw Poco::RuntimeException("Not implemented: buildDataResponse(request, attribs)");
			}

			virtual void buildTemplateResponse() = 0;

			virtual void buildSetResponse(const std::string strSetName) = 0;

			virtual void buildFailResponse( ) = 0;

	};
}


