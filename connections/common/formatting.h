#ifndef _FORMATTING_H_
#define _FORMATTING_H_

#include <iostream>
#include <string>
#include <stdlib.h>

#include "../common/common.h"
#include <nlohmann/json.hpp>

void replace_all(std::string&, const std::string&, const std::string&);

std::vector<std::string> tokenize_json_array(std::string, std::string token = "}");

#endif

