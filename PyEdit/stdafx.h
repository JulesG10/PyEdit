#pragma once

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/stc/stc.h>
#include <wx/aui/aui.h>
#include <wx/config.h>
#include <wx/process.h>
#include <wx/txtstrm.h>
#include <wx/artprov.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/regex.h>


#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <map>
#include <system_error>
#include <unordered_map>
#include <thread>

#include<pybind11/embed.h>
namespace py = pybind11;
using namespace py::literals;

#include<SFML/Graphics.hpp>

#include<shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include "resource.h"


#define PY_ICO 0
#define PYD_ICO 1
#define PYC_ICO 2

inline std::string FGetParent(std::string path)
{
    return path.substr(0, path.find_last_of("/\\"));
}

inline std::string FGetName(std::string path, bool ext  = false)
{
    std::string basef = path.substr(path.find_last_of("/\\") + 1);
    if (ext)
    {
        return basef;
    }
    std::string::size_type const fsize(basef.find_last_of('.'));
    return basef.substr(0, fsize);
}

inline std::vector<std::string> FListDir(const std::string& directory)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = directory + "\\*";
    std::vector<std::string> dir_list;

    hFind = FindFirstFileA(full_path.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return dir_list;
    }

    while (FindNextFileA(hFind, &findData) != 0)
    {
        if (std::string(findData.cFileName) != std::string(".."))
        {
            dir_list.push_back(directory + "\\" + std::string(findData.cFileName));
        }
    }

    FindClose(hFind);
    return dir_list;
}

inline std::string FAbsolute(std::string path)
{
    DWORD fsize = 0;
    LPSTR file;

    LPSTR absolutePath = new CHAR[MAX_PATH];
    if (!GetFullPathNameA(path.c_str(), MAX_PATH, absolutePath, &file))
    {
        return path;
    }
    return  std::string(absolutePath);
}

template<typename T>
inline T FGetExtension(T const& s)
{
    if (s.find_last_of('.') == std::string::npos)
    {
        return s;
    }
    return s.substr(s.find_last_of('.'));
}

inline std::vector<std::string> split(std::string str,std::string delimiters)
{
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    std::vector<std::string> parts;

    while (std::string::npos != pos || std::string::npos != lastPos) {
        
        parts.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }

    return parts;
}

inline wxColor GetColor(std::string c)
{
    if (c[0] == '#')
    {
        return wxColor(c);
    }
    else {
        auto vals = split(c, ",");
        while(vals.size() < 4)
        {
            vals.push_back("255");
        }
        wxColor c = wxColor();
        c.Set(
            std::atoi(vals[0].c_str()),
            std::atoi(vals[1].c_str()),
            std::atoi(vals[2].c_str()),
            std::atoi(vals[3].c_str()));
        return c;
    }
}