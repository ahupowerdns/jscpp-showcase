#include "mongoose.h"
#include <iostream>
#include <string>
#include <map>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include "jsonhelpers.hh"
#include <sys/time.h>
#include <sys/resource.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using boost::lexical_cast;

string getInterfaces(mg_connection* conn)
{
  struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
      perror("getifaddrs");
      exit(EXIT_FAILURE);
    }
    typedef map<string, map<string,string> > interfaces_t;
    interfaces_t interfaces;
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr == NULL)
        continue;
        
      string address;
      family = ifa->ifa_addr->sa_family;
      
      if( family == AF_INET || family == AF_INET6 ) {
        s = getnameinfo(ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                           host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (s) {
           printf("getnameinfo() failed: %s\n", gai_strerror(s));
           exit(EXIT_FAILURE);
        }
        interfaces[ifa->ifa_name][family == AF_INET ? "IPv4" : "IPv6"]=host;
      }
      else if(family == AF_PACKET) {
        boost::format macfmt("%02x:%02x:%02x:%02x:%02x:%02x");
        unsigned char* ptr = (unsigned char*)ifa->ifa_addr;
        ptr+=12;
        
        for(int n=0; n < 6; ++n)
          macfmt % (int)ptr[n];
          
        interfaces[ifa->ifa_name]["Hardware"]=macfmt.str();
      }
    }
    freeifaddrs(ifaddr);
    
    jsonvectoritem ifvec;
    BOOST_FOREACH(const interfaces_t::value_type& val, interfaces) {
      jsonmapitem jmi;
      jmi["name"]=val.first;
      typedef map<string, string> map_t;
      BOOST_FOREACH(const map_t::value_type& ival, val.second) {
        jmi[ival.first]=ival.second;
      }
      ifvec.push_back(jmi);
    }
    map<string, jsonitem_t> root;
    root["interfaces"]=ifvec;
    return pt2string(root);
    
    
}

string getRusage(struct mg_connection*)
{
  struct rusage ru;
  
  getrusage(RUSAGE_SELF, &ru);
  jsonmapitem jmi, root;
  jmi["utime"] = ru.ru_utime.tv_sec*1000.0 + ru.ru_utime.tv_usec/1000.0;
  jmi["stime"] = ru.ru_stime.tv_sec*1000.0 + ru.ru_stime.tv_usec/1000.0;
#define getru(x) jmi[#x] = ru.ru_##x;
  getru(maxrss);
  getru(minflt);
  getru(majflt);
  getru(inblock);
  getru(oublock);
  getru(nvcsw);
  getru(nivcsw);
#undef getru
  root["rusage"] = jmi;
  return pt2string(root);
}

static void *callback(enum mg_event event,
                      struct mg_connection *conn) 
{
  const struct mg_request_info *request_info = mg_get_request_info(conn);
  
  if (event != MG_NEW_REQUEST)
    return 0;
    
  string resp;
  string uri = request_info->uri;
  
  if(uri == "/interfaces") {
    resp = getInterfaces(conn);
  } else if(uri == "/rusage") {
    resp = getRusage(conn);
  } 
    
  if(!resp.empty()) {
    mg_printf(conn,
              "HTTP/1.1 200 OK\r\n"
              "Content-Type: application/json\r\n"
              "Content-Length: %ld\r\n"        // Always set Content-Length
              "\r\n"
              "%s",
              resp.length(), resp.c_str());
    // Mark as processed
    return (void*)"";
  } else {
    return NULL;
  }
}


int main()
{
  struct mg_context *ctx;
  const char *options[] = {"listening_ports", "8080", 
                           "document_root", "./html", 
                           NULL};

  ctx = mg_start(&callback, NULL, options);
  if(!ctx) {
    throw runtime_error("Could not start webserver!");
  }
  getchar();  // Wait until user hits "enter"
  mg_stop(ctx);

  return 0;
}
