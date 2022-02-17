// @file performance.c
#include "performance.h"

/**
  Performance points
*/
void performance_point_init_(performance_point_t *p) {

	p->worth = 0;
	p->timestamp = 0;
}

performance_point_t *performance_point_init() {

	performance_point_t *p = malloc(sizeof(performance_point_t));
	performance_point_init_(p);
	return p;
}

void performance_point_set(performance_point_t *p, float worth, ulong timestamp) {

	p->worth = worth;
	p->timestamp = timestamp;
}

void performance_point_init_set_(performance_point_t *p, float worth, ulong timestamp) {

	performance_point_init_(p);
	performance_point_set(p, worth, timestamp);
}

performance_point_t *performance_point_init_set(float worth, ulong timestamp) {

	performance_point_t *p = performance_point_init();
	performance_point_set(p, worth, timestamp);
	return p;
}

void performance_point_clear(performance_point_t *p) {

	free(p);
}

void performance_point_print(performance_point_t *p) {

	printf("PERFORMANCE POINT\n");
	printf("  Worth: %f\n", p->worth);
	printf("  Timestamp: %lu\n", p->timestamp);
}

/**
  Performance data (arrays of points)
*/
void performance_data_init_(performance_data_t *d) {

	d->size = 0;
	d->data = malloc(sizeof(performance_data_t) * PERFORMANCE_DATA_SIZE);
}

performance_data_t *performance_data_init() {

	performance_data_t *p = malloc(sizeof(performance_data_t));
	performance_data_init_(p);
	return p;
}

void performance_data_set(performance_data_t *d, uint size, performance_point_t *data) {

	d->size = size;
	memcpy(d->data, data, sizeof(performance_point_t) * size); // Woah mama
}

void performance_data_init_set_(performance_data_t *d, uint size, performance_point_t *data) {

	performance_data_init_(d);
	performance_data_set(d, size, data);
}

performance_data_t *performance_data_init_set(uint size, performance_point_t *data) {

	performance_data_t *d = performance_data_init();
	performance_data_set(d, size, data);
	return d;
}

void performance_data_clear(performance_data_t *d) {

	free(d->data);
	free(d);
}

void performance_data_print(performance_data_t *d) {

	printf("PERFORMANCE DATA\n");
	printf("  Size: %u\n", d->size);
	for(int i = 0; i < d->size; i++) performance_point_print(d->data + i);
}

