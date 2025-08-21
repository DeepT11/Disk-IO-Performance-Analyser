#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <thread>
#include <cstring>
#include <random>

using namespace std;
using namespace std::chrono;

const string TEST_FILE = "test_file.bin";
const size_t FILE_SIZE_MB = 100; // Size of test file in MB
const size_t FILE_SIZE = FILE_SIZE_MB * 1024 * 1024;

// Function to generate random data buffer
vector<char> generate_data(size_t size)
{
    vector<char> buffer(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    for (size_t i = 0; i < size; i++)
    {
        buffer[i] = static_cast<char>(dis(gen));
    }
    return buffer;
}

// Write test
void write_test(size_t block_size)
{
    auto buffer = generate_data(block_size);

    ofstream file(TEST_FILE, ios::binary | ios::trunc);
    if (!file)
    {
        cerr << "Error opening file for write\n";
        return;
    }

    auto start = high_resolution_clock::now();
    for (size_t written = 0; written < FILE_SIZE; written += block_size)
    {
        file.write(buffer.data(), block_size);
    }
    file.close();
    auto end = high_resolution_clock::now();

    double seconds = duration<double>(end - start).count();
    double throughput = (FILE_SIZE_MB / seconds);

    cout << "[WRITE] Block size: " << block_size / 1024
         << " KB, Time: " << seconds << "s, Speed: "
         << throughput << " MB/s\n";
}

// Read test
void read_test(size_t block_size)
{
    vector<char> buffer(block_size);

    ifstream file(TEST_FILE, ios::binary);
    if (!file)
    {
        cerr << "Error opening file for read\n";
        return;
    }

    auto start = high_resolution_clock::now();
    while (file.read(buffer.data(), block_size))
    {
        // simulate usage of data
        volatile char x = buffer[0];
        (void)x;
    }
    file.close();
    auto end = high_resolution_clock::now();

    double seconds = duration<double>(end - start).count();
    double throughput = (FILE_SIZE_MB / seconds);

    cout << "[READ ] Block size: " << block_size / 1024
         << " KB, Time: " << seconds << "s, Speed: "
         << throughput << " MB/s\n";
}

int main()
{
    vector<size_t> block_sizes = {4 * 1024, 64 * 1024, 1024 * 1024}; // 4KB, 64KB, 1MB

    cout << "Disk I/O Performance Analyzer\n";
    cout << "File size: " << FILE_SIZE_MB << " MB\n\n";

    for (auto bs : block_sizes)
    {
        write_test(bs);
        read_test(bs);
        cout << "-----------------------------\n";
    }

    return 0;
}
