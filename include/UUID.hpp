#pragma once

#include <string>
#include <random>
#include <sstream>
class uuid{
    public:
        uuid();
        std::string uuid_string() const;
    private:
        std::string uuidStr;
        unsigned int random_char();
        std::string generate_hex(const unsigned int len);
};
