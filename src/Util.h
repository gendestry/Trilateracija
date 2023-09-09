#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <algorithm>


template <typename... Args>
const char* concat(Args ... args) {
    std::stringstream ss;
    (ss << ... << args);
    return ss.str().c_str();
}


typedef std::array<float, 3> DataPoint;

template<unsigned int I>
class CircularBuffer {
private:
    std::array<DataPoint, I> m_Buffer{{0.0f, 0.0f, 0.0f}};
    unsigned int m_Index = 0;
public:
    // pushes new datapoint and returns the overwritten one
    DataPoint insert(DataPoint data) {
        DataPoint overwritten = m_Buffer[m_Index];
        m_Buffer[m_Index] = data;
        m_Index = (m_Index + 1) % I;
        return overwritten;
    }

    // returns the datapoint at the given index
    DataPoint& operator[] (unsigned int index) {
        return m_Buffer[index % I];
    }
};


class DataManipulator {
private:
    constexpr static unsigned int m_Size = 50;
    unsigned int m_Count = 0;
    
    CircularBuffer<m_Size> m_Buffer;
    DataPoint m_Accumulator{0.0f};
public:
    // pushes new datapoint
    void push(DataPoint data) {
        if (*std::max_element(data.begin(), data.end()) > 4000.0f)
            return;

        DataPoint ret = m_Buffer.insert(data);

        for(int i = 0; i < 3; i++) {
            m_Accumulator[i] += data[i];    // add new values
            m_Accumulator[i] -= ret[i];     // subtract overwritten values
        }   

        m_Count++;
    }

    // gets the average
    DataPoint getAverage() {
        // pazimo da ne delimo z 0
        unsigned int divisor = std::min(m_Size, std::max(m_Count, (unsigned)1));
        DataPoint average{0.0f};

        for(int i = 0; i < 3; i++)
            average[i] = m_Accumulator[i] / divisor;

        return average;
    }

    // gets the standard deviation
    DataPoint getStandardDeviation() {
        DataPoint average = getAverage();
        DataPoint deviation{0.0f};

        // return zero vector if no data is available
        if(m_Count == 0)
            return deviation;

        unsigned int max = std::min(m_Size, m_Count);

        for(int i = 0; i < max; i++) {
            for(int j = 0; j < 3; j++) {
                deviation[j] += (m_Buffer[i][j] - average[j]) * (m_Buffer[i][j] - average[j]);
            }
        }

        for(int i = 0; i < 3; i++)
            deviation[i] = sqrt(deviation[i] / max);

        return deviation;
    }
};