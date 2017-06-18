#include "VbFile.hpp"
#include <lp3/core.hpp>

namespace nnd3d {

VbFile::VbFile(const std::string & filePath)
:   file(),
    fullPath(filePath)
{
    LP3_LOG_DEBUG("Opening file %s...", fullPath);
    file.open(fullPath.c_str(), std::ios::in | std::ios::binary);
    if (file.bad() || file.fail())
    {
        LP3_LOG_ERROR("Error opening file %s!", fullPath);
        LP3_THROW2(lp3::core::Exception, "Error opening file!");
    }
}

VbFile::VbFile(VbFile && other)
:   file(std::move(other.file)),
    fullPath(other.fullPath) {
}


VbFile::~VbFile() {
    LP3_LOG_DEBUG("Closing file %s...", fullPath);
    file.close();
}

void VbFile::get_line(std::string & line) {
    LP3_LOG_DEBUG("Reading line from %s", fullPath);
    if (!std::getline(this->file, line)) {
        LP3_LOG_ERROR("Error reading line from %s", fullPath);
        LP3_THROW2(lp3::core::Exception, "Failure to read line!");
    }
}

void VbFile::set_arg(std::stringstream & ss, bool & arg) {
    std::string text;
    ss >> text;
    if (text == "#TRUE#" || text != "0") {
        arg = true;
    } else {
        arg = false;
    }
}

void VbFile::set_arg(std::stringstream & ss, std::string & arg) {
    ss >> arg;
    // Visual Basic files allowed for items to be wrapped in quotes.
    // This unwraps them.
    if (arg.size() > 1 && arg[0] == '"' && arg[arg.size() - 1] == '"') {
        arg = arg.substr(1, arg.size() - 2);
    }
}

}   // end namespace
