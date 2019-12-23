#include "wikipedia.hpp"
using namespace std;
uuid::uuid(){
    uuid_t binuuid;
    uuid_generate_random(binuuid);
    generated_uuid = (char*)malloc(37);
    uuid_unparse(binuuid, generated_uuid);   
}

uuid::~uuid(){
    free(generated_uuid);
}

const string uuid::uuid_string() const{
    return string(generated_uuid);
}
