#include <string>
#include <vector>
#include <map>

template<typename T>
void test(std::vector<std::string> testVar)
{
    std::map<std::string, std::vector<T> > test; 

    std::vector<int> intis; 
    intis.push_back(12);
    std::vector<std::string> strings;
    strings.push_back("test");

    test["intis"] = intis;
    test["strings"] = strings;
}
int main ()
{
    std::vector<std::string> testVec;
    test<std::string>(testVec);
}