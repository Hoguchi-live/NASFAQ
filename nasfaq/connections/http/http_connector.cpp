#include "http_connector.h"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

//class http_connection {
//public:
//	http_connection(int id, CURL *hdl, std::string uri)
//		: m_id(id)
//		; m_hdl(hdl)
//		; m_uri(uri)
//		; m_response("N/A")
//	{}
//
//private:
//	int m_id;
//	CURL *m_hdl;
//	std::string m_uri;
//	CURLcode m_status;
//	std::string m_response;
//	std::vector<std::string> m_payloads;
//}
//
//class http_endpoint {
//public:
//	http_endpoint() {
//
//	}
//private:
//}

std::string get_sid()
{
	CURL *curl;
  	CURLcode res;

  	static const char *POLLING_URL_0 = "https://nasfaq.biz/socket/?EIO=4&transport=polling&t=Ny7z439";
	static const char *POLLING_URL_1 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z472";
  	static const char *POLLING_URL_2 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z4Bn&sid=";
	static const char *POLLING_URL_3 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z4Bp&sid=";
	static const char *POLLING_URL_4 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z4EU&sid=";

	long http_code = 0;
	std::string sid_final = "";
	std::string readBuffer = "";

  	curl = curl_easy_init();

    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

	/* Header */
    	struct curl_slist *slist1;
	slist1 = NULL;
  	slist1 = curl_slist_append(slist1 , "Cookie: holosesh=s%3AxmS8xBlQk4kH_rXQOaNjHk_3OuaBDsfA.M0yi%2BZmkiq%2BAmJBRj%2FNg9S%2BaSQpsfHRJcEeYVHLiKXg");
	slist1 =curl_slist_append(slist1, "referer : https://nasfaq.biz/market");
  	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);

  	if(curl) {

		/************** ZEROTH STAGE ********/
		readBuffer = "";
    	  	curl_easy_setopt(curl, CURLOPT_URL, POLLING_URL_0);
    	  	res = curl_easy_perform(curl);

		/* Check for result */
		if ( res != CURLE_ABORTED_BY_CALLBACK) {
    			std::cout << "Zeroth stage ok" << std::endl;
		}else {
    			std::cout << "Failure" << std::endl;
			return "";
		}


		/************** FIRST STAGE ********/
		readBuffer = "";
    	  	curl_easy_setopt(curl, CURLOPT_URL, POLLING_URL_1);

    	  	/* Perform the request, res will get the return code */
    	  	res = curl_easy_perform(curl);

		/* Check for result */
		if (res != CURLE_ABORTED_BY_CALLBACK) {
    			std::cout << "First stage ok" << std::endl;
		}else {
    			std::cout << "Failure" << std::endl;
			return "";
		}

		/* Get sid */
    	  	nlohmann::json r = nlohmann::json::parse(readBuffer.substr(1));
    	  	sid_final = r["sid"];


		/************** SECOND STAGE ********/
		readBuffer = "";
    	  	std::string polling_url_2 =  POLLING_URL_2 +  sid_final;

    		curl_easy_setopt(curl, CURLOPT_URL, polling_url_2.c_str());
    		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "40");
  		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    	  	/* Perform the request, res will get the return code */
    		res = curl_easy_perform(curl);

		/* Check for result */
		if (res != CURLE_ABORTED_BY_CALLBACK && readBuffer == "ok") {
    			std::cout << "Second stage ok" << std::endl;
		}else {
    			std::cout << "Failure" << std::endl;
			return "";
		}

		/************** THIRD STAGE ********/
		readBuffer = "";

		// Format next url
    	  	std::string polling_url_3 =  POLLING_URL_3 +  sid_final;

    	  	curl_easy_setopt(curl, CURLOPT_URL, polling_url_3.c_str());
  		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

    	  	/* Perform the request, res will get the return code */
    	  	res = curl_easy_perform(curl);


		/* Check for result */
		if (res != CURLE_ABORTED_BY_CALLBACK) {
    	  		nlohmann::json r2 = nlohmann::json::parse(readBuffer.substr(2));
    			std::cout << "Third stage ok" << std::endl;
		}else {
    			std::cout << "Failure" << std::endl;
			return "";
		}


		/************** FOURTH STAGE ********/
		readBuffer = "";
    	  	std::string polling_url_4 =  POLLING_URL_4 + sid_final;

    	  	curl_easy_setopt(curl, CURLOPT_URL, polling_url_4.c_str());
  		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

    	  	/* Perform the request, res will get the return code */
    	  	res = curl_easy_perform(curl);

		/* Check for result */
		if ( res != CURLE_ABORTED_BY_CALLBACK) {
    			std::cout << "Fourth stage ok" << std::endl;
		}else {
    			std::cout << "Failure" << std::endl;
			return "";
		}



    		/* Check for errors */
    		if(res != CURLE_OK)
    		  fprintf(stderr, "curl_easy_perform() failed: %s\n",
    		          curl_easy_strerror(res));

    		/* always cleanup */
    		curl_easy_cleanup(curl);
  }

  return sid_final;
}
