#include "json.h"
#include <stdexcept>
#include <sstream>

using namespace json;

using std::get;

using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;

using std::make_shared;

using std::variant;
using std::string;
using std::vector;
using std::map;

using std::logic_error;
using std::runtime_error;

template<typename T>
static string to_string(T content) {
    std::stringstream sstr;
    sstr << content;
    return sstr.str();
}

const std::vector<std::string> json::JSON_TYPE_TO_STRING{
        "NULL",
        "BOOL",
        "INT",
        "FLOAT",
        "STRING",
        "ARRAY",
        "OBJECT",
};


Json::Json() {
    m_Value = make_shared<JsonBasic>(Void{});
}

Json::~Json() = default;

Json::Json(const JsonType& tp) {
    switch (tp) {
        case JSON_NULL: {
            m_Value = make_shared<JsonBasic>(Void{});
            break;
        }
        case JSON_BOOL: {
            m_Value = make_shared<JsonBasic>(false);
            break;
        }
        case JSON_INT: {
            m_Value = make_shared<JsonBasic>(0l);
            break;
        }
        case JSON_FLOAT: {
            m_Value = make_shared<JsonBasic>(double(0));
            break;
        }
        case JSON_STRING: {
            m_Value = make_shared<JsonBasic>(string{});
            break;
        }
        case JSON_ARRAY: {
            m_Value = make_shared<JsonBasic>(vector<Json>{});
            break;
        }
        case JSON_OBJECT: {
            m_Value = make_shared<JsonBasic>(map<string, Json>{});
            break;
        }
        default:
            break;
    }
}

Json::Json(const bool& bl) {
    m_Value = make_shared<JsonBasic>(bl);
}

Json::Json(const int& it) {
    m_Value = make_shared<JsonBasic>(it);
}

Json::Json(const double& dbl) {
    m_Value = make_shared<JsonBasic>(dbl);
}

Json::Json(const std::string& str) {
    m_Value = make_shared<JsonBasic>(str);
}

Json::Json(const char* ch) {
    m_Value = make_shared<JsonBasic>(ch);
}

Json::Json(const Json& js) = default;

Json::Json(Json&& js) noexcept = default;

Json& Json::operator=(const bool& bl) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(bl);
    } else {
        *m_Value = bl;
    }
    return *this;
}

Json& Json::operator=(const int& it) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(it);
    } else {
        *m_Value = it;
    }
    return *this;
}

Json& Json::operator=(const char& ch) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(string{ch, 1});
    } else {
        *m_Value = string{ch, 1};
    }
    return *this;
}

Json& Json::operator=(const char* chPtr) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(std::move(string{chPtr}));
    } else {
        *m_Value = std::move(string{chPtr});
    }
    return *this;
}

Json& Json::operator=(const std::string& str) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(str);
    } else {
        *m_Value = str;
    }
    return *this;
}

Json& Json::operator=(const Json& js) {
    m_Value = js.m_Value;
    return *this;
}

Json& Json::operator=(Json&& js) noexcept = default;

Json& Json::operator[](unsigned int n) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(get<vector<Json>>(*m_Value));
    }
    return get<vector<Json>>(*m_Value)[n];
}

Json& Json::operator[](const std::string& str) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(get<map<string, Json>>(*m_Value));
    }
    return get<map<string, Json>>(*m_Value)[str];
}

Json& Json::operator[](const char* chPtr) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(get<map<string, Json>>(*m_Value));
    }
    return get<map<string, Json>>(*m_Value)[string{chPtr}];
}

Json::operator bool() {
    return get<bool>(*m_Value);
}

Json::operator int() {
    return static_cast<int>(get<long long>(*m_Value));
}

Json::operator long long() {
    return get<long long>(*m_Value);
}

Json::operator float() {
    return static_cast<float>(get<double>(*m_Value));
}

Json::operator double() {
    return get<double>(*m_Value);
}

Json::operator std::string() {
    return get<string>(*m_Value);
}

