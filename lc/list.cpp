#include <iostream>
#include <vector>
using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

// 倒序打印单链表
void reversePrint(ListNode* head){
    if(head==nullptr) return;
    if(head->next == nullptr) {
        cout<<head->val<<" ";
        return;
    }
    reversePrint(head->next);
    cout<<head->val<<" ";
}

// 反转链表
ListNode* reveseList(ListNode* head){
    if(head==nullptr || head->next == nullptr)
            return head;
    ListNode* pre = nullptr;
    ListNode* cur = head;
    while(cur){
        ListNode* next = cur->next;     
        cur->next = pre;
        pre = cur;
        cur = next; 
    }
    return pre; 
}

int main(){
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(4);
    head->next->next->next->next = new ListNode(5);

    // 打印链表
    ListNode* current = head;
    while (current != nullptr) {
        std::cout << current->val << " ";
        current = current->next;
    }
    std::cout << std::endl;

    // reversePrint(head);

    ListNode* head2 = reveseList(head);
    current = head2;
    while (current != nullptr) {
        std::cout << current->val << " ";
        current = current->next;
    }
    std::cout << std::endl;

    // 释放链表节点的内存
    current = head;
    while (current != nullptr) {
        ListNode* temp = current;
        current = current->next;
        delete temp;
    }

    return 0;
}

