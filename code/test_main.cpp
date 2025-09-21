//
// Created by QinBu_Rua on 2025/9/6.
//

#include "json.h"
#include <iostream>
#include <string>

using json::Json;
using json::JsonParser;

using std::cout;
using std::string;
using std::endl;

int main() {
   Json a(json::JSON_ARRAY);
   Json b{true};
   Json c{"c"};

   a.array().push_back(b);
   a.array().push_back(c);

   Json d = a;
   d.array().push_back(Json{"extra"});


   cout << a.str() << endl;
   cout << d.str() << endl;

   b=false;

   const Json cc=d;

   cout<<cc.array().size()<<endl;

   cout<<b.str()<<endl;

   cout<<d.str()<<endl;

   return 0;
}
