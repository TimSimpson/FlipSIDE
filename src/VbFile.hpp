#ifndef NND3D_VBFILE_HPP
#define NND3D_VBFILE_HPP
#pragma once

#include <fstream>
#include <string>
#include <sstream>

namespace nnd3d {

class VbFile {
public:
    VbFile(const std::string & filePath);

    VbFile(VbFile && vb_file);

    ~VbFile();

    // Mimics the old Visual Basic `Input` command, which would read into
    // any number of variables placed in it's call list.
    template<typename... ParameterTypes>
    void input(ParameterTypes & ... args)
    {
        std::string line;
        this->get_line(line);
        for (std::size_t i = 0; i < line.size(); ++i) {
            if (line[i]== ',') {
                line[i] = ' ';
            }
        }
        std::stringstream ss(line);
        set_arguments(ss, args...);
    }

    template<typename ParameterType>
    void set_arg(std::stringstream & ss, ParameterType & arg)
    {
        ss >> arg;
    }

    void set_arg(std::stringstream & ss, std::string & arg);

    void set_arg(std::stringstream & ss, bool & arg);

    template<typename HeadType, typename... TailTypes>
    void set_arguments(std::stringstream & ss, HeadType & head,
                      TailTypes & ... tail)
    {
        set_arg(ss, head);
        set_arguments(ss, tail...);
    }

    template<typename LastType>
    void set_arguments(std::stringstream & ss, LastType & arg)
    {
        set_arg(ss, arg);
    }
private:
    std::fstream file;
    std::string fullPath;
    bool used;

    void get_line(std::string & line);

    void open();
};

}

#endif
