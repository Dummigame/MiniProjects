#include <iostream>
#include <vector>

std::vector<int> sort(const std::vector<int> &unsorted);

int main()
{
    std::vector<int> list{16,34,64,50,48,40,95,51,57,63,35,95,71,17,96,81,48,66,81,10,73,96,89,18,50,57,42,85,75,87,60,82,25,19,19,39,56,30,59,72,72,48,51,61,21,15,86,78,73,98,13,59,70,14,96,70,92,89,15,19,41,58,26,43,85,52,70,52,42,9,6,100,14,82,68,69,73,5,91,13,4,60,45,12,40,20,2,49,62,57,23,78,56,13,36,51,56,11,48,10,67,5,99,69,39,5,91,45,58,63,62,84,16,89,81,82,61,99,93,2,64,19,76,82,76,70,12,91,11,31,93,86,31,63,78,45,2,77,21,94,23,14,41,92,66,45,19,88,91,55,17,49,44,75,35,86,75,97,41,6,73,89,25,90,69,7,34,5,83,22,70,7,3,21,47,3,5,92,62,43,40,77,74,36,73,19,91,57,87,25,88,29,52,89,51,33,74,89,94,99};
    std::vector<int> sorted=sort(list);
    for(uint i{}; i<sorted.size(); i++) std::cout<<sorted.at(i)<<'\n';


    return 0;
}

std::vector<int> sort(const std::vector<int> &unsorted)
{
    std::vector<int> left{};
    std::vector<int> right{};
    if(1>=unsorted.size()) return unsorted;
    for(uint i{}; i<unsorted.size(); i++)
    {
        if(i>=unsorted.size()/2) right.push_back(unsorted.at(i));
        else left.push_back(unsorted.at(i));
    }

    if(left.size()>=1)
    {
        left = sort(left);
        right = sort(right);
    }
    std::vector<int> sorted{};

    int offsetLeft{};
    int offsetRight{};
    for(uint j{}; j<left.size()+offsetLeft || j<right.size()+offsetRight; j++)
    {
        if(j>=left.size()+offsetLeft)
        {
            sorted.push_back(right.at(j-offsetRight)); continue;
        }
        else
        {
            if(j>=right.size()+offsetRight)
            {
                sorted.push_back(left.at(j-offsetLeft)); continue;
            }
        }
        if(left.at(j-offsetLeft)<right.at(j-offsetRight))
        {
            sorted.push_back(left.at(j-offsetLeft));
            offsetRight++;
        }
        else
        {
            sorted.push_back(right.at(j-offsetRight));
            offsetLeft++;
        }
    }
    return sorted;
}