#include<iostream>
#include<vector>

using namespace std;

// Tmd 冒泡排序每次都是最后一个先确定下来 
// 注意第一层for循环是小于size，方便记忆
// j<size-i-1，随着i的增大，j往后移动的次数变少了，因为后面的数据已经排好了
void bubbleSort(vector<int> &datas)
{
    size_t size = datas.size();
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size - i - 1; ++j)
        {
            if (datas[j] > datas[j + 1])
            {
                swap(datas[j + 1], datas[j]);
            }
        }
    }
}

// 选择排序 不稳定
// 固定selectIndex is i, j is from i+1 to size, if selectIndex data > j data, change index = j, last swap data index and data i
void selectSort(vector<int>&datas){
    size_t size = datas.size();
    for(int i = 0; i<size; i++){
        int selectIndex = i;
        for(int j = i+1; j<size;j++){
            if(datas[j] < datas[selectIndex]){
                selectIndex = j;
            }
        }
        swap(datas[selectIndex],datas[i]);
    }
}

// 插入排序
// 插入排序比较特殊的是只有一个外层的for循环，从i=1开始，里面是一个while循环。
void insertSort(vector<int>& datas){
    size_t size = datas.size();
    for(int i = 1; i<size; ++i){
        if(datas[i]<datas[i-1]){
            int j = i-1;
            int x = datas[i];
            while(x < datas[j] && j>=0){
                datas[j+1] =datas[j];
                j--;
            }
            datas[j+1] = x;
        }
    }
}

// 快速排序 不稳定
void quickSort(vector<int>&datas, int begin, int end){
    if(begin>=end) return;
    int low = begin;
    int high = end;
    int key = datas[low];

    while(low<high){
        while(low<high && datas[high] >= key){
            high--;
        }
        if(low <high){
            datas[low++] = datas[high];
        }
        while (low<high && datas[low]<=key)
        {
            low++;
        }
        if(low<high){
            datas[high--] = datas[low];
        }
        
    }

    datas[low] = key;
    quickSort(datas,begin,low-1);
    quickSort(datas,low+1,end);
}

// 归并排序 稳定的
void mergeSortCore(vector<int>& data, vector<int>& dataTemp, int begin, int end) {

	if (begin >= end) return;
	int len = end - begin, mid = begin + len / 2;
	int begin1 = begin, end1 = mid, begin2 = mid + 1, end2 = end;
	mergeSortCore(data, dataTemp, begin1, end1);
	mergeSortCore(data, dataTemp, begin2, end2);
	int index = begin;
	while (begin1 <= end1 && begin2 <= end2) {
		dataTemp[index++] = data[begin1] < data[begin2] ? data[begin1++] : data[begin2++];
	}

	while (begin1 <= end1) {
		dataTemp[index++] = data[begin1++];
	}

	while (begin2 <= end2) {
		dataTemp[index++] = data[begin2++];
	}

	for (index = begin; index <= end; ++index) {
		data[index] = dataTemp[index];
	}
}

void mergeSort(vector<int>& data) {
	int len = data.size();
	vector<int> dataTemp(len, 0);
	mergeSortCore(data, dataTemp, 0, len - 1);
}

// 二分查找
int binarySearch(vector<int> &datas, int target){
    int left = 0;
    int right = datas.size()-1;
    while(left<=right){
        int mid = left + (right-left)/2;
        if(datas[mid] == target) return mid;
        else if(datas[mid] > target) left = mid+1;
        else right = mid -1;
    }
    return -1;
}

int main()
{
    vector<int> data;
    data.push_back(1);
    data.push_back(4);
    data.push_back(3);
    data.push_back(2);
    data.push_back(9);
    data.push_back(6);
    data.push_back(8);
    data.push_back(7);
    data.push_back(5);
    // bubbleSort(data);
    // selectSort(data);
    // quickSort(data,0,data.size()-1);
    
    mergeSort(data);
    for (int i = 0; i < data.size(); i++)
    {
        cout << data[i] << " ";
    }
    cout << endl;

    int res = binarySearch(data,5);
    cout << res << endl;
    return 0;
}