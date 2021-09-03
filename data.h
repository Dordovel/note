#ifndef DATA_STRUCT
#define DATA_STRUCT

#include <string>

struct Data
{
    int index;
    std::string text;
    bool status;
	std::string style;

	Data():index(0), text(""), status(false){};
	Data(int id, std::string text, bool status, std::string color): index(id), text(text), status(status), style(color){}
	~Data() = default;
};

#endif //DATA_STRUCT
