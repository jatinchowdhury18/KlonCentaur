#ifndef JSON2RNNPARSER_H_INCLUDED
#define JSON2RNNPARSER_H_INCLUDED

#include "SharedJuceHeader.h"
#include "MLUtils/Model.h"

class Json2RnnParser
{
public:
    Json2RnnParser() {}

    std::unique_ptr<Model<float>> parseJson (InputStream& input);

    std::unique_ptr<Dense<float>> createDense (size_t in_size, size_t out_size, var& weights);
    std::unique_ptr<GRULayer<float>> createGRU (size_t in_size, size_t out_size, var& weights);
};

#endif // JSON2RNNPARSER_H_INCLUDED
