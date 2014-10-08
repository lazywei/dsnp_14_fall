#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

template <class T>
class Compare
{
public:
	virtual bool operator() (T x, T y) const = 0;
};

class Less : public Compare<string>
{
public:
	bool operator() (string x, string y) const
	{
		return x < y;
	}
};

class Greater : public Compare<double>
{
public:
	bool operator() (double x, double y) const
	{
		return x > y;
	}
};

template <class T>
void selectionSort(vector<T>&, const Compare<T>&);

template <class T>
void printArr(vector<T>&);

template <class T>
void mySwap(T&, T&);

int main()
{
	vector<string> str_arr;
	vector<double> dbl_arr;

	int n_elems;

	double dbl;
	string str;

	cout << "How many strings? ";
	cin >> n_elems;

	for (int i = 0; i < n_elems; ++i)
	{
		cin >> str;
		str_arr.push_back(str);
	}

	cout << "Before sort:" << endl;
	printArr(str_arr);

	cout << "Ascending sort:" << endl;
	selectionSort(str_arr, Less());
	printArr(str_arr);

	cout << "How many doubles? ";
	cin >> n_elems;

	for (int i = 0; i < n_elems; ++i)
	{
		cin >> dbl;
		dbl_arr.push_back(dbl);
	}

	cout << "Before sort:" << endl;
	printArr(dbl_arr);

	cout << "Descending sort:" << endl;
	selectionSort(dbl_arr, Greater());
	printArr(dbl_arr);


	return 0;
}

template <class T>
void selectionSort(vector<T>& array, const Compare<T>& compare)
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

template <class T>
void mySwap(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
}

template <class T>
void printArr(vector<T>& arr)
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
