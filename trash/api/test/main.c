#include "../src/items.h"
#include "../src/mute.h"

int main() {

	item_t *t = item_init();

	item_print(t);

	item_clear(t);
};

