#include <iostream>
#include <vector>
using namespace std;


void printVectors(vector<int> & nums){
    for(auto a:nums){
        cout<< a << " ";
    }
    cout<<endl;
}

// 找出数组中所有的重复数据
// 给你一个长度为n的整数数组nums ，其中nums的所有整数都在范围[1, n]内，且每个整数出现一次或两次 。请你找出所有出现两次的整数，并以数组形式返回。
// 这个添加负号的做法，不是按照数据的固定位置进行顺序比较的，是跳动的
vector<int> findDuplicates(vector<int>& nums){
    vector<int> res;
    // 1 3 5 6 3 2 5
    for(int i = 0; i<nums.size(); ++i){
        int x = abs(nums[i]);
        if(nums[x-1] >0){ // 因为这些数据都是在1到n的范围内，如果用nums[x] > 0 进行判断的话，有可能会出现越界情况。
            nums[x-1] = -nums[x-1];
        }else{
            res.push_back(x);
        }

        printVectors(nums);
    }
    printVectors(res);
    return res;
}

// 二维数组的查找
// 判断数组中是否存在目标值。矩阵是行主序的，matrix.size()是矩阵的行数。
bool searchMatrix(vector<vector<int>>& matrix, int target){
    int m = matrix.size();
    int n = matrix.at(0).size(); // 列数
    int index_m = 0;
    int index_n = n-1;
    while(index_m < m && index_n >=0){
        if(matrix[index_m][index_n] == target) return true;
        else if(matrix[index_m][index_n] > target) index_n--;
        else index_m++;
    }
    return false;
}



int main(){

    vector<int> data;
    data.push_back(1);
    data.push_back(3);
    data.push_back(5);
    data.push_back(6);
    data.push_back(3);
    data.push_back(2);
    data.push_back(5);

    findDuplicates(data);
    return 0;
}