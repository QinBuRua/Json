# Json对象&解析器
This project is licensed under the [Apache License](https://www.apache.org/licenses/LICENSE-2.0, "Click to the official website of Apache License 2.0").<br>
这个项目基于[Apache开源协议](https://www.apache.org/licenses/LICENSE-2.0, "点击前往Apache协议2.0官网")。
## 优势
在复制Json对象时，各个Json对象之间可以共用资源，<br>
即在进行数组或对象操作时，可以确保闲置的子Json对象不会被重复复制。<br>
并且在进行函数返回时，减少了重复的拷贝。
## 原理

## JSON对象
### 创建
```
Json json;
```
支持的初始化器
```
Json();                      //空初始化（默认为NULL）
Json(const JsonType &tp);    //指定的类型初始化
Json(const bool &bl);        //初始化为bool类型并赋值
Json(const int &it);         //初始化为int类型并赋值
Json(const double &dbl);     //初始化为float类型并赋值
Json(const std::string &str);//初始化为string类型并赋值
Json(const char *&ch);       //初始化为string类型并赋值
Json(const Json &js);        //复制初始化（从一个Json类中初始化）
```
