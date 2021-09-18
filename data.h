#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <string>

struct Data
{
    int index;
    std::string title;
    std::string note;
    bool status;

	Data(): index(0), title(""), status(false), note(""){};
	~Data() = default;
};

#endif //DATA_STRUCT
