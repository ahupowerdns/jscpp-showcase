#include "jsonhelpers.hh"
#include <boost/lexical_cast.hpp>

using namespace boost::property_tree;
using namespace boost::property_tree::json_parser;
using std::string;
using std::map;
using std::vector;

ptree json2pt(const jsonitem_t& input)
{
  ptree pt;
  if(const string* str = boost::get<string>(&input)) {
    pt.put("", *str);
  }
  else if(const long* l = boost::get<long>(&input)) {
    pt.put("", boost::lexical_cast<string>(*l));
  }
  else if(const unsigned long* ul = boost::get<unsigned long>(&input)) {
    pt.put("", boost::lexical_cast<string>(*ul));
  } 
  else if(const double* d = boost::get<double>(&input)) {
    pt.put("", boost::lexical_cast<string>(*d));
  } 
  else if(const jsonmapitem* jmi = boost::get<jsonmapitem>(&input)) {
    for(map<string, jsonitem_t>::const_iterator iter = jmi->inner.begin(); iter != jmi->inner.end() ; ++ iter) {
      pt.push_back(make_pair(iter->first, json2pt(iter->second)));
    }
  }
  else if(const jsonvectoritem* jvi = boost::get<jsonvectoritem>(&input)) {
    for(vector<jsonitem_t>::const_iterator iter = jvi->inner.begin(); iter != jvi->inner.end(); ++iter) {
      pt.push_back(make_pair("", json2pt(*iter)));
    }
  }
  return pt;
}

string pt2string(const jsonitem_t& input)
{
  ptree pt = json2pt(input);
  std::ostringstream str;
  write_json(str, pt);
  return str.str();
}
