#ifndef BYTES_H_
#define BYTES_H_

#include <array>
#include <string>
#include <bitset>
#include <memory>
#include <cstring>
#include <cstdint>
#include <iomanip>
#include <sstream>


template <uint16_t N>
class Bytes
{
private:
    uint8_t data[N];

public:
    Bytes();
    explicit Bytes(const void* ptr);
    Bytes(const uint64_t val);

    template <uint16_t M>
    Bytes(const Bytes<M>& other);

    template <uint16_t M>
    Bytes& operator=(const Bytes<M>& other);

    Bytes(Bytes<N>&& other) = default;
    Bytes& operator=(Bytes<N>&& other) = default;

    const uint8_t* getData() const;
    uint16_t getSize() const;

    const std::string& toStringHex() const;
    const std::string& toStringBinary() const;
#if 0
    const std::string& toStringDecimal() const;
#endif

    template <uint16_t M>
    bool operator==(const Bytes<M>& other) const;

    template <uint16_t M>
    bool operator!=(const Bytes<M>& other) const;

    template <uint16_t M>
    bool operator<(const Bytes<M>& other) const;

    template <uint16_t M>
    bool operator<=(const Bytes<M>& other) const;

    template <uint16_t M>
    bool operator>(const Bytes<M>& other) const;

    template <uint16_t M>
    bool operator>=(const Bytes<M>& other) const;

    template <uint16_t M>
    Bytes<N> operator+(const Bytes<M>& other) const;

    template <uint16_t M>
    Bytes<N>& operator+=(const Bytes<M>& other);

    Bytes<N> operator+(const uint64_t val) const;
    Bytes<N>& operator+=(const uint64_t val);
};
static_assert(sizeof(Bytes<1>) == 1, "Size of Bytes<1> must be 1 byte.");
static_assert(sizeof(Bytes<2>) == 2, "Size of Bytes<2> must be 2 bytes.");
static_assert(sizeof(Bytes<3>) == 3, "Size of Bytes<3> must be 3 bytes.");
static_assert(sizeof(Bytes<4>) == 4, "Size of Bytes<4> must be 4 bytes.");
static_assert(sizeof(Bytes<5>) == 5, "Size of Bytes<5> must be 5 bytes.");


template <uint16_t N>
Bytes<N>::Bytes()
    : data{}
{
}

template <uint16_t N>
Bytes<N>::Bytes(const void* ptr)
    : Bytes<N>{}
{
    if (ptr)
    {
        std::memcpy(data, ptr, N);
    }
}

template <uint16_t N>
Bytes<N>::Bytes(const uint64_t val)
    : Bytes<N>{}
{
    std::memcpy(data, &val, N);
}

template <uint16_t N>
template <uint16_t M>
Bytes<N>::Bytes(const Bytes<M>& other)
    : Bytes<N>{}
{
    std::memcpy(data, other.getData(), N < M ? N : M);
}

template <uint16_t N>
template <uint16_t M>
Bytes<N>& Bytes<N>::operator=(const Bytes<M>& other)
{
    if (static_cast<const void*>(this) != static_cast<const void*>(&other))
    {
        std::memset(data, 0, N);
        std::memcpy(data, other.getData(), N < M ? N : M);
    }
    return *this;
}

template <uint16_t N>
const uint8_t* Bytes<N>::getData() const
{
    return data;
}

template <uint16_t N>
uint16_t Bytes<N>::getSize() const
{
    return sizeof(data);
}

template <uint16_t N>
const std::string& Bytes<N>::toStringHex() const
{
    static std::string hexStr;

    if (hexStr.empty())
    {
        std::stringstream ss;
        for (int i = N - 1; i >= 0; i--)
        {
            if (data[i])
            {
                ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int32_t>(data[i]);
            }
        }

        std::string tmpStr = ss.str();
        const auto idx = tmpStr.find_first_not_of('0');
        if (idx == std::string::npos)
        {
            tmpStr = "0x0";
        }
        else
        {
            tmpStr = std::string("0x") + tmpStr.substr(idx);
        }
        hexStr = std::move(tmpStr);
    }

    return hexStr;
}