bool Json::empty() const noexcept {
    switch (m_Value->index()) {
        case JSON_NULL:
            return true;
        case JSON_STRING:
            return get<string>(*m_Value).empty();
        case JSON_ARRAY:
            return get<vector<Json>>(*m_Value).empty();
        case JSON_OBJECT:
            return get<map<string, Json>>(*m_Value).empty();
        default:
            return false;
    }
}

unsigned int Json::size() const {
    return get<vector<Json>>(*m_Value).size();
}


void Json::clear() {
    m_Value = make_shared<JsonBasic>(Void{});
}

void Json::copy(const Json& js) {
    if (m_Value == js.m_Value) {
        return;
    }
    switch (js.m_Value->index()) {
        case JSON_NULL: {
            m_Value = make_shared<JsonBasic>(Void{});
            break;
        }
        case JSON_BOOL: {
            m_Value = make_shared<JsonBasic>(get<bool>(*js.m_Value));
            break;
        }
        case JSON_INT: {
            m_Value = make_shared<JsonBasic>(get<long long>(*js.m_Value));
            break;
        }
        case JSON_FLOAT: {
            m_Value = make_shared<JsonBasic>(get<double>(*js.m_Value));
            break;
        }
        case JSON_STRING: {
            m_Value = make_shared<JsonBasic>(get<string>(*js.m_Value));
            break;
        }
        case JSON_ARRAY: {
            auto& thatArray = get<vector<Json>>(*js.m_Value);
            m_Value = make_shared<JsonBasic>(vector<Json>(thatArray.size()));
            for (auto iter = get<vector<Json>>(*m_Value).begin(),
                         thatIter = thatArray.begin();
                 thatIter != thatArray.end();
                 iter++, thatIter++) {
                iter->copy(*thatIter);
            }
            break;
        }
        case JSON_OBJECT: {
            auto thatJson = get<map<string, Json>>(*js.m_Value);
            auto thisJson = get<map<string, Json>>(*m_Value);
            m_Value = make_shared<JsonBasic>(JSON_OBJECT);
            for (auto& iter: thatJson) {
                thisJson[iter.first].copy(iter.second);
            }
            break;
        }
    }
}

JsonType Json::get_type() const {
    return JsonType(m_Value->index());
}

void Json::set_type(JsonType ttp) {
    m_Value = make_shared<JsonBasic>(ttp);
}

std::string Json::str() const {
    switch (m_Value->index()) {
        case JSON_NULL: {
            return string{"null"};
        }
        case JSON_BOOL: {
            if (get<bool>(*m_Value)) {
                return string{"true"};
            } else {
                return string{"false"};
            }
        }
        case JSON_INT: {
            return std::to_string(get<long long>(*m_Value));
        }
        case JSON_FLOAT: {
            return std::to_string(get<double>(*m_Value));
        }
        case JSON_STRING: {
            string res{};
            res += '"';
            const auto& str = get<string>(*m_Value);
            for (auto i = 0; i < str.length(); i++) {
                switch (str[i]) {
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
                        res += str[i];
                        break;
                    }
                }
            }
            res += '"';
            return res;
        }
        case JSON_ARRAY: {
            const auto& arr = get<vector<Json>>(*m_Value);
            if (arr.empty()) {
                return string{"[]"};
            }
            string str = "[";
            str += arr.front().str();
            for (auto iter = ++arr.begin(); iter != arr.end(); ++iter) {
                str += ",";
                str += iter->str();
            }
            str += "]";
            return str;
        }
        case JSON_OBJECT: {
            const auto& obj = get<map<string, Json>>(*m_Value);
            if (obj.empty()) {
                return string{"{}"};
            }
            string str = "{\"";
            str += obj.begin()->first;
            str += "\":";
            str += obj.begin()->second.str();
            for (auto iter = ++obj.begin(); iter != obj.end(); ++iter) {
                str += ",\"";
                str += iter->first;
                str += "\":";
                str += iter->second.str();
            }
            str += "}";
            return str;
        }
        default:
            break;
    }
    return string{};
}

void Json::resize(unsigned int new_size) {
    get<vector<Json>>(*m_Value).resize(new_size);
}

