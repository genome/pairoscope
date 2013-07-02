#include "YBDConfig.h"
#include <map>

using namespace std;

namespace {
    typedef std::map<std::string, std::string> SSMapType;
    char const* temp_lib_props[] = {"lib", "samp"};
    vector<string> LIB_PROPS(temp_lib_props, temp_lib_props + 1);

    char const* temp_read_groups[] = {"readgroup", "group", "lib", "samp"};
    vector<string> READ_GROUP_PROPS(temp_read_groups, temp_read_groups + sizeof(temp_read_groups) / sizeof(temp_read_groups[0]));

    template<typename T>
    T _as(std::string const& s) {
        stringstream ss(s);
        T rv;
        if (!(ss >> rv)) {
            std::stringstream error_message;
            error_message << "Failed to convert " << s << " to the requested type";
            throw runtime_error(error_message.str());
        }
        return rv;
    }
}

void YBDConfigEntry::parse() {
    vector<string> fields;
    SSMapType directives;

    stringstream stream(_line);
    string item;
    while(getline(stream, item, '\t')) {
        fields.push_back(item);
    }
    typedef vector<string>::const_iterator VIterType;
    for (VIterType iter = fields.begin(); iter != fields.end(); ++iter) {
        string::size_type colon = iter->find_first_of(":");
        if (colon == string::npos)
            continue;
        directives[iter->substr(0, colon)] = iter->substr(colon+1);
    }

    library_name = find_required(directives, LIB_PROPS);
    readgroup = find_required(directives, READ_GROUP_PROPS);
    bam_file = find_required(directives, "map");
}

YBDConfig::YBDConfig(istream& cfg_stream) {
    string line;
    while (getline(cfg_stream, line)) {
        YBDConfigEntry entry(line);
        map<string, YBDConfigEntry>::iterator iter = _entries.find(entry.readgroup);
        if(iter == _entries.end()) {
            _entries.insert(pair<string, YBDConfigEntry>(entry.readgroup, entry));    //this will overwrite duplicate readgroup ids, but those should really be an exception...
        }
        else {
            stringstream error_message;
            error_message << "Duplicate readgroup: '" << entry.readgroup << "' found in BreakDancer configuration file";
            throw runtime_error(error_message.str());
        }
        _readgroups.insert(entry.readgroup);
        _library_names.insert(entry.library_name);
    }
}
