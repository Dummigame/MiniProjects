#include <cstdint>
#include <cstdlib>
#include <float.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

void displayHelp();
bool isValidInput(const char);

enum drawPos
{
    ZERO,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

enum pass
{
    SUBEXPRESSIONS,
    UNARYOPS,
    UNARYMINUS,
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
    ROOTARGRIGHT,
    ROOTARGLEFT,
    SUBEXPR,
    VARIABLE,
    INVALID
};

enum class tokenCategory_t
{
    NUMBER,
    SUBEXPR,
    ROOTARG,
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

    if(input.find("inf")!=std::string::npos) throw std::runtime_error("Encountered infinity");
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
struct point
{
    double x{};
    double y{};
    point(double inX, double inY)
    {
        this->x=inX;
        this->y=inY;
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct options
{
    bool graph{};
    double xMin{};
    double xMax{};
    double xStep{}; //Hey, reference
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class token
{

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
        //else if(isRoot(value)) return token_t::ROOT;
        else if(isRootArgRight(value)) return token_t::ROOTARGRIGHT;
        else if(isRootArgLeft(value)) return token_t::ROOTARGLEFT;
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
    bool isRootArgRight(std::string &input)
    {
        if(input.at(0)!=',') return false;
        input.erase(0,1);
        for(uint i{}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    bool isRootArgLeft(std::string &input)
    {
        if(input.find("root(") != 0) return false;
        
        for(uint i{5}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    static bool isSubexpr(std::string &input)
    {
        bool isTrue{};
        if(input.length()<3) return false;
        if((input.at(0)=='(' && input.at(input.length()-1)==')'))
        {
            isTrue=true;
            input.erase(0, 1);
            input.pop_back();
        }
        return isTrue;
    }
    ///////////////////////////////////////////////
    static tokenCategory_t determineTokenCategory(token_t &type)
    {
        if(type==token_t::NUMBER || type==token_t::VARIABLE) return tokenCategory_t::NUMBER;
        else if(type==token_t::SUBEXPR || type==token_t::ROOTARGLEFT || type==token_t::ROOTARGRIGHT) return tokenCategory_t::SUBEXPR;
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

        tokenCategory=determineTokenCategory(tokenType);
        if(tokenValue=="")tokenValue = value;
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
void getVariableArgs(std::vector<token>&, options&);
void graph(const std::vector<point>&points, const double yMin, const double yMax, int yClosestToZeroIndex, int xClosestToZeroIndex, double yClosestToZero, double xClosestToZero, const options &options);
double calculation(std::vector<token>, double xValue=NAN);
double evaluateRoot(token denominator, token enumerator, double xValue=NAN);
double evaluateUnary(token, token, double xValue=NAN);
double evaluateBinary(token, token, token, double xValue=NAN);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    options options;

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
        if(equation.at(0)=='?')
        {
            displayHelp();
            return 0;
        }
        if(equation.at(0)=='q'|| equation.at(0)=='Q')
        {
            std::cout<<"\nWhy have you done this..?\n";
            return 0;
        }
        for(int i{}; i<equation.length(); i++) if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--);
        if(equation!="")
        {
            std::cout<<"\nPassed " <<equation<< " as input from command line\n";
            passedInAsArg=true;
        }
    }
    if(equation.find('x')!=std::string::npos)
    {
        if(argc>3)
        {
            options.xMin=std::stod(argv[2]);
            options.xMax=std::stod(argv[3]);
            options.xStep=std::stod(argv[4]);
            
            if(options.xMin>=options.xMax) throw std::runtime_error("Invalid range!");
            if(options.xMax-options.xMin>options.xStep*1000) throw std::runtime_error("Too many calculations requested!");
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
        if(equation.at(0)=='q' || equation.at(0)=='Q') break;
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
            getVariableArgs(tokens, options);
        }
        if(options.xMin==options.xMax) 
        {
            result = calculation(tokens);
            std::ostringstream resultAsOSStream;
            resultAsOSStream<<result;
            if(resultAsOSStream.str().find("nan")!=std::string::npos)
            {
                resultAsOSStream.str("");
                resultAsOSStream.clear();
                resultAsOSStream<<"Not a Number";
            }
            if(resultAsOSStream.str()=="-0")
            {
                resultAsOSStream.str("");
                resultAsOSStream.clear();
                resultAsOSStream<<"0";               
            }

            for(unsigned long int i{}; i<resultAsOSStream.str().length()+2; i++) std::cout <<"=";
            std::cout << "\n " << resultAsOSStream.str() << '\n';
            for(unsigned long int i{}; i<resultAsOSStream.str().length()+2; i++) std::cout <<"=";
        }
        else if(!options.graph)
            for(double xValue=options.xMin; xValue<=options.xMax; xValue+=options.xStep)
            {
                result=calculation(tokens, xValue);
                std::ostringstream resultAsOSStream;
                resultAsOSStream<<result;
                if(resultAsOSStream.str().find("nan")!=std::string::npos)
                {
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
                    resultAsOSStream<<"Not a Number";
                }
                if(resultAsOSStream.str()=="-0")
                {
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
                    resultAsOSStream<<"0";               
                }
                std::cout<<"\nFor x = " << xValue << ": " << resultAsOSStream.str();
            }
        else
        {
            double largestY{-DBL_MAX};
            double smallestY{DBL_MAX};
            double yClosestToZero{DBL_MAX};
            double xClosestToZero{DBL_MAX};
            int yClosestToZeroIndex{INT32_MAX};
            int xClosestToZeroIndex{INT32_MAX};
            std::vector<point> points;
            uint i{};
            for(double xValue=options.xMin; xValue<=options.xMax; xValue+=options.xStep)
            {
                points.push_back(point(xValue,calculation(tokens,xValue)));

                if(std::abs(points.at(i).y)<yClosestToZero)
                {
                    yClosestToZero=std::abs(points.at(i).y);
                    yClosestToZeroIndex=i;
                }
                if(std::abs(points.at(i).x)<xClosestToZero) 
                {
                    xClosestToZero=std::abs(points.at(i).x);
                    xClosestToZeroIndex=i;
                }
                if(points.at(i).y<smallestY) smallestY=points.at(i).y;
                if(points.at(i).y>largestY) largestY=points.at(i).y;
                i++;
            }
            graph(points,smallestY,largestY,yClosestToZeroIndex,xClosestToZeroIndex,yClosestToZero,xClosestToZero,options);        
        }
        std::cout<<"\n";
        equation.clear();
        tokens.clear();
        options.graph=false;
        options.xMax=0;
        options.xMin=0;
        options.xStep=0;
        if(passedInAsArg) break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void graph(const std::vector<point>&points, const double yMin, const double yMax, int yClosestToZeroIndex, int xClosestToZeroIndex, double yClosestToZero, double xClosestToZero, const options &options)
{
    double length;
    double height;

    double xRange=points.size();
    double yRange=(std::abs(yMax)+std::abs(yMin))/options.xStep;

    uint zoomReduction{1};

    if(xRange>200 || yRange>100) throw std::runtime_error("Graph would suck very bad lmao");

    height=yRange;
    length=xRange-1;

    drawPos yAxisPos=ZERO;
    if(options.xMin>=0) yAxisPos=LEFT;
    else if(options.xMax<=0) yAxisPos=RIGHT;

    drawPos xAxisPos=ZERO;
    // if(yMin>=0) xAxisPos=BOTTOM;
    // else if(yMax<=0) xAxisPos=TOP;


    for(uint rows{}; rows<height; rows++)
    {
        for(uint i{}; i<length; i++)
        {
            
            //Plot point
            if(std::round((points.at(i).y)) == std::round(((height/2)-rows))) std::cout<<'+';

            //Draw Y axis
            else if(i==0 && rows==0 && yAxisPos==LEFT) std::cout<<'^';
            else if(i==length-1 && rows==0 && yAxisPos==RIGHT) std::cout<<'^';
            else if(i==xClosestToZeroIndex && rows==0 && yAxisPos==ZERO) std::cout<<'^';

            else if(i==0 && yAxisPos==LEFT) std::cout<<'|';
            else if(i==length-1 && yAxisPos==RIGHT) std::cout<<'|';
            else if(i==xClosestToZeroIndex && rows>0 && yAxisPos==ZERO) std::cout<<'|';

            //Draw X axis
            else if(
            (xAxisPos==BOTTOM && rows==height-1 && i<length-1)||
            (xAxisPos==TOP && rows==0 && i<length-1)||
            (std::round(yRange/2)==rows && xAxisPos==ZERO && i<length-1)) std::cout<<'-';
            
            else if(
            (xAxisPos==BOTTOM && rows==height-1 && i==length-1)||
            (xAxisPos==TOP && rows==0 && i==length-1)||
            (std::round(yRange/2)==rows && xAxisPos==ZERO && i==length-1)) std::cout<<"-  >"; //Man...

            else std::cout<<' ';
            std::cout<<"  ";

            if(i==length-1) std::cout<<'\n';
        }
    }


    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayHelp()
{
    std::cout<<"\nThis calculator allows you to write out an equation using numbers, +, -, *, /, ^ (or **), x, !, !! and the function root(denominator, enumerator)\nExample: 3+root(2,1+3) = 5\nroot() may be called with one argument, defaulting to square root. Example: root(4) is 2.\n\nInput from the command line is also accepted, though you may need to preface some characters with \\ to prevent your terminal from interpreting them.\nExample: \"root(5\\!\\!,10\\!\\!)\" -> \"root(5!!, 10!!)\"\nCommand line input values: equation lowestX highestX stepSizeX\n\n";
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isValidInput(const char c)
{
    return (c>='0'&&c<='9')||c=='.'||c=='x'||c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')'||c=='^'||c=='!'||c=='r'||c=='o'||c=='t'||c==','||c=='e';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<token> getTokens(const std::string &input)
{
    uint nestingLevel{};
    uint nestingOfFunction{};
    uint startOfFunction{};
    uint endOfFirstArg{};
    std::vector<token> tokens{};
    std::string currentToken{};
    bool isFunctionCall{};
    bool fixOffByOne{};
    bool inFunctionCall{};
    bool rootHasTwoArgs{};

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
            if(input.at(i)=='r') isFunctionCall=true;
            else isFunctionCall=false;
            for(; i<input.length() && !isFunctionCall; i++)
            {
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                currentToken.push_back(input.at(i));
                if(nestingLevel==0) break;
            }
            for(; i<input.length() && isFunctionCall; i++)
            {
                if(input.at(i)=='r' && !inFunctionCall)
                {
                    if(input.find("root(", i)==i)
                    {
                        startOfFunction=i;
                        i+=5;
                        nestingLevel++;
                        currentToken.append("root(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else throw std::runtime_error("Bad function name or stray characters!");
                }
                if(inFunctionCall && nestingLevel==nestingOfFunction && input.at(i)==',' && rootHasTwoArgs==false) //std::cout<<input.substr(startOfFunction,i-startOfFunction+1);
                {
                    rootHasTwoArgs=true;
                    endOfFirstArg=i;
                    tokens.push_back(input.substr(startOfFunction,i-startOfFunction));
                }
                else if(inFunctionCall && nestingLevel<=nestingOfFunction && input.at(i)==')' && rootHasTwoArgs==false)
                {
                    tokens.push_back(','+input.substr(startOfFunction+5/*char after root(<-*/,i-startOfFunction-5));
                    break;
                }
                else if(inFunctionCall && nestingLevel<=nestingOfFunction && input.at(i)==')' && rootHasTwoArgs==true)
                {
                    tokens.push_back(input.substr(endOfFirstArg,i-endOfFirstArg));
                    break;
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
            }
        }
        else for(fixOffByOne=true; i<input.length() && ((input.at(i)>='0' && input.at(i)<='9') || input.at(i)=='.' || input.at(i)=='e'); i++)
        {
            if(i+1<input.length() && input.at(i)=='e' && (input.at(i+1)=='+' || input.at(i+1)=='-'))
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
        if(currentToken!="root(") tokens.push_back({currentToken});
        currentToken.clear();
    }

    return tokens;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getVariableArgs(std::vector<token> &tokens, options &options)
{
    bool obtainedArgs{};
    for(uint i{}; i<tokens.size(); i++)
    {
        if(tokens.at(i).value().find('x')!=std::string::npos) break;
        else if(i==tokens.size()-1) return;
    }
    std::string input;

    std::cout << "\nSpecify variable minimum: ";
    std::cin>>input;
    options.xMin=std::stod(input);

    std::cout << "\nSpecify variable maximum: ";
    std::cin>>input;
    options.xMax=std::stod(input);

    std::cout << "\nSpecify variable increment/step: ";
    std::cin>>input;
    options.xStep=std::stod(input);

    std::cout << "\nGraph? y/n: ";
    std::cin>>input;
    if(input.at(0)=='y' || input.at(0)=='Y') options.graph=true;
    if(options.xMin>=options.xMax) throw std::runtime_error("Invalid range!");
    if(options.xMax-options.xMin>options.xStep*1000) throw std::runtime_error("Too many calculations requested!");
    std::cin.ignore();
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double calculation(std::vector<token> tokens, double xValue)
{
    double result{};

    for(uint i{1}; i<tokens.size(); i++)
    {
        if(tokens.at(i).type()==token_t::VARIABLE && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
            tokens.insert(tokens.begin()+i++, token("*"));
        if(tokens.at(i).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i-1).typeCategory()!=tokenCategory_t::OPERATOR&&tokens.at(i-1).type()!=token_t::ROOTARGLEFT)
            tokens.insert(tokens.begin()+i++, token("*"));
        if(tokens.at(i).value()=="-" && tokens.at(i-1).typeCategory()!=tokenCategory_t::OPERATOR)
            tokens.insert(tokens.begin()+i++, token("+"));
        if(tokens.at(i-1).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i).typeCategory()!=tokenCategory_t::OPERATOR&&tokens.at(i).type()!=token_t::ROOTARGRIGHT)
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
                else if(tokens.at(i).type()==token_t::ROOTARGRIGHT)
                {
                    double evaluatedRoot;
                    if(i==0) evaluatedRoot=evaluateRoot(token("0"),tokens.at(i), xValue);
                    else evaluatedRoot=evaluateRoot(tokens.at(i-1),tokens.at(i), xValue);
                    std::ostringstream evaluatedRootAsOSStream;
                    evaluatedRootAsOSStream << evaluatedRoot;
                    tokens.at(i) = token(evaluatedRootAsOSStream.str());
                    if(i>0 && tokens.at(i-1).type()==token_t::ROOTARGLEFT) tokens.erase(tokens.begin()+i-1);
                }
                else continue;
            }
            else if(pass==UNARYOPS)
            {
                if(i==0) continue;
                if((tokens.at(i).type()==token_t::UNARYOP || tokens.at(i).type()==token_t::MULTICHARUNARY) && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
                {
                    double evaluatedUnary=evaluateUnary(tokens.at(i-1), tokens.at(i), xValue);
                    std::ostringstream evaluatedResultAsOSStream;
                    evaluatedResultAsOSStream << evaluatedUnary;
                    tokens.at(i-1)=token(evaluatedResultAsOSStream.str());
                    tokens.erase(tokens.begin()+i);
                    i--;
                }
            }
            else if (pass==UNARYMINUS)
            {
                if(i!=0&&tokens.at(i-1).value()=="-" && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    double evaluatedUnary=evaluateUnary(tokens.at(i), tokens.at(i-1), xValue);
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
    
    if(tokens.size()==1 && tokens.at(0).type()==token_t::NUMBER) result=std::stod(tokens.at(0).value());
    else throw std::runtime_error("Malformed expression!");
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double evaluateRoot(token denominatorArg, token enumeratorArg, double xValue)
{
    double denominator{};

    std::vector<token> tokenToEval{denominatorArg};
    if(denominatorArg.type()!=token_t::ROOTARGLEFT) denominator=2;
    else denominator=calculation(getTokens(denominatorArg.value()), xValue);

    tokenToEval.at(0)=enumeratorArg;
    double enumerator=calculation(getTokens(enumeratorArg.value()), xValue);

    if(denominator==0) throw std::runtime_error("0th root is undefined");
    if(denominator==static_cast<int>(denominator) && static_cast<int>(denominator)%2==0 && enumerator<0) throw std::runtime_error("Result not a real number");

    return std::pow(enumerator, 1/denominator);
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
    if(number<0) throw std::runtime_error("Cannot evaluate factorial of negative number!");
    if(operation.value()=="!!")
        for(int i{static_cast<int>(std::round(number))%2+2}; i<static_cast<int>(std::round(number))+1; i+=2)
        {
            if(number>=300.5) throw std::runtime_error("Input for double factorial too large!");
            uint numberAsInt{static_cast<uint>(std::round(number))};
            if(numberAsInt>300) throw std::runtime_error("Input for double factorial too large!");
            if(numberAsInt==0) return 1.0;
            if(numberAsInt<=3) return numberAsInt;
            result*=i;
        }
    else if(operation.value()=="!")
        for(int i{2}; i<static_cast<int>(std::round(number))+1; i++)
        {
            if(number>=170.5) throw std::runtime_error("Input for factorial too large!");
            uint numberAsInt{static_cast<uint>(std::round(number))};
            if(numberAsInt>170) throw std::runtime_error("Input for factorial too large!");
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
    root(2+4,10-2) 
        2+4: RootArgLeft                                -> SUBEXPR
            2: Number                                   -> NUMBER
            +: BinaryOp                                 -> OPERATOR
            4: Number                                   -> NUMBER
        10-2: RootArgRight                              -> SUBEXPR
            10: Number                                  -> NUMBER
            -: UnaryMinus                               -> OPERATOR
            2: Number                                   -> NUMBER
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