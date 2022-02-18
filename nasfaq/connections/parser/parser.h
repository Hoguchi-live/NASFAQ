#ifndef _PARSER_H_
#define _PARSER_H_

#include <iostream>
#include <string>
#include <stdlib.h>

#include <nlohmann/json.hpp>

#include "../common/common.h"
#include "../safe_queue/safe_queue.h"

MSG_TYPE msg_type_detect(std::string);

#endif

