#include "json.h"
#include <stdexcept>
#include <sstream>
#include <stack>
#include <fstream>
#include <iostream>

using std::string;
using std::string_view;
using std::vector;
using std::map;
using std::stack;

using std::logic_error;
using std::out_of_range;
using std::runtime_error;

using std::ifstream;
using std::ofstream;
using std::fstream;

using namespace ns_json;
using namespace ns_json::privacy;

template<typename T>
static string to_string(T content) {
    std::stringstream sstr;
    sstr << content;
    return sstr.str();
}

const std::vector<std::string> ns_json::JSON_TYPE_TO_STRING{
    "NULL",
    "BOOL",
    "INT",
    "FLOAT",
    "STRING",
    "ARRAY",
    "OBJECT",
};
const std::map<std::string, JsonType> ns_json::STRING_TO_JSON_TYPE{
    {"NULL", JSON_NULL},
    {"BOOL", JSON_BOOL},
    {"INT", JSON_INT},
    {"STRING", JSON_STRING},
    {"FLOAT", JSON_FLOAT},
    {"ARRAY", JSON_ARRAY},
    {"OBJECT", JSON_OBJECT},
};

Json::Json() {
    m_Value = new JsonBasic{};
}

Json::Json(const bool &bl) {
    m_Value = new JsonBasic{static_cast<bool>(bl)};
}

Json::operator bool() {
    if (m_Value->type != JSON_BOOL) {
        throw logic_error(string("From class Json(") + to_string(this)
                          + "):Json.bool() can only be done for BOOL! But the type of this variable is "
                          + JSON_TYPE_TO_STRING[m_Value->type] + "."
        );
    }
    return m_Value->value.bl;
}

JsonType Json::type() const {
    return m_Value->type;
}

Json &Json::operator=(const bool &bl) {
    if (m_Value->quoteTimes != 0) {
        only_delete();
        m_Value = new JsonBasic{bl};
        return *this;
    }
    if (m_Value->type != JSON_BOOL) {
        m_Value->conversion_type(JSON_BOOL);
    }
    m_Value->value.bl = bl;
    return *this;
}

void Json::clear() {
    if (m_Value->quoteTimes == 0) {
        m_Value->clear();
    } else {
        m_Value->quoteTimes--;
        m_Value = new JsonBasic{};
    }
}

Json::Json(const JsonType &tp) {
    m_Value = new JsonBasic(static_cast<JsonType>(tp));
}

Json::Json(const int &it) {
    m_Value = new JsonBasic(JSON_INT);
    m_Value->value.it = it;
}

Json::Json(const double &dbl) {
    m_Value = new JsonBasic{JSON_FLOAT};
    m_Value->value.dbl = dbl;
}

Json::Json(const string &str) {
    m_Value = new JsonBasic{JSON_STRING};
    m_Value->value.str = str;
}

Json::Json(const Json &js) {
    m_Value = js.m_Value;
    (m_Value->quoteTimes)++;
}

Json &Json::operator=(const Json &js) {
    if (m_Value == js.m_Value) {
        return *this;
    }
    only_delete();
    m_Value = js.m_Value;
    m_Value->quoteTimes++;
    return *this;
}

Json &Json::operator=(const string &str) {
    if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        m_Value = new JsonBasic{str};
        return *this;
    }
    if (m_Value->type != JSON_STRING) {
        m_Value->conversion_type(JSON_STRING);
    }
    m_Value->value.str = str;
    return *this;
}

void Json::only_delete() {
    if (m_Value->quoteTimes == 0) {
        m_Value->clear();
        delete m_Value;
    } else {
        m_Value->quoteTimes--;
        m_Value = nullptr;
    }
}

Json::~Json() {
    only_delete();
}

Json::operator std::string() {
    if (m_Value->type != JSON_STRING) {
        throw logic_error(string("From class Json(") + to_string(this)
                          + "):Json.string() can only be done for STRING! But the type of this variable is "
                          + JSON_TYPE_TO_STRING[m_Value->type] + "."
        );
    }
    return m_Value->value.str;
}