void Json::push_back(const Json& js) {
    if (m_Value.use_count() > 1) {
        m_Value = make_shared<JsonBasic>(get<vector<Json>>(*m_Value));
    }
    get<vector<Json>>(*m_Value).push_back(js);
}

unsigned int Json::capacity() const {
    return get<vector<Json>>(*m_Value).capacity();
}


void Json::erase(const std::string& str) {
    get<map<string, Json>>(*m_Value).erase(str);
}

void Json::erase(const char* chPtr) {
    get<map<string, Json>>(*m_Value).erase(chPtr);
}

void Json::merge(const Json& js) {
    get<map<string, Json>>(*m_Value).merge(get<map<string, Json>>(*js.m_Value));
}

Json::iterator& Json::iterator::operator++() {
    if (m_Value.index() == 0) {
        ++get<vector<Json>::iterator>(m_Value);
    } else {
        ++get<map<string, Json>::iterator>(m_Value);
    }
    return *this;
}

std::string Json::iterator::key() {
    return get<map<string, Json>::iterator>(m_Value)->first;
}

Json& Json::iterator::value() {
    if (m_Value.index() == 0) {
        return *get<vector<Json>::iterator>(m_Value);
    } else {
        return get<map<string, Json>::iterator>(m_Value)->second;
    }
}

Json::iterator Json::begin() {
    iterator iter;
    if (m_Value->index() == JSON_ARRAY) {
        iter.m_Value = get<vector<Json>>(*m_Value).begin();
    } else if (m_Value->index() == JSON_OBJECT) {
        iter.m_Value = get<map<string, Json>>(*m_Value).begin();
    } else {
        throw logic_error{"From class json::Json(" + to_string(this) +
                          ").begin():iterator can only use for ARRAY or OBJECT, but the type is " +
                          JSON_TYPE_TO_STRING[m_Value->index()] + "!"};
    }
    return iter;
}

Json::iterator Json::end() {
    iterator iter;
    if (m_Value->index() == JSON_ARRAY) {
        iter.m_Value = get<vector<Json>>(*m_Value).end();
    } else if (m_Value->index() == JSON_OBJECT) {
        iter.m_Value = get<map<string, Json>>(*m_Value).end();
    } else {
        throw logic_error{"From class json::Json(" + to_string(this) +
                          ").end():iterator can only use for ARRAY or OBJECT, but the type is " +
                          JSON_TYPE_TO_STRING[m_Value->index()] + "!"};
    }
    return iter;
}

JsonParser::JsonParser(const std::string& str) {
    m_Str = str;
    m_Idx = 0;
    m_Json = parse_json();
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
        default:
            break;
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
                    string("From class JsonParser(") + to_string(this) + "):Parse array error(c:" +
                    std::to_string(m_Idx) +
                    "):This str of Json misses a ','."
            };
        }
        m_Idx++;

        try {
            json.push_back(parse_json());
        } catch (logic_error& logicError) {
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
                        string("From class JsonParser(") + to_string(this) + "):Parse error(c:" +
                        std::to_string(m_Idx) +
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
    get<map<string, Json>>(*json.m_Value)[key] = parse_json();
    skip_white_space();
    while (m_Str[m_Idx] != '}') {
        if (m_Str[m_Idx] != ',') {
            throw logic_error{
                    string("From class JsonParser(") + to_string(this) + "):Parse object error(c:" +
                    std::to_string(m_Idx) +
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
                            "From class JsonParser(" + to_string(this) + "):Parse object error(c:" +
                            std::to_string(m_Idx) +
                            "):Unexpected character '" + m_Str[m_Idx] + "'. It should be '\"'.")
            };
        }
        key = parse_string();
        skip_white_space();
        if (m_Str[m_Idx] != ':') {
            throw logic_error{
                    string(
                            "From class JsonParser(" + to_string(this) + "):Parse object error(c:" +
                            std::to_string(m_Idx) +
                            "):Unexpected character '" + m_Str[m_Idx] + "'. It should be ':'.")
            };
        }
        read_token_char();
        get<map<string, Json>>(*json.m_Value)[key] = parse_json();
        skip_white_space();
    }
    m_Idx++;
    return json;
}

JsonParser::operator Json() {
    return m_Json;
}
