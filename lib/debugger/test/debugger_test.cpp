#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// from src/debugger.cpp
vector<string> split(const string& s);

void test_split(const string& input, const vector<string>& output) {
    cout << "Testing: " << input << endl;
    auto result = split(input);
    assert(result.size() == output.size());
    assert(equal(result.begin(), result.end(), output.begin()));
}

int main() {
    test_split("this", {"this"});
    test_split("  this", {"this"});
    test_split("  this  ", {"this"});
    test_split("", {});
    test_split(" ", {});
    test_split("  ", {});
    test_split("this is a test", {"this", "is", "a", "test"});
    test_split("this  is   a test", {"this", "is", "a", "test"});
    test_split("  this is a test ", {"this", "is", "a", "test"});
    return 0;
}