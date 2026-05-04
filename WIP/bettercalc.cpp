#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

void displayHelp();
bool isValidInput(const char);

enum pass
{
    SUBEXPRESSIONS,
    UNARYOPS,
    EXPONENTIATION,
    MULTIPLICATION,
    ADDITION
};

enum class token_t
{
    BINARYOP,
    UNARYOP,
    MULTICHARBINARY,
    MULTICHARUNARY,
    NUMBER,
    ROOT,
    SUBEXPR,
    VARIABLE,
    INVALID
};

enum class tokenCategory_t
{
    NUMBER,
    SUBEXPR,
    OPERATOR
};

//Todo: Implement evaluation, recursively call getTokens() for each subexpression and evaluate those first

bool isNumberPart(char input)
{
    return (input>='0' && input<='9') || input=='.' || input=='e';
}

bool isNumber(const std::string &input)
{
    uint dotCount{};
    uint eCount{};

    if(input=="inf") throw std::runtime_error("Encountered infinity");
    for(uint i{}; i<input.length(); i++)
    {
        if(input.at(0)=='-') continue;
        if(input.at(i)=='e') 
        {
            if(i+2<input.length() && input.at(i)=='e' && (input.at(i+1)=='+' || input.at(i+1)=='-')) i+=2;
            eCount++;
        }
        if(input.at(i)=='.')
        {
            dotCount++;
            if(eCount) return false;
            if(dotCount>1) return false;
        }
        if(eCount>1) return false;

        if(!isNumberPart(input.at(i))) return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class token
{
    public:

    private:

    token_t tokenType{};
    tokenCategory_t tokenCategory{};
    std::string tokenValue{};

    token_t determineType(std::string &value)
    {
        if(value.length()==0) throw std::runtime_error("Empty argument!");

        if(value.length()==1)
        {
            if(isBinaryOp(value.at(0))) return token_t::BINARYOP;
            else if(isUnaryOp(value.at(0))) return token_t::UNARYOP;
        }
        if(value.length()>1)
        {
            if(isBinaryOp(value.at(0)) && isBinaryOp(value.at(1)) && value.at(0)==value.at(1) && value.at(0)=='*') return token_t::MULTICHARBINARY;
            if(isUnaryOp(value.at(0)) && isUnaryOp(value.at(1)) && value.at(0)==value.at(1) && value.at(0)=='!') return token_t::MULTICHARUNARY;
        }
        if(isNumber(value)) return token_t::NUMBER;
        else if(isRoot(value)) return token_t::ROOT;
        else if(isSubexpr(value)) return token_t::SUBEXPR;
        else if(value=="x") return token_t::VARIABLE;
        return token_t::INVALID;
    }
    ///////////////////////////////////////////////
    static bool isBinaryOp(const char c)
    {
        return c=='+' || c=='*' || c=='/' || c=='^';
    }

    static bool isUnaryOp(const char c)
    {
        return c=='!'|| c=='-';
    }
    ///////////////////////////////////////////////
    static bool isRoot(std::string &input)
    {
        uint argc{1};
        for(uint i{}; i<input.length(); i++)
        {
            //root(expr,expr)
            if(i==0)
            {
                if(input.find("root(") != 0) return false;
                i += input.find("root(")+5;
            }

            if(i>=input.length()) throw std::runtime_error("Syntax error in root() call!");
            if(input.find(',') == std::string::npos)
            {
                input.insert(i, "2,");
            }
            if(input.at(i)==',') argc++;
            if(argc>2) throw std::runtime_error("Too many arguments in root() call!");
            if(!(input.at(input.length()-1)==')')) throw std::runtime_error("Did you forget closing parentheses?\n\t   Bad parentheses in root() call!");
        }
        return true;
    }
    ///////////////////////////////////////////////
    static bool isSubexpr(std::string &input)
    {
        bool isTrue{};
        if(input.length()<3) return false;
        if((input.at(0)=='(' && input.at(input.length()-1)==')')) isTrue=true;
        input.erase(0, 1);
        input.pop_back();
        return isTrue;
    }
    ///////////////////////////////////////////////
    static tokenCategory_t determineTokenCategory(token_t type)
    {
        if(type==token_t::NUMBER || type==token_t::VARIABLE) return tokenCategory_t::NUMBER;
        else if(type==token_t::SUBEXPR) return tokenCategory_t::SUBEXPR;
        else return tokenCategory_t::OPERATOR;
    }
    ///////////////////////////////////////////////
    public:
    token(std::string value)
    {
        tokenType = determineType(value);
        if(tokenType==token_t::INVALID)
        {
            std::cerr<<"Bad Token: "<<value<<'\n';
            throw std::runtime_error("tokenType==tokenType_t::INVALID");
        }

        if(tokenType==token_t::ROOT)
        {
            std::cerr<<"\nRoot not implemented yet lmao\n";
            std::terminate();
        }
        tokenCategory=determineTokenCategory(tokenType);
        tokenValue = value;
    }
    ///////////////////////////////////////////////
    double number(double xValue=NAN)
    {
        if(xValue!=NAN && this->tokenType==token_t::VARIABLE)
        {
            std::ostringstream asString;
            asString << xValue;
            this->tokenValue=asString.str();
            this->tokenType=token_t::NUMBER;
        }

        if (tokenType != token_t::NUMBER) throw std::runtime_error("tokenType != tokenType_t::NUMBER");
        double valueAsDouble{};
        valueAsDouble = std::stod(tokenValue);
        return valueAsDouble;
    }
    ///////////////////////////////////////////////
    std::string value()
    {
        return tokenValue;
    }  
    token_t type()
    {
        return tokenType;
    }
    tokenCategory_t typeCategory()
    {
        return tokenCategory;
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<token> getTokens(const std::string&);
std::vector<double> getVariableArgs(std::vector<token>&);
double calculation(std::vector<token>, double xValue=NAN);
double evaluateRoot(std::vector<token>, double xValue=NAN);
double evaluateUnary(token, token, double xValue=NAN);
double evaluateBinary(token, token, token, double xValue=NAN);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

    // token number("x");
    // token operand("-");
    // double xValue{6};
    // std::cout<<evaluateUnary(number, operand, xValue);
    // return 0;

    bool passedInAsArg{};
    std::string equation{};
    if(argc>1)
    {
        equation+=argv[1];
        std::cout<<"\nPassed " <<equation<< " as input from command line\n";
        passedInAsArg=true;
    }
    std::vector<double> varRange{};
    if(equation.find('x')!=std::string::npos)
    {
        if(argc>3)
        {
            varRange.push_back(std::stod(argv[2]));
            varRange.push_back(std::stod(argv[3]));
            varRange.push_back(std::stod(argv[4]));
            
            if(varRange[0]>=varRange[1]) throw std::runtime_error("Invalid range!");
            if(varRange[1]-varRange[0]>varRange[2]*1000) throw std::runtime_error("Too many calculations requested!");
        }
        else throw std::runtime_error("Included variable but did not specify all of the following: min, max, step");
    }
    while(true)
    {
        double result{};
        if(equation!="") goto passedInAsArg;
        std::cout << "Type your equation (? for help, q to quit):\n=> ";
        std::getline(std::cin, equation);
        if(equation.length()==0) throw std::runtime_error("Empty input");
        if(equation.at(0)=='q') break;
        if(equation.at(0)=='?')
        {
            displayHelp();
            equation.clear();
            continue;
        }
        passedInAsArg:
        for(int i{}; i<equation.length(); i++) if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--); //Basic garbage removal
        if(equation.length()==0) throw std::runtime_error("No valid input");
        std::vector<token> tokens = getTokens(equation);
        if(!passedInAsArg)
        {
            varRange = getVariableArgs(tokens);
        }
        if(varRange.empty()) std::cout << "\nResult: " << calculation(tokens) << '\n';
        else for(double xValue=varRange.at(0); xValue<=varRange.at(1); xValue+=varRange.at(2))
        {
            result=calculation(tokens, xValue);
            std::cout<<"\nFor x = " << xValue << ": " << result;
        }
        std::cout<<'\n';
        equation.clear();
        tokens.clear();
        if(passedInAsArg) break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayHelp()
{
    std::cout<<"\nThis calculator allows you to write out an equation using numbers +, -, *, /, ^, x and the function root(denominator, enumerator)\nExample: 3+root(2,1+3) = 5\nroot() may be called with one argument, defaulting to square root.\nFor example, root(4) is 2.\n\n";
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isValidInput(const char c)
{
    return (c>='0'&&c<='9')||c=='.'||c=='x'||c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')'||c=='r'||c=='o'||c=='t'||c==','||c=='e'||c=='^';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<token> getTokens(const std::string &input)
{
    uint nestingLevel{};
    std::vector<token> tokens{};
    std::string currentToken{};
    bool fixOffByOne{};

    for(uint i{}; i<input.length(); i++)
    {
        if(input.at(i)=='+') currentToken='+';
        else if (input.at(i)=='-') currentToken='-';
        else if (input.at(i)=='^') currentToken='^';
        else if (input.at(i)=='/') currentToken='/';

        else if (input.at(i)=='!')
        {
            currentToken='!';
            if(input.length()>i+1) if(input.at(i+1)=='!')
            {
                currentToken="!!";
                i++;
            }
        }
        else if (input.at(i)=='*')
        {
            currentToken="*";
            if(input.length()>i+1) if(input.at(i+1)=='*')
            {
                currentToken="**";
                i++;
            }
        }
        else if (input.at(i)=='x') currentToken='x';
        else if(input.at(i)=='r' || input.at(i)=='(')
        {
            currentToken.clear();
            if(input.at(i)=='r')
            {
                if(input.find("root(", i)==i)
                {

                    i+=5;
                    currentToken.append("root(");
                    if(i==input.length()) throw std::runtime_error("Bad function call!");
                }
                else throw std::runtime_error("Bad function name or stray characters!");
            }
            nestingLevel++;
            for(uint j=i; j<input.length() && nestingLevel>0; j++)
            {
                if(input.at(j)==')') nestingLevel--;
                else if(input.at(j)=='(' && j!=i) nestingLevel++;
                currentToken.push_back(input.at(j));
                i=j;
            }
        }
        else for(fixOffByOne=true; i<input.length() && ((input.at(i)>='0' && input.at(i)<='9') || input.at(i)=='.' || input.at(i)=='e'); i++)
        {
            if(i+1<input.length() && input.at(i)=='e' && (input.at(i+1)=='+' || input.at(i)=='-'))
            {
                currentToken.push_back(input.at(i));
                i++;
            }
            currentToken.push_back(input.at(i));
        }
        if(fixOffByOne)
        {
            fixOffByOne=false;
            i--;
        }
        tokens.push_back({currentToken});
        currentToken.clear();
    }

    return tokens;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<double> getVariableArgs(std::vector<token> &tokens)
{
    std::vector<double> varRange{};
    bool obtainedArgs{};
    for(uint i{}; i<tokens.size(); i++)
    {
        if(tokens.at(i).value().find('x')!=std::string::npos && obtainedArgs==false)
        {                
            for(uint j{}; j<3; j++)
            {
                if(j==0)    std::cout << "\nSpecify variable minimum: ";
                if(j==1)    std::cout << "\nSpecify variable maximum: ";
                if(j==2)    std::cout << "\nSpecify variable increment/step: ";
                std::string input;
                std::cin>>input;
                for(uint l{}; l<input.length(); l++) if(!(isValidInput(input.at(l)))) input.erase(input.begin()+l--);

                if(isNumber(input))
                {
                    varRange.push_back(std::stod(input)); //Min, Max, Step
                }
            }
            std::cin.clear();
            obtainedArgs=true;
        }
    }
    
    if(!varRange.empty())
    {
        if(varRange[0]>=varRange[1]) throw std::runtime_error("Invalid range!");
        if(varRange[1]-varRange[0]>varRange[2]*1000) throw std::runtime_error("Too many calculations requested!");
        std::cin.ignore();
    }
    return varRange;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double calculation(std::vector<token> tokens, double xValue)
{
    double result{};

    for(uint i{1}; i<tokens.size(); i++)
    {
        if(tokens.at(i).type()==token_t::VARIABLE && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
            tokens.insert(tokens.begin()+i++, token("*"));
        if(tokens.at(i).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i-1).typeCategory()!=tokenCategory_t::OPERATOR)
            tokens.insert(tokens.begin()+i++, token("*"));
        if(tokens.at(i).value()=="-" && tokens.at(i-1).typeCategory()!=tokenCategory_t::OPERATOR)
            tokens.insert(tokens.begin()+i++, token("+"));
        if(tokens.at(i-1).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i).typeCategory()!=tokenCategory_t::OPERATOR)
            tokens.insert(tokens.begin()+i++, token("*"));
    }

    for(uint pass{}; pass<=ADDITION; pass++)
    {
        for(int i{}; i<tokens.size(); i++)
        {
            if(pass==SUBEXPRESSIONS)
            {
                if(tokens.at(i).type()==token_t::SUBEXPR)
                {
                    double evaluatedSubexpr=calculation(getTokens(tokens.at(i).value()), xValue);
                    std::ostringstream evaluatedSubexprAsOSStream;
                    evaluatedSubexprAsOSStream << evaluatedSubexpr;
                    tokens.at(i) = token(evaluatedSubexprAsOSStream.str());
                }
                else if(tokens.at(i).type()==token_t::ROOT)
                {
                    double evaluatedRoot=evaluateRoot(getTokens(tokens.at(i).value()), xValue);
                    std::ostringstream evaluatedRootAsOSStream;
                    evaluatedRootAsOSStream << evaluatedRoot;
                    tokens.at(i) = token(evaluatedRootAsOSStream.str());
                }
                else continue;
            }
            else if(pass==UNARYOPS)
            {
                if(i==0) continue;
                if(tokens.at(i-1).value()=="-" && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    double evaluatedUnary=evaluateUnary(tokens.at(i), tokens.at(i-1), xValue);
                    std::ostringstream evaluatedResultAsOSStream;
                    evaluatedResultAsOSStream << evaluatedUnary;
                    tokens.at(i-1)=token(evaluatedResultAsOSStream.str());
                    tokens.erase(tokens.begin()+i);
                    i--;
                }
                if((tokens.at(i).type()==token_t::UNARYOP || tokens.at(i).type()==token_t::MULTICHARUNARY) && tokens.at(i-1).type()==token_t::NUMBER)
                {
                    double evaluatedUnary=evaluateUnary(tokens.at(i-1), tokens.at(i), xValue);
                    std::ostringstream evaluatedResultAsOSStream;
                    evaluatedResultAsOSStream << evaluatedUnary;
                    tokens.at(i-1)=token(evaluatedResultAsOSStream.str());
                    tokens.erase(tokens.begin()+i);
                    i--;
                }
            }
            else if(pass==EXPONENTIATION)
            {
                if(i==0)
                    for(i=tokens.size()-1; i>0; i--)
                    {
                        if(i-2<tokens.size())
                        {
                            if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="^" || tokens.at(i-1).value()=="**") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                            {
                                double evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
                                std::ostringstream evaluatedResultAsOSStream;
                                evaluatedResultAsOSStream << evaluatedBinary;
                                tokens.at(i-2)=token(evaluatedResultAsOSStream.str());
                                tokens.erase(tokens.begin()+i-1);
                                tokens.erase(tokens.begin()+i-1);
                            }
                        }
                    }
            }
            else if(pass==MULTIPLICATION)
            {
                if(i<=1) continue;
                if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="*" || tokens.at(i-1).value()=="/") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    double evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
                    std::ostringstream evaluatedResultAsOSStream;
                    evaluatedResultAsOSStream << evaluatedBinary;
                    tokens.at(i-2)=token(evaluatedResultAsOSStream.str());
                    tokens.erase(tokens.begin()+i-1);
                    tokens.erase(tokens.begin()+i-1);
                    i-=2;
                }
            }
            else if(pass==ADDITION)
            {
                if(i<=1) continue;
                if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="+" || tokens.at(i-1).value()=="-") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    double evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
                    std::ostringstream evaluatedResultAsOSStream;
                    evaluatedResultAsOSStream << evaluatedBinary;
                    tokens.at(i-2)=token(evaluatedResultAsOSStream.str());
                    tokens.erase(tokens.begin()+i-1);
                    tokens.erase(tokens.begin()+i-1);
                    i-=2;
                }
            }
        }
    }
    if(tokens.size()==1 && tokens.at(0).value()=="x")
    {
        std::ostringstream xValueAsOSStream;
        xValueAsOSStream<<xValue;
        tokens.at(0)=token(xValueAsOSStream.str());
    }
    
    if(tokens.size()==1) result=std::stod(tokens.at(0).value());
    else throw std::runtime_error("Malformed expression!");
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double evaluateRoot(std::vector<token> tokens, double xValue)
{ 
    double result{};
    for(uint i{}; i<tokens.size(); i++)
    {
        if(tokens.at(i).type()==token_t::SUBEXPR) calculation(getTokens(tokens.at(i).value()), xValue);
        if(tokens.at(i).type()==token_t::ROOT) evaluateRoot(getTokens(tokens.at(i).value()), xValue);
    }    
    //root(2,4) == pow(4, 1/2)
    //return std::pow(right, 1/rootvalue)
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double evaluateBinary(token numberStringLeft, token operation, token numberStringRight, double xValue)
{
    double numberLeft{numberStringLeft.number(xValue)};
    double numberRight{numberStringRight.number(xValue)};

    if(operation.value()=="+") return numberLeft+numberRight;
    if(operation.value()=="*") return numberLeft*numberRight;
    if(operation.value()=="/") return numberLeft/numberRight;
    if(operation.value()=="^" || operation.value()=="**") return std::pow(numberLeft, numberRight);
    else throw std::runtime_error("Somehow, an unhandled binary-type operation?\nCongrats bro, tell me how you did that");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double evaluateUnary(token numberString, token operation, double xValue)
{
    double number=numberString.number(xValue);
    double result{1};
    if(operation.value()=="-") return number*-1;
    if(operation.value()=="!!")
        for(int i{static_cast<int>(std::round(number))%2+2}; i<static_cast<int>(std::round(number))+1; i+=2)
        {
            int numberAsInt{static_cast<int>(std::round(number))};
            if(numberAsInt==0) return 1.0;
            if(numberAsInt<=3) return numberAsInt;
            result*=i;
        }
    else if(operation.value()=="!")
        for(int i{1}; i<static_cast<int>(std::round(number))+1; i++)
        {
            int numberAsInt{static_cast<int>(std::round(number))};
            if(numberAsInt==0) return 1.0;
            if(numberAsInt<=2) return numberAsInt;
            result*=i;
        }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

3+(15/root(2+4,10-2))-25x

3: Number                                               -> NUMBER
+: BinaryOp                                             -> OPERATOR
(15/root(2+4,10-2)): SubExpr                            -> SUBEXPR
    15: Number                                          -> NUMBER
    /: BinaryOp                                         -> OPERATOR
    root(2+4,10-2): Root                                -> SUBEXPR
        2: Number                                       -> NUMBER
        +: BinaryOp                                     -> OPERATOR
        4: Number                                       -> NUMBER
        ,: ArgSeparator?
        10: Number                                      -> NUMBER
        -: UnaryOp (Will later be treated as +-)        -> OPERATOR
        2: Numbers                                      -> NUMBER
-:UnaryOp (Will later be treated as +-)                 -> OPERATOR
25:Number                                               -> NUMBER
x:Variable (Will later be replaced by Number)           -> NUMBER

*/
/*
    Grammar:
    NUMBER||SUBEXPR then SUBEXPR||UNARYOP
    NUMBER||SUBEXPR then BINARYOP then NUMBER||SUBEXPR

    SUBEXPR then ANY (SUBEXPR, OPERATOR, NUMBER)
*/   