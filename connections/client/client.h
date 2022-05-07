#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "../common/common.h"
#include "../ws/ssl_ws.h"

namespace proxy {
	/**
	  NASFAQ proxy client.
	*/
	class client {
	public:
		client(void);
		~client(void);

		void init(void);
	private:
		SafeQueue<std::string> q;
		ws::websocket_endpoint endpoint;
	}
}

#endif

