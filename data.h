#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <string>

struct Block
{
    int index;
    std::string header;
    std::string text;
    bool is_active;

	Block(): index(0), is_active(false) {};
	~Block() = default;
};

#endif //DATA_STRUCT
