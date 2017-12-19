#ifndef BASE_BASIC_H
#define BASE_BASIC_H

// Include Statements
#include <core/basemessage.h>
#include <string>
namespace Base{
	class Basic : public ::BaseMessage{
		public:
			int int_val;
			std::string str_val;
	};
};

#endif //BASE_BASIC_H
