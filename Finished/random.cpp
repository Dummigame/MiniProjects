#include <cstdint>
#include <random>
#include <iostream>
#include <string>
#include <type_traits>
#include <fstream>
#include <filesystem>


template<typename T> T toNumber(std::string inputString)
{
    if(std::is_same<T, unsigned int>::value)
    {
        for(uint i{}; i<inputString.length(); i++)
        {
            if(inputString.at(i)<'0' || inputString.at(i)>'9')
            {
                inputString.erase(i);
                i--;
            }
        }
        
        for(uint i{'0'}; i<='9'; i++)
        {
            if(inputString.find(i) != std::string::npos) break;
            else if(i=='9')
            {
                inputString.clear();
                inputString.push_back('1');
                std::cout<<"\nInvalid input, defaulted to 1\n";
            }
        }
        

        const T number=std::stoul(inputString);
        return number;
    }
    else
    {
        bool foundDecimal{};
        for(uint i{}; i<inputString.length(); i++)
        {
            if((inputString.at(i)<'0' || inputString.at(i)>'9') && (inputString.at(i) !='.' || (inputString.at(i) == '.' && foundDecimal==true)) && inputString.at(i)!='-')
            {
                if(inputString.at(i=='.' && foundDecimal==false))
                {
                    foundDecimal=true;
                }
                inputString.erase(i);
                
                i--;
            }
        }
        
        for(unsigned char i{'0'}; i<='9'; i++)
        {
            if(inputString.find(i) != std::string::npos) break;
            else if(i=='9')
            {
                inputString.clear();
                inputString.push_back('0');
                inputString.push_back('.');
                inputString.push_back('5');
                std::cout<<"\nInvalid input, defaulted to 0.5\n";
            }
        }
        
        const T number=std::stold(inputString);
        return number;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct args
{
    bool useFloats{};
    std::string separation;
    long double minimum{};
    long double maximum{1};
    uint amount{1};
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool isFloatAsString(const std::string&);
void generateToFile(args args);
void generateToStdout(args args);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    bool useFloats{};
    bool toFile{};

    std::cout<<"\nRandom Numbers at unreasonable speeds!";
    retry:
    std::cout<<"\nSeparated by: ";
    std::string separation;
    std::string inputString;
    std::getline(std::cin, separation);
    if(separation.length()>1)
    {
        std::cout<<"\nMore than one character specified. Is this what you intended? (y/n): ";
        std::cin>>inputString;
        if (inputString.length()>1) for(int i=inputString.length(); i>1; i++)inputString.pop_back();
        if(inputString.at(0)!='y'|| inputString.at(0)!='Y')
        {
            inputString.clear();
            std::cin.ignore();
            goto retry;
        }
    }
    if(separation.length()==0)
    {
        std::cout<<"\nNo separation specified. Is this what you intended? (y/n): ";
        std::cin>>inputString;
        if (inputString.length()>1) for(int i=inputString.length(); i>1; i++)inputString.pop_back();
        if(inputString.at(0)!='y'|| inputString.at(0)!='Y')
        {
            std::cin.ignore();
            inputString.clear();
            goto retry;
        }
    }

    std::cout<<"\nMinimum: ";
    std::cin>>inputString;
    long double minimum{toNumber<long double>(inputString)};
    if(isFloatAsString(inputString)) useFloats=true;
    inputString.clear();

    std::cout<<"\nMaximum: ";
    std::cin>>inputString;
    long double maximum{toNumber<long double>(inputString)};
    if(isFloatAsString(inputString)) useFloats=true;
    inputString.clear();

    if(minimum>=maximum)
    {
        std::cout<<"\nRange is <=0. Exiting\n";
        return 1;
    }
    std::cout<<"\nAmount of random numbers: ";
    std::cin>>inputString;
    for(unsigned long i{}; i<inputString.length(); i++)
    {
        if(inputString.at(i)=='-')
        {
            inputString.erase(i);
            i--;
        }
    }
    if(isFloatAsString(inputString))
    {
        std::cout<<"\nSpecified non-integer as number count. Floored.";
        for(unsigned long i{inputString.find('.')}; i<inputString.length(); i++) inputString.erase(i);
    }
    uint amount=toNumber<unsigned int>(inputString);
    if(amount>1000)
    {
        toFile=true;
        goto writeToFile;
    }
    std::cout<<"\nWrite to file? (y/n): ";
    std::cin>>inputString;
    if (inputString.length()>1) for(int i=inputString.length(); i>1; i++)inputString.pop_back();
    if(inputString.at(0)=='y' || inputString.at(0)=='Y') toFile=true;

    writeToFile:
    args args={useFloats,separation,minimum,maximum,amount};

    if(toFile) generateToFile(args);
    else generateToStdout(args);
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool isFloatAsString(const std::string& numberAsString)
{
    if(numberAsString.find('.') == std::string::npos) return false;
    else return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void generateToFile(args args)
{
    std::random_device randev;
    if(!args.useFloats)
    {
        std::ofstream file{"output.txt"};
        if(!file)
        {
            std::cerr << "File did not open.";
            return;
        }
        std::uniform_int_distribution<int> intDist(args.minimum,args.maximum);
        for(uint i{}; i<args.amount && i<UINT32_MAX; i++)
        {
            if(i>0) file<<args.separation;
            file<<intDist(randev);
        }
        std::cout<<"File saved to " << std::filesystem::current_path() << "\n";
        return;
    }
    else
    {
        std::ofstream file{"output.txt"};
        if(!file)
        {
            std::cerr << "File did not open.";
            return;
        }
        std::uniform_real_distribution<long double> longDoubleDist(args.minimum,args.maximum);
        std::cout<<'\n';
        for(uint i{}; i<args.amount && i<UINT32_MAX; i++)
        {
            if(i>0) file<<args.separation;
            file<<longDoubleDist(randev);
        }
        std::cout<<"File saved to " << std::filesystem::current_path() << "\n";
        return;
    } 
    return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void generateToStdout(args args)
{
    std::random_device randev;
    if(!args.useFloats)
    {
        std::cout<<'\n';
        std::uniform_int_distribution<int> intDist(args.minimum,args.maximum);
        for(uint i{}; i<args.amount && i<UINT32_MAX; i++)
        {
            if(i>0) std::cout<<args.separation;
            std::cout<<intDist(randev);
        }
        std::cout<<'\n';
        return;
    }
    else
    {
        std::uniform_real_distribution<long double> longDoubleDist(args.minimum,args.maximum);
        std::cout<<'\n';
        for(uint i{}; i<args.amount && i<UINT32_MAX; i++)
        {
            if(i>0) std::cout<<args.separation;
            std::cout<<longDoubleDist(randev);
        }
        std::cout<<'\n';
        return;
    }
}

