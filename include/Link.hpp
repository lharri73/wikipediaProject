#pragma once
#include <string>

class Link{
    public:
        Link(std::string Href, std::string Title);
        friend bool operator<(const Link &l1, const Link &l2);
        const std::string get_href() const;
        const std::string get_title() const;
    protected:
        std::string href;
        std::string title;
};
