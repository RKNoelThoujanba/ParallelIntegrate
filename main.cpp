#include <iostream>
#include <string>
#include <cstring>

#include <functional>
#include <future>

#include <memory>

#include <cstdint>


#include "Timer.h"

double Integrate(std::function<double(double)> f, double lowerLimit, double upperLimit)
{
    double a{}, b{};
    int8_t sign{};
    constexpr double dx = 1e-5;

    if (lowerLimit > upperLimit)
    {
        a = upperLimit;
        b = lowerLimit;
        sign = -1;
    }
    else if(lowerLimit < upperLimit)
    {
        a = lowerLimit;
        b = upperLimit;
        sign = +1;
    }
    else
    {
        return 0.0;
    }

    double sum = 0.0;
    for(double x = a; x < b; x += dx)
    {
        sum += (f(x) + f(x + dx)) * dx * 0.5;
    }

    return sign * sum;
}

int main(int argc, char** argv)
{
    std::launch policy = std::launch::deferred;
    uint32_t threadCount = 1;

    

    if(argc > 1)
    {
        constexpr uint32_t i = 1;
        if(strcmp(argv[i], "--launch") == 0)
        {
            if(i + 1 >= argc)
            {
                std::cout << "Invalid args. Provide launch policy if --launch is specified.\n";
                return 1;
            }
            if(strcmp(argv[i + 1], "deferred") == 0)
            {
                std::cout << "Launch Polify = defered.\n";
                //launch policy is already initialized to deferred
                if(i + 2 < argc)
                {
                    std::cout << "Excess Args supplied. Panic Crashing.\n";
                    return 2;
                }
            }
            else if(strcmp(argv[i + 1], "async") == 0)
            {
                std::cout << "Launch Polify = async\n";
                policy = std::launch::async;
                if(i + 2 >= argc)
                {
                    std::cout << "Invalid args. Provide thread count if --launch async is specified.\n";
                    return 3;
                }

                bool fucked;
                
                threadCount = 
                    [] (const char* str, bool& isFucked) -> uint32_t {
                        size_t len = strlen(str);
                        uint32_t num = 0;
                        uint32_t place = 1;
                        for(int64_t i = len - 1; i >= 0; --i)
                        {
                            if(!(str[i] >= '0' && str[i] <= '9'))
                            {
                                isFucked = true;
                                return 0;
                            }
                            num += (str[i] - '0') * place;
                            place *= 10;
                        }

                        isFucked = false;
                        return num;
                    }(argv[i + 2], fucked);
                
                if(fucked || threadCount == 0)
                {
                    std::cout << "ThreadCount must be a non negative integer\n";
                    return 4;
                }
                if(i + 3 < argc)
                {
                    std::cout << "Excess Args supplied. Panic Crashing.\n";
                    return 5;
                }

            }
            else
            {
                std::cout << "Invalid Launch Policy.\n";
                return 6;
            }
        }
        else
        {
            std::cout << "Invalid Args.\n";
            return 7;
        }
    }

    std::cout << "Threads Spawned: " << threadCount << '\n';

    double lowerLimit, upperLimit;
    std::cout << "Lower Limit and Upper Limit: ";
    std::cin >> lowerLimit >> upperLimit;

    Timer timer;

    std::unique_ptr<std::future<double>[]> tasks = std::make_unique<std::future<double>[]>(threadCount);

    const double rangeSize = (upperLimit - lowerLimit) / threadCount;

    for(uint32_t i = 0; i < threadCount; ++i)
    {
        tasks[i] = std::async(policy, Integrate, [] (double x) { return 1.0 / x;}, lowerLimit + rangeSize * i , lowerLimit + rangeSize * (i + 1));
    }

    double integ = 0.0;
    for(uint32_t i = 0; i < threadCount; ++i)
        integ += tasks[i].get();

    std::cout << "Integral = " << integ << "\n";
    

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timer.GetTimeEllapsed());
    std::cout << "Time Ellapsed: " << duration.count() << " ms. \n";
}