#include<iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

/*
重建二叉树：给出中序和先序数据，构造出二叉树，并返回根节点
先序：中左右
中序：左中右
递归是肯定的
*/ 
TreeNode* deduceTree(vector<int>& preorder, vector<int>& inorder) {
    if(preorder.size()==0 || inorder.size()==0) return nullptr;
    TreeNode* root = new TreeNode(preorder[0]);
    int mid = 0;
    for(auto i : inorder){
        if( i == preorder[0]) break;
        else mid++;
    }
    // int mid = distance(inorder.begin(),find(inorder.begin(),inorder.end(),preorder[0]));
    vector<int> left_pre;
    vector<int> right_pre;

    vector<int> left_in;
    vector<int> right_in;

    // 先序是从1开始的
    for(int i =1; i< preorder.size(); i++){
        if(i<=mid){
            left_pre.push_back(preorder[i]);
        }else{
            right_pre.push_back(preorder[i]);
        }
    }
    // 中序是从0开始的，都是<=mid
    for(int i=0;i< inorder.size(); ++i){
        if(i<=mid) {
            left_in.push_back(inorder[i]);
        }else{
            right_in.push_back(inorder[i]);
        }
    }

    root->left = deduceTree(left_pre,left_in);
    root->right = deduceTree(right_pre,right_in);
    return root;
}




