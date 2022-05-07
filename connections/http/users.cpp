#include "users.h"

namespace users {
	std::vector<user_t> get_users() {
		nlohmann::json j;
		user_t user_tmp;
		std::vector<user_t> rop;

		static const char *LEADERBOARD_URL = "https://nasfaq.biz/api/getLeaderboard?leaderboard";
		nsfq_http::http_connector::connector c;

		/* Start handshake, first result is discarded. */
		c.get(LEADERBOARD_URL);

		j = nlohmann::json::parse(c.get_buffer())["leaderboard"]["leaderboard"];

		for(auto const & element:j) {
			user_tmp.userid = element["userid"];
			user_tmp.username = element["username"];
			user_tmp.icon = element["icon"];
			user_tmp.networth = element["networth"];

			rop.push_back(user_tmp);
		}

		return rop;
	}
}
