#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
bool sanitize_equation(std::string &equation);
std::string get_operators(const std::string &input);
std::vector<long double> get_numbers(const std::string &input_string);
long double calculation(std::string &operators, std::vector<long double> &numbers);
long double evaluate_two_numbers(const long double &input_a, const long double &input_b, const char selected_operator);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    long double result{};
    std::string equation;
    std::cout << "Type your equation (+-*/^ allowed):\n=> ";
    std::getline(std::cin, equation);
    if(equation.length()<3)
    {
        std::cout << "Equation too short! Exiting.\n";
        return 0;
    }
    bool broken_equation{};
    broken_equation=sanitize_equation(equation);
    broken_equation:
    if(broken_equation)
    {
        std::cout << "\n==============================================\nYour equation could not be sanitized! Exiting.\n==============================================\n";
        return 0;
    }
    std::string operators {get_operators(equation)};
    std::vector<long double> numbers{get_numbers(equation)};

    if(numbers.size()<= operators.size())
    {
        broken_equation=true;
        goto broken_equation;
    }

    if(numbers.size()<2)
    {
        std::cout << equation << std::endl;
        return 0;
    }

    result=calculation(operators, numbers);

    std::ostringstream result_as_string;
    result_as_string << result;
    equation=result_as_string.str();


    for(unsigned long int i{}; i<equation.length()+2; i++) std::cout <<"=";
    std::cout << "\n " << result << std::endl;
    for(unsigned long int i{}; i<equation.length()+2; i++) std::cout <<"=";
    std::cout << "\n";

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string get_operators(const std::string &input)
{
    std::string operators;
    for(long unsigned int i=0; i<input.length(); i++)
    {
        if(input.at(i) == '+' || input.at(i) == '*' || input.at(i) == '/' || input.at(i) == '^') operators.push_back(input.at(i));
        if(input.at(i)=='-' && i!=0 && input.at(i-1)!= '^' && input.at(i-1)!= '*' && input.at(i-1)!= '/' && input.at(i-1)!= '+' ) operators.push_back('+');
    }
    return operators;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<long double> get_numbers(const std::string &input_string)
{
    std::vector<long double> numbers;

    uint offset=0;
    uint offset_negative_number{};
    std::string current_number;
    bool number_present=false;

    for(long unsigned int i_=0; i_<input_string.length(); i_++)
    {
        for(uint i=offset; i<=input_string.length(); i++)
        {
            if(i == input_string.length())
            {
                goto skip;
            }

            if((input_string[i] >='0' && input_string[i]<='9') || input_string[i] == '.')
            {
                if(i>0) if(input_string.at(i-1)=='-') current_number.push_back('-');
                current_number.push_back(input_string.at(i));
                number_present=true;
            }
            else
            {
                skip:
                offset=i+1;
                break;
            }
        }
        if(number_present==true) numbers.push_back (std::stold(current_number));
        number_present=false;
        current_number.clear();
    }
    return numbers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double calculation(std::string &operators, std::vector<long double> &numbers)
{
    long double result_of_previous{};
    uint passes{2};

    for(long unsigned int i_{}; i_<=passes; i_++)
    {
        for(long unsigned int i{}; i<operators.length(); i++)
        {
            if(i_==0 && i==0)
            {
                for(int i__=operators.length()-1; i__>=0; i__--)
                {
                    if(operators.at(i__)=='^')
                    {                    
                        result_of_previous=evaluate_two_numbers(numbers.at(i__), numbers.at(i__+1), operators.at(i__));

                        numbers[i__+1]=result_of_previous;
                        if(numbers.at(i__)<0 && numbers.at(i__+1)>0)
                        {
                            numbers.at(i__+1)=(result_of_previous*(-1));
                            result_of_previous=result_of_previous*(-1);
                        }
                        
                        numbers.erase(numbers.begin()+i__);
                        operators.erase(operators.begin()+i__);
                        continue;
                    }
                }
            }
            if(!operators.length()) continue;
            if((operators.at(i)=='/' || operators.at(i)=='*') && i_==1)
            {
                //Replaces calculated two-number equation with its result and removes the used operator and numbers
                result_of_previous=evaluate_two_numbers(numbers.at(i), numbers.at(i+1), operators.at(i));

                numbers.at(i)=result_of_previous;
                numbers.erase(numbers.begin()+i+1);
                operators.erase(operators.begin()+i);
                i--;
                continue;
                //I feel smart now
            }
            else if(operators.at(i)=='+' && i_==2)
            {
                if(i==0) result_of_previous=evaluate_two_numbers(numbers.at(0), numbers.at(1), operators.at(i));
                else result_of_previous = evaluate_two_numbers(result_of_previous, numbers.at(i+1), operators.at(i));
            }
        }
    }
    return result_of_previous;
} 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool sanitize_equation(std::string &equation) 
{
    bool is_unsavable{};
    bool sanitization_required{};

    int digits{};
    int symbols{};


    //Removes garbage and operators before start of equation
    for(int i__{}; i__<=equation.length(); i__++)
    {
        if(!(equation.at(i__)>='0' && equation.at(i__)<='9') && equation.at(i__)!='-')
        {
            equation.erase(equation.begin());
            if(!equation.length())
            {
                is_unsavable=true;
                break;
            }
            i__--;
            sanitization_required=true;
        }
        else break;
    }

    //Removes garbage in equation
    for(long unsigned int i_{}; i_<equation.length(); i_++)
    {
        if(!(equation.at(i_)>='0' && equation.at(i_)<='9') && equation.at(i_)!='-' && equation.at(i_)!='+' && equation.at(i_)!= '*' && equation.at(i_)!= '/' && equation.at(i_)!='.' && equation.at(i_)!='^')
        {
            equation.erase(equation.begin()+i_);
            i_--; //Yes this causes an underflow but it doesn't matter since it is the last time i_ is used before being incremented again
            sanitization_required=true;
        }
        long unsigned int final_element{equation.length()-1};
        if(equation.at(final_element) == '+' || equation.at(final_element) == '*' || equation.at(final_element) == '/' || equation.at(final_element) == '^') equation.erase(--equation.end());
    }

    for(long unsigned int i{}; i<equation.length(); i++)
    {
        if(i+1<equation.length())
        {
            if(equation.at(i)=='-' && equation.at(i+1)=='-')
            {
                equation.at(i)='+';
                equation.erase(equation.begin()+i+1);
                if(i==0)
                {
                    equation.erase(equation.begin());
                    i--;
                    sanitization_required=true;
                    continue;
                }
                
                if(i>1) if((equation.at(i-1)=='*' || equation.at(i-1)=='/' || equation.at(i-1)=='+' || equation.at(i-1)=='^'))
                {
                    equation.erase(equation.begin()+i);
                    i--;
                    sanitization_required=true;
                    continue;
                }
            }
            if((equation[i]=='+' && equation[i+1]=='+') || (equation[i]=='+' && equation[i+1]=='-'))
            {
                equation.erase(equation.begin()+i);
                sanitization_required=true;
            }
            if(equation[i]=='-' && equation[i+1]=='+') is_unsavable=true;

            if(((equation.at(i)=='*' || equation.at(i)=='/' || equation.at(i)=='+' || equation.at(i)=='^' || equation.at(i)=='-') && (equation.at(i+1)=='*' || equation.at(i+1)=='/' || equation.at(i+1)=='+' || equation.at(i+1)=='^')))
            {
                is_unsavable=true;
            }
            if((equation.at(i)=='-' && (equation.at(i+1)=='*' || equation.at(i+1)=='/')))
            {
                is_unsavable=true;
            }
        }
    }

    for(unsigned long i___{}; i___<equation.length(); i___++)
    {
        if((equation.at(i___)>='0' && equation.at(i___)<='9') || equation.at(i___)=='-') digits++;
        else symbols++;
    }
    if(symbols >= digits) is_unsavable=true;
    if(equation.length()<3) return true; // <3
    if(sanitization_required && !is_unsavable) std::cout << "\n=================================================================\n|| Your equation had to be sanitized. Output may be incorrect! ||\n=================================================================\nNew equation: " << equation << "\n";
    return is_unsavable;
}
//The fact that it sanitizes anything is funny because you can use this to calculate some keyboard smashes

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluate_two_numbers(const long double &input_a, const long double &input_b, const char selected_operator)
{
    long double result{};

    switch (selected_operator)
    {
        case '+': {result=input_a+input_b; break;}
        case '-': {result=input_a-input_b; break;}
        case '*': {result=input_a*input_b; break;}
        case '/': {result=(long double)input_a/(long double)input_b; break;}
        case '^': {result=std::pow(input_a,input_b); break;}
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
