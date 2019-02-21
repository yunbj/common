#include <iostream>
#include <chrono>
#include <list>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <numeric>
#include <iomanip>
#include "base/Buffer.h"
#include "base/Gcd.h"
#include "base/ConcurrentFixedSizeMemoryPool.h"
#include "base/LogMgr.h"

using namespace grid;


int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "usage: perf_buffer thread_count alloc_count size_count" << std::endl;
        return 1;
    }

    INIT_LOG_MGR(log::LOG_LEVEL::LOG_DEBUG, log::LOG_WRITER_TYPE::LOG_WRITER_ALL, 1);
    
    uint32_t threadCount = std::atoi(argv[1]);
    uint32_t allocCount = std::atoi(argv[2]);
    uint32_t sizeCount = std::atoi(argv[3]);
    
    std::vector<uint32_t> sizes;
    const std::string kData = "abcdefghijklmnopqrstuvwxyz";
    
    std::cout << "* allocation sizes:" << std::endl;
    std::srand(std::time(0));
    for (uint32_t i = 0; i < sizeCount; ++i) {
        const uint32_t maxSize = 1*1024*1024;
        
        auto size = std::rand() % maxSize + kData.length();
        sizes.push_back(size);
        
        std::cout << " " << size;
    }
    
    std::cout << std::endl << std::endl;

    //init
    std::vector<std::shared_ptr<Gcd>> gcds;
    for (uint32_t i = 0; i < threadCount; ++i) {
        auto gcd = std::make_shared<Gcd>();
        if (gcd->Init() != 0) {
            std::cerr << "Gcd::Init() failed" << std::endl;
            return 1;
        }
        gcds.emplace_back(std::move(gcd));
    }
    
    //run
    for (int i = 0; i < 2; ++i) {
        std::string makerName;
        
        std::function<Buffer (uint32_t)> maker;
        if (i == 0) {
            //maker = (Buffer (BufferFactory::*)(uint32_t))(&BufferFactory::MakeFixedSizePoolBuffer);
            makerName = "BufferFactory::MakeFixedSizePoolBuffer";
        } else if (i == 1) {
            //maker = &BufferFactory::MakeDefaultBuffer;
            makerName = "BufferFactory::MakeDefaultBuffer";
        } else if (i == 2) {
            //maker = &BufferFactory::MakePoolBuffer;
            makerName = "BufferFactory::MakePoolBuffer";
        }
        
        std::vector<uint32_t> gcdRunCounts(threadCount, 0);
        std::atomic<uint32_t> gcdDoneCount = 0;
        

        std::cout << "* started(" << makerName << ")" << std::endl;
        const auto started = std::chrono::system_clock::now();
        
        for (uint32_t i = 0; i < threadCount; ++i) {
            auto& gcd = gcds[i];
            auto& runCount = gcdRunCounts[i];
            
            gcd->DispatchAsync([allocCount, maker, kData, sizes, &gcdDoneCount, &runCount]() {
                for (uint32_t i = 0; i < allocCount; ++i) {
                    const auto index = std::rand() % sizes.size();
                    auto buffer = maker(sizes[index]);
                    std::memcpy(buffer.PosToWrite(), kData.c_str(), kData.length());
                    buffer.Written(kData.length());
                    
                    ++runCount;
                }
                ++gcdDoneCount;
            });
        }
        
        bool complete = false;
        while (!complete) {
            for (uint32_t i = 0; i < 1000; ++i) {
                if (gcdDoneCount == threadCount) {
                    complete = true;
                    break;
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            
            //std::cout << ".";
            
            uint32_t sumCounts = std::accumulate(gcdRunCounts.begin(), gcdRunCounts.end(), 0);
            std::cout << "- progress: " << std::setprecision(3) << 100.0f * sumCounts / (allocCount * threadCount) << "%" << std::endl;
        }
        
        if (i == 0) {
            std::cout << "* pool count = " << ConcurrentFixedSizeMemoryPool::Instance().GetPoolCount() << ", size = " << ConcurrentFixedSizeMemoryPool::Instance().GetSize() << std::endl;
        }
        
        const auto ended = std::chrono::system_clock::now();
        std::cout << "* elapsed = " << (ended - started).count() / 1000 / 1000 << " msec" << std::endl << std::endl;
    }

    //fini
    for (auto gcd : gcds) {
        gcd->Fini();
    }

    UNINIT_LOG_MGR();
}
