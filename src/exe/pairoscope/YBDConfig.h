#ifndef YBDCONFIG_H
#define YBDCONFIG_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>
#include <sstream>

//This code shamelessly copied and pasted from Breakdancer.

class YBDConfigEntry {
public:
    explicit YBDConfigEntry(std::string const& line)
        : _line(line)
    {
        parse();
    }

    std::string readgroup;
    std::string platform;
    std::string bam_file;
    std::string library_name;
    double readlen;
    double mean;
    double stddev;
    double lower;
    double upper;
    std::string view_executable;

private: // data
    std::string _line;

private: // functions
    void parse();

    template<typename MapType>
    std::string const& find_required(MapType const& m, char const* k) {
        return find_required(m, std::vector<std::string>(1, k));
    }

    template<typename MapType, typename KeySequenceType>
    std::string const& find_required(MapType const& m, KeySequenceType const& k) {
        std::stringstream tried;

        typedef typename KeySequenceType::const_iterator KIter;
        for (KIter iter = k.begin(); iter != k.end(); ++iter) {
            typename MapType::const_iterator x = m.find(*iter);
            if (x != m.end())
                return x->second;
            if (iter != k.begin())
                tried << ", " << *iter;
        }

        std::stringstream error_message;
        error_message << "While parsing line '" << _line << "': failed to find a required value. Needed one of " << tried.str();

        throw std::runtime_error(error_message.str());
    }
};

class YBDConfig {
public:
    explicit YBDConfig(std::istream& cfg_stream);

    std::set<std::string> readgroups() const {
        return _readgroups;
    }

    std::set<std::string> library_names() const {
        return _library_names;
    }

protected:
    std::vector<YBDConfigEntry> _entries;
    std::set<std::string> _readgroups;
    std::set<std::string> _library_names;
};

#endif
