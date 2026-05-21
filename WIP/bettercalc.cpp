#include <cctype>
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
    FUNCTIONS,
    UNARYMINUS,
    MULTIPLICATION,
    MODULUS,
    ADDITION
};

enum class token_t
{
    BINARYOP,
    UNARYOP,
    MULTICHARBINARY,
    MULTICHARUNARY,
    FUNCTION,
    NUMBER,
    ROOTARGRIGHT,
    ROOTARGLEFT,
    ABSARG,
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
    FUNCTION,
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
        if(value.length()==0) return token_t::INVALID;

        if(value.length()==1)
        {
            if(isBinaryOp(value.at(0))) return token_t::BINARYOP;
            else if(isUnaryOp(value.at(0))) return token_t::UNARYOP;
        }

        if(isMultiCharUnary(value)) return token_t::MULTICHARUNARY;
        if(isMultiCharBinary(value)) return token_t::MULTICHARBINARY;

        if(isConstant(value)) return token_t::CONSTANT;
        if(isNumber(value)) return token_t::NUMBER;
        else if(isFunction(value)) return token_t::FUNCTION;
        else if(isRootArgRight(value)) return token_t::ROOTARGRIGHT;
        else if(isRootArgLeft(value)) return token_t::ROOTARGLEFT;
        else if(isLogArgRight(value)) return token_t::LOGARGRIGHT;
        else if(isLogArgLeft(value)) return token_t::LOGARGLEFT;
        else if(isSubexpr(value)) return token_t::SUBEXPR;
        else if(isAbsArg(value)) return token_t::ABSARG;
        else if(value=="x") return token_t::VARIABLE;
        return token_t::INVALID;
    }
    ///////////////////////////////////////////////
    static bool isConstant(const std::string &input)
    {
        return input=="pi" 
            || input=="e" 
            || input=="c" 
            || input=="G"
            || input=="H0"
            || input=="tau" 
            || input=="phi" 
            || input=="eul" 
            || input=="rad" 
            || input=="deg" 
            || input=="i" 
            || input=="inf"
            || input=="ppm"
            || input=="ppb"
            || input=="ppt"
            || input=="prc";
    }

    static bool isBinaryOp(const char c)
    {
        return c=='+' || c=='*' || c=='/' || c=='^' || c=='%';
    }
    static bool isMultiCharBinary(const std::string &input)
    {
        return input=="mod" || 
               input == "**" || 
               input=="npk" || 
               input=="nck";
    }

    static bool isMultiCharUnary(const std::string &input)
    {
        return input=="!!";
    }

    static bool isFunction(const std::string &input)
    {
        return input=="sin" || 
            input=="cos" ||
            input=="tan" ||
            input=="sinh" || 
            input=="cosh" || 
            input=="tanh" ||
            input=="asinh" || 
            input=="acosh" || 
            input=="atanh" ||
            input=="asin" || 
            input=="acos" || 
            input=="atan" ||
            input=="sec" || 
            input=="csc" || 
            input=="cot" ||
            input=="sech" || 
            input=="csch" || 
            input=="coth" ||
            input=="asec" || 
            input=="acsc" || 
            input=="acot" ||
            input=="asech" || 
            input=="acsch" || //Screw math. Genuinely. What the hell.
            input=="acoth" ||
            input=="ln" ||
            input=="abs" ||
            input=="floor" ||
            input=="ceil" ||
            input=="round";
    }

    static bool isUnaryOp(const char c)
    {
        return c=='!'|| c=='-';
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
        if(input=="e") return "2.718281828459045235360287471352";
        if(input=="pi") return "3.14159265358979323846";
        if(input=="tau") return "6.28318530717958647692";
        if(input=="phi") return "1.61803398874989484820";
        if(input=="eul") return "0.57721566490153286060";
        if(input=="rad") return "57.2957795130823209";
        if(input=="deg") return "0.0174532925199432958";
        if(input=="ppm") return "0.000001";
        if(input=="ppb") return "0.000000001";
        if(input=="ppt") return "0.000000000001";
        if(input=="prc") return "0.01";
        if(input=="i") return "nan";
        if(input=="c") return "299792458";
        if(input=="G") return "6.6743e-11";
        if(input=="inf") return "inf";
        if(input=="H0") return "2.2e-18";
        else return input;
    }
    ///////////////////////////////////////////////
    static tokenCategory_t determineTokenCategory(token_t &type)
    {
        if(type==token_t::NUMBER || type==token_t::VARIABLE || type==token_t::CONSTANT) return tokenCategory_t::NUMBER;
        else if(type==token_t::SUBEXPR ||
                type==token_t::ROOTARGLEFT || type==token_t::ROOTARGRIGHT || type==token_t::ABSARG||
                type==token_t::LOGARGLEFT || type==token_t::LOGARGRIGHT) return tokenCategory_t::SUBEXPR;
        else if(type==token_t::FUNCTION) return tokenCategory_t::FUNCTION;
        else return tokenCategory_t::OPERATOR;
    }
    ///////////////////////////////////////////////
    public:
    token(std::string value)
    {
        tokenType = determineType(value);
        if(tokenType==token_t::CONSTANT)
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
        long double valueAsLongDouble{};
        valueAsLongDouble = std::stold(tokenValue);
        return valueAsLongDouble;
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

std::vector<token> getTokens(const std::string&, const std::string &previousResult="nan");
void getVariableArgs(std::vector<token>&, options&);
void graph(const std::vector<point>&points, const long double yMin, const long double yMax, const uint xClosestToZeroIndex, const options &options);
long double calculation(std::vector<token>, const long double xValue,const bool resetInvalid=false);
long double evaluateAbs(token &arg, const long double xValue);

long double evaluateRoot(token denominator, token &enumerator, const long double xValue);
long double evaluateLog(token denominatorArg, token &enumeratorArg, const long double xValue);

long double evaluateUnary(token&, token&, const long double xValue);
long double evaluateBinary(token&, token&, token&, const long double xValue);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    std::string resultHistory;
    std::string previousResult{"nan"};
    bool firstPass{true};
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
        else if(equation.at(0)=='?' || equation.at(0)=='h')
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
            else {std::cerr<<"\nYou did not enter a number\n"; return 0;}
            if(isNumber(argv[3])) options.xMax=std::stold(argv[3]);
            else {std::cerr<<"\nYou did not enter a number\n"; return 0;}
            if(argv[4][0]=='y' || argv[4][0]=='Y' || argv[4][0]=='g' || argv[4][0]=='G')
            {
                options.graph=true;
                if(argv[4][0]=='g' || argv[4][0]=='G') options.xStep=0.2;
                else options.xStep=0.05;
            }
            else if(isNumber(argv[4])) options.xStep=std::stold(argv[4]);
            else {std::cerr<<"\nYou did not enter a number\n"; return 0;}
            
            if(options.xMin>=options.xMax) {std::cerr<<"\nInvalid range\n"; return 0;}
            if(options.xMax-options.xMin>options.xStep*1000) {std::cerr<<"\nToo many calculations requested\n"; return 0;}
        }
        else {std::cerr<<"\nIncluded variable but did not specify all of the following: min, max, step/graphing(g or y (close zoom))n"; return 0;}
    }

    while(!std::cin.eof())
    {
        std::cout.precision(LDBL_DIG);
        resultAsOSStream.precision(LDBL_DIG);
        if(equation!="") goto passedInAsArg;
        if(firstPass) std::cout << "Type your equation (? for help, q to quit):\n=> ";
        else std::cout << "Type your equation:\n=> ";
        std::getline(std::cin, equation);

        if(equation.find("how do i exit vim")!=std::string::npos||equation.find("how to exit vim")!=std::string::npos)
        {
            std::cout<<":q\n\n";
            return 0;
        }

        if(equation.length()==0) continue;
        if(equation.at(0)=='q' || equation.at(0)=='Q' || std::cin.eof() || equation.find("exit")!=std::string::npos || equation.find("quit")!=std::string::npos) break;
        if(equation.at(0)=='?')
        {
            displayHelp();
            equation.clear();
            continue;
        }                                 
        passedInAsArg:

        for(uint i{}; i<equation.length(); i++) if(equation.at(i)>='A' && equation.at(i)<='Z' && equation.at(i)!='G' && equation.at(i)!='H') equation.at(i)=equation.at(i)+32;//'X' -> 'x' ToLower
        
        if(equation.at(0)=='h'||equation.find("hist")!=std::string::npos)
        {
            if(resultHistory!="") std::cout<<"\nHistory:"<<resultHistory<<"\n\n"; 
            equation.clear();
            continue;
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
        for(int i{}; i<equation.length(); i++)
        {
            if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--); //Basic garbage removal
            if(i>=0)
            {
                if(equation.at(i)=='[') equation.at(i)='('; //Cheating
                else if(equation.at(i)==']') equation.at(i)=')';
                else if(equation.at(i)==';') equation.at(i)=',';
            }
        }
        int parenthesesImbalance{};
        uint absValueLineCount{};
        for(uint i{}; i<equation.length(); i++)
        {
            if(equation.at(i)=='|') absValueLineCount++;
            if(equation.at(i)=='(') parenthesesImbalance++;
            else if(equation.at(i)==')') parenthesesImbalance--;
            if(parenthesesImbalance<0 || (equation.length()==i+1 && absValueLineCount%2!=0))
            {
                std::cerr<<"\nParentheses are not balanced!\n\n";
                equation.clear();
            }
        }

        if(absValueLineCount%2!=0||parenthesesImbalance<0) continue;
        
        if(equation.length()==0)
        {
            std::cerr<<"\nNo valid input\n\n";
            equation.clear();
            continue;
        }
        std::vector<token> tokens = getTokens(equation,previousResult);
        if(!passedInAsArg)
        {
            getVariableArgs(tokens, options);
        }
        if(options.xMin==options.xMax) //No x found
        {
            resultAsOSStream<<calculation(tokens, NAN);

            if(resultAsOSStream.str().find("nan")!=std::string::npos)
            {
                previousResult="nan";
                resultAsOSStream.str("");
                resultAsOSStream.clear();
                resultAsOSStream<<"Not a Number";
            }
            else if(resultAsOSStream.str()=="-0")
            {
                previousResult='0';
                resultAsOSStream.str("");
                resultAsOSStream.clear();
                resultAsOSStream<<"0";               
            }
            else previousResult=resultAsOSStream.str();

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
                    previousResult="nan";
                    continue;
                }
                else if(resultAsOSStream.str()=="-0")
                {
                    resultAsOSStream.str()="";
                    resultAsOSStream.clear();       
                    resultAsOSStream<<"0";      
                    previousResult="0"; 
                }
                else previousResult=resultAsOSStream.str();
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
        cleanUp:
        std::cout<<"\n\n";

        if(options.xMin==options.xMax && tokens.size()>1) resultHistory+='\n'+equation+" = "+previousResult;

        resultAsOSStream.str("");
        resultAsOSStream.clear();
        equation.clear();
        tokens.clear();
        options.graph=false;
        options.xMax=0;
        options.xMin=0;
        options.xStep=0;
        firstPass=false;
        calculation(std::vector<token>(),NAN,true); //Reset seenInvalid in calculation, so if an invalid expression is passed on the next iteration, it prints the error text
        if(passedInAsArg) break;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This function is ugly.
void graph(const std::vector<point>&points, const long double yMin, const long double yMax, const uint xClosestToZeroIndex, const options &options)
{
    if(yMin>yMax) return;
    long double xRange=points.size();
    long double yRange=(std::abs(yMax)+std::abs(yMin))/options.xStep+std::abs(yMin*5); //Absurd line

    long double height=yRange+(1/(yRange+0.5))*700; //Trust
    if(height>yRange*3) height=yRange+15;
    const long double length=xRange;

    drawPos yAxisPos=ZERO;
    if(options.xMin>=0) yAxisPos=LEFT;
    else if(options.xMax<=0) yAxisPos=RIGHT;

    if(height>2000 || length>500)
    {
        std::cout<<"\nToo many calculations for plotting.\n";
        return; 
    }

    std::vector<std::string> graph;
    std::ostringstream graphLine;
    for(uint rows{}; rows<height; rows++)
    {
        if(rows>height/2+1 && yMin>=(height/2+1-rows)*options.xStep) break; //End if bottom of graph reached (chops lines at bottom)
        for(uint i{}; i<length; i++)
        {
            if(points.at(i).y==INFINITY || points.at(i).y==-INFINITY) throw std::runtime_error("Encountered infinity!");
            
            //Plot point
            else if((i<length-1&&((points.at(i+1).y)/options.xStep >= height/2-rows)&&(points.at(i).y)/options.xStep<=height/2-rows)||
                    (std::round((points.at(i).y)/options.xStep) == std::round(height/2-rows+options.xStep))||
                    (i>0&&((points.at(i).y)/options.xStep <= height/2-rows)&&(points.at(i-1).y)/options.xStep>=height/2-rows)) graphLine<<'+';
           
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

    uint i{1};
    for(; graph.at(i).find('+')==std::string::npos; i++); //Skip until a line with a point (chops off unnecessary lines from top)


    if(graph.size()-i>300 || length >200)
    {
        std::cerr<<"\nThe graph would be too large.\n";
        return;
    }
    std::cout<<graph.at(0); //Print line with top of y axis

    for(; i<graph.size(); i++) std::cout<<graph.at(i);
    return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void displayHelp()
{
    std::cout<<"\nThis calculator accepts an equation using numbers, ans(previous result) +, -, *, /, ^ (or **), x, !, !!, % (or mod), npk, nck, |expr|, (expr) or [expr] and these functions:\n"<<
    "    root(denominator, enumerator), log(base,value)\n"<<
    "    sin(), cos(), tan(), sec(), cosec(), cot(), arcsin(), arccos(), arctan(), arcsec(), arccosec(), arccot()\n"<<
    "    sinh(), cosh(), tanh(), sech(), cosech(), coth(), arcsinh(), arccosh(), arctanh(), arcsech(), arccosech(), arccoth()\n"<<
    "    floor(), ceil(), round(), abs(), ln()"<<
    "\nConstants: pi, e, c, G, H0, phi, inf(infinity), eul(euler's number), tau(2*pi), rad(180/pi) and deg(pi/180, useful for sin() and stuff),prc(1%), ppm, ppb, ppt\n"<<
    "\nExample: 3+root(2,1+3) = 5\nroot() may be called with one argument, defaulting to square root. Example: root(4) is 2."<<
    "\nYou may also have an equation graphed if you include at least one instance of x."<<
    "\nInput from the command line is also accepted, though you may need to preface some characters with \\ to prevent your terminal from interpreting them."<<
    "\nExample: \"root(5\\!\\!,10\\!\\!)\" -> \"root(5!!, 10!!)\"\nCommand line input values: equation lowestX highestX stepSizeX or graphing (g/y, y for high zoom)\n\n";
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isValidInput(const char c)
{
    return (c>='0'&&c<='9')||c=='.'||c=='x'||c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')'||c=='^'||c=='!'||c=='r'||c=='o'||c=='t'
            ||c==','||c=='e'||c=='s'||c=='i'||c=='n'||c=='c'||c=='a' ||c=='l'||c=='f'||c=='u'||c=='d'||c=='|'||c=='b'||c=='g'||c=='p'
            ||c=='u'||c=='h'||c=='m'||c=='%'||c=='k'||c=='['||c==']'||c=='h'||c=='G'||c=='H'||c==';';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<token> getTokens(const std::string &input, const std::string& previousResult)
{
    static std::string lastSeenResult{};
    if(previousResult!="nan") lastSeenResult=previousResult;
    int nestingLevel{};
    static bool memed{};
    int absNestingLevel{};
    int nestingOfFunction{};
    uint startOfFunction{};
    uint endOfFirstArg{};
    std::vector<token> tokens{};
    std::string currentToken{};
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
        else if (input.at(i)=='%') currentToken='%';
        else if (input.find("mod",i)==i) {currentToken="mod"; i+=2;}
        else if (input.find("npk",i)==i) {currentToken="npk"; i+=2;}
        else if (input.find("nck",i)==i) {currentToken="nck"; i+=2;}
        else if (input.find("ans",i)==i) {currentToken=lastSeenResult; i+=2;}

        //Functions

        else if (input.find("asinh",i)==i) {currentToken="asinh"; i+=4;}
        else if (input.find("acosh",i)==i) {currentToken="acosh"; i+=4;}
        else if (input.find("atanh",i)==i) {currentToken="atanh"; i+=4;}

        else if (input.find("asech",i)==i) {currentToken="asech"; i+=4;}
        else if (input.find("acsch",i)==i) {currentToken="acsch"; i+=4;}
        else if (input.find("acoth",i)==i) {currentToken="acoth"; i+=4;}

        else if (input.find("arcsech",i)==i) {currentToken="asech"; i+=6;} //Alias
        else if (input.find("arccsch",i)==i) {currentToken="acsch"; i+=6;} //Alias
        else if (input.find("arccosech",i)==i) {currentToken="acsch"; i+=8;} //Alias
        else if (input.find("acosech",i)==i) {currentToken="acsch"; i+=6;} //Alias
        else if (input.find("arccoth",i)==i) {currentToken="acoth"; i+=6;} //Alias
        else if (input.find("arcsinh",i)==i) {currentToken="asinh"; i+=6;} //Alias
        else if (input.find("arccosh",i)==i) {currentToken="acosh"; i+=6;} //Alias
        else if (input.find("arctanh",i)==i) {currentToken="atanh"; i+=6;} //Alias

        else if (input.find("asec",i)==i) {currentToken="asec"; i+=4;}
        else if (input.find("acsc",i)==i) {currentToken="acsc"; i+=4;}
        else if (input.find("acot",i)==i) {currentToken="acot"; i+=4;}
        else if (input.find("arcsec",i)==i) {currentToken="asec"; i+=5;} //Alias
        else if (input.find("arccsc",i)==i) {currentToken="acsc"; i+=5;} //Alias
        else if (input.find("acosec",i)==i) {currentToken="acsc"; i+=5;} //Alias
        else if (input.find("arccosec",i)==i) {currentToken="acsc"; i+=7;} //Alias
        else if (input.find("arccot",i)==i) {currentToken="acot"; i+=5;} //Alias

        else if (input.find("arcsin",i)==i) {currentToken="asin"; i+=5;} //Alias
        else if (input.find("arccos",i)==i) {currentToken="acos"; i+=5;} //Alias
        else if (input.find("arctan",i)==i) {currentToken="atan"; i+=5;} //Alias
        else if (input.find("asin",i)==i) {currentToken="asin"; i+=3;}
        else if (input.find("acos",i)==i) {currentToken="acos"; i+=3;}
        else if (input.find("atan",i)==i) {currentToken="atan"; i+=3;}

        else if (input.find("sinh",i)==i) {currentToken="sinh"; i+=3;}
        else if (input.find("cosh",i)==i) {currentToken="cosh"; i+=3;}
        else if (input.find("tanh",i)==i) {currentToken="tanh"; i+=3;}

        else if (input.find("sech",i)==i) {currentToken="sech"; i+=3;}
        else if (input.find("csch",i)==i) {currentToken="csch"; i+=3;}
        else if (input.find("coth",i)==i) {currentToken="coth"; i+=3;}
        else if (input.find("cosech",i)==i) {currentToken="csch"; i+=5;} //Alias
        else if (input.find("cotanh",i)==i) {currentToken="coth"; i+=5;} //Alias

        else if (input.find("sec",i)==i) {currentToken="sec"; i+=2;}
        else if (input.find("csc",i)==i) {currentToken="csc"; i+=2;}
        else if (input.find("cosec",i)==i) {currentToken="csc"; i+=4;} //Alias
        else if (input.find("cot",i)==i) {currentToken="cot"; i+=2;}
        else if (input.find("cotan",i)==i) {currentToken="cot"; i+=4;} //Alias

        else if (input.find("sin",i)==i) {currentToken="sin"; i+=2;}
        else if (input.find("cos",i)==i) {currentToken="cos"; i+=2;}
        else if (input.find("tan",i)==i) {currentToken="tan"; i+=2;}

        else if (input.find("ln",i)==i) {currentToken="ln"; i++;}
        else if (input.find("abs",i)==i) {currentToken="abs"; i+=2;}
        else if (input.find("floor",i)==i) {currentToken="floor"; i+=4;}
        else if (input.find("ceil",i)==i) {currentToken="ceil"; i+=3;}
        else if (input.find("round",i)==i) {currentToken="round"; i+=4;}
        
        //Variable
        else if (input.at(i)=='x') currentToken='x';
        
        //Constants
        else if (input.find("pi",i)==i) {currentToken="pi"; i++;}
        else if (input.find("inf",i)==i) {currentToken="inf"; i+=2;}
        else if (input.find("prc",i)==i) {currentToken="prc"; i+=2;}
        else if (input.find("ppc",i)==i) {currentToken="prc"; i+=2;} //Alias
        else if (input.find("ppm",i)==i) {currentToken="ppm"; i+=2;}
        else if (input.find("ppb",i)==i) {currentToken="ppb"; i+=2;}
        else if (input.find("ppt",i)==i) {currentToken="ppt"; i+=2;}
        else if (input.find("rad",i)==i) {currentToken="rad"; i+=2;}
        else if (input.find("deg",i)==i) {currentToken="deg"; i+=2;}
        else if (input.find("tau",i)==i) {currentToken="tau"; i+=2;}
        else if(input.find("phi",i)==i) {currentToken="phi"; i+=2;}
        else if(input.find("eul", i)==i) {currentToken="eul"; i+=2;}
        else if (input.find("H0", i)==i) {currentToken="H0"; i++;}
        else if (input.at(i)=='e') currentToken='e';
        else if (input.at(i)=='c') currentToken='c';
        else if (input.at(i)=='G') currentToken='G';
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

            
        //Parse |x|... or ||x|| if the user hates me... or ||||x||||. whatever.
        if(currentToken=="" && input.at(i)=='|') for(startOfFunction=i; i<input.length(); i++)
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
            if(i>startOfFunction+1 && nestingLevel<=0 && absNestingLevel==0 && inParentheses==false && input.at(i)=='|' || 
               (i==input.length()-1 && input.at(i)=='|')) 
            {
                currentToken.push_back(input.at(i));
                tokens.emplace_back(currentToken);
                break;
            }
            else if(i==input.length()-1 && input.at(i)!='|') throw std::runtime_error("Bad absolute value... parentheses? Things? Lines?");

            currentToken.push_back(input.at(i));
        }
        //Parse root()
        if(currentToken=="" && input.find("root(",i)==i) for(; i<input.length(); i++)
        {
            if(!inFunctionCall)
            {
                inFunctionCall=true;
                startOfFunction=i;
                i+=5;
                nestingLevel++;
                currentToken.append("root(");
                nestingOfFunction=nestingLevel;
                if(i==input.length()) continue;
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
        if(currentToken=="" && input.find("log(",i)==i) for(; i<input.length(); i++)
        {
            if(!inFunctionCall)
            {
                if(input.find("log(", i)==i)
                {
                    inFunctionCall=true;
                    startOfFunction=i;
                    i+=4;
                    nestingLevel++;
                    currentToken.append("log(");
                    nestingOfFunction=nestingLevel;
                    if(i==input.length()) continue;
                }
                else continue;
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
        if(currentToken=="" && !inFunctionCall && input.at(i)=='(') for(; i<input.length(); i++)
        {
            if(input.at(i)==')') nestingLevel--;
            else if(input.at(i)=='(') nestingLevel++;
            currentToken.push_back(input.at(i));
            if(nestingLevel==0 || i==input.length()-1) break;                
        }

        if(currentToken=="") for(; i<input.length() && ((input.at(i)>='0' && input.at(i)<='9') || (i<input.length()-1 && input.at(i)=='.' && std::isdigit(input.at(i+1))) || (i>0 && std::isdigit(input.at(i-1)) && input.at(i)=='e' && currentToken!="e" && i<input.length()-2 && (input.at(i+1)=='+' || input.at(i+1)=='-') && std::isdigit(input.at(i+2)))); i++)
        {
            fixOffByOne=true;
            if(i+1<input.length() && (input.at(i)=='e' && (input.at(i+1)=='+' || input.at(i+1)=='-')))
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
        if(inFunctionCall) currentToken.clear();
        if(currentToken!="") tokens.emplace_back(currentToken);
        currentToken.clear();
        inFunctionCall=false;
        rootHasTwoArgs=false;
        logHasTwoArgs=false;
        startOfFunction=0;
    }

    return tokens;
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
    else 
    {
        std::cerr<<"\nYou did not enter a number\n";
        options.xMin=1;
    }

    std::cout << "\nSpecify variable maximum: ";
    std::cin>>input;
    if(isNumber(input)) options.xMax=std::stold(input);
    else 
    {
        std::cerr<<"\nYou did not enter a number\n";
        options.xMax=1;
    }
    
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
        else 
        {
            std::cerr<<"\nYou did not enter a number\n";
            options.xStep=1;
        }
    }


    if(options.xMin>=options.xMax) options.xMax=options.xMin+0.01;
    if(options.xMax-options.xMin>options.xStep*1000) 
    {
        options.xMax=options.xMin+options.xStep*1000;
        if(!options.graph) std::cerr<<"\nToo many calculations requested!\n";
    }
    std::cin.ignore();
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double calculation(std::vector<token> tokens, const long double xValue, const bool resetInvalid)
{
    static bool invalidExpressionSeen{};
    if(resetInvalid) invalidExpressionSeen=false;
    if(tokens.size()==0) return NAN;
    if(tokens.size()==1 && tokens.at(0).typeCategory()==tokenCategory_t::NUMBER) return tokens.at(0).number(xValue);
    std::ostringstream resultAsOSStream;
    resultAsOSStream.precision(LDBL_DIG);
    long double result{};

    for(uint i{1}; i<tokens.size(); i++)
    {
        if((tokens.at(i).type()==token_t::VARIABLE||tokens.at(i).type()==token_t::CONSTANT) && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(i==tokens.size()) break;
        if((tokens.at(i).typeCategory()==tokenCategory_t::NUMBER) && (tokens.at(i-1).type()==token_t::VARIABLE||tokens.at(i-1).type()==token_t::CONSTANT))
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(i==tokens.size()) break;
        if((tokens.at(i).typeCategory()==tokenCategory_t::FUNCTION) && (tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER))
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(i==tokens.size()) break;
        if((tokens.at(i).typeCategory()==tokenCategory_t::SUBEXPR||tokens.at(i).typeCategory()==tokenCategory_t::FUNCTION) && tokens.at(i-1).typeCategory()!=tokenCategory_t::OPERATOR&& tokens.at(i-1).typeCategory()!=tokenCategory_t::FUNCTION&&tokens.at(i-1).type()!=token_t::ROOTARGLEFT&&tokens.at(i-1).type()!=token_t::LOGARGLEFT)
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(i==tokens.size()) break;
        if(tokens.at(i).value()=="-" && tokens.at(i-1).type()!=token_t::BINARYOP && tokens.at(i-1).type()!=token_t::MULTICHARBINARY && tokens.at(i-1).type()!=token_t::UNARYOP && tokens.at(i-1).type()!=token_t::MULTICHARUNARY)
            tokens.emplace(tokens.begin()+i++, token("+"));
        if(i==tokens.size()) break;
        if((tokens.at(i-1).typeCategory()==tokenCategory_t::FUNCTION) && tokens.at(i).typeCategory()!=tokenCategory_t::OPERATOR&& tokens.at(i).typeCategory()!=tokenCategory_t::NUMBER&& tokens.at(i).typeCategory()!=tokenCategory_t::SUBEXPR&&tokens.at(i).type()!=token_t::ROOTARGRIGHT&&tokens.at(i).type()!=token_t::LOGARGRIGHT&&tokens.at(i).type()!=token_t::FUNCTION)
            tokens.emplace(tokens.begin()+i++, token("*"));
        if((tokens.at(i-1).typeCategory()==tokenCategory_t::SUBEXPR) && tokens.at(i).typeCategory()!=tokenCategory_t::OPERATOR&& tokens.at(i).typeCategory()!=tokenCategory_t::SUBEXPR&&tokens.at(i).type()!=token_t::ROOTARGRIGHT&&tokens.at(i).type()!=token_t::LOGARGRIGHT&&tokens.at(i).type()!=token_t::FUNCTION)
            tokens.emplace(tokens.begin()+i++, token("*"));
    }

    for(uint i{1}; i<tokens.size(); i++)
    {
        if(tokens.at(i).type()==token_t::BINARYOP && tokens.at(i-1).type()==token_t::BINARYOP && tokens.at(i).value()!="-" && tokens.at(i-1).value()!="+")
        {
            tokens.erase(tokens.begin()+i);
        }
    }

    for(long unsigned int pass{}; pass<=ADDITION; pass++)
    {
        for(int i{}; i<tokens.size(); i++)
        {
            if(pass==SUBEXPRESSIONS)
            {
                long double evaluatedSubexpr{};
                if(tokens.at(i).type()==token_t::SUBEXPR)
                {
                    evaluatedSubexpr=calculation(getTokens(tokens.at(i).value()), xValue);
                }
                else if(tokens.at(i).type()==token_t::ABSARG)
                {
                    evaluatedSubexpr=evaluateAbs(tokens.at(i), xValue);
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
            else if (pass==FUNCTIONS)
            {
                if(i!=0&&(tokens.at(i-1).type()==token_t::FUNCTION) && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
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
            else if (pass==UNARYMINUS)
            {
                if(i!=0&&(tokens.at(i-1).value()=="-") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
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
                if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="*" || tokens.at(i-1).value()=="/" || tokens.at(i-1).value()=="npk" || tokens.at(i-1).value()=="nck" || tokens.at(i-1).value()=="mod" || tokens.at(i-1).value()=="%") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
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
    else if(!invalidExpressionSeen)std::cerr<<"\nExpression could not be evaluated\n"; //throw std::runtime_error("Malformed expression!");
    invalidExpressionSeen=true;
    return NAN;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateAbs(token &arg, const long double xValue)
{
    return std::abs(calculation(getTokens(arg.value()), xValue));
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
    else if(operation.value()=="*") return numberLeft*numberRight;
    else if(operation.value()=="/") return numberLeft/numberRight;
    else if(operation.value()=="^" || operation.value()=="**") return std::pow(numberLeft, numberRight);
    else if(operation.value()=="mod" || operation.value()=="%") return std::fmod(numberLeft,numberRight);
    else if(operation.value()=="npk") return (std::tgamma(numberLeft+1)/std::tgamma(numberLeft-numberRight+1));
    else if(operation.value()=="nck") return (std::tgamma(numberLeft+1)/(std::tgamma(numberRight+1)*std::tgamma(numberLeft-numberRight+1)));

    else throw std::runtime_error("Somehow, an unhandled binary-type operation?\nCongrats bro, tell me how you did that");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluateUnary(token &numberString, token &operation, const long double xValue)
{
    long double number=numberString.number(xValue);
    long double result{1};
    if(operation.value()=="-") return number*-1;

    if(operation.value()=="sin") return std::sin(std::fmod(number,2*M_PI));
    if(operation.value()=="cos") return std::cos(std::fmod(number,2*M_PI));
    if(operation.value()=="tan") return std::tan(std::fmod(number,M_PI));

    if(operation.value()=="sec") return 1/std::cos(std::fmod(number,2*M_PI));
    if(operation.value()=="csc") return 1/std::sin(std::fmod(number,2*M_PI));
    if(operation.value()=="cot") return 1/std::tan(std::fmod(number,M_PI));

    if(operation.value()=="asec") return std::acos(1/number);
    if(operation.value()=="acsc") return std::asin(1/number);
    if(operation.value()=="acot") return std::atan(1/number);

    if(operation.value()=="sinh") return std::sinh(number);
    if(operation.value()=="cosh") return std::cosh(number);
    if(operation.value()=="tanh") return std::tanh(number);

    if(operation.value()=="asinh") return std::asinh(number);
    if(operation.value()=="acosh") return std::acosh(number);
    if(operation.value()=="atanh") return std::atanh(number);

    if(operation.value()=="asech") return std::acosh(1/number);
    if(operation.value()=="acsch") return std::asinh(1/number);
    if(operation.value()=="acoth") return std::atanh(1/number);

    if(operation.value()=="sech") return 1/std::cosh(number);
    if(operation.value()=="csch") return 1/std::sinh(number);
    if(operation.value()=="coth") return 1/std::tanh(number);

    if(operation.value()=="asin") return std::asin(number);
    if(operation.value()=="acos") return std::acos(number);
    if(operation.value()=="atan") return std::atan(number);
    //It's all just trig...
    if(operation.value()=="round") return std::round(number);
    if(operation.value()=="floor") return std::floor(number);
    if(operation.value()=="ceil") return std::ceil(number);
    if(operation.value()=="abs") return std::abs(number);
    if(operation.value()=="ln") return std::log(number);

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