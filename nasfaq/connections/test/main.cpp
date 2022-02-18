#include "../http/http_connector.h"
#include "../ws/my_ssl.h"
#include "../safe_queue/safe_queue.h"
#include "../parser/parser.h"
#include "../common/common.h"

int main(void) {
	std::string sid = get_sid();
	connect_ws(sid);

}
