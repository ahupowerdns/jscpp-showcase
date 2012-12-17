#pragma once
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/foreach.hpp>
#include <boost/variant.hpp>
#include <sstream>
#include <string>
#include <map>
#include <boost/variant/recursive_variant.hpp>

struct jsonmapitem;
struct jsonvectoritem;
typedef boost::variant<boost::recursive_wrapper<jsonmapitem> , std::string, boost::recursive_wrapper<jsonvectoritem> > jsonitem_t;
using  boost::property_tree::ptree;

struct jsonmapitem
{
  jsonmapitem(){}
  jsonmapitem(const std::map<std::string, jsonitem_t>& in) : inner(in){}
  std::map<std::string, jsonitem_t> inner;
};
struct jsonvectoritem
{
  jsonvectoritem(){}
  jsonvectoritem(const std::vector<jsonitem_t>& in) : inner(in){}
  std::vector<jsonitem_t> inner;
};

class jsonitem_visitor : public boost::static_visitor<ptree>
{
public:
    jsonitem_visitor(ptree& pt) : d_pt(pt) {}
    
    ptree operator()(const std::string & str)
    {
      d_pt.push_back(make_pair("", str));
      return d_pt;
    }
    ptree operator()(const jsonmapitem& sub) 
    {
      for(std::map<std::string, jsonitem_t>::const_iterator iter = sub.inner.begin(); iter != sub.inner.end() ; ++ iter) {
        jsonitem_visitor jv(d_pt);
        d_pt.push_back(make_pair(iter->first, apply_visitor(jv, iter->second)));
      }
      
      return d_pt;
    }
    
    ptree operator()(const jsonvectoritem& vec) 
    {
      ptree vectree;
      BOOST_FOREACH(const jsonitem_t& val, vec.inner) {
        jsonitem_visitor jv(vectree);
        vectree.push_back(make_pair("", apply_visitor(jv, val)));
      }
      return d_pt;
    }
private:
    ptree& d_pt;
    
};

std::string pt2string(const jsonitem_t& input);
