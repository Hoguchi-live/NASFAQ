#include "formatting.h"

/*
   See https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
*/
void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

/*
   Splits a string "{...}, ..., {...}" in an array ["{...}", ..., "{...}"].
   ONLY HANDLES DICTIONARIES OF DEPTH 1.
*/
std::vector<std::string> tokenize_json_array(std::string op, std::string token) {
	int start = 0;
	int end = op.find("}");
	std::vector<std::string> ret;

	while( end != -1 ) {
		ret.push_back(op.substr(start, end - start + 1));
		start = end + token.size() + 1; // + 1 accounts for the ",".
        	end = op.find(token, start);
	}

	return ret;
}
