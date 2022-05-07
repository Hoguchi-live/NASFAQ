#ifndef _PERFORMANCE_H_
#define _PERFORMANCE_H_

#include "common.h"

#define PERFORMANCE_DATA_SIZE 365*2 // Very dangerous TODO: REPLACE SIZE WITH LENGTH WHEN DEALING WITH ARRAYS

/**
  Custom types
*/
typedef struct performance_point_t {

	float worth;
	ulong timestamp;
} performance_point_t;

typedef struct performance_data_t {

	uint size;
	performance_point_t *data;
} performance_data_t;


/**
  Performance points
*/
void performance_point_init_(performance_point_t *);
performance_point_t *performance_point_init();
void performance_point_set(performance_point_t *, float, ulong);
void performance_point_init_set_(performance_point_t *, float, ulong);
performance_point_t *performance_point_init_set(float, ulong);
void performance_point_clear(performance_point_t *);

void performance_point_print(performance_point_t *);

/**
  Performance data
*/
void performance_data_init_(performance_data_t *);
performance_data_t *performance_data_init();
void performance_data_set(performance_data_t *, uint, performance_point_t *);
void performance_data_init_set_(performance_data_t *, uint, performance_point_t *);
performance_data_t *performance_data_init_set(uint, performance_point_t *);
void performance_data_clear(performance_data_t *);

void performance_data_print(performance_data_t *);


#endif

