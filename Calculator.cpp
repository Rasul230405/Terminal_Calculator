// Calculator.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Simple terminal calculator
//  Revised by Rasul Abbaszazdeh September 2022

/*
                               ----GRAMMER----
            Calculation:
               Calculation
               Quit
               Print
               Calulation Statement
            Statement:
                Decleration
                Expression
            Decleration:
                "let" Name "=" Expression
            Expression:
               Term
               Term + Expression
               Term - Expression
           Term:
               Primary
               Primary * Term
               Primary / Term
           Secondary:
               Primary
               Number"!"
           Primary:
               Number
               ( Expression )
               sqrt "(" Expression ")"
               "-" Primary
               "+" Primary
           Number:
               floation-point literal
*/


#include "std_lib_facilities.h"

class Variable {
public:
    string name;
    double value;
    const char kind;     // represent constants
    Variable (string n, double v) : name(n), kind(0), value(v) { }
    Variable (string n, char k, double v) : name(n), kind(k), value(v) { }
};

class Symbol_table {
public:
    vector<Variable>var_table;
    void set_value(string s, double v);
    double get_value(string s);
    double define_name(string var, char c, double val);   // create Variable and add it to the var_table
    bool is_declared(string var);          // check if s declared in var_table
};

Symbol_table st;

class Token {
public:
    char kind;
    double value;
    string name;

    Token(char ch) : kind(ch), value(0) { }
    Token(char ch, double val) : kind(ch), value(val) { }
    Token(char ch, string s) : kind(ch), value(0), name(s) { }
};

double term();          // read and evaluate Term
double secondary();     // read and evaluate Secondary
double primary();       // read and evaluate Primary
double expression();    // read and evauuate Expression
double statement();     // read and evaluate Statement
double declaration(bool b);   // read and evaluate Decleration
void calculate();       // handle calculation loop

double pow(double i1, int i2); // calculate i1^i2
double calc_sin();
int factorial(int n);
double calc_cos();


void clean_up_mess();       // in case of error recover program
void print_help();


const char number = '8';  // indicate number's kind
const char print = ';';   // t.kind == print means that t is a print token
const char quit = 'q';    // t.kind == quit means that t is a quit token
const char prompt = '>';
const char result = '=';
const char let = 'L';      // token for declaring variable
const char name = 'a';     // token that represents Name
string deckey = "let";     // decleration key for decleration
const char sq = 'S';     // token that represents sqrt()
string decsquare = "sqrt"; // declaration key for sqrt()
const char powt = 'P';     // token that represents pow()
string decpow = "pow";        // declaration key for pow()
string decexit = "exit";       // declaration key for exit
const char constant = 'C';      // constant token
const char ch = 'c';       // represents constants
string decconst = "const";  // declaration key for user defined constants
const char help = 'h';      // token that rperesents help
string dechelp = "help";    // declaration key for help
const char sinus = 's';      //  token represents sinus
string decsinus = "sin";    // declaration key for sinus
const char cosinus = 'W';
string deccos = "cos";
const string greeting_msg = "Welcome to Simple Calculator\n\n'=' for print\nType 'exit' for exit\n'h' for help\n";


class Token_stream{
    public:
        Token_stream();         // make a Token_stream that rads from cin
        Token get();            // get a Token
        void putback(Token t);  // put a Token back
        void ignore(char c);    // ignore characters up to and c included
    private:
        bool full {false};      // is there a Token in the buffer?
        Token buffer = {'0'};           // where we store a 'putback' Token
};

Token_stream::Token_stream()
    :full(false), buffer(0)
{
}

void Token_stream::putback(Token t)
{
    if (full) error("putback() into a full buffer");
    buffer = t;         // copy t to buffer
    full = true;        // buffer is now full
}

Token_stream ts;    // provides get() and putback()

