#pragma once
#include <cctype>
#include <random>
#include <cfloat>
#include <cstdint>
#include <cfloat>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <boost/math/constants/constants.hpp>
#include <sstream>
#include <boost/multiprecision/cpp_dec_float.hpp>
void displayHelp(char arg='a');
bool isValidInput(const char);

using boost::multiprecision::cpp_dec_float_100;
using boost::math::constants::pi;

std::random_device randev;
std::mt19937 randomMt(randev());


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
    MEAN,
    RNDINT,
    RNDSEL,
    ABS,
    GREATEST,
    LEAST,
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
    cpp_dec_float_100 x{};
    cpp_dec_float_100 y{};
    point(cpp_dec_float_100 inX, cpp_dec_float_100 inY)
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
    cpp_dec_float_100 xMin{};
    cpp_dec_float_100 xMax{};  
    cpp_dec_float_100 xStep{}; //Hey, reference
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class token
{

    private:

    token_t tokenType{};
    tokenCategory_t tokenCategory{};
    std::string tokenValue{};

    ///////////////////////////////////////////////
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
        else if(isAbsArg(value)) return token_t::ABS;
        else if(isMeanArg(value)) return token_t::MEAN;
        else if(isLeastArg(value)) return token_t::LEAST;
        else if(isGreatestArg(value)) return token_t::GREATEST;
        else if(isRndintArg(value)) return token_t::RNDINT;
        else if(isRndselArg(value)) return token_t::RNDSEL;
        else if(value=="x") return token_t::VARIABLE;
        return token_t::INVALID;
    }
    ///////////////////////////////////////////////
    static bool isConstant(const std::string &input)
    {
        return input=="pi" 
            || input=="e" 
            || input=="a" //Is it in the game?
            || input=="rnd" 
            || input=="rndint" 
            || input=="ec" 
            || input=="c"
            || input=="R" 
            || input=="G"
            || input=="g"
            || input=="o"
            || input=="h"
            || input=="k"
            || input=="H0"
            || input=="Z0"
            || input=="U0"
            || input=="E0"
            || input=="tau" 
            || input=="phi" 
            || input=="eul" 
            || input=="rad" 
            || input=="deg"
            || input=="inf"
            || input=="ppm"
            || input=="ppb"
            || input=="ppt"
            || input=="prc"
            || input=="me"
            || input=="ma"
            || input=="Na";
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
        
        if(input.at(0)=='|') for(size_t i{1}; i<input.length()-1; i++) tokenValue.push_back(input.at(i));
        else for(size_t i{4}; i<input.length(); i++) tokenValue.push_back(input.at(i));
        return true;
    }    
    ///////////////////////////////////////////////
    bool isMeanArg(std::string &input)
    {
        if(input.find("mean(")!=0) return false;
        for(size_t i{5}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }  
    ///////////////////////////////////////////////
    bool isRndselArg(std::string &input)
    {
        if(input.find("rndsel(")!=0) return false;
        for(size_t i{7}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    } 
    ///////////////////////////////////////////////
    bool isRndintArg(std::string &input)
    {
        if(input.find("rndint(")!=0) return false;
        for(size_t i{7}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }  
    ///////////////////////////////////////////////
    bool isGreatestArg(std::string &input)
    {
        if(input.find("greatest(")!=0) return false;
        for(size_t i{9}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }  
    ///////////////////////////////////////////////
    bool isLeastArg(std::string &input)
    {
        if(input.find("least(")!=0) return false;
        for(size_t i{6}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }  
    ///////////////////////////////////////////////
    bool isRootArgRight(std::string &input)
    {
        if(input.find("root,")!=0) return false;
        
        for(size_t i{5}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    bool isRootArgLeft(std::string &input)
    {
        if(input.find("root(") != 0) return false;
        
        for(size_t i{5}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    bool isLogArgRight(std::string &input)
    {
        if(input.find("log,")!=0) return false;
        
        for(size_t i{4}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    bool isLogArgLeft(std::string &input)
    {
        if(input.find("log(") != 0) return false;
        
        for(size_t i{4}; i<input.length(); i++)
        {
            tokenValue.push_back(input.at(i));
        }
        return true;
    }
    ///////////////////////////////////////////////
    static bool isSubexpr(std::string &input)
    {
        if(input.find(')')!=std::string::npos && input.length()<2) return false;
        if(input.at(0)=='(')
        {
            input.erase(0, 1);
            return true;
        }
        return false;
    }
    ///////////////////////////////////////////////
    static std::string replaceConstants(std::string &input)
    {
        if(input=="e") return "2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427";
        if(input=="pi") return "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068";
        if(input=="tau") return "6.283185307179586476925286766559005768394338798750211641949889184615632812572417997256069650684234136";
        if(input=="phi") return "1.618033988749894848204586834365638117720309179805762862135448622705260462818902449707207204189391137";
        if(input=="eul") return "0.5772156649015328606065120900824024310421593359399235988057672348848677267776646709369470632917467495";
        if(input=="rad") return "57.29577951308232087679815481410517033240547246656432154916024386120284714832155263244096899585111094";
        if(input=="deg") return "0.01745329251994329576923690768488612713442871888541725456097191440171009114603449443682241569634509482";
        if(input=="ppm") return "0.000001";
        if(input=="ppb") return "0.000000001";
        if(input=="ppt") return "0.000000000001";
        if(input=="prc") return "0.01";
        if(input=="c") return "299792458";
        if(input=="G") return "6.6743e-11";
        if(input=="g") return "9.80665";
        if(input=="o") return "5.670374419e-08";
        if(input=="k") return "1.380649e-23";
        if(input=="a") return "0.0072973525693";
        if(input=="h") return "6.62607015e-34";
        if(input=="inf") return "inf";
        if(input=="H0") return "2.2e-18";
        if(input=="me") return "5.9722e+24";
        if(input=="ec") return "1.602176634e-19";
        if(input=="Z0") return "376.730313668";
        if(input=="U0") return "1.25663706212e-06";
        if(input=="E0") return "8.8541878128e-12";
        if(input=="ma") return "1.6605390666e-27";
        if(input=="R") return "8.31446261815";
        if(input=="Na") return "6.02214076e+23";
        if(input=="rnd") return "rnd"; // These are replaced later
        if(input=="rndint") return "rndint";
        std::unreachable();
    }
    ///////////////////////////////////////////////
    static tokenCategory_t determineTokenCategory(token_t &type)
    {
        if(type==token_t::NUMBER || type==token_t::VARIABLE || type==token_t::CONSTANT) return tokenCategory_t::NUMBER;
        else if(type==token_t::SUBEXPR ||
                type==token_t::ROOTARGLEFT || type==token_t::ROOTARGRIGHT || type==token_t::ABS|| type==token_t::GREATEST|| type==token_t::LEAST||
                type==token_t::LOGARGLEFT || type==token_t::LOGARGRIGHT || type==token_t::MEAN || type==token_t::RNDINT || type==token_t::RNDSEL) return tokenCategory_t::SUBEXPR;
        else if(type==token_t::FUNCTION) return tokenCategory_t::FUNCTION;
        else return tokenCategory_t::OPERATOR;
    }
    ///////////////////////////////////////////////
    ///////////////////////////////////////////////

    public:

    token(std::string value)
    {
        tokenType = determineType(value);
        if(tokenType==token_t::CONSTANT) tokenValue=replaceConstants(value);

        tokenCategory=determineTokenCategory(tokenType);
        if(tokenValue=="")tokenValue = value;
    }
    ///////////////////////////////////////////////
    cpp_dec_float_100 number(cpp_dec_float_100 xValue=NAN)
    {
        if(xValue!=NAN && this->tokenType==token_t::VARIABLE)
        {
            std::ostringstream asOSStream;
            asOSStream.precision(100);
            asOSStream << xValue;
            this->tokenValue=asOSStream.str();
            this->tokenType=token_t::NUMBER;
        }

        if(tokenValue=="rnd" || tokenValue=="rndint") return NAN;

        if (tokenType != token_t::NUMBER && tokenType != token_t::CONSTANT) return NAN;
        return static_cast<cpp_dec_float_100>(tokenValue);
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
void graph(const std::vector<point>&points, const cpp_dec_float_100 yMin, const cpp_dec_float_100 yMax, const uint xClosestToZeroIndex, const options &options);
cpp_dec_float_100 calculation(std::vector<token>, const cpp_dec_float_100 xValue,const bool resetInvalid=false);
cpp_dec_float_100 evaluateAbs(token &arg, const cpp_dec_float_100 xValue);

cpp_dec_float_100 evaluateRoot(token denominator, token &enumerator, const cpp_dec_float_100 xValue);
cpp_dec_float_100 evaluateLog(token denominatorArg, token &enumeratorArg, const cpp_dec_float_100 xValue);

cpp_dec_float_100 evaluateUnary(token&, token&, const cpp_dec_float_100 xValue);
cpp_dec_float_100 evaluateBinary(token&, token&, token&, const cpp_dec_float_100 xValue);
cpp_dec_float_100 evaluateMean(token &arg, const cpp_dec_float_100 xValue);
cpp_dec_float_100 evaluateRndint(token &arg, const cpp_dec_float_100 xValue);
cpp_dec_float_100 evaluateRndsel(token &arg, const cpp_dec_float_100 xValue);
cpp_dec_float_100 evaluateGreatest(token &arg, const cpp_dec_float_100 xValue);
cpp_dec_float_100 evaluateLeast(token &arg, const cpp_dec_float_100 xValue);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    std::string resultHistory;
    std::string previousResult{"nan"};
    bool firstPass{true};
    options options;
    std::ostringstream resultAsOSStream;
    resultAsOSStream.precision(100);
    std::cout.precision(100);
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
        else if(equation.find('?')!=std::string::npos)
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
        for(size_t i{}; i<equation.length(); i++) if(!(isValidInput(equation.at(i)))) equation.erase(equation.begin()+i--);
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
            
            if(isNumber(argv[2])) options.xMin=static_cast<cpp_dec_float_100>(argv[2]);
            else {std::cerr<<"\nYou did not enter a number\n"; return 0;}
            if(isNumber(argv[3])) options.xMax=static_cast<cpp_dec_float_100>(argv[3]);
            else {std::cerr<<"\nYou did not enter a number\n"; return 0;}
            if(argv[4][0]=='y' || argv[4][0]=='Y' || argv[4][0]=='g' || argv[4][0]=='G')
            {
                options.graph=true;
                if(argv[4][0]=='g' || argv[4][0]=='G') options.xStep=0.2;
                else options.xStep=0.05;
            }
            else if(isNumber(argv[4])) options.xStep=static_cast<cpp_dec_float_100>(argv[4]);
            else {std::cerr<<"\nYou did not enter a number\n"; return 0;}
            
            if(options.xMin>=options.xMax) {std::cerr<<"\nInvalid range\n"; return 0;}
            if(options.xMax-options.xMin>options.xStep*1000) {std::cerr<<"\nToo many calculations requested\n"; return 0;}
        }
        else {std::cerr<<"\nIncluded variable but did not specify all of the following: min, max, step/graphing(g or y (close zoom))\n"; return 0;}
    }

    while(!std::cin.eof())
    {
        std::cout.precision(100);
        resultAsOSStream.precision(100);
        if(equation!="") goto passedInAsArg;
        if(firstPass) std::cout << "Type your equation (? for help, q to quit):\n=> ";
        else std::cout << "Type your equation:\n=> ";
        std::getline(std::cin, equation);
        std::cout<<'\n';

        if(equation.find("how do i exit vim")!=std::string::npos||equation.find("how to exit vim")!=std::string::npos)
        {
            std::cout<<":q\n\n";
            return 0;
        }

        if(equation.length()==0) continue;
        if(equation.at(0)=='q' || equation.at(0)=='Q' || equation.find("exit")!=std::string::npos || equation.find("quit")!=std::string::npos) break;
        if(equation.at(0)=='?')
        {
            if(equation.length()>1 && (equation.at(1)=='a' || equation.at(1)=='f' || equation.at(1)=='c' || equation.at(1)=='n' || equation.at(1)=='h'))
            {
                displayHelp(equation.at(1));

            }
            else
            {
                std::cout<<"\nSelect from 'a'll, 'f'unctionality, 'c'onstants, 'n'otes and 'h'ints:\n=> ";
                displayHelp(std::cin.get());
                std::cin.ignore(10000,'\n');
            }
            std::cout<<'\n';
            equation.clear();
            continue;
        }                                 
        passedInAsArg:

        for(size_t i{}; i<equation.length(); i++) if(equation.at(i)>='A' && 
                                                   equation.at(i)<='Y' && 
                                                   equation.at(i)!='G' && 
                                                   equation.at(i)!='E' && 
                                                   equation.at(i)!='U' &&
                                                   equation.at(i)!='R' && 
                                                   equation.at(i)!='N' && 
                                                   equation.at(i)!='H') equation.at(i)=equation.at(i)+32; //'X' -> 'x' ToLower with exceptions

        for(size_t i{}; i<equation.length(); i++) if(equation.at(i)<32) equation.erase(i--,1);
        
        if(equation.find("hist")!=std::string::npos)
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
        for(size_t i{}; i<equation.length(); i++)
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

            for(size_t i{}; i<resultAsOSStream.str().length()+2; i++) std::cout <<"=";
            std::cout << "\n " << resultAsOSStream.str() << '\n';
            for(size_t i{}; i<resultAsOSStream.str().length()+2; i++) std::cout <<"=";
            resultAsOSStream.str("");
            resultAsOSStream.clear();
        }
        else if(!options.graph)
        {
            std::cout.precision(100);
            resultAsOSStream.precision(100);
            for(cpp_dec_float_100 xValue=options.xMin; xValue<=options.xMax; xValue+=options.xStep)
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
                    resultAsOSStream.str("");
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
            cpp_dec_float_100 largestY{-DBL_MAX};
            cpp_dec_float_100 smallestY{DBL_MAX};
            cpp_dec_float_100 yClosestToZero{DBL_MAX};
            cpp_dec_float_100 xClosestToZero{DBL_MAX};
            uint xClosestToZeroIndex{INT32_MAX};
            std::vector<point> points;
            size_t i{};
            for(cpp_dec_float_100 xValue=options.xMin; xValue<=options.xMax; xValue+=options.xStep)
            {
                if(xValue>(-0.0000002) && xValue<0.0000002) xValue=0;
                points.push_back(point(xValue,calculation(tokens,xValue)));

                if(abs(points.at(i).y)<yClosestToZero)
                {
                    yClosestToZero=abs(points.at(i).y);
                }
                if(abs(points.at(i).x)<xClosestToZero) 
                {
                    xClosestToZero=abs(points.at(i).x);
                    xClosestToZeroIndex=i;
                }
                if(points.at(i).y<smallestY) smallestY=points.at(i).y;
                if(points.at(i).y>largestY) largestY=points.at(i).y;
                i++;
            }
            graph(points,smallestY,largestY,xClosestToZeroIndex,options);        
        }
        std::cout<<"\n\n";

        if(options.xMin==options.xMax) resultHistory+='\n'+equation+" = "+previousResult;

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
    std::cout<<'\n';
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This function is ugly.
void graph(const std::vector<point>&points, const cpp_dec_float_100 yMin, const cpp_dec_float_100 yMax, const uint xClosestToZeroIndex, const options &options)
{
    if(yMin>yMax) return;
    const cpp_dec_float_100 yMin5 = yMin*5;
    const cpp_dec_float_100 yRange=(abs(yMax)+abs(yMin))/options.xStep+abs(yMin5); //Absurd line
    cpp_dec_float_100 height=yRange+(1/(yRange+0.5))*700; //Trust
    if(height>yRange*3) height=yRange+15;
    
    const cpp_dec_float_100 length=points.size();

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
        if(rows>height/2+1 && yMin>=(height/2+1-rows)*options.xStep) break; //End if bottom of graph reached
        for(size_t i{}; i<length; i++)
        {
            if(points.at(i).y==INFINITY || points.at(i).y==-INFINITY) return; //This should never trigger.
            
            //Plot point
            else if((i<length-1&&((points.at(i+1).y)/options.xStep >= height/2-rows)&&(points.at(i).y)/options.xStep<=height/2-rows)||
                    (round((points.at(i).y)/options.xStep) == round(height/2-rows+options.xStep))||
                    (i>0&&((points.at(i).y)/options.xStep <= height/2-rows)&&(points.at(i-1).y)/options.xStep>=height/2-rows)) graphLine<<'+';
           
            //Draw X axis
            else if((round(height/2)==rows && i<length-1)) graphLine<<'-';

            else if((round(height/2)==rows && i==length-1)) graphLine<<"-  >";

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

    size_t i{1};
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

void displayHelp(char arg)
{

    if(arg>='A' && arg<='Z') arg=arg+32; //'X' -> 'x' ToLower

    if(arg=='a' || arg=='f')
        std::cout<<"\nThis calculator takes an expression using numbers, rnd, rndint, ans<prev. result> +, -, *, /, ^ (or **), x, !, !!, % (mod), npk, nck, |expr|, (expr) or [expr] and these functions:\n"<<
        "    root(denominator, enumerator), log(base,value), mean(arg,arg,arg,...), rndint(arg1,arg2), rndsel(arg,arg,arg,...), least(arg,arg,arg,...), greatest(arg,arg,arg,...)\n"<<
        "    sin, cos, tan, sec, cosec, cot, arcsin, arccos, arctan, arcsec, arccosec, arccot\n"<<
        "    sinh, cosh, tanh, sech, cosech, coth, arcsinh, arccosh, arctanh, arcsech, arccosech, arccoth\n"<<
        "    floor, ceil, round, abs, ln\n\n";

    if(arg=='a' || arg=='c')
        std::cout<<"\nConstants:"<<
        "\n    Mathematics:" <<
        "\n        pi, e, phi, inf, eul<Euler-Mascheroni>, tau<2pi>, rad<180/pi>, deg<pi/180>, prc, ppm, ppb, ppt" <<
        "\n    Physics:"<<
        "\n        c, G, g, me, H0, ec<e>, Z0, U0, E0, h, a, ma, R, o, Na\n\n";

    if(arg=='a' || arg=='h' || arg=='n')
        std::cout<<"\nNotes and Hints:\n"<<
        "    You may graph an equation if you include at least one instance of x.\n"<<
        "    Enter \"hist\" for a calculation history.\n"<<
        "    Input into trig functions is treated as input in radiants. To input as degrees, use the \"deg\" constant.\n"<<
        "    Single argument functions may be called without parentheses, however, the interpretation is unorthodox. sin 5x = sin(5)*x, sin 5^2 = sin25\n"<<
        "    You may use the following notation for numbers: 2.5e+5 = 250000 = 2.5*10^5\n"<<
        "    root() and log() may be called with one argument, with defaults for the other. Example: root(4) = 2, log(10) = 1.\n"<<
        "    Input from the command line is also accepted, though you may need to preface some characters with \\ to prevent your terminal from interpreting them.\n"<<
        "    Example: \"root(5\\!\\!,10\\!\\!)\" -> \"root(5!!, 10!!)\"\n"<<
        "    Command line input values: equation lowestX highestX stepSizeX or graphing (g/y, y for high zoom)\n\n";
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isValidInput(const char c)
{
    return (c>='0'&&c<='9')||c=='.'||c=='x'||c=='+'||c=='-'||c=='*'||c=='/'||c=='('||c==')'||c=='^'||c=='!'||c=='r'||c=='o'||c=='t'
            ||c==','||c=='e'||c=='s'||c=='i'||c=='n'||c=='c'||c=='a' ||c=='l'||c=='f'||c=='u'||c=='d'||c=='|'||c=='b'||c=='g'||c=='p'
            ||c=='u'||c=='h'||c=='m'||c=='%'||c=='k'||c=='['||c==']'||c=='h'||c=='G'||c=='H'||c==';'||c=='Z'||c=='U'||c=='E'||c=='R'
            ||c=='N';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::vector<token> getTokens(const std::string &input, const std::string& previousResult)
{
    static std::string_view lastSeenResult{};
    if(previousResult!="nan") lastSeenResult=previousResult;
    int nestingLevel{};
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

    for(size_t i{}; i<input.length(); i++)
    {

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
                if(i==input.length()-1) continue;
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
            else if(i==input.length()-1 && input.at(i)!='|') continue;

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

        //Parse mean()
        if(currentToken=="" && !inFunctionCall && input.find("mean(", i)==i) for(; i<input.length(); i++)
        {
            if(!inFunctionCall)
            {
                currentToken.append("mean(");
                i+=5;
                inFunctionCall=true;
                if(i==input.length()) continue;
                nestingLevel++;
            }
            if(input.at(i)==')') nestingLevel--;
            else if(input.at(i)=='(') nestingLevel++;
            currentToken.push_back(input.at(i));
            if((i==input.length()-2 && input.at(i)==',' && input.at(i+1)==')') || (input.at(i-1)==',' && input.at(i)==',') || (nestingLevel==0 && input.at(i-1)==',')) //Check for some bad argument cases
            {
                currentToken.clear();
                continue;
            }
            if(nestingLevel==0 || i==input.length()-1)
            {
                tokens.emplace_back(currentToken);
                break;
            }
        }

        //Parse greatest()
        if(currentToken=="" && !inFunctionCall && input.find("greatest(", i)==i) for(; i<input.length(); i++)
        {
            if(!inFunctionCall)
            {
                currentToken.append("greatest(");
                i+=9;
                inFunctionCall=true;
                if(i==input.length()) continue;
                nestingLevel++;
            }
            if(input.at(i)==')') nestingLevel--;
            else if(input.at(i)=='(') nestingLevel++;
            currentToken.push_back(input.at(i));
            if((i==input.length()-2 && input.at(i)==',' && input.at(i+1)==')') || (input.at(i-1)==',' && input.at(i)==',') || (nestingLevel==0 && input.at(i-1)==',')) //Check for some bad argument cases
            {
                currentToken.clear();
                continue;
            }
            if(nestingLevel==0 || i==input.length()-1)
            {
                tokens.emplace_back(currentToken);
                break;
            }
        }

        //Parse least()
        if(currentToken=="" && !inFunctionCall && input.find("least(", i)==i) for(; i<input.length(); i++)
        {
            if(!inFunctionCall)
            {
                currentToken.append("least(");
                i+=6;
                inFunctionCall=true;
                if(i==input.length()) continue;
                nestingLevel++;
            }
            if(input.at(i)==')') nestingLevel--;
            else if(input.at(i)=='(') nestingLevel++;
            currentToken.push_back(input.at(i));
            if((i==input.length()-2 && input.at(i)==',' && input.at(i+1)==')') || (input.at(i-1)==',' && input.at(i)==',') || (nestingLevel==0 && input.at(i-1)==',')) //Check for some bad argument cases
            {
                currentToken.clear();
                continue;
            }
            if(nestingLevel==0 || i==input.length()-1)
            {
                tokens.emplace_back(currentToken);
                break;
            }
        }

        //Parse rndsel()
        if(currentToken=="" && !inFunctionCall && input.find("rndsel(", i)==i) for(; i<input.length(); i++)
        {
            if(!inFunctionCall)
            {
                currentToken.append("rndsel(");
                i+=7;
                inFunctionCall=true;
                if(i==input.length()) continue;
                nestingLevel++;
            }
            if(input.at(i)==')') nestingLevel--;
            else if(input.at(i)=='(') nestingLevel++;
            currentToken.push_back(input.at(i));
            if((i==input.length()-2 && input.at(i)==',' && input.at(i+1)==')') || (input.at(i-1)==',' && input.at(i)==',') || (nestingLevel==0 && input.at(i-1)==',')) //Check for some bad argument cases
            {
                currentToken.clear();
                continue;
            }
            if(nestingLevel==0 || i==input.length()-1)
            {
                tokens.emplace_back(currentToken);
                break;
            }
        }

        //Parse rndint()
        uint argCount{1};
        if(currentToken=="" && !inFunctionCall && input.find("rndint(", i)==i) for(; i<input.length(); i++)
        {
            if(!inFunctionCall)
            {
                currentToken.append("rndint(");
                i+=7;
                inFunctionCall=true;
                if(i==input.length()) continue;
                nestingLevel++;
            }
            if(input.at(i)==')') nestingLevel--;
            else if(input.at(i)=='(') nestingLevel++;
            currentToken.push_back(input.at(i));

            if(input.at(i)==',' && nestingLevel==1) argCount++;
            if(argCount>2)
            {
                for(; i<input.length() && nestingLevel>0; i++)
                {
                    if(input.at(i)==')') nestingLevel--;
                    else if(input.at(i)=='(') nestingLevel++;                    
                }
                break;
            }

            if(((i==input.length()-2 && input.at(i)==',') && input.at(i+1)==')') || (input.at(i-1)==',' && input.at(i)==',') || (nestingLevel==0 && input.at(i-1)==',')) //Check for some bad argument cases
            {
                currentToken.clear();
                continue;
            }
            if(nestingLevel==0 || i==input.length()-1)
            {
                tokens.emplace_back(currentToken);
                break;
            }
        }

        //Parse Subexpression
        if(currentToken=="" && !inFunctionCall && input.at(i)=='(') for(; i<input.length(); i++)
        {
            if(input.at(i)==')') nestingLevel--;
            else if(input.at(i)=='(') nestingLevel++;
            currentToken.push_back(input.at(i));
            if(nestingLevel==0 || i==input.length()-1) break;                
        }

        if(input.at(i)=='+') currentToken='+';
        else if (input.at(i)=='-') currentToken='-';
        else if (input.at(i)=='^') currentToken='^';
        else if (input.at(i)=='/') currentToken='/';
        else if (input.at(i)=='%') currentToken='%';
        else if (input.find("mod",i)==i) {currentToken="mod"; i+=2;}
        else if (input.find("npk",i)==i) {currentToken="npk"; i+=2;}
        else if (input.find("nck",i)==i) {currentToken="nck"; i+=2;}
        else if (input.find("npr",i)==i) {currentToken="npk"; i+=2;}
        else if (input.find("ncr",i)==i) {currentToken="nck"; i+=2;}
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
        else if (input.find("arccosecanth",i)==i) {currentToken="acsch"; i+=11;} //Alias
        else if (input.find("acosech",i)==i) {currentToken="acsch"; i+=6;} //Alias
        else if (input.find("arccoth",i)==i) {currentToken="acoth"; i+=6;} //Alias
        else if (input.find("arcsinh",i)==i) {currentToken="asinh"; i+=6;} //Alias
        else if (input.find("arccosh",i)==i) {currentToken="acosh"; i+=6;} //Alias
        else if (input.find("arctanh",i)==i) {currentToken="atanh"; i+=6;} //Alias

        else if (input.find("asec",i)==i) {currentToken="asec"; i+=3;}
        else if (input.find("acsc",i)==i) {currentToken="acsc"; i+=3;}
        else if (input.find("acot",i)==i) {currentToken="acot"; i+=3;}
        else if (input.find("arcsec",i)==i) {currentToken="asec"; i+=5;} //Alias
        else if (input.find("arcsecant",i)==i) {currentToken="asec"; i+=8;} //Alias
        else if (input.find("arccsc",i)==i) {currentToken="acsc"; i+=5;} //Alias
        else if (input.find("acosec",i)==i) {currentToken="acsc"; i+=5;} //Alias
        else if (input.find("arccosec",i)==i) {currentToken="acsc"; i+=7;} //Alias
        else if (input.find("arccosecant",i)==i) {currentToken="acsc"; i+=10;} //Alias
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
        else if (input.find("rndint",i)==i && input.find("rndint(",i)!=i) {currentToken="rndint"; i+=5;} //Not really a constant but treated like one
        else if (input.find("rnd",i)==i && input.find("rndint(",i)!=i) {currentToken="rnd"; i+=2;}       //Not really a constant but treated like one
        else if (input.find("pi",i)==i) {currentToken="pi"; i++;}
        else if (input.find("inf",i)==i) {currentToken="inf"; i+=2;}
        else if (input.find("prc",i)==i) {currentToken="prc"; i+=2;}
        else if (input.find("ppc",i)==i) {currentToken="prc"; i+=2;} //Alias
        else if (input.find("ppm",i)==i) {currentToken="ppm"; i+=2;}
        else if (input.find("ppb",i)==i) {currentToken="ppb"; i+=2;}
        else if (input.find("ppt",i)==i) {currentToken="ppt"; i+=2;}
        else if (input.find("rad",i)==i) {currentToken="rad"; i+=2;}
        else if (input.find("deg",i)==i) {currentToken="deg"; i+=2;}
        else if (input.find("drg",i)==i) {currentToken="deg"; i+=2;} //Weird alias
        else if (input.find("dgr",i)==i) {currentToken="deg"; i+=2;} //Alias
        else if (input.find("tau",i)==i) {currentToken="tau"; i+=2;}
        else if(input.find("phi",i)==i) {currentToken="phi"; i+=2;}
        else if(input.find("eul", i)==i) {currentToken="eul"; i+=2;}
        else if (input.find("H0", i)==i) {currentToken="H0"; i++;}
        else if (input.find("E0", i)==i) {currentToken="E0"; i++;}
        else if (input.find("Z0", i)==i) {currentToken="Z0"; i++;}
        else if (input.find("U0", i)==i) {currentToken="U0"; i++;}
        else if (input.find("me", i)==i && input.find("mean(", i)!=i) {currentToken="me"; i++;}
        else if (input.find("ma", i)==i) {currentToken="ma"; i++;}
        else if (input.find("ec", i)==i) {currentToken="ec"; i++;}
        else if (input.find("Na", i)==i) {currentToken="Na"; i++;}
        else if (input.at(i)=='e') currentToken='e';
        else if (input.at(i)=='a') currentToken='a';
        else if (input.at(i)=='c') currentToken='c';
        else if (input.at(i)=='G') currentToken='G';
        else if (input.at(i)=='g') currentToken='g';
        else if (input.at(i)=='h') currentToken='h';
        else if (input.at(i)=='k') currentToken='k';
        else if (input.at(i)=='R') currentToken='R';
        else if (input.at(i)=='o') currentToken='o';
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
    for(size_t i{}; i<tokens.size(); i++)
    {
        if(tokens.at(i).value().find('x')!=std::string::npos) break;
        else if(i==tokens.size()-1) return;
    }
    std::string input;

    std::cout << "\nSpecify variable minimum: ";
    std::cin>>input;
    if(!std::cin.eof() && input.length()>0 && isNumber(input)) options.xMin=static_cast<cpp_dec_float_100>(input);
    else 
    {
        std::cerr<<"\nYou did not enter a number\n";
        options.xMin=1;
    }

    std::cout << "\nSpecify variable maximum: ";
    std::cin>>input;
    if(!std::cin.eof() && input.length()>0 && isNumber(input)) options.xMax=static_cast<cpp_dec_float_100>(input);
    else 
    {
        std::cerr<<"\nYou did not enter a number\n";
        options.xMax=1;
    }
    
    std::cout << "\nGraph? y/n: ";
    std::cin>>input;
    if(!std::cin.eof() && input.length()>0)
    {
        if(input.at(0)=='y' || input.at(0)=='Y')
        {
            options.graph=true;
            std::cout<<"\nHigh zoom? Use small ranges with this. y/n: ";
            std::cin>>input;
            if(!std::cin.eof() && input.at(0)=='y' || input.at(0)=='Y') options.xStep=0.05;
            else options.xStep=0.2;
        }
    }

    if(!options.graph)
    {
        std::cout << "\nSpecify variable increment/step: ";
        std::cin>>input;
        if(!std::cin.eof() && isNumber(input) && input.length()>0) options.xStep=static_cast<cpp_dec_float_100>(input);
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

cpp_dec_float_100 calculation(std::vector<token> tokens, const cpp_dec_float_100 xValue, const bool resetInvalid)
{
    static bool invalidExpressionSeen{};
    if(resetInvalid) invalidExpressionSeen=false;
    if(tokens.size()==0) return 0;
    std::ostringstream resultAsOSStream;
    resultAsOSStream.precision(100);

    for(size_t i{}; i<tokens.size(); i++)
    {
        if(tokens.at(i).value()=="rnd" || tokens.at(i).value()=="rndint")
        {
            // This probably sucks
            std::uniform_real_distribution<> longDoubleDist(0,1);
            resultAsOSStream<<longDoubleDist(randomMt);
            std::string randomAsStr {resultAsOSStream.str()};
            if(tokens.at(i).value()=="rndint") // To get random integers, it literally deletes the decimal point
            {
                for(size_t i{}; i<randomAsStr.length(); i++)
                {
                    if(randomAsStr.at(i)=='.')
                    {
                        randomAsStr.erase(i,1);
                        break;
                    }
                }
            }
            tokens.at(i)=token(randomAsStr);
            resultAsOSStream.str("");
            resultAsOSStream.clear();
        }
    }
    if(tokens.size()==1 && tokens.at(0).typeCategory()==tokenCategory_t::NUMBER) return tokens.at(0).number(xValue);
    if(tokens.size()==1 && tokens.at(0).type()==token_t::INVALID) return NAN;
    for(size_t i{1}; i<tokens.size(); i++)
    {
        if(tokens.at(i).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).type()==token_t::UNARYOP && tokens.at(i-1).value()!="-" || tokens.at(i-1).type()==token_t::MULTICHARUNARY))
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(i==tokens.size()) break;
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
        if(tokens.at(i).value()=="-" && tokens.at(i-1).type()!=token_t::BINARYOP && tokens.at(i-1).type()!=token_t::MULTICHARBINARY && tokens.at(i-1).type()!=token_t::UNARYOP && tokens.at(i-1).type()!=token_t::MULTICHARUNARY && tokens.at(i-1).type()!=token_t::FUNCTION)
            tokens.emplace(tokens.begin()+i++, token("+"));
        if(i==tokens.size()) break;
        if((tokens.at(i-1).typeCategory()==tokenCategory_t::FUNCTION) && tokens.at(i).typeCategory()!=tokenCategory_t::OPERATOR&& tokens.at(i).typeCategory()!=tokenCategory_t::NUMBER&& tokens.at(i).typeCategory()!=tokenCategory_t::SUBEXPR&&tokens.at(i).type()!=token_t::ROOTARGRIGHT&&tokens.at(i).type()!=token_t::LOGARGRIGHT&&tokens.at(i).type()!=token_t::FUNCTION)
            tokens.emplace(tokens.begin()+i++, token("*"));
        if((tokens.at(i-1).typeCategory()==tokenCategory_t::SUBEXPR) && tokens.at(i).typeCategory()!=tokenCategory_t::OPERATOR&& tokens.at(i).typeCategory()!=tokenCategory_t::SUBEXPR&&tokens.at(i).type()!=token_t::ROOTARGRIGHT&&tokens.at(i).type()!=token_t::LOGARGRIGHT&&tokens.at(i).type()!=token_t::FUNCTION)
            tokens.emplace(tokens.begin()+i++, token("*"));
        if(tokens.at(i).type()==token_t::INVALID) return NAN;

        if(tokens.at(i).value()=="+" && tokens.at(i-1).typeCategory()!=tokenCategory_t::NUMBER && tokens.at(i-1).typeCategory()!=tokenCategory_t::SUBEXPR) tokens.erase(tokens.begin()+i--); //Lowk this line shouldn't work but it works?
    }

    if(tokens.at(0).typeCategory()==tokenCategory_t::OPERATOR && tokens.at(0).value()!="-") tokens.erase(tokens.begin());

    for(int i{1}; i<tokens.size(); i++)
    {
        if(i>0 && (tokens.at(i).type()==token_t::BINARYOP || tokens.at(i).type()==token_t::MULTICHARBINARY) && (tokens.at(i-1).type()==token_t::BINARYOP || tokens.at(i-1).type()==token_t::MULTICHARBINARY))
        {
            tokens.erase(tokens.begin()+i--);
        }
        if(i>0 && tokens.at(i).value()=="-" && tokens.at(i-1).type()==token_t::UNARYOP && tokens.at(i-1).value()=="-" && tokens.at(i-1).type()==token_t::UNARYOP)
        {
            tokens.erase(tokens.begin()+i-1,tokens.begin()+i+1);
            i-=2;
        }
    }
    long unsigned int pass{};
    long unsigned int failedPass{ADDITION+1};
    for(; pass<=ADDITION; pass++)
    {
        for(int i{}; i<tokens.size(); i++)
        {
            if(pass==SUBEXPRESSIONS)
            {
                cpp_dec_float_100 evaluatedSubexpr{};
                if(tokens.at(i).type()==token_t::SUBEXPR)
                {
                    evaluatedSubexpr=calculation(getTokens(tokens.at(i).value()), xValue);
                }
                else if(tokens.at(i).type()==token_t::MEAN)
                {
                    evaluatedSubexpr=evaluateMean(tokens.at(i), xValue);
                }
                else if(tokens.at(i).type()==token_t::GREATEST)
                {
                    evaluatedSubexpr=evaluateGreatest(tokens.at(i), xValue);
                }
                else if(tokens.at(i).type()==token_t::LEAST)
                {
                    evaluatedSubexpr=evaluateLeast(tokens.at(i), xValue);
                }
                else if(tokens.at(i).type()==token_t::RNDSEL)
                {
                    evaluatedSubexpr=evaluateRndsel(tokens.at(i), xValue);
                }
                else if(tokens.at(i).type()==token_t::RNDINT)
                {
                    evaluatedSubexpr=evaluateRndint(tokens.at(i), xValue);
                }
                else if(tokens.at(i).type()==token_t::ABS)
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
                if(i==0)
                {
                    for(uint j{}; j<tokens.size(); j++)
                    {
                        if(tokens.at(j).type()==token_t::SUBEXPR) failedPass=SUBEXPRESSIONS;
                    }
                    continue;
                }
                if((tokens.at(i).type()==token_t::UNARYOP || tokens.at(i).type()==token_t::MULTICHARUNARY) && tokens.at(i-1).typeCategory()==tokenCategory_t::NUMBER)
                {
                    cpp_dec_float_100 evaluatedUnary=evaluateUnary(tokens.at(i-1), tokens.at(i), xValue);
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
                {
                    for(uint j{}; j<tokens.size(); j++)
                    {
                        if(tokens.at(j).type()==token_t::UNARYOP && failedPass==ADDITION) failedPass=UNARYOPS;
                    }
                    
                    for(i=tokens.size()-1; i>0; i--)
                    {
                        if(i-2<tokens.size())
                        {
                            //Account for something like x^-1
                            if((tokens.at(i-2).value()=="^" || tokens.at(i-1).value()=="**") && tokens.at(i-1).value()=="-" && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                            {
                                cpp_dec_float_100 evaluatedUnary=evaluateUnary(tokens.at(i), tokens.at(i-1), xValue);
                                resultAsOSStream << evaluatedUnary;
                                tokens.at(i-1)=token(resultAsOSStream.str());
                                resultAsOSStream.str("");
                                resultAsOSStream.clear();
                                tokens.erase(tokens.begin()+i);                               
                            }
                            if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="^" || tokens.at(i-1).value()=="**") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                            {
                                cpp_dec_float_100 evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
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
            }
            else if (pass==FUNCTIONS)
            {
                if(i==0)
                    for(uint j{}; j<tokens.size(); j++)
                    {
                        if(tokens.at(j).value()=="^" && failedPass==ADDITION) failedPass=EXPONENTIATION;
                    }
                if(i!=0&&(tokens.at(i-1).type()==token_t::FUNCTION) && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    cpp_dec_float_100 evaluatedUnary=evaluateUnary(tokens.at(i), tokens.at(i-1), xValue);
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
                if(i==0)
                    for(uint j{}; j<tokens.size(); j++)
                    {
                        if(tokens.at(j).type()==token_t::FUNCTION && failedPass==ADDITION) failedPass=FUNCTIONS;
                    }
                if(i!=0&&(tokens.at(i-1).value()=="-") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    cpp_dec_float_100 evaluatedUnary=evaluateUnary(tokens.at(i), tokens.at(i-1), xValue);
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
                if(i==0)
                    for(uint j{}; j<tokens.size(); j++)
                    {
                        if(tokens.at(j).value()=="-" && failedPass==ADDITION) failedPass=UNARYMINUS;
                    }
                if(i<=1) continue;
                if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="*" || tokens.at(i-1).value()=="/" || tokens.at(i-1).value()=="npk" || tokens.at(i-1).value()=="nck" || tokens.at(i-1).value()=="mod" || tokens.at(i-1).value()=="%") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    cpp_dec_float_100 evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
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
                if(i==0)
                    for(uint j{}; j<tokens.size(); j++)
                    {
                        if((tokens.at(j).value()=="*"||tokens.at(j).value()=="%"||tokens.at(j).value()=="mod"||tokens.at(j).value()=="npk"||tokens.at(j).value()=="nck"||tokens.at(j).value()=="/") && failedPass==ADDITION) failedPass=MULTIPLICATION;
                    }
                if(i<=1) continue;
                if(tokens.at(i-2).typeCategory()==tokenCategory_t::NUMBER && (tokens.at(i-1).value()=="+" || tokens.at(i-1).value()=="-") && tokens.at(i).typeCategory()==tokenCategory_t::NUMBER)
                {
                    cpp_dec_float_100 evaluatedBinary=evaluateBinary(tokens.at(i-2), tokens.at(i-1), tokens.at(i), xValue);
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
    if(tokens.size()==1 && tokens.at(0).type()==token_t::VARIABLE) return xValue;
    
    if(tokens.size()==1 && (tokens.at(0).type()==token_t::NUMBER|| tokens.at(0).type()==token_t::CONSTANT)) return static_cast<cpp_dec_float_100>(tokens.at(0).value());
    else if(tokens.size()==1 && tokens.at(0).value()=="nan") return NAN;
    else if(!invalidExpressionSeen)
    {
        std::cerr<<"\nExpression could not be evaluated\n";
        if(tokens.size()>0)
        {
            std::cerr<<"Leftover tokens: ";
            for(size_t i{}; i<tokens.size(); std::cerr<<tokens.at(i++).value()<<' ');
            std::cerr<<"\nEvaluation step: ";
            switch(failedPass)
            {
                case SUBEXPRESSIONS: {std::cerr<<"Subexpressions, ()"; break;}
                case UNARYOPS:       {std::cerr<<"Unary Operations, ! and !!";  break;}
                case EXPONENTIATION: {std::cerr<<"Exponentiation";  break;}
                case FUNCTIONS:      {std::cerr<<"Function evaluation";  break;}
                case UNARYMINUS:     {std::cerr<<"Negation, - operand";  break;}
                case MULTIPLICATION: {std::cerr<<"Multiplication";  break;}
                default: {std::cerr<<"Uncertain or addition"; break;}
            }
            std::cerr<<"\n\n";
        }
        invalidExpressionSeen=true;
    }
    return NAN;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateAbs(token &arg, const cpp_dec_float_100 xValue)
{
    return abs(calculation(getTokens(arg.value()), xValue));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateMean(token &arg, const cpp_dec_float_100 xValue)
{
    cpp_dec_float_100 result{};
    std::vector<cpp_dec_float_100> intermediateResults;
    std::string currentToken;
    int nestingLevel{};
    for(size_t i{}; i<arg.value().length() && nestingLevel>=0; i++)
    {
        if(arg.value().at(i)=='(') nestingLevel++;
        else if(arg.value().at(i)==')') nestingLevel--;
        if(nestingLevel<0) break;
        if(!(arg.value().at(i)==',' && nestingLevel==0) && i<arg.value().length()) currentToken.push_back(arg.value().at(i));
        else
        {
            intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
            currentToken.clear();
        }
    }

    if(currentToken!="") intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue)); //Account for something like mean(3 which is stupid but valid
    for(size_t i{}; i<intermediateResults.size(); i++)
    {
        result+=intermediateResults.at(i);
    }
    result=result/(intermediateResults.size());

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateRndint(token &arg, const cpp_dec_float_100 xValue)
{
    std::vector<long double> intermediateResults;
    std::string currentToken;
    int nestingLevel{};
    for(size_t i{}; i<arg.value().length() && nestingLevel>=0; i++)
    {
        if(arg.value().at(i)=='(') nestingLevel++;
        else if(arg.value().at(i)==')') nestingLevel--;
        if(nestingLevel<0) break;
        if(!(arg.value().at(i)==',' && nestingLevel==0) && i<arg.value().length()) currentToken.push_back(arg.value().at(i));
        else
        {
            intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
            currentToken.clear();
        }
    }
    if(currentToken!="") intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));

    if(intermediateResults.size()!=2)
    {
        std::cerr<<"Did not supply 2 arguments for rndint()\n";
        return NAN;
    }
    if(std::round(intermediateResults.at(0)) > std::round(intermediateResults.at(1))) std::swap(intermediateResults.at(0), intermediateResults.at(1));

    std::uniform_int_distribution<> intDist(static_cast<int>(std::round(intermediateResults.at(0))),static_cast<int>(std::round(intermediateResults.at(1))));
    return intDist(randomMt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateRndsel(token &arg, const cpp_dec_float_100 xValue)
{
    std::vector<cpp_dec_float_100> intermediateResults;
    std::string currentToken;
    int nestingLevel{};
    for(size_t i{}; i<arg.value().length() && nestingLevel>=0; i++)
    {
        if(arg.value().at(i)=='(') nestingLevel++;
        else if(arg.value().at(i)==')') nestingLevel--;
        if(nestingLevel<0) break;
        if(!(arg.value().at(i)==',' && nestingLevel==0) && i<arg.value().length()) currentToken.push_back(arg.value().at(i));
        else
        {
            intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
            currentToken.clear();
        }
    }

    if(currentToken!="") intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
    std::uniform_int_distribution<> intDist(0, intermediateResults.size()-1);
    return intermediateResults.at(intDist(randomMt));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateGreatest(token &arg, const cpp_dec_float_100 xValue)
{
    std::vector<cpp_dec_float_100> intermediateResults;
    std::string currentToken;
    int nestingLevel{};
    for(size_t i{}; i<arg.value().length() && nestingLevel>=0; i++)
    {
        if(arg.value().at(i)=='(') nestingLevel++;
        else if(arg.value().at(i)==')') nestingLevel--;
        if(nestingLevel<0) break;
        if(!(arg.value().at(i)==',' && nestingLevel==0) && i<arg.value().length()) currentToken.push_back(arg.value().at(i));
        else
        {
            intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
            currentToken.clear();
        }
    }

    if(currentToken!="") intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
    for(size_t i{}; i<intermediateResults.size(); i++) if(intermediateResults.at(i)>intermediateResults.at(0)) intermediateResults.at(0)=intermediateResults.at(i);
    return intermediateResults.at(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateLeast(token &arg, const cpp_dec_float_100 xValue)
{
    std::vector<cpp_dec_float_100> intermediateResults;
    std::string currentToken;
    int nestingLevel{};
    for(size_t i{}; i<arg.value().length() && nestingLevel>=0; i++)
    {
        if(arg.value().at(i)=='(') nestingLevel++;
        else if(arg.value().at(i)==')') nestingLevel--;
        if(nestingLevel<0) break;
        if(!(arg.value().at(i)==',' && nestingLevel==0) && i<arg.value().length()) currentToken.push_back(arg.value().at(i));
        else
        {
            intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
            currentToken.clear();
        }
    }

    if(currentToken!="") intermediateResults.emplace_back(calculation(getTokens(currentToken), xValue));
    for(size_t i{}; i<intermediateResults.size(); i++) if(intermediateResults.at(i)<intermediateResults.at(0)) intermediateResults.at(0)=intermediateResults.at(i);
    return intermediateResults.at(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateRoot(token denominatorArg, token &enumeratorArg, const cpp_dec_float_100 xValue)
{
    cpp_dec_float_100 denominator{};

    std::vector<token> tokenToEval{denominatorArg};
    if(denominatorArg.type()!=token_t::ROOTARGLEFT) denominator=2;
    else denominator=calculation(getTokens(denominatorArg.value()), xValue);

    tokenToEval.at(0)=enumeratorArg;
    cpp_dec_float_100 enumerator=calculation(getTokens(enumeratorArg.value()), xValue);

    if(denominator==static_cast<int>(denominator) && static_cast<int>(denominator)%2==0 && enumerator<0) return NAN;

    if(enumerator<0) return -pow(-enumerator,1/denominator);
    else return pow(enumerator, 1/denominator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateLog(token denominatorArg, token &enumeratorArg, const cpp_dec_float_100 xValue)
{
    cpp_dec_float_100 denominator{};

    std::vector<token> tokenToEval{denominatorArg};
    if(denominatorArg.type()!=token_t::LOGARGLEFT) denominator=10;
    else denominator=calculation(getTokens(denominatorArg.value()), xValue);

    return log(calculation(getTokens(enumeratorArg.value()), xValue))/log(denominator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateBinary(token &numberStringLeft, token &operation, token &numberStringRight, const cpp_dec_float_100 xValue)
{
    cpp_dec_float_100 numberLeft{numberStringLeft.number(xValue)};
    cpp_dec_float_100 numberRight{numberStringRight.number(xValue)};

    if(operation.value()=="+") return numberLeft+numberRight;
    else if(operation.value()=="*") return numberLeft*numberRight;
    else if(operation.value()=="/") return numberLeft/numberRight;
    else if(operation.value()=="^" || operation.value()=="**") return pow(numberLeft, numberRight);
    else if(operation.value()=="mod" || operation.value()=="%") return fmod(numberLeft,numberRight);
    else if(operation.value()=="npk") return (tgamma(numberLeft+1)/tgamma(numberLeft-numberRight+1));
    else if(operation.value()=="nck") return (tgamma(numberLeft+1)/(tgamma(numberRight+1)*tgamma(numberLeft-numberRight+1)));

    std::unreachable();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cpp_dec_float_100 evaluateUnary(token &numberString, token &operation, const cpp_dec_float_100 xValue)
{
    cpp_dec_float_100 number=numberString.number(xValue);
    cpp_dec_float_100 result{1};
    if(operation.value()=="-") return -number;

    if(operation.value()=="sin") return sin(fmod(number,2*pi<cpp_dec_float_100>()));
    if(operation.value()=="cos") return cos(fmod(number,2*pi<cpp_dec_float_100>()));
    if(operation.value()=="tan") return tan(fmod(number,pi<cpp_dec_float_100>()));

    if(operation.value()=="sec") return 1/cos(fmod(number,2*pi<cpp_dec_float_100>()));
    if(operation.value()=="csc") return 1/sin(fmod(number,2*pi<cpp_dec_float_100>()));
    if(operation.value()=="cot") return 1/tan(fmod(number,pi<cpp_dec_float_100>()));

    if(operation.value()=="asec") return acos(1/number);
    if(operation.value()=="acsc") return asin(1/number);
    if(operation.value()=="acot") return atan(1/number);

    if(operation.value()=="sinh") return sinh(number);
    if(operation.value()=="cosh") return cosh(number);
    if(operation.value()=="tanh") return tanh(number);

    if(operation.value()=="asinh") return asinh(number);
    if(operation.value()=="acosh") return acosh(number);
    if(operation.value()=="atanh") return atanh(number);

    if(operation.value()=="asech") return acosh(1/number);
    if(operation.value()=="acsch") return asinh(1/number);
    if(operation.value()=="acoth") return atanh(1/number);

    if(operation.value()=="sech") return 1/cosh(number);
    if(operation.value()=="csch") return 1/sinh(number);
    if(operation.value()=="coth") return 1/tanh(number);

    if(operation.value()=="asin") return asin(number);
    if(operation.value()=="acos") return acos(number);
    if(operation.value()=="atan") return atan(number);

    if(operation.value()=="round") return round(number);
    if(operation.value()=="floor") return floor(number);
    if(operation.value()=="ceil") return ceil(number);
    if(operation.value()=="abs") return abs(number);
    if(operation.value()=="ln") return log(number);

    if(operation.value()=="!!")
    {
        if(number<0) return NAN;
        number=round(number);
        for(cpp_dec_float_100 i{fmod(number, 2)+2}; i<number+1; i+=2)
        {
            if(number>19572801.5) return INFINITY;
            if(number==0) return 1.0;
            result*=i;
        }
        if(number<=3) return number;
    }
    else if(operation.value()=="!")
    {
        return tgamma(number+1);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isNumber(const std::string &input)
{
    for(size_t i{}; i<input.length(); i++) if((input.at(i)<'0' || input.at(i)>'9') && 
                                             input.at(i)!='e' && 
                                             input.at(i)!='.' &&
                                             input.at(i)!='+' &&
                                             input.at(i)!='-') return false;
    uint dotCount{};
    uint eCount{};

    if(input=="inf") return true;
    if(input=="-inf") return true;
    if(input=="nan") return true;
    if(input=="-nan") return true;
    if(input=="e") return false;
    for(size_t i{}; i<input.length(); i++)
    {
        if(input.at(0)=='-') continue;
        if(input.at(i)=='e') 
        {
            if(i+2<input.length() && input.at(i)=='e' && (input.at(i+1)=='+' || input.at(i+1)=='-') && std::isdigit(input.at(i+2))) i+=2;
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