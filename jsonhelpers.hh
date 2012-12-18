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
typedef boost::variant<boost::recursive_wrapper<jsonmapitem> , std::string, long, double, boost::recursive_wrapper<jsonvectoritem> > jsonitem_t;
using  boost::property_tree::ptree;

struct jsonmapitem
{
  jsonmapitem(){}
  jsonmapitem(const std::map<std::string, jsonitem_t>& in) : inner(in){}
  std::map<std::string, jsonitem_t> inner;
  jsonitem_t& operator[](const std::string& lookup) { return inner[lookup]; }
};
struct jsonvectoritem
{
  jsonvectoritem(){}
  jsonvectoritem(const std::vector<jsonitem_t>& in) : inner(in){}
  std::vector<jsonitem_t> inner;
  void push_back(const jsonitem_t& ji) { inner.push_back(ji); }
};

std::string pt2string(const jsonitem_t& input);