Token Token_stream::get()
{
    if (full) {
        full = false;
        return buffer;
    }
    char ch;
    cin >> ch;
   
    switch (ch) {
    case print: 
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/':
    case '{':
    case '}':
    case '!':
    case '%':
    case '=':
    case ',':
        return Token{ ch };   // let each character represent itself
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        cin.putback(ch);
        double val;
        cin >> val;
        return Token{ number, val };
    }
    default:
        if (isalpha(ch)) {
            string s;
            s += ch;
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
            cin.putback(ch);
            if (s == deckey) return Token{ let };
            else if (s == decsquare) return Token{ sq };
            else if (s == decpow) return Token{ powt };
            else if (s == decexit) return Token{ quit };
            else if (s == decconst) return Token{ constant };
            else if (s == dechelp) return Token{ help };
            else if (s == decsinus) return Token{ sinus };
            else if (s == deccos) return Token{ cosinus };
            return Token{ name, s }; 
        }
        if (isspace(ch)) {
            if (ch == '\n')
                return Token{ print };
        }
        error("Bad Token");
    }
}

void Token_stream::ignore(char c)
{
    if (full && c == buffer.kind) {
        full = false;
        return;
    }
    full = false;

    // now look for c
    char ch;
    while (cin >> ch) {
        if (ch == print) return;
    } 
}

//------------------------------------------------------------------------------


int main()
{
    try {
        st.define_name("k", ch, 1000);
        st.define_name("pi", ch, 3.1415926535);
        st.define_name("e", ch, 2.7182818284);

        calculate();
        keep_window_open();
        return 0;
    }
    catch (runtime_error& e) {
        cerr << e.what() << endl;
        keep_window_open();
        cout << "Enter '~' to close the window\n";
        char ch;
        while (cin >> ch)
            if (ch == '~') return 1;
        return 1;
    }
    catch (...) {
        cerr << "Unknown error\n";
        return 2;
    }
    
}

void calculate() {
    cout << greeting_msg;
        while (cin) {
            try {
                cout << prompt;
                Token t = ts.get();
                if (t.kind == quit) return;
                if (t.kind == help) print_help();
                while (t.kind == print) t = ts.get(); 
                ts.putback(t);
                cout << result << statement() << endl;
            }
            catch (exception& e) {
                cerr << e.what()  << ".enter ';' to continue" << endl;
                clean_up_mess();
            }
        }
}

void print_help () {
    cout << "Simple Calculator Manual\n"
        << "========================\n"
        << "This calculator program supports +, -, *, and / operations\n"
        << "Enter any form of compound statement followed by ';' for result\n"
        << "- ex: 4 + 1; (5-2)/{6*(8+14)}\n"
        << "The modulo operator % may be used on all numbers\n"
        << "An '!' placed after a value will calculate the factorial of it\n"
        << "- ex: 4! = 4 * 3 * 2 * 1\n"
        << "Square root and exponentiation are provided by 'sqrt' and 'pow'\n"
        << "- ex: sqrt(25) = 5, pow(5,2) = 25\n"
        << "Variable assignment is provided using the 'let' keyword:\n"
        << "- ex: let x = 37; x * 2 = 74; x = 4; x * 2 = 8\n\n";
}

void clean_up_mess() {
    ts.ignore(print);
}

double Symbol_table::get_value(string s) {
    for (int i = 0; i < st.var_table.size(); ++i) {
        if (s == st.var_table[i].name) return st.var_table[i].value;
    }
    error("get:undefined variable", s);
}

void Symbol_table::set_value(string s, double v) {
    for (int i = 0; i < st.var_table.size(); ++i)
    {
        if (s == st.var_table[i].name) {
            if (st.var_table[i].kind == ch) error("You can not change constant's value");
            st.var_table[i].value = v;
            return;
        }
    }
}

bool Symbol_table::is_declared(string var) 
// chech if var already defined or not
{
    for (int i = 0; i < st.var_table.size(); ++i) {
        if (var == st.var_table[i].name) return true;
    }
    return false;
}

