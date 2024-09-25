#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 替换空格
string replaceSpace(string s){
    int count = 0, len = s.size();
    // 统计空格数量
    for(char c:s){
        if(c == ' ') count++;
    }

    // 修改s的长度
    s.resize(len + 2*count); // %20是两个字符
    // 倒序遍历修改
    for(int i = len - 1, j = s.size() - 1; i < j; i--, j--) {
        if (s[i] != ' ')
            s[j] = s[i];
        else {
            s[j--] = '0';
            s[j--] = '2';
            s[j] = '%';
        }
    }
    return s;
}

int main(){

    string s = "we happy";
    cout<<s.size()<<endl;
    string s1 = "we%20happy";
    cout<<s1.size()<<endl;
    return 0;
}