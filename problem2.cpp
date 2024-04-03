// David Huynh
// COP 4520, Spring 2024

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <random>
#include <algorithm>

#define THREADS 8
#define TIMEINMILLISECONDS 50

class TemperatureReadingModule
{
private:
    std::shared_mutex mtx;
    std::vector<int> data;
    int timer;
    bool scanned;

public:
    TemperatureReadingModule() : timer(0), scanned(true) {}

    // Generates a random temperature for the scan.
    void scanTemperature(int tNum)
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(-100, 70);

        std::unique_lock lock(mtx);
        int n = dist(mt);
        
        std::cout << "> Sensor " << tNum << " got " << n << "F.\n";
        
        data.push_back(n);
        scanned = true;
    }

    // Checks if a thread is allowed to take a temperature scan.
    bool scanAvailable(int n)
    {
        std::shared_lock lock(mtx);
        // Allows only 1 scan from a specific thread.
        if (scanned == false && timer <= 60 && timer % 8 == n)
        {
            return true;
        }

        return false;
    }

    // Allows the main thread to act as a timer. Everytime it increments, a new scan is allowed.
    void incrementTimer()
    {
        std::unique_lock lock(mtx);
        timer++;
        scanned = false;
        std::cout << "Minute " << timer << "\n";
    }

    // Allows the threads to check the timer.
    int getTimer()
    {
        std::shared_lock lock(mtx);
        return timer;
    }

    // Prints the report after the temperature readings.
    void compileReport()
    {
        std::cout << "\n******\n";
        std::cout << "REPORT\n";
        std::cout << "******\n";

        std::vector<int> sortedData = data;
        std::sort(sortedData.begin(), sortedData.end());

        std::cout << "Top 5 Highest Temperatures:\n";
        int j = sortedData.size() - 1;

        for (auto i = 0; i < 5; i++)
        {
            std::cout << sortedData[j--] << "F\n";
        }

        std::cout << "\nTop 5 Lowest Temperatures:\n";
        for (int i = 0; i < 5; i++)
        {
            std::cout << sortedData[i] << "F\n";
        }

        std::vector<int> interval;
        int maxDiff = 0;
        int lowMin = 0;
        int high = 0;
        int low = 0;

        // Check each interval for the largest difference.
        for (int i = 0; i < 51; i++)
        {
            std::vector<int> unsorted;

            for (int j = i; j < i+10; j++)
            {
                unsorted.push_back(data[j]);
            }

            std::vector<int> sorted = unsorted;
            std::sort(sorted.begin(), sorted.end());
            int n = sorted[9] - sorted[0];

            if (n > maxDiff)
            {
                maxDiff = n;
                lowMin = i;
                interval = unsorted;
                high = sorted[9];
                low = sorted[0];
            }
        }

        std::cout << "\n10-minute interval of largest temperature difference (inclusive):\n";
        std::cout << "> Minute " << lowMin + 1 << " to " << lowMin + 10 << "\n";
        std::cout << "> Highest: " << high << "F\n";
        std::cout << "> Lowest: " << low << "F\n";
        std::cout << "> Difference: " << high - low << "F\n";
        std::cout << "\nInterval:\n";

        for (auto i : interval)
        {
            std::cout << i << "F ";
        }
        std::cout << "\n";
    }
};

// Allows each scanner to take a temperature reading.
void temperatureSensor(int tNum, TemperatureReadingModule &t)
{
    // Loop until we go over an hour.
    while (t.getTimer() <= 60)
    {
        // Each thread checks if it is allowed to scan.
        if (t.scanAvailable(tNum))
        {
            t.scanTemperature(tNum);
        }
    }
}

// Main function that spawns 8 threads (sensors).
int main(void)
{
    TemperatureReadingModule t;

    std::thread t1(temperatureSensor, 0, std::ref(t));
    std::thread t2(temperatureSensor, 1, std::ref(t));
    std::thread t3(temperatureSensor, 2, std::ref(t));
    std::thread t4(temperatureSensor, 3, std::ref(t));
    std::thread t5(temperatureSensor, 4, std::ref(t));
    std::thread t6(temperatureSensor, 5, std::ref(t));
    std::thread t7(temperatureSensor, 6, std::ref(t));
    std::thread t8(temperatureSensor, 7, std::ref(t));

    // Main thread acts as a timer for the other threads.
    for (int i = 0; i <= 60; i++)
    {
        t.incrementTimer();
        std::this_thread::sleep_for (std::chrono::milliseconds(TIMEINMILLISECONDS));
    }
    
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();

    t.compileReport();

    return 0;
}
