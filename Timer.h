#include <chrono>

class Timer
{
public:
    Timer()
        :m_StartTime(std::chrono::high_resolution_clock::now())
    {}

    Timer(const Timer&) = delete;
    Timer(Timer&& ) = delete;

    ~Timer() = default;

    void Reset(void)
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
    }

    std::chrono::duration<double> GetTimeEllapsed(void) const
    {
        return std::chrono::high_resolution_clock::now() - m_StartTime;
    }
private:
    std::chrono::high_resolution_clock::time_point m_StartTime;
};
