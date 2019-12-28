#include "wikipedia.hpp"
using namespace std;
uuid::uuid(){
    uuidStr = generate_hex(32);
}

string uuid::uuid_string() const{
    return uuidStr;
}

unsigned int uuid::random_char() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}

string uuid::generate_hex(const unsigned int len) {
    stringstream ss;
    for (size_t i = 0; i < len; i++) {
        const unsigned int rc = random_char();
        stringstream hexstream;
        hexstream << std::hex << rc;
        string hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}