Json::operator int() {
    if (m_Value->type != JSON_INT) {
        throw logic_error(string("From class Json(") + to_string(this)
                          + "):Json.int() can only be done for INT! But the type of this variable is "
                          + JSON_TYPE_TO_STRING[m_Value->type] + "."
        );
    }
    return m_Value->value.it;
}

Json::operator double() {
    if (m_Value->type != JSON_FLOAT) {
        throw logic_error(string("From class Json(") + to_string(this)
                          + "):Json.double() can only be done for FLOAT! But the type of this variable is "
                          + JSON_TYPE_TO_STRING[m_Value->type]);
    }
    return m_Value->value.dbl;
}

Json &Json::operator=(const int &it) {
    if (m_Value->quoteTimes != 0) {
        only_delete();
        m_Value = new JsonBasic{it};
        return *this;
    }
    if (m_Value->type != JSON_INT) {
        m_Value->conversion_type(JSON_INT);
    }
    m_Value->value.it = it;
    return *this;
}

Json &Json::operator[](const string &str) {
    if (m_Value->type != JSON_OBJECT) {
        only_delete();
        m_Value = new JsonBasic{JSON_OBJECT};
    } else if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        const JsonBasic &that = *m_Value;
        m_Value = new JsonBasic{};
        this->copy_highest_layer(that);
    }
    return m_Value->value.obj[str];
}

void Json::copy(const Json &js) {
    if (m_Value == js.m_Value) {
        return;
    }
    only_delete();
    switch (js.m_Value->type) {
        case JSON_NULL: {
            m_Value = new JsonBasic{};
            break;
        }
        case JSON_BOOL: {
            m_Value = new JsonBasic{js.m_Value->value.bl};
            break;
        }
        case JSON_INT: {
            m_Value = new JsonBasic{js.m_Value->value.it};
            break;
        }
        case JSON_FLOAT: {
            m_Value = new JsonBasic{js.m_Value->value.dbl};
            break;
        }
        case JSON_STRING: {
            m_Value = new JsonBasic(js.m_Value->value.str);
            break;
        }
        case JSON_ARRAY: {
            m_Value = new JsonBasic{JSON_ARRAY};
            m_Value->value.arr.resize(js.m_Value->value.arr.size());
            for (auto iterThis = m_Value->value.arr.begin(),
                      iterThat = js.m_Value->value.arr.begin();
                 iterThis != m_Value->value.arr.end();
                 ++iterThis, ++iterThat) {
                iterThis->copy(*iterThat);
            }
            break;
        }
        case JSON_OBJECT: {
            m_Value = new JsonBasic{JSON_OBJECT};
            for (auto iter = js.m_Value->value.obj.begin();
                 iter != js.m_Value->value.obj.end();
                 ++iter) {
                m_Value->value.obj[iter->first].copy(iter->second);
            }
            break;
        }
    }
}

Json &Json::operator=(const char *chPtr) {
    if (m_Value->quoteTimes != 0) {
        only_delete();
        m_Value = new JsonBasic(static_cast<const char*&>(chPtr));
        return *this;
    }
    if (m_Value->type != JSON_STRING) {
        m_Value->conversion_type(JSON_STRING);
    }
    m_Value->value.str = chPtr;
    return *this;
}

Json &Json::operator=(const char &ch) {
    if (m_Value->quoteTimes != 0) {
        only_delete();
        m_Value = new JsonBasic{static_cast<const char&>(ch)};
        return *this;
    }
    if (m_Value->type != JSON_STRING) {
        m_Value->conversion_type(JSON_STRING);
    }
    m_Value->value.str = ch;
    return *this;
}

void Json::swap(Json &js) noexcept {
    JsonBasic *temp = m_Value;
    m_Value = js.m_Value;
    js.m_Value = temp;
}

void Json::conversion_type(JsonType ttp) {
    if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        m_Value = new JsonBasic{ttp};
    } else {
        m_Value->conversion_type(ttp);
    }
}

