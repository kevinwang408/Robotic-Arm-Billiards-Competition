#include <cmath>
#include <limits>
#include <iostream>
#include <vector>
int main(){
    std::vector<std::pair<std::vector<double>, std::vector<double>>> result;
    result.emplace_back(std::vector<double>{1.0, 2.0}, std::vector<double>{3.0, 4.0});
    result.emplace_back(std::vector<double>{5.0, 6.0}, std::vector<double>{7.0, 8.0});
    return 0;
}