template <uint16_t N>
const std::string& Bytes<N>::toStringBinary() const
{
    static std::string binStr;

    if (binStr.empty())
    {
        std::stringstream ss;
        for (int i = N - 1; i >= 0; i--)
        {
            if (data[i])
            {
                ss << std::bitset<8>(data[i]);
            }
        }

        std::string tmpStr = ss.str();
        const auto idx = tmpStr.find_first_not_of('0');
        if (idx == std::string::npos)
        {
            tmpStr = "0b0";
        }
        else
        {
            tmpStr = std::string("0b") + tmpStr.substr(idx);
        }
        binStr = std::move(tmpStr);
    }

    return binStr;
}

template <uint16_t N>
template <uint16_t M>
bool Bytes<N>::operator==(const Bytes<M>& other) const
{
    return toStringHex() == other.toStringHex();
}

template <uint16_t N>
template <uint16_t M>
bool Bytes<N>::operator!=(const Bytes<M>& other) const
{
    return !(*this == other);
}

template <uint16_t N>
template <uint16_t M>
bool Bytes<N>::operator<(const Bytes<M>& other) const
{
    const auto& s1 = toStringHex();
    const auto& s2 = other.toStringHex();

    return s1.size() < s2.size() || s1 < s2;
}

template <uint16_t N>
template <uint16_t M>
bool Bytes<N>::operator<=(const Bytes<M>& other) const
{
    return *this < other || *this == other;
}

template <uint16_t N>
template <uint16_t M>
bool Bytes<N>::operator>(const Bytes<M>& other) const
{
    return !(*this <= other);
}

template <uint16_t N>
template <uint16_t M>
bool Bytes<N>::operator>=(const Bytes<M>& other) const
{
    return !(*this < other);
}

template <uint16_t N>
template <uint16_t M>
Bytes<N> Bytes<N>::operator+(const Bytes<M>& other) const
{
    uint16_t K = (N > M ? N : M) + 1;

    using sum_t = uint16_t;
    sum_t sum = 0;
    uint8_t carry = 0;
    std::unique_ptr<uint8_t[]> result_data(new uint8_t[K]{});
    for (int i = 0; i < K; i++)
    {
        const auto& lhs = static_cast<sum_t>(i < N ? data[i] : 0);
        const auto& rhs = static_cast<sum_t>(i < M ? other.getData()[i] : 0);

        sum = lhs + rhs + carry;
        result_data.get()[i] = static_cast<uint8_t>(sum & 0xFF);
        carry = static_cast<uint8_t>(sum >> 8);
    }

    return Bytes<N>{ result_data.get() };
}

template <uint16_t N>
template <uint16_t M>
Bytes<N>& Bytes<N>::operator+=(const Bytes<M>& other)
{
    uint16_t K = (N > M ? N : M) + 1;

    using sum_t = uint16_t;
    sum_t sum = 0;
    uint8_t carry = 0;
    std::unique_ptr<uint8_t[]> result_data(new uint8_t[K]{});
    for (int i = 0; i < K; i++)
    {
        const auto& lhs = static_cast<sum_t>(i < N ? data[i] : 0);
        const auto& rhs = static_cast<sum_t>(i < M ? other.getData()[i] : 0);

        sum = lhs + rhs + carry;
        result_data.get()[i] = static_cast<uint8_t>(sum & 0xFF);
        carry = static_cast<uint8_t>(sum >> 8);
    }

    std::memcpy(data, result_data.get(), N);

    return *this;
}

template <uint16_t N>
Bytes<N> Bytes<N>::operator+(const uint64_t val) const
{
    return *this + Bytes<8>{val};
}

template <uint16_t N>
Bytes<N>& Bytes<N>::operator+=(const uint64_t val)
{
    return *this += Bytes<8>{val};
}

#endif
