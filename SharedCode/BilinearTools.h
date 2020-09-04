#ifndef BILINEARTOOLS_H_INCLUDED
#define BILINEARTOOLS_H_INCLUDED

namespace Bilinear
{

template<typename T, size_t N>
struct BilinearTransform
{
    static inline void call(T (&b)[N], T (&a)[N], T (&bs)[N], T (&as)[N], T K);
};

template<typename T>
struct BilinearTransform<T, 2>
{
    static inline void call(T (&b)[2], T (&a)[2], T (&bs)[2], T (&as)[2], T K)
    {
        const auto a0 = as[0] * K + as[1];
        b[0] = ( bs[0] * K + bs[1]) / a0;
        b[1] = (-bs[0] * K + bs[1]) / a0;
        a[0] = 1.0f;
        a[1] = (-as[0] * K + as[1]) / a0;
    }
};

template<typename T>
struct BilinearTransform<T, 3>
{
    static inline void call(T (&b)[3], T (&a)[3], T (&bs)[3], T (&as)[3], T K)
    {
        const auto KSq = K * K;
        const float a0 = as[0] * KSq + as[1] * K + as[2];

        a[0] = 1.0f;
        a[1] = 2.0f * (as[2] - as[0] * KSq) / a0;
        a[2] = (as[0] * KSq - as[1] * K + as[2]) / a0;
        b[0] = (bs[0] * KSq + bs[1] * K + bs[2]) / a0;
        b[1] = 2.0f * (bs[2] - bs[0] * KSq) / a0;
        b[2] = (bs[0] * KSq - bs[1] * K + bs[2]) / a0;
    }
};

inline float calcPoleFreq (float a, float b, float c)
{
    auto radicand = b*b - 4.0f*a*c;
    if (radicand >= 0.0f)
        return 0.0f;

    return std::sqrt (-radicand) / (2.0f * a);
}

}

#endif // BILINEARTOOLS_H_INCLUDED
