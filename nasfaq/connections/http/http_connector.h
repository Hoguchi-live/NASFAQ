#ifndef _HTTP_CONNECTOR_H_
#define _HTTP_CONNECTOR_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

/**
  Some of these functions such as perform_request should be private
*/
namespace nsfq_http {
	namespace http_connector {

		class connector {
		public:
			connector();
			~connector();

			void set_uri(std::string);
			int perform_request();
			void clear_buffer();
			void get(std::string);
			void post(std::string, std::string);
			std::string get_buffer() const;
		private:
			int m_id;
			std::string m_uri;
		    	struct curl_slist *m_slist1;
			CURL *m_hdl;
			CURLcode m_result;
			long m_http_code;
			std::string m_response;
			std::string m_read_buffer;
		};
	}
}
#endif

