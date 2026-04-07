#include <iostream>
#include <string>
#include <vector>

//Todo: Make negative numbers consistently get their sign (Line 70)

bool sanitize_equation(std::string &equation);
std::string get_operators(const std::string &input);
std::vector<double> get_numbers(const std::string &input_string);
std::vector<double> multiply_and_divide(const std::string &operators, const std::vector<double> &numbers, uint &i, std::vector<double> &new_numbers);
double calculation(std::string &operators, std::vector<double> &numbers);
double evaluate_two_numbers(const double &input_a, const double &input_b, const char selected_operator);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    double result{};
    std::string equation;
    std::cout << "Type your equation (+-*/ allowed):\n";
    std::getline(std::cin, equation);
    if(equation.length()<3)
    {
        std::cout << "Equation too short! Exiting.\n";
        return 0;
    }
    bool broken_equation=sanitize_equation(equation);
    if(broken_equation)
    {
        std::cout << "Your equation could not be sanitized! Exiting.\n";
        return 0;
    }
    std::string operators {get_operators(equation)};
    std::vector<double> numbers{get_numbers(equation)};
    if(numbers.size()<2)
    {
        std::cout << equation << std::endl;
        return 0;
    }

    result=calculation(operators, numbers);
    std::cout << "\n" << result << std::endl;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string get_operators(const std::string &input)
{
    std::string operators;
    for(long unsigned int i=0; i<input.length(); i++)
    {
        if(input[i] == '+' || input[i] == '*' || input[i] == '/') operators.push_back(input[i]);
        if(input[i]=='-'&&i!=0) operators.push_back('+');
    }
    return operators;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<double> get_numbers(const std::string &input_string)
{
    std::vector<double> numbers;

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
                if(i>0) if(input_string[i-1]=='-') current_number.push_back('-');
                current_number.push_back(input_string[i]);
                number_present=true;
            }
            else
            {
                skip:
                offset=i+1;
                break;
            }
        }
        if(number_present==true) numbers.push_back (std::stod(current_number));
        number_present=false;
        current_number.clear();
    }
    return numbers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double calculation(std::string &operators, std::vector<double> &numbers)
{
    double result_of_previous{};
    uint passes{1};
    uint offset{};

    for(long unsigned int i_{}; i_<=passes; i_++)
    {
        for(int i{}; i<operators.length(); i++)
        {
            if((operators[i]=='/' || operators[i]=='*') && i_==0)
            {
                //Replaces calculated two-number equation with its result and removes the used operator and numbers
                result_of_previous=evaluate_two_numbers(numbers[i], numbers[i+1], operators[i]);

                if(numbers[i+1]<0) operators.erase(operators.begin()+i);

                numbers[i]=result_of_previous;
                numbers.erase(numbers.begin()+i+1);
                operators.erase(operators.begin()+i);
                offset++;
                i--;
                //I feel smart now
            }
            else if((operators[i]=='+') && i_==1)
            {
                if(i==0) result_of_previous=evaluate_two_numbers(numbers[0], numbers[1], operators[i]);
                else result_of_previous = evaluate_two_numbers(result_of_previous, numbers[i+1], operators[i]);
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

    //Removes garbage and operators before start of equation
    for(int i__{}; i__<=equation.length(); i__++)
    {
        if(!(equation[i__]>='0' && equation[i__]<='9') && equation[i__]!='-')
        {
            equation.erase(equation.begin());
        }
        else break;
    }

    //Removes garbage in equation
    for(int i_{}; i_<equation.length(); i_++)
    {
        if(!(equation[i_]>='0' && equation[i_]<='9') && equation[i_]!='-' && equation[i_]!='+' && equation[i_]!= '*' && equation[i_]!= '/' && equation[i_]!='.')
        {
            equation.erase(equation.begin()+i_);
            i_--;
            sanitization_required=true;
        }
    }

    for(long unsigned int i{}; i<equation.length(); i++)
    {
        if(i+1<equation.length())
        {
            if(equation[i]=='-' && equation[i+1]=='-')
            {
                equation[i]='+';
                equation.erase(equation.begin()+i+1);
                if(i==0) equation.erase(equation.begin());
                sanitization_required=true;
            }
            if((equation[i]=='+' && equation[i+1]=='+') || (equation[i]=='+' && equation[i+1]=='-'))
            {
                equation.erase(equation.begin()+i);
            }
            if(equation[i]=='-' && equation[i+1]=='+') is_unsavable=true;

            if(((equation[i]=='*' || equation[i]=='/' || equation[i]=='+') && (equation[i+1]=='*' || equation[i+1]=='/' || equation[i+1]=='+')))
            {
                is_unsavable=true;
            }
            if((equation[i]=='-' && (equation[i+1]=='*' || equation[i+1]=='/')))
            {
                is_unsavable=true;
            }
        }
    }
    if(equation.length()<3) return true; // <3
    if(sanitization_required && !is_unsavable) std::cout << "\n=================================================================\n|| Your equation had to be sanitized. Output may be incorrect! ||\n=================================================================\n";
    return is_unsavable;
}
//The fact that it sanitizes anything is funny because you can use this to calculate some keyboard smashes

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double evaluate_two_numbers(const double &input_a, const double &input_b, const char selected_operator)
{
    double result{};

    switch (selected_operator)
    {
        case '+': {result=input_a+input_b; break;}
        case '-': {result=input_a-input_b; break;}
        case '*': {result=input_a*input_b; break;}
        case '/': {result=(double)input_a/(double)input_b; break;}
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
