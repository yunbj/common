#include <iostream>
#include <chrono>
#include <list>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include "base/Buffer.h"
#include "base/Gcd.h"


using namespace grid;


int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "usage: perf_buffer thread_count alloc_count size_count" << std::endl;
        return 1;
    }
    
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
    std::list<std::shared_ptr<Gcd>> gcds;
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
        
        std::function<Buffer (std::size_t)> maker;
        if (i == 0) {
            maker = &BufferFactory::MakeDefaultBuffer;
            makerName = "BufferFactory::MakeDefaultBuffer";
        } else if (i == 1) {
            maker = &BufferFactory::MakeFixedSizePoolBuffer;
            makerName = "BufferFactory::MakeFixedSizePoolBuffer";
        } else if (i == 2) {
            maker = &BufferFactory::MakePoolBuffer;
            makerName = "BufferFactory::MakePoolBuffer";
        }
        
        std::cout << "* started(" << makerName << ")" << std::endl;
        
        const auto started = std::chrono::system_clock::now();
        
        std::atomic_int done = 0;
        
        for (auto gcd : gcds) {
            gcd->DispatchAsync([allocCount, maker, kData, sizes, &done]() {
                for (uint32_t i = 0; i < allocCount; ++i) {
                    const auto index = std::rand() % sizes.size();
                    auto buffer = maker(sizes[index]);
                    std::memcpy(buffer.PosToWrite(), kData.c_str(), kData.length());
                    buffer.Written(kData.length());
                }
                ++done;
            });
        }
        
        while (done != gcds.size()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        const auto ended = std::chrono::system_clock::now();
        
        std::cout << "* elapsed = " << (ended - started).count() / 1000 << "msec" << std::endl << std::endl;
    }

    //fini
    for (auto gcd : gcds) {
        gcd->Fini();
    }
}
