#include <cmath>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

const uint MAX_ITERATIONS = 100;

template <typename T> using pair = std::pair<T, T>;

struct DataEntry
{
    pair<int> data;
    int partition;
};

std::vector<pair<int> > getCluster(std::vector<DataEntry> data, int cluster)
{
    std::vector<pair<int> > result;

    for (const DataEntry& entry : data)
    {
        if (entry.partition == cluster)
        {
            result.push_back(entry.data);
        }
    }

    return result;
}

pair<double> centroid(std::vector<pair<int> > values)
{
    int xSum = 0;
    int ySum = 0;

    int n = values.size();

    for (int i = 0; i < n; i++)
    {
        xSum += values.at(i).first;
        ySum += values.at(i).second;
    }

    return {xSum / (double) n, ySum / (double) n};
}

double euclidean(pair<double> p1, pair<double> p2)
{
    return std::sqrt((p2.first - p1.first) * (p2.first - p1.first) + (p2.second - p1.second) * (p2.second - p1.second));
}

int main(int argc, char** args)
{
    int partitions = std::stoi(args[1]);

    std::ifstream dataFile{args[2]};

    std::vector<DataEntry> data;

    int position = 0;
    std::string line;

    // read data from file
    while (std::getline(dataFile, line))
    {
        data.push_back({});
        sscanf(line.c_str(), "%d %d", &data.at(position).data.first, &data.at(position).data.second);
        position++;
    }

    // partition data evenly
    for (int i = 0; i < data.size(); i++)
    {
        for (int c = 1; c <= partitions; c++)
        {
            if (i < (c * data.size()) / static_cast<double>(partitions))
            {
                data.at(i).partition = c;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ITERATIONS; i++) {}

    dataFile.close();
}