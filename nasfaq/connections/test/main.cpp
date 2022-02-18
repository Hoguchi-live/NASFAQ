#include "../http/http_connector.h"
#include "../ws/my_ssl.h"
#include "../safe_queue/safe_queue.h"

int main(void) {
	std::string sid = sid();
	connect_ws(sid);

}