unsigned int Json::quote_times() {
    return m_Value->quoteTimes;
}

std::string Json::str() const {
    switch (m_Value->type) {
        case JSON_NULL: {
            return string{"null"};
        }
        case JSON_BOOL: {
            if (m_Value->value.bl == true) {
                return string{"true"};
            } else {
                return string{"false"};
            }
        }
        case JSON_INT: {
            return std::to_string(m_Value->value.it);
        }
        case JSON_FLOAT: {
            return std::to_string(m_Value->value.dbl);
        }
        case JSON_STRING: {
            string res{};
            res += '"';
            for (auto i = 0; i < m_Value->value.str.length(); i++) {
                switch (m_Value->value.str[i]) {
                    case '\\': {
                        res += R"(\\)";
                        break;
                    }
                    case '\t': {
                        res += R"(\t)";
                        break;
                    }
                    case '\n': {
                        res += R"(\n)";
                        break;
                    }
                    case '\"': {
                        res += R"(\")";
                        break;
                    }
                    default: {
                        res += m_Value->value.str[i];
                        break;
                    }
                }
            }
            res += '"';
            return res;
        }
        case JSON_ARRAY: {
            if (m_Value->value.arr.empty()) {
                return string{"[]"};
            }
            string str = "[";
            str += m_Value->value.arr.front().str();
            for (auto iter = ++m_Value->value.arr.begin(); iter != m_Value->value.arr.end(); ++iter) {
                str += ",";
                str += iter->str();
            }
            str += "]";
            return str;
        }
        case JSON_OBJECT: {
            if (m_Value->value.obj.empty()) {
                return string{"{}"};
            }
            string str = "{\"";
            str += m_Value->value.obj.begin()->first;
            str += "\":";
            str += m_Value->value.obj.begin()->second.str();
            for (auto iter = ++m_Value->value.obj.begin(); iter != m_Value->value.obj.end(); ++iter) {
                str += ",\"";
                str += iter->first;
                str += "\":";
                str += iter->second.str();
            }
            str += "}";
            return str;
        }
        default: break;
    }
    throw runtime_error(
        string("From class Json(") + to_string(this) + ").str():Unknown Error!!!It is impossible to run here!!!");
    return string{};
}

std::string Json::type_str() const {
    return JSON_TYPE_TO_STRING[m_Value->type];
}

Json::Json(const char *&ch) {
    m_Value = new JsonBasic{ch};
}

void Json::copy_highest_layer(const Json &js) {
    only_delete();
    switch (js.m_Value->type) {
        case JSON_NULL: {
            m_Value = new JsonBasic{};
            break;
        }
        case JSON_BOOL: {
            m_Value = new JsonBasic{js.m_Value->value.bl};
            break;
        }
        case JSON_INT: {
            m_Value = new JsonBasic{js.m_Value->value.it};
            break;
        }
        case JSON_FLOAT: {
            m_Value = new JsonBasic{js.m_Value->value.dbl};
            break;
        }
        case JSON_STRING: {
            m_Value = new JsonBasic(js.m_Value->value.str);
            break;
        }
        case JSON_ARRAY: {
            m_Value = new JsonBasic{};
            m_Value->type = JSON_ARRAY;
            new(&m_Value->value.arr) vector<Json>(js.m_Value->value.arr);
            break;
        }
        case JSON_OBJECT: {
            m_Value = new JsonBasic{};
            m_Value->type = JSON_OBJECT;
            new(&m_Value->value.obj) map<string, Json>(js.m_Value->value.obj);
            break;
        }
    }
}

Json &Json::operator[](const int &n) {
    if (m_Value->type != JSON_ARRAY) {
        only_delete();
        m_Value = new JsonBasic{JSON_ARRAY};
    } else if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        const JsonBasic &that = *m_Value;
        m_Value = new JsonBasic{};
        this->copy_highest_layer(that);
    }
    if (m_Value->value.arr.size() <= n) {
        m_Value->value.arr.resize(n + 1);
    }
    return m_Value->value.arr[n];
}

