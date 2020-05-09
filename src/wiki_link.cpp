#include "wikipedia.hpp"

using namespace std;

Link::Link(string Href, string Title){
    href = "https://en.wikipedia.org" + Href;
    title = Title;
}

const string Link::get_href() const{
    return href;
}

const string Link::get_title() const{
    return title;
}

bool operator<(const Link &l1, const Link &l2){
    return l1.href < l2.href;
}
