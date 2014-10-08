#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

void selectionSort(vector<int>&);
bool compare(int, int);
void mySwap(int&, int&);
void printArr(vector<int>&);


int main()
{
	vector<int> arr;
	int n_elems;
	int input;

	cout << "How many numbers? ";
	cin >> n_elems;

	for (int i = 0; i < n_elems; ++i)
	{
		cin >> input;
		arr.push_back(input);
	}

	cout << "Before sort:" << endl;
	printArr(arr);

	selectionSort(arr);

	cout << "After sort:" << endl;
	printArr(arr);


	return 0;
}

void selectionSort(vector<int>& array)
{
	for (size_t i = 0, n = array.size(); i < n - 1; ++i)
	{
		size_t pivot = i;
		for (size_t j = i+1; j < n; ++j)
		{
			if (!compare(array[pivot], array[j])) pivot = j;
		}
		if (pivot != i)
			mySwap(array[pivot], array[i]);
	}
}

bool compare(int a, int b)
{
	return a < b;
}

void mySwap(int& a, int& b)
{
	int c = a;
	a = b;
	b = c;
}

void printArr(vector<int>& arr)
{
	stringstream ss;
	for (size_t i = 0; i < arr.size(); ++i)
	{
		if (i != 0)
		{
			ss << " ";
		}
		ss << arr[i];
		
	}

	cout << ss.str() << endl;
}