Json Json::operator[](const int &n) const {
    if (m_Value->type != JSON_ARRAY) {
        throw logic_error{
            string("From class Json(") + to_string(this) +
            "):Json.operator[](n)const can only be done for ARRAY! But the type is " +
            JSON_TYPE_TO_STRING[m_Value->type] + "."
        };
    }
    if (m_Value->value.arr.size() <= n) {
        m_Value->value.arr.resize(n + 1);
    }
    return m_Value->value.arr[n];
}

Json Json::operator[](const std::string &str) const {
    if (m_Value->type != JSON_OBJECT) {
        throw logic_error{
            string("From class Json(") + to_string(this) +
            "):Json.operator[](n)const can only be done for OBJECT! But the type is " +
            JSON_TYPE_TO_STRING[m_Value->type] + "."
        };
    }
    return m_Value->value.obj[str];
}

Json Json::operator[](const char *chPtr) const {
    if (m_Value->type != JSON_OBJECT) {
        throw logic_error{
            string("From class Json(") + to_string(this) +
            "):Json.operator[](n)const can only be done for OBJECT! But the type is " +
            JSON_TYPE_TO_STRING[m_Value->type] + "."
        };
    }
    return m_Value->value.obj[string{chPtr}];
}

void Json::copy_highest_layer(const JsonBasic &jsb) {
    only_delete();
    switch (jsb.type) {
        case JSON_NULL: {
            m_Value = new JsonBasic{};
            break;
        }
        case JSON_BOOL: {
            m_Value = new JsonBasic{jsb.value.bl};
            break;
        }
        case JSON_INT: {
            m_Value = new JsonBasic{jsb.value.it};
            break;
        }
        case JSON_FLOAT: {
            m_Value = new JsonBasic{jsb.value.dbl};
            break;
        }
        case JSON_STRING: {
            m_Value = new JsonBasic(jsb.value.str);
            break;
        }
        case JSON_ARRAY: {
            m_Value = new JsonBasic{};
            m_Value->type = JSON_ARRAY;
            new(&m_Value->value.arr) vector<Json>(jsb.value.arr);
            break;
        }
        case JSON_OBJECT: {
            m_Value = new JsonBasic{};
            m_Value->type = JSON_OBJECT;
            new(&m_Value->value.obj) map<string, Json>{jsb.value.obj};
            break;
        }
    }
}

Json &Json::operator[](const char *chPtr) {
    if (m_Value->type != JSON_OBJECT) {
        only_delete();
        m_Value = new JsonBasic{JSON_OBJECT};
    } else if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        const JsonBasic &that = *m_Value;
        m_Value = new JsonBasic{};
        this->copy_highest_layer(that);
    }
    return m_Value->value.obj[string(chPtr)];
}

void Json::push_back(const Json &js) {
    if (m_Value->type != JSON_ARRAY) {
        throw logic_error(string("From class Json(") + to_string(this) + string(
                              "):Json.push_pack() can only be done for ARRAY, but the type of this variable is "
                              + JSON_TYPE_TO_STRING[m_Value->type]) + ".");
    } else if (m_Value->quoteTimes != 0) {
        self_separate();
    }
    m_Value->value.arr.push_back(js);
}

Json &Json::at(const int &n) {
    if (n < 0) {
        throw out_of_range(string("From class Json(") + to_string(this) +
                           "):Json.operator[](const int& n), args-n have to be >=0, but args-n==" + std::to_string(n));
    }
    if (m_Value->type != JSON_ARRAY) {
        only_delete();
        m_Value = new JsonBasic{JSON_ARRAY};
    } else if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        const JsonBasic &that = *m_Value;
        m_Value = new JsonBasic{};
        this->copy_highest_layer(that);
    }
    if (m_Value->value.arr.size() <= n) {
        m_Value->value.arr.resize(n + 1);
    }
    return m_Value->value.arr[n];
}

