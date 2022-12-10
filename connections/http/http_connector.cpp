#include "http_connector.h"

namespace nsfq_http {
	namespace http_connector {

		/**
		  Make curl return strings instead of curl objects.
		*/
		static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
		{
		    ((std::string*)userp)->append((char*)contents, size * nmemb);
		    return size * nmemb;
		}

		connector::connector() {
			m_hdl = curl_easy_init();
		 	m_uri = "";
		 	m_response = "N/A";
		 	m_read_buffer = "";

			/* Basic buffers and functions. */
			curl_easy_setopt(m_hdl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(m_hdl, CURLOPT_WRITEDATA, &m_read_buffer);
			curl_easy_getinfo(m_hdl, CURLINFO_RESPONSE_CODE, &m_http_code);

			/* Header definition. TODO: cookie should be handled safely? */
			m_slist1 = NULL;
			m_slist1 = curl_slist_append(m_slist1 , "Cookie: holosesh=s%3aSQpsfHRJcEeYVHLiKXg");
			m_slist1 = curl_slist_append(m_slist1, "referer : https://nasfaq.biz/market");
			curl_easy_setopt(m_hdl, CURLOPT_HTTPHEADER, m_slist1);
		}

		connector::~connector() {
			curl_easy_cleanup(m_hdl);
		}

		void connector::set_uri(std::string uri) {
			/* TODO: check for url sanity. */
			m_uri = uri;
		  	curl_easy_setopt(m_hdl, CURLOPT_URL, m_uri.c_str());
		}

		int connector::perform_request() {
		  	m_result = curl_easy_perform(m_hdl);

			if (m_result == CURLE_OK) {
				return 0;
			}else {
				return -1;
			}
		}

		void connector::clear_buffer() {
			m_read_buffer = "";
		}

		void connector::get(std::string url) {
			clear_buffer();
			set_uri(url);
			curl_easy_setopt(m_hdl, CURLOPT_CUSTOMREQUEST, "GET");

			if(perform_request() == -1) {
				std::cout << "> Error occured during GET with url " << m_uri
					  << " CURLcode: " << curl_easy_strerror(m_result)
					  << " HTTP response: " << m_http_code << std::endl;
			};
		}

		void connector::post(std::string url, std::string payload) {
			clear_buffer();
			set_uri(url);
			curl_easy_setopt(m_hdl, CURLOPT_CUSTOMREQUEST, "POST");
			curl_easy_setopt(m_hdl, CURLOPT_POSTFIELDS, payload.c_str());

			if(perform_request() == -1) {
				std::cout << "> Error occured during POST with url " << m_uri
					  << " Payload: " << payload
					  << " CURLcode: " << curl_easy_strerror(m_result)
					  << " HTTP response: " << m_http_code << std::endl;
			};
		}

		std::string connector::get_buffer() const {
			return m_read_buffer;
		}

		//class http_connector {
		//public:
		//	http_connector() {
		//		m_hdl = curl_easy_init();
		//	 	m_uri = "";
		//	 	m_response = "N/A";
		//	 	m_read_buffer = "";
		//
		//		/* Basic buffers and functions. */
		//    		curl_easy_setopt(m_hdl, CURLOPT_WRITEFUNCTION, WriteCallback);
		//    		curl_easy_setopt(m_hdl, CURLOPT_WRITEDATA, &m_read_buffer);
		//		curl_easy_getinfo(m_hdl, CURLINFO_RESPONSE_CODE, &m_http_code);
		//
		//		/* Header definition. TODO: cookie should be handled safely? */
		//		m_slist1 = NULL;
		//  		m_slist1 = curl_slist_append(m_slist1 , "Cookie: holosesh=s%3AxmS8xBlQk4kH_rXQOaNjHk_3OuaBDsfA.M0yi%2BZmkiq%2BAmJBRj%2FNg9S%2BaSQpsfHRJcEeYVHLiKXg");
		//		m_slist1 = curl_slist_append(m_slist1, "referer : https://nasfaq.biz/market");
		//  		curl_easy_setopt(m_hdl, CURLOPT_HTTPHEADER, m_slist1);
		//	}
		//
		//	~http_connector() {
		//    		curl_easy_cleanup(m_hdl);
		//	}
		//
		//	void set_uri(std::string uri) {
		//		/* TODO: check for url sanity. */
		//		m_uri = uri;
		//    	  	curl_easy_setopt(m_hdl, CURLOPT_URL, m_uri.c_str());
		//	}
		//
		//	int perform_request() {
		//    	  	m_result = curl_easy_perform(m_hdl);
		//
		//		if (m_result == CURLE_OK) {
		//			return 0;
		//		}else {
		//			return -1;
		//		}
		//	}
		//
		//	void clear_buffer() {
		//		m_read_buffer = "";
		//	}
		//
		//	void get(std::string url) {
		//		clear_buffer();
		//		set_uri(url);
		//		curl_easy_setopt(m_hdl, CURLOPT_CUSTOMREQUEST, "GET");
		//
		//  		if(perform_request() == -1) {
		//			std::cout << "> Error occured during GET with url " << m_uri
		//				  << " CURLcode: " << curl_easy_strerror(m_result)
		//				  << " HTTP response: " << m_http_code << std::endl;
		//		};
		//	}
		//
		//	void post(std::string url, std::string payload) {
		//		clear_buffer();
		//		set_uri(url);
		//		curl_easy_setopt(m_hdl, CURLOPT_CUSTOMREQUEST, "POST");
		//    		curl_easy_setopt(m_hdl, CURLOPT_POSTFIELDS, payload.c_str());
		//
		//  		if(perform_request() == -1) {
		//			std::cout << "> Error occured during POST with url " << m_uri
		//				  << " Payload: " << payload
		//				  << " CURLcode: " << curl_easy_strerror(m_result)
		//				  << " HTTP response: " << m_http_code << std::endl;
		//		};
		//	}
		//
		//	std::string get_buffer() const {
		//		return m_read_buffer;
		//	}
		//private:
		//	int m_id;
		//	std::string m_uri;
		//    	struct curl_slist *m_slist1;
		//	CURL *m_hdl;
		//	CURLcode m_result;
		//	long m_http_code;
		//	std::string m_response;
		//	std::string m_read_buffer;
		//};
	}
}
