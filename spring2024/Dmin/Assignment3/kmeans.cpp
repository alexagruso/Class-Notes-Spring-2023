#include <cmath>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>

const uint MAX_ITERATIONS = 100;
const std::string OUTPUT_FILE = "output.txt";

template <typename T> using pair = std::pair<T, T>;

struct DataEntry
{
    pair<int> entry;
    int cluster;
};

std::vector<pair<int> > getCluster(std::vector<DataEntry> data, int cluster)
{
    std::vector<pair<int> > result;

    for (const DataEntry& entry : data)
    {
        if (entry.cluster == cluster)
        {
            result.push_back(entry.entry);
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
    int clusters = std::stoi(args[1]);

    std::ifstream dataFile{args[2]};

    std::vector<DataEntry> data;

    int position = 0;
    std::string line;

    // read data from file
    while (std::getline(dataFile, line))
    {
        data.push_back({});
        sscanf(line.c_str(), "%d %d", &data.at(position).entry.first, &data.at(position).entry.second);
        position++;
    }

    dataFile.close();

    // partition data evenly
    for (int i = 0; i < data.size(); i++)
    {
        for (int c = 1; c <= clusters; c++)
        {
            if (i < (c * data.size()) / static_cast<double>(clusters))
            {
                data.at(i).cluster = c;
                break;
            }
        }
    }

    std::vector<pair<double> > centroids;
    centroids.resize(clusters);

    int iterations = 0;
    bool changed;

    do
    {
        changed = false;

        for (int c = 1; c <= clusters; c++)
        {
            centroids.at(c - 1) = centroid(getCluster(data, c));
        }

        for (int i = 0; i < data.size(); i++)
        {
            int oldCluster = data.at(i).cluster;

            int closestCluster = 1;
            double closestDistance = euclidean(data.at(i).entry, centroids.at(0));

            for (int c = 2; c <= clusters; c++)
            {
                double distance = euclidean(data.at(i).entry, centroids.at(c - 1));

                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    closestCluster = c;
                }
            }

            data.at(i).cluster = closestCluster;
            changed = oldCluster != closestCluster;
        }

        iterations++;
    } while (changed && iterations <= MAX_ITERATIONS);

    std::ofstream dataOutput{OUTPUT_FILE};

    for (int i = 0; i < data.size(); i++)
    {
        dataOutput << data.at(i).entry.first << " " << data.at(i).entry.second << " " << data.at(i).cluster << '\n';
    }

    dataOutput.close();

    std::cout << "Evaluated in " << iterations << " iteration(s)." << '\n';

    return 0;
}