Json &Json::at(const std::string &str) {
    if (m_Value->type != JSON_OBJECT) {
        only_delete();
        m_Value = new JsonBasic{JSON_OBJECT};
    } else if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        const JsonBasic &that = *m_Value;
        m_Value = new JsonBasic{};
        this->copy_highest_layer(that);
    }
    return m_Value->value.obj.at(str);
}

Json &Json::at(const char *chPtr) {
    if (m_Value->type != JSON_OBJECT) {
        only_delete();
        m_Value = new JsonBasic{JSON_OBJECT};
    } else if (m_Value->quoteTimes != 0) {
        m_Value->quoteTimes--;
        const JsonBasic &that = *m_Value;
        m_Value = new JsonBasic{};
        this->copy_highest_layer(that);
    }
    return m_Value->value.obj.at(string{chPtr});
}

void JsonParser::read_token_char() {
    m_Idx++;
    while (m_Str[m_Idx] == ' ' || m_Str[m_Idx] == '\t' || m_Str[m_Idx] == '\n') {
        m_Idx++;
    }
    if (m_Idx >= m_Str.length()) {
        throw logic_error(
            string("From class JsonParser(") + to_string(this) +
            "):Parse error(c:" + std::to_string(m_Idx) +
            "):This str of Json is NOT intact. Probably it misses ']' or '}'.");
    }
}

char JsonParser::peek_token_char() {
    unsigned long long tmp = m_Idx + 1;
    while (m_Str[tmp] == ' ' || m_Str[tmp] == '\t' || m_Str[tmp] == '\n') {
        tmp++;
    }
    return m_Str[tmp];
}

bool JsonParser::is_white_space() {
    return (m_Str[m_Idx] == ' ' || m_Str[m_Idx] == '\t' || m_Str[m_Idx] == '\n' || m_Str[m_Idx] == '\r');
}

void JsonParser::skip_white_space() {
    while (m_Str[m_Idx] == ' ' || m_Str[m_Idx] == '\t' || m_Str[m_Idx] == '\n' || m_Str[m_Idx] == '\r') {
        m_Idx++;
    }
}

Json JsonParser::parse_json() {
    skip_white_space();
    switch (m_Str[m_Idx]) {
        case 'n': {
            return parse_null();
        }
        case 't':
        case 'f': {
            return parse_bool();
        }
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            return parse_number();
        }
        case '"': {
            return parse_string();
        }
        case '[': {
            return parse_array();
        }
        case '{': {
            return parse_object();
        }
        default: break;
    }
    throw logic_error{
        string("From class JsonParser(") + to_string(this) + "):Parse error(c:" + std::to_string(m_Idx) +
        "):Unexpected character '" + m_Str[m_Idx] + "'."
    };
}

Json JsonParser::parse_null() {
    if (!m_Str.compare(m_Idx, 4, "null")) {
        m_Idx += 4;
        return Json{JSON_NULL};
    } else {
        throw logic_error(
            string("From class JsonParser(") + to_string(this) +
            "):Parse null error(c:" + std::to_string(m_Idx) + ").");
    }
}

Json JsonParser::parse_bool() {
    if (!m_Str.compare(m_Idx, 5, "false")) {
        m_Idx += 5;
        return Json{false};
    } else if (!m_Str.compare(m_Idx, 4, "true")) {
        m_Idx += 4;
        return Json{true};
    } else {
        throw logic_error(
            string("From class JsonParser(") + to_string(this) +
            "):Parse bool error(c:" + std::to_string(m_Idx) + ").");
    }
}

Json JsonParser::parse_number() {
    char sign = 1;
    if (m_Str[m_Idx] == '-') {
        sign = -1;
        m_Idx++;
    }
    int it = 0;
    while (m_Str[m_Idx] >= '0' && m_Str[m_Idx] <= '9') {
        it *= 10;
        it += m_Str[m_Idx] - '0';
        m_Idx++;
        if (m_Str[m_Idx] == '.') {
            m_Idx++;
            double dbl = 0;
            double bit = 1;
            while (m_Str[m_Idx] >= '0' && m_Str[m_Idx] <= '9') {
                bit /= 10;
                dbl += (m_Str[m_Idx] - '0') * bit;
                m_Idx++;
            }
            return Json{(dbl + it) * sign};
        }
    }
    return Json{it * sign};
}

