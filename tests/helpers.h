#ifndef SOCK_CONNECT_HELPERS_H
#define SOCK_CONNECT_HELPERS_H

#include <random>
#include <chrono>
#include <netinet/in.h>

namespace
{

const std::string sun_path = "/tmp/sun_path";
constexpr uint32_t ip_addr = INADDR_LOOPBACK;
constexpr uint16_t ip_port = 8010;
constexpr size_t array_size = 8;

struct value_storage final
{
    uint8_t ui_8;
    uint16_t ui_16;
    uint32_t ui_32;
    uint64_t ui_64;

    uint8_t ui_8_a[array_size];
    uint16_t ui_16_a[array_size];
    uint32_t ui_32_a[array_size];
    uint64_t ui_64_a[array_size];

    void randomize_fill()
    {
        unsigned seed = static_cast<unsigned int>
        (std::chrono::system_clock::now().time_since_epoch().count());
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<int> distribution(0, 255);

        std::fill(ui_8_a, ui_8_a + array_size, distribution(generator));
        std::fill(ui_16_a, ui_16_a + array_size, distribution(generator));
        std::fill(ui_32_a, ui_32_a + array_size, distribution(generator));
        std::fill(ui_64_a, ui_64_a + array_size, distribution(generator));

        ui_8 = static_cast<uint8_t>(distribution(generator));
        ui_16 = static_cast<uint16_t>(distribution(generator));
        ui_32 = static_cast<uint32_t>(distribution(generator));
        ui_64 = static_cast<uint64_t>(distribution(generator));
    }

    bool operator==(value_storage const &other) const
    {
        return (ui_8 == other.ui_8 && ui_16 == other.ui_16 &&
                ui_32 == other.ui_32 && ui_64 == other.ui_64) &&
               (compare_array(ui_8_a, other.ui_8_a) &&
                compare_array(ui_16_a, other.ui_16_a) &&
                compare_array(ui_32_a, other.ui_32_a) &&
                compare_array(ui_64_a, other.ui_64_a));
    }
private:
    template<typename T, size_t sz>
    bool compare_array(T const(&lhs)[sz], T const(&rhs)[sz]) const
    {
        bool ret = true;
        for (size_t i = 0; i < sz; ++i)
        {
            if (lhs[i] != rhs[i])
            {
                ret = false;
            }
        }
        return ret;
    }
} v;

}

#endif //SOCK_CONNECT_HELPERS_H
