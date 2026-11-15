#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
enum class Kind { End, Identifier, Number, String, Plus, Minus, Star, Slash, Equal,
                  LParen, RParen, Semicolon, ShiftLeft, Dot, Scope };
struct Token { Kind kind; std::string text; std::size_t line; };

class Lexer {
    const std::string& source; std::size_t pos = 0, line = 1;
    char peek(std::size_t n = 0) const { return pos + n < source.size() ? source[pos+n] : '\0'; }
    char take() { char c = peek(); if (c) { ++pos; if (c == '\n') ++line; } return c; }
public:
    explicit Lexer(const std::string& s) : source(s) {}
    std::vector<Token> scan() {
        std::vector<Token> out;
        while (peek()) {
            if (std::isspace(static_cast<unsigned char>(peek()))) { take(); continue; }
            if (peek() == '/' && peek(1) == '/') { while (peek() && take() != '\n') {} continue; }
            const auto startLine = line; char c = peek();
            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                std::string word; while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') word += take();
                out.push_back({Kind::Identifier, word, startLine}); continue;
            }
            if (std::isdigit(static_cast<unsigned char>(c))) {
                std::string number; while (std::isdigit(static_cast<unsigned char>(peek())) || peek() == '.') number += take();
                out.push_back({Kind::Number, number, startLine}); continue;
            }
            if (c == '"') {
                take(); std::string value;
                while (peek() && peek() != '"') { if (peek() == '\\') { take(); char e = take(); value += e == 'n' ? '\n' : e; } else value += take(); }
                if (!peek()) throw std::runtime_error("line " + std::to_string(line) + ": unterminated string");
                take(); out.push_back({Kind::String, value, startLine}); continue;
            }
            take();
            switch (c) {
                case '+': out.push_back({Kind::Plus,"+",startLine}); break;
                case '-': out.push_back({Kind::Minus,"-",startLine}); break;
                case '*': out.push_back({Kind::Star,"*",startLine}); break;
                case '/': out.push_back({Kind::Slash,"/",startLine}); break;
                case '=': out.push_back({Kind::Equal,"=",startLine}); break;
                case '(': out.push_back({Kind::LParen,"(",startLine}); break;
                case ')': out.push_back({Kind::RParen,")",startLine}); break;
                case ';': out.push_back({Kind::Semicolon,";",startLine}); break;
                case '.': out.push_back({Kind::Dot,".",startLine}); break;
                case '<':
                    if (peek() == '<') { take(); out.push_back({Kind::ShiftLeft,"<<",startLine}); }
                    else throw std::runtime_error("line " + std::to_string(line) + ": unexpected '<'");
                    break;
                case ':':
                    if (peek() == ':') { take(); out.push_back({Kind::Scope,"::",startLine}); }
                    else throw std::runtime_error("line " + std::to_string(line) + ": unexpected ':'");
                    break;
                default: throw std::runtime_error("line " + std::to_string(line) + ": unexpected character '" + c + "'");
            }
        }
        out.push_back({Kind::End,"",line}); return out;
    }
};

struct Value {
    double number = 0; std::string string; bool isString = false;
    static Value num(double n) { return {n,{},false}; }
    static Value str(std::string s) { return {0,std::move(s),true}; }
    std::string show() const { if (isString) return string; std::ostringstream o; o << number; return o.str(); }
};

class Interpreter {
    std::vector<Token> tokens; std::size_t current = 0;
    std::unordered_map<std::string,Value> vars;
    Token& token() { return tokens[current]; }
    bool match(Kind k) { if (token().kind != k) return false; ++current; return true; }
    Token expect(Kind k, const std::string& what) {
        if (token().kind != k) throw std::runtime_error("line " + std::to_string(token().line) + ": expected " + what + ", got '" + token().text + "'");
        return tokens[current++];
    }
    int precedence(Kind k) const { if (k == Kind::Plus || k == Kind::Minus) return 10; if (k == Kind::Star || k == Kind::Slash) return 20; return -1; }
    Value expression(int min = 0) {
        Value left;
        if (match(Kind::Minus)) { left = Value::num(-expression(30).number); }
        else if (match(Kind::LParen)) { left = expression(); expect(Kind::RParen, "')'"); }
        else if (token().kind == Kind::Number) { auto t = tokens[current++]; left = Value::num(std::stod(t.text)); }
        else if (token().kind == Kind::String) { left = Value::str(tokens[current++].text); }
        else if (token().kind == Kind::Identifier) {
            auto name = tokens[current++].text;
            auto it = vars.find(name); if (it == vars.end()) throw std::runtime_error("line " + std::to_string(token().line) + ": unknown variable '" + name + "'");
            left = it->second;
        } else throw std::runtime_error("line " + std::to_string(token().line) + ": expected expression");
        while (precedence(token().kind) >= min) {
            Kind op = token().kind; int p = precedence(op); ++current; Value right = expression(p + 1);
            if (left.isString || right.isString) throw std::runtime_error("string arithmetic is not supported");
            if (op == Kind::Plus) left = Value::num(left.number + right.number);
            if (op == Kind::Minus) left = Value::num(left.number - right.number);
            if (op == Kind::Star) left = Value::num(left.number * right.number);
            if (op == Kind::Slash) { if (right.number == 0) throw std::runtime_error("division by zero"); left = Value::num(left.number / right.number); }
        }
        return left;
    }
    void statement() {
        if (token().kind == Kind::Identifier &&
            (token().text == "int" || token().text == "double" || token().text == "float")) {
            ++current; auto name = expect(Kind::Identifier, "variable name").text;
            expect(Kind::Equal, "'='"); Value value = expression(); expect(Kind::Semicolon, "';'");
            vars[name] = value; return;
        }
        if (token().kind == Kind::Identifier && token().text == "std") {
            ++current; expect(Kind::Scope, "'::'"); auto stream = expect(Kind::Identifier, "'cout' or 'endl'").text;
            if (stream == "endl") { std::cout << '\n'; expect(Kind::Semicolon, "';'"); return; }
            if (stream != "cout") throw std::runtime_error("only std::cout is supported");
            while (match(Kind::ShiftLeft)) {
                if (token().kind == Kind::Identifier && token().text == "std") {
                    ++current; expect(Kind::Scope, "'::'"); expect(Kind::Identifier, "'endl'"); std::cout << '\n';
                } else std::cout << expression().show();
            }
            expect(Kind::Semicolon, "';'"); return;
        }
        if (token().kind == Kind::Identifier) {
            auto name = tokens[current++].text;
            if (vars.find(name) == vars.end()) throw std::runtime_error("unknown variable '" + name + "'");
            expect(Kind::Equal, "'='"); Value value = expression(); expect(Kind::Semicolon, "';'");
            vars[name] = value; return;
        }
        throw std::runtime_error("line " + std::to_string(token().line) + ": unsupported statement");
    }
public:
    explicit Interpreter(const std::string& source) : tokens(Lexer(source).scan()) {}
    void run() { while (token().kind != Kind::End) statement(); }
};
}

int main(int argc, char** argv) {
    if (argc != 2) { std::cerr << "usage: cxx <source.cpp>\n"; return 2; }
    try {
        std::ifstream file(argv[1]);
        if (!file) throw std::runtime_error("cannot open source file: " + std::string(argv[1]));
        std::ostringstream buffer; buffer << file.rdbuf();
        Interpreter(buffer.str()).run();
    } catch (const std::exception& e) {
        std::cerr << "cxx: " << e.what() << '\n'; return 1;
    }
    return 0;
}