std::string JsonParser::parse_string() {
    string str;
    m_Idx++;
    while (m_Str[m_Idx] != '"') {
        if (m_Str[m_Idx] == '\\') {
            m_Idx++;
            switch (m_Str[m_Idx]) {
                case 't': {
                    str += '\t';
                    break;
                }
                case 'n': {
                    str += '\n';
                    break;
                }
                case 'r': {
                    str += '\r';
                    break;
                }
                case '"': {
                    str += '"';
                    break;
                }
                default: {
                    str += '\\';
                    str += m_Str[m_Idx];
                }
            }
            m_Idx++;
        } else {
            str += m_Str[m_Idx++];
        }
    }
    m_Idx++;
    return str;
}

Json JsonParser::parse_array() {
    m_Idx++;
    skip_white_space();
    if (m_Str[m_Idx] == ']') {
        m_Idx++;
        return Json{JSON_ARRAY};
    }
    Json json = JSON_ARRAY;
    json.push_back(parse_json());
    skip_white_space();
    while (m_Str[m_Idx] != ']') {
        if (m_Str[m_Idx] != ',') {
            throw logic_error{
                string("From class JsonParser(") + to_string(this) + "):Parse array error(c:" + std::to_string(m_Idx) +
                "):This str of Json misses a ','."
            };
        }
        m_Idx++;

        try {
            json.push_back(parse_json());
        } catch (logic_error &logicError) {
            if (string(logicError.what()).find("Unexpected character ']'") != string::npos) {
                throw logic_error{
                    string("From class JsonParser(") + to_string(this) + "):Parse array error(c:" +
                    std::to_string(m_Idx) + "):There is NOT a value between ',' and ']'."
                };
            } else if (string(logicError.what()).find("Unexpected character ','") != string::npos) {
                throw logic_error{
                    string("From class JsonParser(") + to_string(this) + "):Parse array error(c:" +
                    std::to_string(m_Idx) + "):There is NOT a value between ',' and ','."
                };
            } else {
                throw logic_error{
                    string("From class JsonParser(") + to_string(this) + "):Parse error(c:" + std::to_string(m_Idx) +
                    "):Unexpected character '" + m_Str[m_Idx] + "'."
                };
            }
        }
        skip_white_space();
    }
    m_Idx++;
    return json;
}

Json JsonParser::parse_object() {
    m_Idx++;
    skip_white_space();
    if (m_Str[m_Idx] == '}') {
        m_Idx++;
        return Json{JSON_OBJECT};
    }
    Json json{JSON_OBJECT};
    if (m_Str[m_Idx] != '"') {
        throw logic_error{
            string(
                "From class JsonParser(" + to_string(this) + "):Parse object error(c:" + std::to_string(m_Idx) +
                "):Unexpected character '" + m_Str[m_Idx] + "'. It should be '\"'.")
        };
    }
    string key = parse_string();
    skip_white_space();
    if (m_Str[m_Idx] != ':') {
        throw logic_error{
            string(
                "From class JsonParser(" + to_string(this) + "):Parse object error(c:" + std::to_string(m_Idx) +
                "):Unexpected character '" + m_Str[m_Idx] + "'. It should be ':'.")
        };
    }
    read_token_char();
    json.m_Value->value.obj[key] = parse_json();
    skip_white_space();
    while (m_Str[m_Idx] != '}') {
        if (m_Str[m_Idx] != ',') {
            throw logic_error{
                string("From class JsonParser(") + to_string(this) + "):Parse object error(c:" + std::to_string(m_Idx) +
                "):This str of Json misses a ','."
            };
        }
        read_token_char();
        if (m_Str[m_Idx] != '"') {
            if (m_Str[m_Idx] == '}') {
                throw logic_error{
                    string("From class JsonParser(") + to_string(this) + "):Parse object error(c:" +
                    std::to_string(m_Idx) + "):There is NOT a value between ',' and '}'."
                };
            } else if (m_Str[m_Idx] == ',') {
                throw logic_error{
                    string("From class JsonParser(") + to_string(this) + "):Parse object error(c:" +
                    std::to_string(m_Idx) + "):There is NOT a value between ',' and ','."
                };
            }
            throw logic_error{
                string(
                    "From class JsonParser(" + to_string(this) + "):Parse object error(c:" + std::to_string(m_Idx) +
                    "):Unexpected character '" + m_Str[m_Idx] + "'. It should be '\"'.")
            };
        }
        key = parse_string();
        skip_white_space();
        if (m_Str[m_Idx] != ':') {
            throw logic_error{
                string(
                    "From class JsonParser(" + to_string(this) + "):Parse object error(c:" + std::to_string(m_Idx) +
                    "):Unexpected character '" + m_Str[m_Idx] + "'. It should be ':'.")
            };
        }
        read_token_char();
        json.m_Value->value.obj[key] = parse_json();
        skip_white_space();
    }
    m_Idx++;
    return json;
}

