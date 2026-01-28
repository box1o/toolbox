#pragma once

#include <filesystem>

namespace ct::vision {



class Media {
public:
    Media(std::filesystem::path path);


private:
    std::filesystem::path mPath;
};



}
