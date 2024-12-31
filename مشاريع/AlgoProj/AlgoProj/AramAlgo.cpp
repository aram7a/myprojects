#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

void generate_data(vector<int>& data, int size, int case_type) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000000);

    data.clear();
    for (int i = 0; i < size; ++i) {
        data.push_back(dis(gen));
    }

    if (case_type == 2) {  // Sorted data
        sort(data.begin(), data.end());
    }
    else if (case_type == 3) {  // Sorted data in reverse order
        sort(data.begin(), data.end(), greater<int>());
    }
}

void insertion_sort(vector<int>& data) {
    int n = data.size();
    for (int i = 1; i < n; ++i) {
        int key = data[i];
        int j = i - 1;
        while (j >= 0 && data[j] > key) {
            data[j + 1] = data[j];
            j = j - 1;
        }
        data[j + 1] = key;
    }
}

void merge(vector<int>& data, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; ++i)
        L[i] = data[left + i];
    for (int i = 0; i < n2; ++i)
        R[i] = data[mid + 1 + i];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            data[k] = L[i];
            i++;
        }
        else {
            data[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        data[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        data[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(vector<int>& data, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(data, left, mid);
        merge_sort(data, mid + 1, right);
        merge(data, left, mid, right);
    }
}

int partition(vector<int>& data, int low, int high) {
    int pivot = data[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (data[j] < pivot) {
            i++;
            swap(data[i], data[j]);
        }
    }
    swap(data[i + 1], data[high]);
    return (i + 1);
}

void quick_sort(vector<int>& data, int low, int high) {
    while (low < high) {
        int pi = partition(data, low, high);
        if (pi - low < high - pi) {
            quick_sort(data, low, pi - 1);
            low = pi + 1;
        }
        else {
            quick_sort(data, pi + 1, high);
            high = pi - 1;
        }
    }
}

void display_time(const string& algorithm, int size, const string& case_name, long long time) {
    cout << algorithm << " | Size: " << size << " | Case: " << case_name << " | Time(ms): " << time << endl;
}

int main() {
    const int num_sets = 6;
    vector<int> sizes(num_sets);
    vector<string> case_names = { "Random", "Sorted", "Reverse Sorted" };

    for (int i = 0; i < num_sets; ++i) {
        cout << "Enter the size of set #" << (i + 1) << ": ";
        cin >> sizes[i];
    }

    vector<int> data;

    for (int set_num = 1; set_num <= num_sets; ++set_num) {
        int size = sizes[set_num - 1];
        cout << "Processing set #" << set_num << " with size " << size << endl;

        for (int case_type = 1; case_type <= 3; ++case_type) {
            generate_data(data, size, case_type);

            auto start = high_resolution_clock::now();
            insertion_sort(data);
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start).count();
            display_time("Insertion Sort", size, case_names[case_type - 1], duration);

            generate_data(data, size, case_type);
            start = high_resolution_clock::now();
            merge_sort(data, 0, data.size() - 1);
            end = high_resolution_clock::now();
            duration = duration_cast<milliseconds>(end - start).count();
            display_time("Merge Sort", size, case_names[case_type - 1], duration);

            generate_data(data, size, case_type);
            start = high_resolution_clock::now();
            quick_sort(data, 0, data.size() - 1);
            end = high_resolution_clock::now();
            duration = duration_cast<milliseconds>(end - start).count();
            display_time("Quick Sort", size, case_names[case_type - 1], duration);
        }
    }

    return 0;
}
