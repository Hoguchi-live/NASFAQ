#include "test_complete_db.h"

int main(void) {
	long ts_up = 1655503200000;
	unsigned int days = 30;

	complete_db(ts_up, days, false);
	return 1;
}
