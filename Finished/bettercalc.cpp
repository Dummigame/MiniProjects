#include <cfloat>
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

enum class functions
{
    NONE,
    ROOT,
    SIN,
    COS,
    TAN,
    SEC,
    CSC,
    COT,
    FLOOR,
    CEIL,
    ROUND
};

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
    SINARG,
    COSARG,
    TANARG,
    SECARG,
    CSCARG,
    COTARG,
    FLOORARG,
    CEILARG,
    ROUNDARG,
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

bool isNumberPart(char input);

bool isNumber(const std::string &input);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct point
{
    long double x{};
    long double y{};
    point(long double x, long double y)
    {
        this->x=x;
        if(y==INFINITY || y==-INFINITY) this->y=NAN;
        else this->y=y;
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
        else if(isRootArgRight(value)) return token_t::ROOTARGRIGHT;
        else if(isRootArgLeft(value)) return token_t::ROOTARGLEFT;
        else if(isSubexpr(value)) return token_t::SUBEXPR;
        else if(isSinArg(value)) return token_t::SINARG;
        else if(isCosArg(value)) return token_t::COSARG;
        else if(isTanArg(value)) return token_t::TANARG;
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
    static bool isBinaryOp(const char c)
    {
        return c=='+' || c=='*' || c=='/' || c=='^';
    }

    static bool isUnaryOp(const char c)
    {
        return c=='!'|| c=='-';
    }
    ///////////////////////////////////////////////
    bool isSinArg(std::string &input)
    {
        if(input.find("sin(")!=0) return false;
        for(uint i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
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
        else if(type==token_t::SUBEXPR || type==token_t::SINARG ||type==token_t::COSARG || type==token_t::TANARG ||
                type==token_t::ROOTARGLEFT || type==token_t::ROOTARGRIGHT ||type==token_t::SECARG ||type==token_t::CSCARG||
                type==token_t::COTARG || type==token_t::FLOORARG || type==token_t::CEILARG || type==token_t::ROUNDARG) return tokenCategory_t::SUBEXPR;
        //else if(type==token_t::SINARG ||type==token_t::COSARG) return tokenCategory_t::FUNCTIONARG;
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

        if (tokenType != token_t::NUMBER) throw std::runtime_error("tokenType != tokenType_t::NUMBER");
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
void getVariableArgs(std::vector<token>&, options&);
void graph(const std::vector<point>&points, const long double yMin, const long double yMax, int yClosestToZeroIndex, int xClosestToZeroIndex, long double yClosestToZero, long double xClosestToZero, const options &options);
long double calculation(std::vector<token>, long double xValue);
long double evaluateRoot(token denominator, token enumerator, long double xValue);
long double evaluateUnary(token, token, long double xValue);
long double evaluateBinary(token, token, token, long double xValue);
long double evaluateSin(token arg, long double xValue); //Evaluate your sins. (sine)
long double evaluateCos(token arg, long double xValue);
long double evaluateTan(token arg, long double xValue);
long double evaluateSec(token arg, long double xValue);
long double evaluateCsc(token arg, long double xValue);
long double evaluateCot(token arg, long double xValue);
long double evaluateFloor(token arg, long double xValue);
long double evaluateCeil(token arg, long double xValue);
long double evaluateRound(token arg, long double xValue);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        for(int i{}; i<equation.length(); i++) if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--);
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
        for(int i{}; i<equation.length(); i++) if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--); //Basic garbage removal

        int parenthesesImbalance{};
        for(uint i{}; i<equation.length(); i++)
        {
            if(equation.at(i)=='(') parenthesesImbalance++;
            else if(equation.at(i)==')') parenthesesImbalance--;
            if(equation.length()==i+1 && parenthesesImbalance!=0)
            {
                std::cerr<<"\nParentheses are not balanced!\n\n";
                equation.clear();
            } 
        }
        if(parenthesesImbalance!=false) continue;
        
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
            int yClosestToZeroIndex{INT32_MAX};
            int xClosestToZeroIndex{INT32_MAX};
            std::vector<point> points;
            uint i{};
            for(long double xValue=options.xMin; xValue<=options.xMax; xValue+=options.xStep)
            {
                if(xValue>(-0.0000002) && xValue<0.0000002) xValue=0;
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
        resultAsOSStream.str("");
        resultAsOSStream.clear();
        if(passedInAsArg) break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This function is ugly.
void graph(const std::vector<point>&points, const long double yMin, const long double yMax, int yClosestToZeroIndex, int xClosestToZeroIndex, long double yClosestToZero, long double xClosestToZero, const options &options)
{
    long double length;
    long double height;

    // if(std::abs(yMax)+std::abs(yMin)<=0)
    // {
    //     std::cout<<"\nNo valid graph can be drawn as the height is 0.\n";
    //     return;
    // }

    long double xRange=points.size();
    long double yRange=(std::abs(yMax)+std::abs(yMin))/options.xStep+std::abs(yMin*5); //Absurd line


    height=yRange+(1/(yRange+0.5))*700; //Trust
    if(height>yRange*3) height=yRange+15;
    length=xRange;
    
    uint zoomReduction{1};

    if(height>600 || length >200)
    {
        std::cerr<<"\nThe graph would be too large.\n";
        return;
    }

    drawPos yAxisPos=ZERO;
    if(options.xMin>=0) yAxisPos=LEFT;
    else if(options.xMax<=0) yAxisPos=RIGHT;

    drawPos xAxisPos=ZERO;
    // if(yMin>=0) xAxisPos=BOTTOM;
    // else if(yMax<=0) xAxisPos=TOP;


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
            //else if(points.size()>i+1 && graph.size()>rows-1 && points.at(i).y<points.at(i+1).y && graph.at(rows-1).at(i)=='+') graphLine<<'+'; //Weird line that doesn't interpolate but does something else that is cool?
            
            //Draw X axis
            else if(
            (xAxisPos==BOTTOM && rows==height-1 && i<length-1)||
            (xAxisPos==TOP && rows==0 && i<length-1)||
            (std::round(height/2)==rows && xAxisPos==ZERO && i<length-1)) graphLine<<'-';

            else if(
            (xAxisPos==BOTTOM && rows==height-1 && i==length-1)||
            (xAxisPos==TOP && rows==0 && i==length-1)||
            (std::round(height/2)==rows && xAxisPos==ZERO && i==length-1)) graphLine<<"-  >"; //Man...

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
                graph.push_back(graphLine.str());
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
    std::cout<<"\nThis calculator allows you to write out an equation using numbers, +, -, *, /, ^ (or **), x, !, !! and the following functions:\n"<<
    "\troot(denominator, enumerator)\n"<<
    "\tsin(expression), cos(expression), tan(expression), sec(expression), csc(expression), cot(expression)\n"<<
    "\tfloor(expression), ceil(expression), round(expression)\n"<<
    "\nExample: 3+root(2,1+3) = 5\nroot() may be called with one argument, defaulting to square root. Example: root(4) is 2.\n\n"<<
    "Input from the command line is also accepted, though you may need to preface some characters with \\ to prevent your terminal from interpreting them."<<
    "\nExample: \"root(5\\!\\!,10\\!\\!)\" -> \"root(5!!, 10!!)\"\nCommand line input values: equation lowestX highestX stepSizeX or graphing (g/y, y for high zoom)\n\n";
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isValidInput(const char c)
{
    return (c>='0'&&c<='9')||c=='.'||c=='x'||c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')'||c=='^'||c=='!'||c=='r'||c=='o'||c=='t'
                           ||c==','||c=='e'||c=='s'||c=='i'||c=='n'||c=='c'||c=='a' ||c=='l'||c=='f'||c=='u'||c=='d';
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
    functions functionCallType{};
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
        else if(input.at(i)=='r'|| input.at(i)=='s' || input.at(i)=='t' || input.at(i)=='c' || input.at(i)=='f' || input.at(i)=='(')
        {
            currentToken.clear();
            if(input.at(i)=='r') functionCallType=functions::ROUND;
            else if(input.at(i)=='s')functionCallType=functions::SIN;
            else if(input.at(i)=='c')functionCallType=functions::COS;
            else if(input.at(i)=='t')functionCallType=functions::TAN;
            else if(input.at(i)=='f')functionCallType=functions::FLOOR;
            else functionCallType=functions::NONE;

            //Parse floor()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::FLOOR; i++)
            {
                if(input.at(i)=='f' && !inFunctionCall)
                {
                    if(input.find("floor(", i)==i)
                    {
                        startOfFunction=i;
                        i+=6;
                        nestingLevel++;
                        currentToken.append("floor(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else throw std::runtime_error("Bad function name or stray characters!");
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }
            
            //Parse your sins | sin()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::SIN; i++)
            {
                if(input.at(i)=='s' && !inFunctionCall)
                {
                    if(input.find("sin(", i)==i)
                    {
                        startOfFunction=i;
                        i+=4;
                        nestingLevel++;
                        currentToken.append("sin(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else 
                    {
                        functionCallType=functions::SEC;
                        break;
                    }
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }
            //Parse sec()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::SEC; i++)
            {
                if(input.at(i)=='s' && !inFunctionCall)
                {
                    if(input.find("sec(", i)==i)
                    {
                        startOfFunction=i;
                        i+=4;
                        nestingLevel++;
                        currentToken.append("sec(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else throw std::runtime_error("Bad function name or stray chararacters!");
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }

            //Parse cos()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::COS; i++)
            {
                if(input.at(i)=='c' && !inFunctionCall)
                {
                    if(input.find("cos(", i)==i)
                    {
                        startOfFunction=i;
                        i+=4;
                        nestingLevel++;
                        currentToken.append("cos(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else 
                    {
                        functionCallType=functions::CSC;
                        break;
                    }
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }

            //Parse csc()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::CSC; i++)
            {
                if(input.at(i)=='c' && !inFunctionCall)
                {
                    if(input.find("csc(", i)==i)
                    {
                        startOfFunction=i;
                        i+=4;
                        nestingLevel++;
                        currentToken.append("csc(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else 
                    {
                        functionCallType=functions::COT;
                        break;
                    }
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }

            //Parse cot()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::COT; i++)
            {
                if(input.at(i)=='c' && !inFunctionCall)
                {
                    if(input.find("cot(", i)==i)
                    {
                        startOfFunction=i;
                        i+=4;
                        nestingLevel++;
                        currentToken.append("cot(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else 
                    {
                        functionCallType=functions::CEIL;
                        break;
                    }
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }

            //Parse ceil()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::CEIL; i++)
            {
                if(input.at(i)=='c' && !inFunctionCall)
                {
                    if(input.find("ceil(", i)==i)
                    {
                        startOfFunction=i;
                        i+=5;
                        nestingLevel++;
                        currentToken.append("ceil(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else throw std::runtime_error("Bad function name or stray characters!");
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }

            //Parse tan()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::TAN; i++)
            {
                if(input.at(i)=='t' && !inFunctionCall)
                {
                    if(input.find("tan(", i)==i)
                    {
                        startOfFunction=i;
                        i+=4;
                        nestingLevel++;
                        currentToken.append("tan(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else throw std::runtime_error("Bad function name or stray characters!");
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }

            }
            //Parse round()
            for(currentToken.clear(); i<input.length() && functionCallType==functions::ROUND; i++)
            {
                if(input.at(i)=='r' && !inFunctionCall)
                {
                    if(input.find("round(", i)==i)
                    {
                        startOfFunction=i;
                        i+=6;
                        nestingLevel++;
                        currentToken.append("round(");
                        inFunctionCall=true;
                        nestingOfFunction=nestingLevel;
                        if(i==input.length()) throw std::runtime_error("Bad function call!");
                    }
                    else
                    {
                        functionCallType=functions::ROOT;
                        break;
                    }
                }
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                if(nestingLevel>=nestingOfFunction) currentToken.push_back(input.at(i));
                else
                {
                    tokens.push_back(currentToken);
                    currentToken.clear();
                    break;      
                }
            }
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

            //Parse Subexpression
            for(; i<input.length() && functionCallType==functions::NONE; i++)
            {
                if(input.at(i)==')') nestingLevel--;
                else if(input.at(i)=='(') nestingLevel++;
                currentToken.push_back(input.at(i));
                if(nestingLevel==0) break;                
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
        if(currentToken!="" && currentToken!="root(" && currentToken.find("sin(")!=0 && currentToken.find("cos(")!=0 && currentToken.find("tan(")!=0) tokens.push_back({currentToken});
        currentToken.clear();
        inFunctionCall=false;
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

long double calculation(std::vector<token> tokens, long double xValue)
{
    std::ostringstream resultAsOSStream;
    resultAsOSStream.precision(LDBL_DIG);
    long double result{};

    for(uint i{1}; i<tokens.size(); i++)
    {
        if(tokens.at(i).type()==token_t::VARIABLE && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
            tokens.insert(tokens.begin()+i++, token("*"));
        if(tokens.at(i).typeCategory()==tokenCategory_t::SUBEXPR && tokens.at(i-1).typeCategory()!=tokenCategory_t::OPERATOR&&tokens.at(i-1).type()!=token_t::ROOTARGLEFT)
            tokens.insert(tokens.begin()+i++, token("*"));
        if(tokens.at(i).value()=="-" && tokens.at(i-1).type()!=token_t::BINARYOP && tokens.at(i-1).type()!=token_t::MULTICHARBINARY)
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
                long double evaluatedSubexpr{};
                if(tokens.at(i).type()==token_t::SUBEXPR)
                {
                    evaluatedSubexpr=calculation(getTokens(tokens.at(i).value()), xValue);
                    resultAsOSStream << evaluatedSubexpr;
                    tokens.at(i) = token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
                }
                else if(tokens.at(i).type()==token_t::ROOTARGRIGHT)
                {
                    long double evaluatedRoot;
                    if(i==0) evaluatedRoot=evaluateRoot(token("0"),tokens.at(i), xValue);
                    else evaluatedRoot=evaluateRoot(tokens.at(i-1),tokens.at(i), xValue);
                    resultAsOSStream << evaluatedSubexpr;
                    tokens.at(i) = token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();
                    
                    if(i>0 && tokens.at(i-1).type()==token_t::ROOTARGLEFT) tokens.erase(tokens.begin()+i-1);
                }
                else if(tokens.at(i).type()==token_t::SINARG)
                {
                    long double evaluatedSin=evaluateSin(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSin;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();                   
                }
                else if(tokens.at(i).type()==token_t::COSARG)
                {
                    evaluatedSubexpr=evaluateCos(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();                   
                }
                else if(tokens.at(i).type()==token_t::TANARG)
                {
                    evaluatedSubexpr=evaluateTan(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();                  
                }
                else if(tokens.at(i).type()==token_t::SECARG)
                {
                    evaluatedSubexpr=evaluateSec(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();                   
                }
                else if(tokens.at(i).type()==token_t::CSCARG)
                {
                    evaluatedSubexpr=evaluateCsc(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();                 
                }
                else if(tokens.at(i).type()==token_t::COTARG)
                {
                    evaluatedSubexpr=evaluateCot(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();     
                }
                else if(tokens.at(i).type()==token_t::FLOORARG)
                {
                    evaluatedSubexpr=evaluateFloor(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();       
                }
                else if(tokens.at(i).type()==token_t::CEILARG)
                {
                    evaluatedSubexpr=evaluateCeil(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();     
                }
                else if(tokens.at(i).type()==token_t::ROUNDARG)
                {
                    evaluatedSubexpr=evaluateRound(tokens.at(i), xValue);
                    resultAsOSStream<<evaluatedSubexpr;
                    tokens.at(i)=token(resultAsOSStream.str());
                    resultAsOSStream.str("");
                    resultAsOSStream.clear();     
                }
                else continue;
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
    
    if(tokens.size()==1 && tokens.at(0).type()==token_t::NUMBER) result=std::stold(tokens.at(0).value());
    else throw std::runtime_error("Malformed expression!");
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateSin(token arg, long double xValue)
{
    return std::sin(calculation(getTokens(arg.value()), xValue));
}


long double evaluateTan(token arg, long double xValue)
{
    return std::tan(calculation(getTokens(arg.value()), xValue));
}


long double evaluateCos(token arg, long double xValue)
{
    return std::cos(calculation(getTokens(arg.value()), xValue));
}

long double evaluateSec(token arg, long double xValue)
{
    return 1/std::cos(calculation(getTokens(arg.value()), xValue));
}

long double evaluateCsc(token arg, long double xValue)
{
    return 1/std::sin(calculation(getTokens(arg.value()), xValue));
}

long double evaluateCot(token arg, long double xValue)
{
    return 1/std::tan(calculation(getTokens(arg.value()), xValue));
}

long double evaluateFloor(token arg, long double xValue)
{
    return std::floor(calculation(getTokens(arg.value()), xValue));
}

long double evaluateCeil(token arg, long double xValue)
{
    return std::ceil(calculation(getTokens(arg.value()), xValue));
}

long double evaluateRound(token arg, long double xValue)
{
    return std::round(calculation(getTokens(arg.value()), xValue));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateRoot(token denominatorArg, token enumeratorArg, long double xValue)
{
    long double denominator{};

    std::vector<token> tokenToEval{denominatorArg};
    if(denominatorArg.type()!=token_t::ROOTARGLEFT) denominator=2;
    else denominator=calculation(getTokens(denominatorArg.value()), xValue);

    tokenToEval.at(0)=enumeratorArg;
    long double enumerator=calculation(getTokens(enumeratorArg.value()), xValue);

    if(denominator==0) throw std::runtime_error("0th root is undefined");
    if(denominator==static_cast<int>(denominator) && static_cast<int>(denominator)%2==0 && enumerator<0) throw std::runtime_error("Result of a root() call is not a real number");

    return std::pow(enumerator, 1/denominator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateBinary(token numberStringLeft, token operation, token numberStringRight, long double xValue)
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

long double evaluateUnary(token numberString, token operation, long double xValue)
{
    long double number=numberString.number(xValue);
    long double result{1};
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

bool isNumber(const std::string &input)
{
    uint dotCount{};
    uint eCount{};

    if(input=="inf") return true;
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

bool isNumberPart(char input)
{
    return (input>='0' && input<='9') || input=='.' || input=='e';
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