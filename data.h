#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <string>

struct data
{
    int index;
    std::string text;
    bool status;

	data() = default;
	data(int id, std::string text, bool status): index(id), text(text), status(status){}
};

#endif //DATA_STRUCT
