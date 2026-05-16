#include <cfloat>
#include <cstdint>
#include <cstdlib>
#include <cfloat>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

void displayHelp();
bool isValidInput(const char);

enum class functions
{
    NONE,
    ABS,
    ABSASFUN,
    ASIN,
    ACOS,
    ATAN,
    ROOT,
    SIN,
    COS,
    TAN,
    SEC,
    CSC,
    COT,
    FLOOR,
    CEIL,
    ROUND,
    LN,
    LOG
};

enum drawPos
{
    ZERO,
    LEFT,
    RIGHT,
};

enum pass
{
    SUBEXPRESSIONS,
    UNARYOPS,
    EXPONENTIATION,
    UNARYMINUS,
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
    ABSARG,
    SINARG,
    COSARG,
    TANARG,
    SECARG,
    CSCARG,
    COTARG,
    ASINARG,
    ACOSARG,
    ATANARG,
    FLOORARG,
    CEILARG,
    ROUNDARG,
    LNARG,
    LOGARGRIGHT,
    LOGARGLEFT,
    SUBEXPR,
    VARIABLE,
    CONSTANT,
    INVALID
};

enum class tokenCategory_t
{
    NUMBER,
    SUBEXPR,
    OPERATOR
};

bool isNumberPart(char input);

bool isNumber(const std::string &input);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct point
{
    long double x{};
    long double y{};
    point(long double inX, long double inY)
    {
        this->x=inX;
        if(inY==INFINITY || inY==-INFINITY) this->y=NAN;
        else this->y=inY;
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct options
{
    bool graph{};   //Whether to draw graph or not
    long double xMin{};
    long double xMax{};  
    long double xStep{}; //Hey, reference
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
        if(isConstant(value)) return token_t::CONSTANT;
        else if(isRootArgRight(value)) return token_t::ROOTARGRIGHT;
        else if(isRootArgLeft(value)) return token_t::ROOTARGLEFT;
        else if(isLogArgRight(value)) return token_t::LOGARGRIGHT;
        else if(isLogArgLeft(value)) return token_t::LOGARGLEFT;
        else if(isSubexpr(value)) return token_t::SUBEXPR;
        else if(isAbsArg(value)) return token_t::ABSARG;
        else if(isLnArg(value)) return token_t::LNARG;
        else if(isSinArg(value)) return token_t::SINARG;
        else if(isAsinArg(value)) return token_t::ASINARG;
        else if(isCosArg(value)) return token_t::COSARG;
        else if(isAcosArg(value)) return token_t::ACOSARG;
        else if(isTanArg(value)) return token_t::TANARG;
        else if(isAtanArg(value)) return token_t::ATANARG;
        else if(isSecArg(value)) return token_t::SECARG;
        else if(isCscArg(value)) return token_t::CSCARG;
        else if(isCotArg(value)) return token_t::COTARG;
        else if(isFloorArg(value)) return token_t::FLOORARG;
        else if(isCeilArg(value)) return token_t::CEILARG;
        else if(isRoundArg(value)) return token_t::ROUNDARG;
        else if(value=="x") return token_t::VARIABLE;
        return token_t::INVALID;
    }
    ///////////////////////////////////////////////
    static bool isConstant(const std::string &input)
    {
        if(input=="pi") return true;
        if(input=="e") return true;
        if(input=="tau") return true;
        if(input=="phi") return true;
        if(input=="eul") return true;
        if(input=="rad") return true;
        if(input=="deg") return true;
        if(input=="i") return true;
        else return false;
    }
    static bool isBinaryOp(const char c)
    {
        return c=='+' || c=='*' || c=='/' || c=='^';
    }

    static bool isUnaryOp(const char c)
    {
        return c=='!'|| c=='-';
    }
    ///////////////////////////////////////////////
    bool isLnArg(std::string &input)
    {
        if(input.find("ln(")!=0) return false;
        
        for(uint i{3}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }    
    ///////////////////////////////////////////////
    bool isSinArg(std::string &input)
    {
        if(input.find("sin(")!=0) return false;
        
        for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }    
    ///////////////////////////////////////////////
    bool isAsinArg(std::string &input)
    {
        if(input.find("asin(")!=0) return false;
        
        for(uint i{5}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }
    ///////////////////////////////////////////////
    bool isAcosArg(std::string &input)
    {
        if(input.find("acos(")!=0) return false;
        
        for(uint i{5}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }
    ///////////////////////////////////////////////
    bool isAtanArg(std::string &input)
    {
        if(input.find("atan(")!=0) return false;
        
        for(uint i{5}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }
    ///////////////////////////////////////////////
    bool isAbsArg(std::string &input)
    {
        if((input.at(0)!='|' || input.at(input.length()-1)!='|')&&input.find("abs(")!=0) return false;
        
        if(input.at(0)=='|') for(uint i{1}; i<input.length()-1; i++) tokenValue.push_back(input.at(i));
        else for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }    
    ///////////////////////////////////////////////
    bool isRoundArg(std::string &input)
    {
        if(input.find("round(")!=0) return false;
        
        for(uint i{6}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }  
    ///////////////////////////////////////////////
    bool isSecArg(std::string &input)
    {
        if(input.find("sec(")!=0) return false;
        
        for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }    
    ///////////////////////////////////////////////
    bool isCscArg(std::string &input)
    {
        if(input.find("csc(")!=0) return false;
        
        for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }    
    ///////////////////////////////////////////////
    bool isCotArg(std::string &input)
    {
        if(input.find("cot(")!=0) return false;
        
        for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }  
    ///////////////////////////////////////////////
    bool isFloorArg(std::string &input)
    {
        if(input.find("floor(")!=0) return false;
        
        for(uint i{6}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }   
    ///////////////////////////////////////////////
    bool isCeilArg(std::string &input)
    {
        if(input.find("ceil(")!=0) return false;
        
        for(uint i{5}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }   
    ///////////////////////////////////////////////
    bool isTanArg(std::string &input)
    {
        if(input.find("tan(")!=0) return false;
        
        for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }    
    ///////////////////////////////////////////////
    bool isCosArg(std::string &input)
    {
        if(input.find("cos(")!=0) return false;
        
        for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }
    ///////////////////////////////////////////////
    bool isRootArgRight(std::string &input)
    {
        if(input.find("root,")!=0) return false;
        
        for(uint i{5}; i<input.length(); i++)
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
    bool isLogArgRight(std::string &input)
    {
        if(input.find("log,")!=0) return false;
        
        for(uint i{4}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    bool isLogArgLeft(std::string &input)
    {
        if(input.find("log(") != 0) return false;
        
        for(uint i{4}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    static bool isSubexpr(std::string &input)
    {
        bool isTrue{};
        if(input.length()<2) return false;
        if(input.find(')')!=std::string::npos && input.length()<3) return false;
        if(input.at(0)=='(' /*&& input.at(input.length()-1)==')'*/)
        {
            isTrue=true;
            input.erase(0, 1);
            //input.pop_back();
        }
        return isTrue;
    }
    ///////////////////////////////////////////////
    static std::string replaceConstants(std::string &input)
    {
        if(input=="e") return "2.7182818284590452354";
        if(input=="pi") return "3.14159265358979323846";
        if(input=="tau") return "6.28318530717958647692";
        if(input=="phi") return "1.61803398874989484820";
        if(input=="eul") return "0.57721566490153286060";
        if(input=="rad") return "57.2957795130823209";
        if(input=="deg") return "0.0174532925199432958";
        if(input=="i") return "nan";
        else return input;
    }
    ///////////////////////////////////////////////
    static tokenCategory_t determineTokenCategory(token_t &type)
    {
        if(type==token_t::NUMBER || type==token_t::VARIABLE || type==token_t::CONSTANT) return tokenCategory_t::NUMBER;
        else if(type==token_t::SUBEXPR || type==token_t::SINARG ||type==token_t::COSARG || type==token_t::TANARG ||
                type==token_t::ROOTARGLEFT || type==token_t::ROOTARGRIGHT ||type==token_t::SECARG ||type==token_t::CSCARG||
                type==token_t::COTARG || type==token_t::FLOORARG || type==token_t::CEILARG || type==token_t::ROUNDARG || type==token_t::ABSARG||
                type==token_t::ASINARG || type==token_t::ACOSARG || type==token_t::ATANARG || type==token_t::LNARG ||
                type==token_t::LOGARGLEFT || type==token_t::LOGARGRIGHT) return tokenCategory_t::SUBEXPR;
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
        else if(tokenType==token_t::CONSTANT)
        {
            this->tokenValue=replaceConstants(value);
        }

        tokenCategory=determineTokenCategory(tokenType);
        if(tokenValue=="")tokenValue = value;
    }
    ///////////////////////////////////////////////
    long double number(long double xValue=NAN)
    {
        if(xValue!=NAN && this->tokenType==token_t::VARIABLE)
        {
            std::ostringstream asString;
            asString.precision(LDBL_DIG);
            asString << xValue;
            this->tokenValue=asString.str();
            this->tokenType=token_t::NUMBER;
        }

        if (tokenType != token_t::NUMBER && tokenType != token_t::CONSTANT) throw std::runtime_error("Tried to get number of token which is not a number");
        long double valueAsDouble{};
        valueAsDouble = std::stold(tokenValue);
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
token getFunctionToken(const std::string& input, const functions functionCallType, uint &i);
void getVariableArgs(std::vector<token>&, options&);
void graph(const std::vector<point>&points, const long double yMin, const long double yMax, const uint xClosestToZeroIndex, const options &options);
long double calculation(std::vector<token>, const long double xValue);

long double evaluateRoot(token denominator, token &enumerator, const long double xValue);
long double evaluateLog(token denominatorArg, token &enumeratorArg, const long double xValue);

long double evaluateUnary(token&, token&, const long double xValue);
long double evaluateBinary(token&, token&, token&, const long double xValue);
long double evaluateLn(token &arg, const long double xValue);
long double evaluateSin(token &arg, const long double xValue); //Evaluate your sins. (sine)
long double evaluateCos(token &arg, const long double xValue);
long double evaluateTan(token &arg, const long double xValue);
long double evaluateSec(token &arg, const long double xValue);
long double evaluateCsc(token &arg, const long double xValue);
long double evaluateCot(token &arg, const long double xValue);
long double evaluateFloor(token &arg, const long double xValue);
long double evaluateCeil(token &arg, const long double xValue);
long double evaluateRound(token &arg, const long double xValue);
long double evaluateAbs(token &arg, const long double xValue);
long double evaluateAsin(token &arg, const long double xValue);
long double evaluateAcos(token &arg, const long double xValue);
long double evaluateAtan(token &arg, const long double xValue);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    options options;
    std::ostringstream resultAsOSStream;
    resultAsOSStream.precision(LDBL_DIG);
    std::cout.precision(LDBL_DIG);

    bool passedInAsArg{};
    std::string equation{};
    if(argc>1)
    {
        equation+=argv[1];
        if(equation.length()>2 && equation.at(0)=='-' && equation.at(1)=='-' && (equation.at(2)=='h' || equation.at(2)=='H'))
        {
            displayHelp();
            return 0;
        }
        else if(equation.length()>1 && equation.at(0)=='-' && (equation.at(1)=='h' || equation.at(1)=='H')) 
        {
            displayHelp();
            return 0;
        }
        else if(equation.at(0)=='?' || equation.at(0)=='h' || equation.at(0)=='H')
        {
            displayHelp();
            return 0;
        }
        //This code is not very dry.
        if(equation.at(0)=='q'|| equation.at(0)=='Q')
        {
            std::cout<<"\nWhy have you done this..?\n";
            return 0;
        }
        if(equation.find("fish")!=std::string::npos) //Fish.
        {                                   
            std::cout<<"\nfish.\n";         
            return 0;                       
        }  
        if(equation.find("nine plus ten")!=std::string::npos)
        {                                   
            std::cout<<"\ntwenty one.\n";         
            return 0;                       
        }  
        for(uint i{}; i<equation.length(); i++) if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--);
        if(equation!="")
        {
            std::cout<<"\nPassed " <<equation<< " as input from command line\n";
            passedInAsArg=true;
        }
    }
    if(equation.find('x')!=std::string::npos)
    {
        if(argc>4)
        {
            
            if(isNumber(argv[2])) options.xMin=std::stold(argv[2]);
            else throw std::runtime_error("You did not enter a number");
            if(isNumber(argv[3])) options.xMax=std::stold(argv[3]);
            else throw std::runtime_error("You did not enter a number");
            if(argv[4][0]=='y' || argv[4][0]=='Y' || argv[4][0]=='g' || argv[4][0]=='G')
            {
                options.graph=true;
                if(argv[4][0]=='g' || argv[4][0]=='G') options.xStep=0.2;
                else options.xStep=0.05;
            }
            else if(isNumber(argv[4])) options.xStep=std::stold(argv[4]);
            else throw std::runtime_error("You did not enter a number");
            
            if(options.xMin>=options.xMax) throw std::runtime_error("Invalid range!");
            if(options.xMax-options.xMin>options.xStep*1000) throw std::runtime_error("Too many calculations requested!");
        }
        else throw std::runtime_error("Included variable but did not specify all of the following: min, max, step/graphing(g or y (close zoom))");
    }

    while(true)
    {
        std::cout.precision(LDBL_DIG);
        resultAsOSStream.precision(LDBL_DIG);
        if(equation!="") goto passedInAsArg;
        std::cout << "Type your equation (? for help, q to quit):\n=> ";
        std::getline(std::cin, equation);

        if(equation.length()==0) throw std::runtime_error("Empty input");
        if(equation.at(0)=='q' || equation.at(0)=='Q') break;
        if(equation.at(0)=='?' || equation.at(0)=='h' || equation.at(0)=='H')
        {
            displayHelp();
            equation.clear();
            continue;
        }                                 
        passedInAsArg:

        for(uint i{}; i<equation.length(); i++) if(equation.at(i)>='A' && equation.at(i)<='Z') equation.at(i)=equation.at(i)+32;//'X' -> 'x' ToLower
        if(equation.find("fish")!=std::string::npos) //Fish.
        {                                   
            std::cout<<"\nfish.\n";         
            return 0;                       
        }  
        if(equation.find("nine plus ten")!=std::string::npos)
        {                                   
            std::cout<<"\ntwenty one.\n";         
            return 0;                       
        }  
        for(uint i{}; i<equation.length(); i++) if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--); //Basic garbage removal

        int parenthesesImbalance{};
        uint absValueLineCount{};
        for(uint i{}; i<equation.length(); i++)
        {
            if(equation.at(i)=='|') absValueLineCount++;
            if(equation.at(i)=='(') parenthesesImbalance++;
            else if(equation.at(i)==')') parenthesesImbalance--;
            if(/*(equation.length()==i+1 && parenthesesImbalance!=0) ||*/ parenthesesImbalance<0 || (equation.length()==i+1 && absValueLineCount%2!=0))
            {
                std::cerr<<"\nParentheses are not balanced!\n\n";
                equation.clear();
            }
        }

        if(absValueLineCount%2!=0) continue;
        
        if(equation.length()==0) throw std::runtime_error("No valid input");
        std::vector<token> tokens = getTokens(equation);
        if(!passedInAsArg)
        {
            getVariableArgs(tokens, options);
        }
        if(options.xMin==options.xMax) //No x found
        {
            resultAsOSStream<< calculation(tokens, NAN);

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
            resultAsOSStream.str("");
            resultAsOSStream.clear();
        }
        else if(!options.graph)
        {
            std::cout.precision(FLT_DIG);
            resultAsOSStream.precision(FLT_DIG);
            for(long double xValue=options.xMin; xValue<=options.xMax; xValue+=options.xStep)
            {
                if(xValue>(-0.0000002) && xValue<0.0000002) xValue=0;
                resultAsOSStream<<calculation(tokens, xValue);
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
                resultAsOSStream.str("");
                resultAsOSStream.clear();
            }
        }
        else
        {
            long double largestY{-DBL_MAX};
            long double smallestY{DBL_MAX};
            long double yClosestToZero{DBL_MAX};
            long double xClosestToZero{DBL_MAX};
            uint xClosestToZeroIndex{INT32_MAX};
            std::vector<point> points;
            uint i{};
            for(long double xValue=options.xMin; xValue<=options.xMax; xValue+=options.xStep)
            {
                if(xValue>(-0.0000002) && xValue<0.0000002) xValue=0;
                points.push_back(point(xValue,calculation(tokens,xValue)));

                if(std::abs(points.at(i).y)<yClosestToZero)
                {
                    yClosestToZero=std::abs(points.at(i).y);
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
            graph(points,smallestY,largestY,xClosestToZeroIndex,options);        
        }
        std::cout<<"\n\n";
        equation.clear();
        tokens.clear();
        options.graph=false;
        options.xMax=0;
        options.xMin=0;
        options.xStep=0;
        resultAsOSStream.str("");
        resultAsOSStream.clear();
        if(passedInAsArg) break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This function is ugly.
void graph(const std::vector<point>&points, const long double yMin, const long double yMax, const uint xClosestToZeroIndex, const options &options)
{
    long double xRange=points.size();
    long double yRange=(std::abs(yMax)+std::abs(yMin))/options.xStep+std::abs(yMin*5); //Absurd line


    long double height=yRange+(1/(yRange+0.5))*700; //Trust
    if(height>yRange*3) height=yRange+15;
    const long double length=xRange;

    if(height>600 || length >200)
    {
        std::cerr<<"\nThe graph would be too large.\n";
        return;
    }

    drawPos yAxisPos=ZERO;
    if(options.xMin>=0) yAxisPos=LEFT;
    else if(options.xMax<=0) yAxisPos=RIGHT;


    std::vector<std::string> graph;
    std::ostringstream graphLine;
    for(uint rows{}; rows<height; rows++)
    {
        if(rows>height/2+1 && yMin>=(height/2+1-rows)*options.xStep) break; //End if bottom of graph reached
        for(uint i{}; i<length; i++)
        {
            if(points.at(i).y==INFINITY || points.at(i).y==-INFINITY) throw std::runtime_error("Encountered infinity!");
            
            //Plot point
            else if(std::round((points.at(i).y)/options.xStep) == std::round(height/2-rows)) graphLine<<'+';
           
            //Draw X axis
            else if((std::round(height/2)==rows && i<length-1)) graphLine<<'-';

            else if((std::round(height/2)==rows && i==length-1)) graphLine<<"-  >";

            //Draw Y axis
            else if(i==0 && rows==0 && yAxisPos==LEFT) graphLine<<'^';
            else if(i==length-1 && rows==0 && yAxisPos==RIGHT) graphLine<<'^';
            else if(i==xClosestToZeroIndex && rows==0 && yAxisPos==ZERO) graphLine<<'^';

            else if(i==0 && yAxisPos==LEFT) graphLine<<'|';
            else if(i==length-1 && yAxisPos==RIGHT) graphLine<<'|';
            else if(i==xClosestToZeroIndex && rows>0 && yAxisPos==ZERO) graphLine<<'|';

            else graphLine<<' ';
            graphLine<<"  ";

            if(i==length-1)
            {
                graphLine<<'\n';
                graph.emplace_back(graphLine.str());
                graphLine.str("");
            }
        }
    }
    for(uint i{}; i<graph.size(); i++) std::cout<<graph.at(i);
    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayHelp()
{
    std::cout<<"\nThis calculator allows you to write out an equation using numbers, +, -, *, /, ^ (or **), x, !, !!, |expression|, (expression) and the following functions:\n"<<
    "\troot(denominator, enumerator), log(base,value)\n"<<
    "\tsin(expression), cos(expression), tan(expression), sec(expression), csc(expression), cot(expression), asin(expression), acos(expression), atan(expression)\n"<<
    "\tfloor(expression), ceil(expression), round(expression), abs(expression), ln(expression)\n"<<
    "\nExample: 3+root(2,1+3) = 5\nroot() may be called with one argument, defaulting to square root. Example: root(4) is 2."<<
    "\nYou may also have an equation graphed if you include at least one instance of x."<<
    "\nThere are also a few constants available: pi, e, phi, eul(euler's number), tau(2*pi), rad(180/pi) and deg(pi/180, useful for sin() and stuff)\n"<<
    "\nInput from the command line is also accepted, though you may need to preface some characters with \\ to prevent your terminal from interpreting them."<<
    "\nExample: \"root(5\\!\\!,10\\!\\!)\" -> \"root(5!!, 10!!)\"\nCommand line input values: equation lowestX highestX stepSizeX or graphing (g/y, y for high zoom)\n\n";
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isValidInput(const char c)
{
    return (c>='0'&&c<='9')||c=='.'||c=='x'||c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')'||c=='^'||c=='!'||c=='r'||c=='o'||c=='t'
            ||c==','||c=='e'||c=='s'||c=='i'||c=='n'||c=='c'||c=='a' ||c=='l'||c=='f'||c=='u'||c=='d'||c=='|'||c=='b'||c=='g'||c=='p'
            ||c=='u'||c=='h';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<token> getTokens(const std::string &input)
{
    int nestingLevel{};
    static bool memed{};
    int absNestingLevel{};
    int nestingOfFunction{};
    uint startOfFunction{};
    uint endOfFirstArg{};
    std::vector<token> tokens{};
    std::string currentToken{};
    functions functionCallType{};
    bool fixOffByOne{};
    bool inFunctionCall{};
    bool rootHasTwoArgs{};
    bool logHasTwoArgs{};
    int inParentheses{};

    for(uint i{}; i<input.length(); i++)
    {
        if(input.at(i)=='+') currentToken='+';
        else if (input.at(i)=='-') currentToken='-';
        else if (input.at(i)=='^') currentToken='^';
        else if (input.at(i)=='/') currentToken='/';

        else if (input.find("pi",i)==i) {currentToken="pi"; i++;}
        else if (input.find("rad",i)==i) {currentToken="rad"; i+=2;}
        else if (input.find("deg",i)==i) {currentToken="deg"; i+=2;}
        else if (input.find("tau",i)==i) {currentToken="tau"; i+=2;}
        else if(input.find("phi",i)==i) {currentToken="phi"; i+=2;}
        else if(input.find("eul", i)==i) {currentToken="eul"; i+=2;}
        else if (input.at(i)=='e') currentToken='e';
        else if (input.at(i)=='i') 
        {
            currentToken='i'; //:troll:
            if(!memed) std::cout<<"\nWhat, did you think this calculator can use complex numbers?\n";
            memed=true;
        }
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
        else if(input.at(i)=='r'|| 
                input.at(i)=='s' || 
                input.at(i)=='t' || 
                input.at(i)=='c' || 
                input.at(i)=='|' ||  
                input.at(i)=='f' || 
                input.at(i)=='a' || 
                input.at(i)=='(' ||
                input.at(i)=='l')
        {
            currentToken.clear();
            if(input.at(i)=='r') functionCallType=functions::ROUND;
            else if(input.at(i)=='s')functionCallType=functions::SIN;
            else if(input.at(i)=='c')functionCallType=functions::COS;
            else if(input.at(i)=='t')functionCallType=functions::TAN;
            else if(input.at(i)=='f')functionCallType=functions::FLOOR;
            else if(input.at(i)=='|')functionCallType=functions::ABS;
            else if(input.at(i)=='a')functionCallType=functions::ABSASFUN;
            else if(input.at(i)=='l') functionCallType=functions::LN;
            else functionCallType=functions::NONE;

            
            //Parse |x|... or ||x|| if the user hates me... or ||||x||||. whatever.
            for(startOfFunction=i; i<input.length() && functionCallType==functions::ABS; i++)
            {
                if(!inFunctionCall)
                {
                    startOfFunction=i;
                    for(;input.at(i)=='|' && i<input.length()-1;i++)
                    {
                        absNestingLevel++;
                        currentToken.push_back('|');
                    }
                    inFunctionCall=true;
                    nestingOfFunction=nestingLevel;
                    if(i==input.length()-1) throw std::runtime_error("Bad absolute value... parentheses? Things? Lines?");
                }
                if(input.at(i)==')')
                {
                    inParentheses--;
                    nestingLevel--;
                }
                else if(input.at(i)=='(')
                {
                    inParentheses++;
                    nestingLevel++;
                }
                if(i<input.length() && inParentheses==false && input.at(i)=='|') absNestingLevel--;
                if(i>startOfFunction+1 && nestingLevel<=0 && absNestingLevel==0 && inParentheses==false && input.at(i)=='|')
                {
                    currentToken.push_back(input.at(i));
                    tokens.emplace_back(currentToken);
                    currentToken.clear();
                    break;
                }
                else if(i==input.length()-1 && input.at(i)!='|') throw std::runtime_error("Bad absolute value... parentheses? Things? Lines?");
                else if (i==input.length()-1 && input.at(i)=='|')
                {
                    i--;
                    continue;
                }
                currentToken.push_back(input.at(i));
            }

            if(functionCallType==functions::ABSASFUN)
            {
                if(input.find("abs(", i)!=i) functionCallType=functions::ASIN;
            }
            if(functionCallType==functions::ASIN)
            {
                if(input.find("asin(", i)!=i) functionCallType=functions::ACOS;
            }
            if(functionCallType==functions::ACOS)
            {
                if(input.find("acos(", i)!=i) functionCallType=functions::ATAN;
            }
            if(functionCallType==functions::ATAN)
            {
                if(input.find("atan(", i)!=i) std::runtime_error("Bad function name or stray characters!");
            }
            if(functionCallType==functions::FLOOR)
            {
                if(input.find("floor(", i)!=i) throw std::runtime_error("Bad function name or stray characters!");
            }
            if(functionCallType==functions::SIN) //Parses your sins
            {
                if(input.find("sin(", i)!=i) functionCallType=functions::SEC;
            }
            if(functionCallType==functions::SEC)
            {
                if(input.find("sec(", i)!=i) throw std::runtime_error("Bad function name or stray chararacters!");
            }
            if(functionCallType==functions::COS)
            {
                if(input.find("cos(", i)!=i) functionCallType=functions::CSC;
            }
            if(functionCallType==functions::CSC)
            {
                if(input.find("csc(", i)!=i) functionCallType=functions::COT;
            }
            if(functionCallType==functions::COT)
            {
                if(input.find("cot(", i)!=i) functionCallType=functions::CEIL;
            }
            if(functionCallType==functions::CEIL)
            {
                if(input.find("ceil(", i)!=i) throw std::runtime_error("Bad function name or stray characters!");
            }
            if(functionCallType==functions::TAN)
            {
                if(input.find("tan(", i)!=i) throw std::runtime_error("Bad function name or stray characters!");
            }
            if(functionCallType==functions::ROUND)
            {
                if(input.find("round(", i)!=i) functionCallType=functions::ROOT;
            }
            if(functionCallType==functions::LN)
            {
                if(input.find("ln(", i)!=i) functionCallType=functions::LOG;
            }
           
            if(functionCallType!=functions::ABS && functionCallType!=functions::ROOT && functionCallType!=functions::LOG && functionCallType!=functions::NONE) tokens.emplace_back(getFunctionToken(input, functionCallType, i));

            //Parse root()
            for(; i<input.length() && functionCallType==functions::ROOT; i++)
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
                    tokens.emplace_back(input.substr(startOfFunction,i-startOfFunction));
                }
                else if(inFunctionCall && ((nestingLevel<=nestingOfFunction && input.at(i)==')')||i==input.length()-1) && rootHasTwoArgs==false)
                {
                    tokens.emplace_back("root,"+input.substr(startOfFunction+5/*char after root(<-*/,i-startOfFunction-4));
                    break;
                }
                else if(inFunctionCall && ((nestingLevel<=nestingOfFunction && input.at(i)==')')||i==input.length()-1) && rootHasTwoArgs==true)
                {
                    tokens.emplace_back("root"+input.substr(endOfFirstArg,i-endOfFirstArg+1));
                    break;
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
            }

            //Parse log()
            for(; i<input.length() && functionCallType==functions::LOG; i++)
            {
                if(input.at(i)=='l' && !inFunctionCall)
                {
                    if(input.find("log(", i)==i)
                    {
                        startOfFunction=i;
                        i+=4;
                        nestingLevel++;
                        currentToken.append("log(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else throw std::runtime_error("Bad function name or stray characters!");
                }
                if(inFunctionCall && nestingLevel==nestingOfFunction && input.at(i)==',' && logHasTwoArgs==false) //std::cout<<input.substr(startOfFunction,i-startOfFunction+1);
                {
                    logHasTwoArgs=true;
                    endOfFirstArg=i;
                    tokens.emplace_back(input.substr(startOfFunction,i-startOfFunction));
                }
                else if(inFunctionCall && ((nestingLevel<=nestingOfFunction && input.at(i)==')')||i==input.length()-1) && logHasTwoArgs==false)
                {
                    tokens.emplace_back("log,"+input.substr(startOfFunction+4/*char after log(<-*/,i-startOfFunction-3));
                    break;
                }
                else if(inFunctionCall && ((nestingLevel<=nestingOfFunction && input.at(i)==')')||i==input.length()-1) && logHasTwoArgs==true)
                {
                    tokens.emplace_back("log"+input.substr(endOfFirstArg,i-endOfFirstArg+1));
                    break;
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
            }

            //Parse Subexpression
            for(; i<input.length() && functionCallType==functions::NONE; i++)
            {
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                currentToken.push_back(input.at(i));
                if(nestingLevel==0 || i==input.length()-1) break;                
            }
        }
        else for(; i<input.length() && ((input.at(i)>='0' && input.at(i)<='9') || input.at(i)=='.' || (input.at(i)=='e' && currentToken!="e")); i++)
        {
            fixOffByOne=true;
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
        if(currentToken!="" && currentToken!="root(" && currentToken.find("sin(")!=0 && currentToken.find("cos(")!=0 && currentToken.find("tan(")!=0 && currentToken.find("log(")!=0) tokens.emplace_back(currentToken);
        currentToken.clear();
        inFunctionCall=false;
        rootHasTwoArgs=false;
        logHasTwoArgs=false;
        startOfFunction=0;
    }

    return tokens;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

token getFunctionToken(const std::string& input, const functions functionCallType, uint &i)
{
    std::string currentToken;
    //in: functionName(expr)
    switch (functionCallType) 
    {
        case functions::FLOOR: {currentToken.append("floor("); i+=6; break;}
        case functions::ROUND: {currentToken.append("round("); i+=6; break;}

        case functions::CEIL: {currentToken.append("ceil("); i+=5; break;}
        case functions::ASIN: {currentToken.append("asin("); i+=5; break;}
        case functions::ACOS: {currentToken.append("acos("); i+=5; break;}
        case functions::ATAN: {currentToken.append("atan("); i+=5; break;}

        case functions::ABSASFUN: {currentToken.append("abs("); i+=4; break;}
        case functions::SIN: {currentToken.append("sin("); i+=4; break;}
        case functions::COS: {currentToken.append("cos("); i+=4; break;}
        case functions::TAN: {currentToken.append("tan("); i+=4; break;}
        case functions::SEC: {currentToken.append("sec("); i+=4; break;}
        case functions::CSC: {currentToken.append("csc("); i+=4; break;}
        case functions::COT: {currentToken.append("cot("); i+=4; break;}
        
        case functions::LN: {currentToken.append("ln("); i+=3; break;}

        default: std::runtime_error("Unhandled function");
    }
    int nestingLevel{};

    for(; i<input.length(); i++)
    {
        if(input.at(i)==')') nestingLevel--;
        else if(input.at(i)=='(') nestingLevel++;

        if(nestingLevel>=0) currentToken.push_back(input.at(i));
        else
        {
            currentToken.push_back(input.at(i));
            return token(currentToken);
        }
        if(i==input.length()-1) return token(currentToken);
    }
    std::cerr<<'\n'<<input<<'\n';
    throw std::runtime_error("Not a valid function!");
    //return token(input);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getVariableArgs(std::vector<token> &tokens, options &options)
{
    if(tokens.size()==0) return;
    for(uint i{}; i<tokens.size(); i++)
    {
        if(tokens.at(i).value().find('x')!=std::string::npos) break;
        else if(i==tokens.size()-1) return;
    }
    std::string input;

    std::cout << "\nSpecify variable minimum: ";
    std::cin>>input;
    if(isNumber(input)) options.xMin=std::stold(input);
    else throw std::runtime_error("You did not enter a number");

    std::cout << "\nSpecify variable maximum: ";
    std::cin>>input;
    if(isNumber(input)) options.xMax=std::stold(input);
    else throw std::runtime_error("You did not enter a number");
    
    std::cout << "\nGraph? y/n: ";
    std::cin>>input;
    if(input.at(0)=='y' || input.at(0)=='Y')
    {
        options.graph=true;
        std::cout<<"\nHigh zoom? Use small ranges with this. y/n: ";
        std::cin>>input;
        if(input.at(0)=='y' || input.at(0)=='Y') options.xStep=0.05;
        else options.xStep=0.2;
    }

    if(!options.graph)
    {
        std::cout << "\nSpecify variable increment/step: ";
        std::cin>>input;
        if(isNumber(input)) options.xStep=std::stold(input);
        else throw std::runtime_error("You did not enter a number");
    }


    if(options.xMin>=options.xMax) throw std::runtime_error("Invalid range!");
    if(options.xMax-options.xMin>options.xStep*1000) throw std::runtime_error("Too many calculations requested!");
    std::cin.ignore();
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double calculation(std::vector<token> tokens, const long double xValue)
{
    std::ostringstream resultAsOSStream;
    resultAsOSStream.precision(LDBL_DIG);
    long double result{};

    for(uint i{1}; i<tokens.size(); i++)
    {
        if((tokens.at(i).type()==token_t::VARIABLE||tokens.at(i).type()==token_t::CONSTANT) && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(tokens.at(i).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i-1).typeCategory()!=tokenCategory_t::OPERATOR&&tokens.at(i-1).type()!=token_t::ROOTARGLEFT&&tokens.at(i-1).type()!=token_t::LOGARGLEFT)
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(tokens.at(i).value()=="-" && tokens.at(i-1).type()!=token_t::BINARYOP && tokens.at(i-1).type()!=token_t::MULTICHARBINARY)
            tokens.emplace(tokens.begin()+i++, token("+"));
        if(tokens.at(i-1).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i).typeCategory()!=tokenCategory_t::OPERATOR&&tokens.at(i).type()!=token_t::ROOTARGRIGHT&&tokens.at(i).type()!=token_t::LOGARGRIGHT)
            tokens.emplace(tokens.begin()+i++, token("*"));
    }

    for(long unsigned int pass{}; pass<=ADDITION; pass++)
    {
        for(uint i{}; i<tokens.size(); i++)
        {
            if(pass==SUBEXPRESSIONS)
            {
                long double evaluatedSubexpr{};
                if(tokens.at(i).type()==token_t::SUBEXPR)
                {
                    evaluatedSubexpr=calculation(getTokens(tokens.at(i).value()), xValue);
                }
                else if(tokens.at(i).type()==token_t::ROOTARGRIGHT)
                {
                    if(i==0) evaluatedSubexpr=evaluateRoot(token("0"),tokens.at(i), xValue);
                    else evaluatedSubexpr=evaluateRoot(tokens.at(i-1),tokens.at(i), xValue);
                    if(i>0 && tokens.at(i-1).type()==token_t::ROOTARGLEFT)
                    {
                        tokens.erase(tokens.begin()+i-1);
                        i--;
                    }
                }

                else if(tokens.at(i).type()==token_t::LOGARGRIGHT)
                {
                    if(i==0) evaluatedSubexpr=evaluateLog(token("0"),tokens.at(i), xValue);
                    else evaluatedSubexpr=evaluateLog(tokens.at(i-1),tokens.at(i), xValue);
                    if(i>0 && tokens.at(i-1).type()==token_t::LOGARGLEFT)
                    {
                        tokens.erase(tokens.begin()+i-1);
                        i--;
                    }
                }

                else if(tokens.at(i).type()==token_t::SINARG) evaluatedSubexpr=evaluateSin(tokens.at(i), xValue); 
                else if(tokens.at(i).type()==token_t::ASINARG) evaluatedSubexpr=evaluateAsin(tokens.at(i), xValue);
                else if(tokens.at(i).type()==token_t::ACOSARG) evaluatedSubexpr=evaluateAcos(tokens.at(i), xValue);   
                else if(tokens.at(i).type()==token_t::ATANARG) evaluatedSubexpr=evaluateAtan(tokens.at(i), xValue);    
                else if(tokens.at(i).type()==token_t::COSARG) evaluatedSubexpr=evaluateCos(tokens.at(i), xValue);
                else if(tokens.at(i).type()==token_t::TANARG) evaluatedSubexpr=evaluateTan(tokens.at(i), xValue);
                else if(tokens.at(i).type()==token_t::SECARG) evaluatedSubexpr=evaluateSec(tokens.at(i), xValue);
                else if(tokens.at(i).type()==token_t::CSCARG) evaluatedSubexpr=evaluateCsc(tokens.at(i), xValue);
                else if(tokens.at(i).type()==token_t::COTARG) evaluatedSubexpr=evaluateCot(tokens.at(i), xValue); 
                else if(tokens.at(i).type()==token_t::FLOORARG) evaluatedSubexpr=evaluateFloor(tokens.at(i), xValue);  
                else if(tokens.at(i).type()==token_t::CEILARG) evaluatedSubexpr=evaluateCeil(tokens.at(i), xValue);
                else if(tokens.at(i).type()==token_t::ROUNDARG) evaluatedSubexpr=evaluateRound(tokens.at(i), xValue);  
                else if(tokens.at(i).type()==token_t::ABSARG) evaluatedSubexpr=evaluateAbs(tokens.at(i), xValue);  
                else if(tokens.at(i).type()==token_t::LNARG) evaluatedSubexpr=evaluateLn(tokens.at(i), xValue); 

                if(tokens.at(i).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i).type()!=token_t::ROOTARGLEFT && tokens.at(i).type()!=token_t::LOGARGLEFT)
                {
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear(); 
                }
            }
            else if(pass==UNARYOPS)
            {
                if(i==0) continue;
                if((tokens.at(i).type()==token_t::UNARYOP || tokens.at(i).type()==token_t::MULTICHARUNARY) && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
                {
                    long double evaluatedUnary=evaluateUnary(tokens.at(i-1), tokens.at(i), xValue);
                    resultAsOSStream << evaluatedUnary;
                    tokens.at(i-1)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
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
                            //Account for something like x^-1
                            if((tokens.at(i-2).value()=="^" || tokens.at(i-1).value()=="**") && tokens.at(i-1).value()=="-" && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                            {
                                long double evaluatedUnary=evaluateUnary(tokens.at(i), tokens.at(i-1), xValue);
                                resultAsOSStream << evaluatedUnary;
                                tokens.at(i-1)=token(resultAsOSStream.str());
                                resultAsOSStream.str("");
                                resultAsOSStream.clear();
                                tokens.erase(tokens.begin()+i);                               
                            }
                            if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="^" || tokens.at(i-1).value()=="**") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                            {
                                long double evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
                                resultAsOSStream << evaluatedBinary;
                                tokens.at(i-2)=token(resultAsOSStream.str());
                                tokens.erase(tokens.begin()+i-1);
                                tokens.erase(tokens.begin()+i-1);
                                resultAsOSStream.str("");
                                resultAsOSStream.clear();
                            }
                        }
                    }
            }
            else if (pass==UNARYMINUS)
            {
                if(i!=0&&tokens.at(i-1).value()=="-" && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    long double evaluatedUnary=evaluateUnary(tokens.at(i), tokens.at(i-1), xValue);
                    resultAsOSStream << evaluatedUnary;
                    tokens.at(i-1)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
                    tokens.erase(tokens.begin()+i);
                    i--;
                }
            }
            else if(pass==MULTIPLICATION)
            {
                if(i<=1) continue;
                if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="*" || tokens.at(i-1).value()=="/") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    long double evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
                    resultAsOSStream << evaluatedBinary;
                    tokens.at(i-2)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
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
                    long double evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
                    resultAsOSStream << evaluatedBinary;
                    tokens.at(i-2)=token(resultAsOSStream.str());
                    tokens.erase(tokens.begin()+i-1);
                    tokens.erase(tokens.begin()+i-1);
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
                    i-=2;
                }
            }
        }
    }
    if(tokens.size()==1 && tokens.at(0).value()=="x")
    {
        resultAsOSStream<<xValue;
        tokens.at(0)=token(resultAsOSStream.str());
    }
    
    if(tokens.size()==1 && (tokens.at(0).type()==token_t::NUMBER|| tokens.at(0).type()==token_t::CONSTANT)) return std::stold(tokens.at(0).value());
    else throw std::runtime_error("Malformed expression!");
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateLn(token &arg, const long double xValue)
{
    return std::log(calculation(getTokens(arg.value()), xValue));
}

long double evaluateSin(token &arg, const long double xValue)
{
    return std::sin(calculation(getTokens(arg.value()), xValue));
}

long double evaluateCos(token &arg, const long double xValue)
{
    return std::cos(calculation(getTokens(arg.value()), xValue));
}

long double evaluateTan(token &arg, const long double xValue)
{
    return std::tan(calculation(getTokens(arg.value()), xValue));
}

long double evaluateCsc(token &arg, const long double xValue)
{
    return 1/std::sin(calculation(getTokens(arg.value()), xValue));
}

long double evaluateSec(token &arg, const long double xValue)
{
    return 1/std::cos(calculation(getTokens(arg.value()), xValue));
}

long double evaluateCot(token &arg, const long double xValue)
{
    return 1/std::tan(calculation(getTokens(arg.value()), xValue));
}

long double evaluateFloor(token &arg, const long double xValue)
{
    return std::floor(calculation(getTokens(arg.value()), xValue));
}

long double evaluateCeil(token &arg, const long double xValue)
{
    return std::ceil(calculation(getTokens(arg.value()), xValue));
}

long double evaluateRound(token &arg, const long double xValue)
{
    return std::round(calculation(getTokens(arg.value()), xValue));
}

long double evaluateAbs(token &arg, const long double xValue)
{
    return std::abs(calculation(getTokens(arg.value()), xValue));
}

long double evaluateAsin(token &arg, const long double xValue)
{
    return std::asin(calculation(getTokens(arg.value()), xValue));
}

long double evaluateAcos(token &arg, const long double xValue)
{
    return std::acos(calculation(getTokens(arg.value()), xValue));
}

long double evaluateAtan(token &arg, const long double xValue)
{
    return std::atan(calculation(getTokens(arg.value()), xValue));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateRoot(token denominatorArg, token &enumeratorArg, const long double xValue)
{
    long double denominator{};

    std::vector<token> tokenToEval{denominatorArg};
    if(denominatorArg.type()!=token_t::ROOTARGLEFT) denominator=2;
    else denominator=calculation(getTokens(denominatorArg.value()), xValue);

    tokenToEval.at(0)=enumeratorArg;
    long double enumerator=calculation(getTokens(enumeratorArg.value()), xValue);

    //if(denominator==0) return NAN;
    if(denominator==static_cast<int>(denominator) && static_cast<int>(denominator)%2==0 && enumerator<0) return NAN;

    if(enumerator<0) return -std::pow(-enumerator,1/denominator);
    else return std::pow(enumerator, 1/denominator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateLog(token denominatorArg, token &enumeratorArg, const long double xValue)
{
    long double denominator{};

    std::vector<token> tokenToEval{denominatorArg};
    if(denominatorArg.type()!=token_t::LOGARGLEFT) denominator=10;
    else denominator=calculation(getTokens(denominatorArg.value()), xValue);

    tokenToEval.at(0)=enumeratorArg;
    long double enumerator=calculation(getTokens(enumeratorArg.value()), xValue);

    //if(denominator==0) return NAN;
    return std::log(enumerator)/std::log(denominator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateBinary(token &numberStringLeft, token &operation, token &numberStringRight, const long double xValue)
{
    long double numberLeft{numberStringLeft.number(xValue)};
    long double numberRight{numberStringRight.number(xValue)};

    if(operation.value()=="+") return numberLeft+numberRight;
    if(operation.value()=="*") return numberLeft*numberRight;
    if(operation.value()=="/") return numberLeft/numberRight;
    if(operation.value()=="^" || operation.value()=="**") return std::pow(numberLeft, numberRight);
    else throw std::runtime_error("Somehow, an unhandled binary-type operation?\nCongrats bro, tell me how you did that");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateUnary(token &numberString, token &operation, const long double xValue)
{
    long double number=numberString.number(xValue);
    long double result{1};
    if(operation.value()=="-") return number*-1;
    if(operation.value()=="!!")
        for(int i{static_cast<int>(std::round(number))%2+2}; i<static_cast<int>(std::round(number))+1; i+=2)
        {
            uint numberAsInt{static_cast<uint>(std::round(number))};
            if(numberAsInt==0) return 1.0;
            if(numberAsInt<=3) return numberAsInt;
            result*=i;
        }
    else if(operation.value()=="!")
        {
            result=std::tgamma(number+1);
        }
    if(operation.value()=="!!" && number<0) return NAN;
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isNumber(const std::string &input)
{
    uint dotCount{};
    uint eCount{};

    if(input=="inf") return true;
    if(input=="-inf") return true;
    if(input=="nan") return true;
    if(input=="-nan") return true;
    if(input=="e") return false;
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

bool isNumberPart(const char input)
{
    return (input>='0' && input<='9') || input=='.' || input=='e';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*

3+(pi/root(2+4,10-2))-25x

3: Number                                               -> NUMBER
+: BinaryOp                                             -> OPERATOR
(pi/root(2+4,10-2)): SubExpr                            -> SUBEXPR
    pi: Constant (Will later be replaced by Number)     -> CONSTANT
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
    Grammar: (Subexpr could also be variable or constant)
    NUMBER||SUBEXPR then SUBEXPR||UNARYOP
    NUMBER||SUBEXPR then BINARYOP then NUMBER||SUBEXPR
    ANY then SUBEXPR
    SUBEXPR then ANY
*/   