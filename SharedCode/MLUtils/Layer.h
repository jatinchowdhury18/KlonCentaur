#ifndef LAYER_H_INCLUDED
#define LAYER_H_INCLUDED

template<typename T>
class Layer
{
public:
    Layer (size_t in_size, size_t out_size) :
        in_size (in_size),
        out_size (out_size)
    {}

    virtual ~Layer() {}

    inline virtual void forward (const T* input, T* out) = 0;

    const size_t in_size;
    const size_t out_size;
};

#endif // LAYER_H_INCLUDED
