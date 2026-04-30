#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
bool sanitize_equation(std::string &equation);
std::string getOperators(const std::string &input);
std::vector<long double> getNumbers(const std::string &input_string);
long double evaluate_two_numbers(const long double &input_a, const long double &input_b, const char selected_operator);
void make_implicit_multiplication_explicit(std::string &equation);
long double calculation(std::string &operators, std::vector<long double> &numbers);
long double calculation_caller(std::string &initial_equation);
long double calculation_caller_caller(std::string &initial_equation); //Take this program very seriously.
bool isDefinitelyOperator(char input); //my function names are impeccable

enum
{
    EXPONENTIATION,
    MULTIPLICATION,
    ADDITION
} operation;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    long double result{};
    std::string equation;
    std::cout << "Type your equation (+-*/^ and parentheses =>() allowed):\n=> ";

    std::getline(std::cin, equation);
    if(equation.length()<3)
    {
        throw std::runtime_error("Your equation was too short.");
    }
    bool broken_equation{};
    broken_equation=sanitize_equation(equation);

    broken_equation:
    if(broken_equation)
    {
        throw std::runtime_error("Your equation could not be sanitized.");
    }
    make_implicit_multiplication_explicit(equation);
    std::string operators {getOperators(equation)};
    std::vector<long double> numbers{getNumbers(equation)};

    if(numbers.size()<2 && numbers.size()>0)
    {
        std::ostringstream numberAsString;
        numberAsString << numbers.at(0);
        equation=numberAsString.str();
        for(unsigned long int i{}; i<equation.length()+2; i++) std::cout <<"=";
        std::cout << "\n " << numbers.at(0) << '\n';
        for(unsigned long int i{}; i<equation.length()+2; i++) std::cout <<"=";
        std::cout<<'\n';
        return 0;
    }
    result=calculation_caller_caller(equation);
    std::ostringstream result_as_string;
    result_as_string << result;
    equation=result_as_string.str();


    for(unsigned long int i{}; i<equation.length()+2; i++) std::cout <<"=";
    std::cout << "\n " << result << '\n';
    for(unsigned long int i{}; i<equation.length()+2; i++) std::cout <<"=";
    std::cout << "\n";

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string getOperators(const std::string &input)
{
    std::string operators;
    for(long unsigned int i=0; i<input.length(); i++)
    {
        if(isDefinitelyOperator(input.at(i)) && input.at(i-1)!='e') operators.push_back(input.at(i));
        if(input.at(i)=='-' && i!=0 && input.at(i-1)!= '^' && input.at(i-1)!= '*' && input.at(i-1)!= '/' && input.at(i-1)!= '+' &&input.at(i-1)!='e') operators.push_back('+');
    }
    return operators;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<long double> getNumbers(const std::string &input_string)
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

            if(i+1<input_string.length())
                if(input_string.at(i)=='e' && (input_string.at(i+1) == '+' || input_string.at(i+1) == '-'))
                {
                    current_number.push_back('e');
                    current_number.push_back(input_string.at(i+1));
                     if(i+2<input_string.length()) i+=2;
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
    if(operators.length()==0) return numbers.at(0);
    long double result_of_previous{};
    uint passes{2};

    for(long unsigned int i_{}; i_<=passes; i_++)
    {
        for(long unsigned int i{EXPONENTIATION}; i<operators.length(); i++)
        {
            if(i_==0 && i==EXPONENTIATION)
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
            if((operators.at(i)=='/' || operators.at(i)=='*') && i_==MULTIPLICATION)
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
            else if(operators.at(i)=='+' && i_==ADDITION)
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
    static bool notFirstRun{};
    if(notFirstRun==0)
    {
        equation.insert(equation.begin(), '(');
        equation.push_back(')');
    }
    //I do not have access to a debugger rn so this is my very bad band-aid. Don't write code like this, kids.

    bool is_unsavable{};
    bool sanitization_required{};

    int digits{};
    int symbols{};


    //Removes garbage and operators before start of equation
    if(notFirstRun==0)
    {
        for(int i__{}; i__<=equation.length(); i__++)
        {
            if(!(equation.at(i__)>='0' && equation.at(i__)<='9') && equation.at(i__)!='-' && equation.at(i__)!='(')
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
            if(!(equation.at(i_)>='0' && equation.at(i_)<='9') && equation.at(i_)!='-' && equation.at(i_)!='+' && equation.at(i_)!= '*' && equation.at(i_)!= '/' && equation.at(i_)!='.' && equation.at(i_)!='^' && equation.at(i_)!='('&& equation.at(i_)!=')')
            {
                equation.erase(equation.begin()+i_);
                i_--; //Yes this causes an underflow but it doesn't matter since it is the last time i_ is used before being incremented again
                sanitization_required=true;
            }
            long unsigned int final_element{equation.length()-1};
            if(equation.at(final_element) == '+' || equation.at(final_element) == '*' || equation.at(final_element) == '/' || equation.at(final_element) == '^') equation.erase(--equation.end());
        }
    }

    for(long unsigned int i{}; i<equation.length(); i++)
    {
        if(i+1<equation.length())
        {
            if(equation.at(i)=='e' && equation.at(i+1)=='+') continue;
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

                if(i>1) if((isDefinitelyOperator(equation.at(i-1))))
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
                i--;
                sanitization_required=true;
                continue;
            }
            if(equation[i]=='-' && equation[i+1]=='+')
            {
                equation.erase(equation.begin()+i+1);
                i--;
                sanitization_required=true;
                continue;
            }

            if((isDefinitelyOperator(equation.at(i) || equation.at(i)=='-')
                &&
                (isDefinitelyOperator(equation.at(i+1)) || equation.at(i+1)==')')))
            {
                is_unsavable=true;
            }
            if((equation.at(i)=='-' && (equation.at(i+1)=='*' || equation.at(i+1)=='/')))
            {
                is_unsavable=true;
            }
            if(equation.at(i)=='(' && isDefinitelyOperator(equation.at(i+1)))
            {
                equation.erase(equation.begin()+i+1);
                sanitization_required=true;
                i--;
                continue;
            }
        }
    }

    for(unsigned long i___{}; i___<equation.length(); i___++)
    {
        if((equation.at(i___)>='0' && equation.at(i___)<='9') || equation.at(i___)=='-') digits++;
        else if (equation.at(i___)!='(' && equation.at(i___)!=')' )symbols++;
    }

    uint parentheses_imbalance{};
    for(unsigned long i____{}; i____<equation.length(); i____++)
    {
        if(equation.at(i____)=='(') parentheses_imbalance++;
        if(equation.at(i____)==')') parentheses_imbalance--;
    }

    if(symbols >= digits) is_unsavable=true;
    if(parentheses_imbalance) is_unsavable=true;
    if(equation.length()<3) return true; // <3
    if(sanitization_required && !is_unsavable) std::cout << "\n=================================================================\n|| Your equation had to be sanitized. Output may be incorrect! ||\n=================================================================\nNew equation: "
        << equation.substr(1, equation.length()-2) << "\n";
    notFirstRun=true;
    return is_unsavable;
}
//The fact that it sanitizes anything is funny because you can use this to calculate some keyboard smashes

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double evaluate_two_numbers(const long double &input_a, const long double &input_b, const char selected_operator)
{
    long double result{};

    switch (selected_operator)
    {
        case '+': {return input_a+input_b;}
        case '-': {return input_a-input_b;}
        case '*': {return input_a*input_b;}
        case '/': {return (long double)input_a/(long double)input_b;}
        case '^': {return std::pow(input_a,input_b);}
    }

    throw std::runtime_error("An unhandled operator was found.");
    return NAN;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double calculation_caller(std::string &initial_equation)
{
    static int recursion_count{};
    static int nesting_levels_calculated{};
    long double result{};
    bool broken_equation{};
    std::string equation{};
    int nesting_levels_inside{};
    static int nesting_levels_found{};
    long double intermediate_result{};
    std::string intermediate_result_as_string{};
    std::string intermediate_equation{};
    int nested_characters_count{2}; //Initialized 2 because of parentheses

    //sanitize_equation(initial_equation);

    if((initial_equation.find('(') != std::string::npos) && (nesting_levels_found>nesting_levels_calculated || recursion_count==0))
    {
        nesting_levels_found++;
        //Recurse until no more parentheses in found, then substitute part in parentheses for result of innermost, return, go again
        for(unsigned long i{}; i<initial_equation.length(); i++)
        {
            if(initial_equation.at(i)=='(')
            {
                for(unsigned long j=i+1; j<initial_equation.length(); j++)
                {
                    //What this does: if you have i.e. 3+3(2+1(4+2)) it will call this same function with 2+1(4+2) and then 4+2
                    if(initial_equation.at(j)=='(')nesting_levels_inside++;
                    else if(initial_equation.at(j)==')') nesting_levels_inside--;
                    if(nesting_levels_inside>=0) nested_characters_count++;

                    if(nesting_levels_inside<0)
                    {
                        recursion_count++;
                        intermediate_result=calculation_caller(equation); //Recurse when parentheses of current closes
                        recursion_count--;
                        std::ostringstream result_as_string;
                        result_as_string << intermediate_result;
                        intermediate_result_as_string=result_as_string.str();

                        for(unsigned long l=0; l<initial_equation.length(); l++)
                        {
                            if(initial_equation.at(l)=='(')
                            {
                                initial_equation.erase(l,nested_characters_count);
                                initial_equation.insert(l, intermediate_result_as_string);
                                l+=nested_characters_count+1;
                            }
                            //else intermediate_equation.push_back(initial_equation.at(l));
                            equation=initial_equation;
                        }

                    }
                    else equation.push_back(initial_equation.at(j));
                }

            }
        }
    }
    else equation=initial_equation;
    sanitize_equation(equation);
    make_implicit_multiplication_explicit(equation);
    std::string operators {getOperators(equation)};
    std::vector<long double> numbers{getNumbers(equation)};
    if(operators.size()==0 && numbers.size()>0)return numbers.at(0);

    if(numbers.size()<= operators.size())
    {
        broken_equation=true;
    }

    if(numbers.size()<2 && numbers.size()>0)
    {
        return numbers.at(0);
    }
    else if(numbers.size()==0) broken_equation=true;
    if(operators.size()==0) return numbers.at(0);
    result=calculation(operators, numbers);
    nesting_levels_calculated++;
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long double calculation_caller_caller(std::string &initial_equation)
{
    std::vector<int> relevant_indexes;
    int nested_sets{};
    int nesting_level{};
    std::vector<std::string> nested_sets_set{};
    std::string nested_set{};
    // if(initial_equation.length()>1 && initial_equation.find('(', 1)==std::string::npos)
    // {
    //     std::string operators {getOperators(initial_equation)};
    //     std::vector<long double> numbers{getNumbers(initial_equation)};        
    //     return calculation(operators, numbers);
    // }
    for(unsigned long i{}; i<initial_equation.length(); i++)
    {
        if(initial_equation.at(i)=='(')
        {
            nesting_level++;
        }
        else if(initial_equation.at(i)==')')
        {
            nesting_level--;
            if(nesting_level<1)
            {
                nested_set.push_back(initial_equation.at(i));
                nested_sets_set.push_back(nested_set);
                nested_set.clear();
                continue;
            }
        }


        if(nesting_level==0 && initial_equation.at(i)=='(')
        {
            nested_sets++;
            relevant_indexes.push_back(i);

        }
        else if (nesting_level==0 && initial_equation.at(i)==')')
        {
            relevant_indexes.push_back(i);
        }
        if(nesting_level>=0)
        {
            nested_set.push_back(initial_equation.at(i));
        }
    }
    long double result{};
    for(unsigned long j{}; j<nested_sets_set.size(); j++)
    {
        result = calculation_caller(nested_sets_set.at(j));
        std::ostringstream result_as_string;
        result_as_string << result;
        if(j+1<nested_sets_set.size())
        {
            nested_sets_set.at(j+1)=nested_sets_set.at(j)+nested_sets_set.at(j+1);
        }

    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void make_implicit_multiplication_explicit(std::string &equation)
{
    for(unsigned long i{1}; i<equation.length(); i++)
    {
        if((equation.at(i)=='(' && (equation.at(i-1)>='0' && equation.at(i-1)<='9') || (equation.at(i-1)==')' && (equation.at(i)>='0' && equation.at(i)<='9'))))
        {
            equation.insert(equation.begin()+i, '*');
        }
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isDefinitelyOperator(char input)
{
    return input=='+' || input=='/' || input=='*' || input=='^';
}