JsonParser::JsonParser(const std::string &str) {
    m_Str = str;
    m_Idx = 0;
    m_Json = parse_json();
}

JsonParser::operator Json() {
    return m_Json;
}

void Json::self_separate() {
    m_Value->quoteTimes--;
    const JsonBasic &that = *m_Value;
    m_Value = new JsonBasic();
    this->copy_highest_layer(that);
}

JsonBasic::JsonBasic() {
    type = JSON_NULL;
}

JsonBasic::JsonBasic(const bool &bl) {
    type = JSON_BOOL;
    value.bl = bl;
}

void JsonBasic::clear() {
    switch (type) {
        case JSON_STRING: {
            value.str.~basic_string();
            break;
        }
        case JSON_ARRAY: {
            value.arr.~vector();
            break;
        }
        case JSON_OBJECT: {
            value.obj.~map();
            break;
        }
        default: break;
    }
    type = JSON_NULL;
}

JsonBasic::JsonBasic(JsonType tp) {
    type = tp;
    switch (type) {
        case JSON_STRING: {
            new(&value.str) string{};
            break;
        }
        case JSON_ARRAY: {
            new(&value.arr) vector<Json>{};
            break;
        }
        case JSON_OBJECT: {
            new(&value.obj) map<string, Json>{};
            break;
        }
        default: break;
    }
}

JsonBasic::~JsonBasic() {
    clear();
}

void JsonBasic::conversion_type(JsonType ttp) {
    switch (type) {
        case JSON_STRING: {
            value.str.~basic_string();
            break;
        }
        case JSON_ARRAY: {
            value.arr.~vector();
            break;
        }
        case JSON_OBJECT: {
            value.obj.~map();
            break;
        }
        default: break;
    }
    type = ttp;
    switch (type) {
        case JSON_STRING: {
            new(&value.str) string{};
            break;
        }
        case JSON_ARRAY: {
            new(&value.arr) vector<Json>{};
            break;
        }
        case JSON_OBJECT: {
            new(&value.obj) map<string, Json>{};
            break;
        }
        default: break;
    }
}

JsonBasic::JsonBasic(const string &str) {
    type = JSON_STRING;
    new(&value.str) string{str};
}

JsonBasic::JsonBasic(const char *&chPtr) {
    type = JSON_STRING;
    new(&value.str) string(chPtr);
}

JsonBasic::JsonBasic(const int &it) {
    type = JSON_INT;
    value.it = it;
}

JsonBasic::JsonBasic(const double &dbl) {
    type = JSON_FLOAT;
    value.dbl = dbl;
}

JsonBasic::JsonBasic(const char &ch) {
    type = JSON_STRING;
    new(&value.str) string{1, ch};
}

JsonBasic::JsonBasic_Union::JsonBasic_Union() {
}

JsonBasic::JsonBasic_Union::~JsonBasic_Union() {
}
