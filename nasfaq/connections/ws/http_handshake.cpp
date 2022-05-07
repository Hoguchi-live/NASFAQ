#include "http_handshake.h"

namespace ws {
	namespace http_handshake {
		std::string get_sid() {
			static const char *POLLING_URL_0 = "https://nasfaq.biz/socket/?EIO=4&transport=polling&t=Ny7z439";
			static const char *POLLING_URL_1 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z472";
			static const char *POLLING_URL_2 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z4Bn&sid=";
			static const char *POLLING_URL_3 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z4Bp&sid=";
			static const char *POLLING_URL_4 = "https://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=polling&t=Ny7z4EU&sid=";
			nlohmann::json json_ret;
			std::string sid;

			nsfq_http::http_connector::connector c;

			/* Start handshake, first result is discarded. */
			c.get(POLLING_URL_0);
			c.get(POLLING_URL_1);

			/* Get session id for websocket. */
			json_ret = nlohmann::json::parse(c.get_buffer().substr(1));
			sid = json_ret["sid"];

			/* Post "40" acknowledgement with sid. */
			c.post(POLLING_URL_2 + sid, "40");

			/* Continue handshake (might be unneeded). See XHR trace. */
			//c.get(POLLING_URL_3 + sid);
			//c.get(POLLING_URL_4 + sid);

			return sid;
		}
	}

}