double Symbol_table::define_name(string var, char c, double val) 
// add new variable to var_table
{
    if (is_declared(var)) error(var, "has already defined");
    if (c == 0) st.var_table.push_back(Variable(var, 0, val));
    if (c == ch) st.var_table.push_back(Variable(var, c, val));
    return val;
}

double declaration(bool is_constant)
{
    Token t = ts.get();
    if (t.kind != name) error("name expected in declaration");
    string var_name = t.name;

    t = ts.get();
    if (t.kind != '=') error("= expected in declaration of", var_name);

    double d = expression();
    if (is_constant == true) st.define_name(var_name, ch, d);
    else { st.define_name(var_name, 0, d); }
    return d;
}

double assignment(string s)
{
    double d = expression();
    st.set_value(s, d);
    return d;
}

double statement() {
    Token t = ts.get();
    switch (t.kind) {
    case let:
        return declaration(false);
    case constant:
        return declaration(true);
    default:
        ts.putback(t);
        return expression();
    }
}

double expression() {
    double left = term();
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
        case '+':
            left += term();
            t = ts.get();
            break;
        case '-':
            left -= term();
            t = ts.get();
            break;
        default:
            ts.putback(t);
            return left;
        }
    }
}

double term() {
    double left = secondary();
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
        case '*':
            left *= secondary();
            t = ts.get();
            break;
        case '/':
        {
            double d = secondary();
            if (d == 0) error("divide by zero");
            left /= d;
            t = ts.get();
            break;
        }
        case '%':
        {
            int i1 = narrow_cast<int>(left);
            int i2 = narrow_cast<int>(primary());
            if (i2 == 0) error("divide by zero");
            left = i1%i2;
            t = ts.get();
            break;
        }
        default:
            ts.putback(t);
            return left;
        }
    }
}

int factorial(int n) {
    if (n > 1)
        return n * factorial(n - 1);
    else
        return 1;
}

double pow(double i1, int i2)
{
    double d = 1;
    for (int i = 0; i < i2; ++i)
        d *= i1;
    return d;
}

double calc_sin() {
    char ch;
    while (cin.get(ch) && ch != '(') error("'(' expected");
    cin.putback(ch);
    double d = expression();
    if (d == 0 || d == 180) return 0;
    return sin(d * 3.1415926535 / 180);
}

double calc_cos() {
    char ch;
    while (cin.get(ch) && ch != '(') error("'(' expected");
    cin.putback(ch);
    double d = expression();
    if (d == 90 || d == 270) return 0;
    return cos(d * 3.1415926535 / 180);
}
double secondary() {
    double left = primary();
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
        case '!':
            return factorial(left);
        default:
            ts.putback(t);
            return left;
        }
    }

}

double primary() {
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
        case '(':
        {
            double d = expression();
            t = ts.get();
            if (t.kind != ')') error("')' expected");
            return d;

        }
        case '{':
        {
            double d = expression();
            t = ts.get();
            if (t.kind != '}') error("'}' expected");
            return d;
        }
        case number:
            return t.value;
        case name:
        {
            string name = t.name;
            double d = st.get_value(name);
            t = ts.get();
            if (t.kind == '=') return assignment(name);
            ts.putback(t);
            return d;
        }
        case '-':
            return -primary();
        case '+':
            return primary();
        case sq:
        {
            t = ts.get();
            if (t.kind != '(') error("'(' expected");
            double d = expression();
            if (d < 0) error("argument of sqrt must be positive expression");
            t = ts.get();
            if (t.kind != ')') error("')' expected");
            return sqrt(d);
        }
        case powt:
        {
            t = ts.get();
            if (t.kind != '(') error("'(' expected");
            double d = expression();
            if (d < 0) error("argument of pow must be positive expression");
            t = ts.get();
            if (t.kind != ',') error("',' expected in pow() function");
            double d2 = expression();
            t = ts.get();
            if (t.kind != ')') error("')' expected");
            return pow(d, narrow_cast<int>(d2));
        }
        case sinus:
            return calc_sin();
        case cosinus:
            return calc_cos();
        default:
            error("primary expected");
        }
    }
}