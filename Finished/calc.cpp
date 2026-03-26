#include <iostream>
#include <string>
#include <vector>

enum operator_names_t
{
ADD,
SUBTRACT,
MULTIPLY,
DIVIDE
};

bool error{};
std::string get_operators(const std::string &input);
std::vector<double> get_numbers(const std::string &input_string);
void sort_equation(std::string &input, std::vector<double> &numbers);

double calculation(std::string &operators, std::vector<double> &numbers);

double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

operator_names_t select_operator(const char &input);
double evaluate_two_numbers(const double &input_a, const double &input_b, const operator_names_t &selected_operator);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    std::string equation;
    std::cout << "Type your equation (+-*/ allowed, evaluated from left to right): " << std::endl;
    std::cin >> equation;

    std::string operators {get_operators(equation)};
    std::vector<double> numbers{get_numbers(equation)};

    double result {calculation(operators, numbers)};
    if(error) return 1;
    std::cout << "\n" << result << std::endl;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string get_operators(const std::string &input)
{
    std::string operators;
    for(uint i=0; i<input.length(); i++)
    {
        if(input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/')
        {
            operators.push_back(input[i]);
        }
    }
    return operators;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<double> get_numbers(const std::string &input_string)
{
    std::vector<double> numbers;

    uint offset=0;
    std::string current_number;
    bool number_present=false;

    for(uint i_=0; i_<input_string.length(); i_++)
    {
        for(uint i=0+offset; i<=input_string.length(); i++)
        {
            if(i == input_string.length())
            {
                goto skip;
            }
            if((input_string[i] >=48 && input_string[i]<=57) || input_string[i] == 46)
            {
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
        current_number = "";
    }
    return numbers;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double calculation(std::string &operators, std::vector<double> &numbers)
{
    double result{};
    double result_of_previous{};
    if(operators.length()>=numbers.size()) goto malformed_expression;
    if(!operators.length()) goto malformed_expression;
    sort_equation(operators, numbers);

    for(uint i=0; i<operators.size(); i++)
    {
        operator_names_t selected_operator;
        selected_operator = select_operator(operators[i]);
        if(i==0)
        {
            result_of_previous = evaluate_two_numbers(numbers[0], numbers[1], selected_operator);
        }
        else result_of_previous = evaluate_two_numbers(result_of_previous, numbers[i+1], selected_operator);
    }
    result = result_of_previous;
    return result;
    malformed_expression:
    std::cout << "\nYou entered a malformed expression.\n";
    error=true;
    return 0.0f;


}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double add(const double a, const double b)
{
    return a+b;
}
double subtract(const double a, const double b)
{
    return a-b;
}
double multiply(const double a, const double b)
{
    return a*b;
}
double divide(const double a, const double b)
{
    return a/b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

operator_names_t select_operator(const char &input)
{
    operator_names_t selected_operator=ADD;
        if(input == '+')
        {
            selected_operator = ADD;
        }
        if(input == '-')
        {
            selected_operator=SUBTRACT;
        }
        else if(input == '*')
        {
            selected_operator= MULTIPLY;
        }
        else if(input == '/')
        {
            selected_operator= DIVIDE;
        }
    return selected_operator;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double evaluate_two_numbers(const double &input_a, const double &input_b, const operator_names_t &selected_operator)
{
    double result{};

    switch (selected_operator)
    {
        case ADD: {result=add(input_a, input_b); break;}
        case SUBTRACT: {result=subtract(input_a, input_b); break;}
        case MULTIPLY: {result=multiply(input_a, input_b); break;}
        case DIVIDE: {result=divide(input_a, input_b); break;}
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sort_equation(std::string &input, std::vector<double> &numbers)
{
    char swapOperator;
    double swapNumber;
    for(int i_=0; i_<input.length(); i_++)
    {
        for(int i=1; i<=input.length(); i++)
        {
            if((input[i]== '*' || input[i]=='/') && (input[i-1]=='+' || input[i-1]=='-'))
            {
                swapOperator = input[i];
                swapNumber = numbers[i+1];

                numbers[i+1] = numbers[i];
                input[i] = input[i-1];

                numbers[i] = swapNumber;
                input[i-1] = swapOperator;
            }
        }
    }
    return;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
