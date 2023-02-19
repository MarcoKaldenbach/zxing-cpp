//
// Created by Marco Kaldenbach on 17-02-23.
// CAUTION: for ZXing a black pixel means a BitMatrix::SET_V (0xFF) in binarized data
//

#pragma once

#include "BinaryBitmap.h"
#include "BitMatrix.h"

#include <cstdint>

namespace ZXing {

class BinarizedData : public BinaryBitmap
{
    using data_t = uint8_t;
    data_t* _data;
    bool _useDataCopy; //defines if the original data could be used or if a copy is preferred (using original data is faster)

public:
    BinarizedData(data_t* data_ptr, int width, int height, bool useDataCopy = true) :
        BinaryBitmap(ImageView(data_ptr, width, height, ZXing::ImageFormat::Lum)),
        _data(data_ptr),
        _useDataCopy(useDataCopy){}

    bool getPatternRow(int row, int rotation, PatternRow& res) const override
    {
        auto buffer = _buffer.rotated(rotation);

        const int stride = buffer.pixStride();
        const data_t* begin = buffer.data(0, row) + GreenIndex(buffer.format());
        const data_t* end = begin + buffer.width() * stride;

        auto* lastPos = begin;
        data_t lastVal = 0;

        res.clear();

        for (const data_t* p = begin; p < end; p += stride) {
            data_t val = *p;
            if (val != lastVal) {
                res.push_back(narrow_cast<PatternRow::value_type>((p - lastPos) / stride));
                lastVal = val;
                lastPos = p;
            }
        }

        res.push_back(narrow_cast<PatternRow::value_type>((end - lastPos) / stride));

        if (*(end - stride) == 0)
            res.push_back(0); // last value is number of white pixels, here 0

        return true;
    }

    std::shared_ptr<const BitMatrix> getBlackMatrix() const override
    {
        return std::make_shared<const BitMatrix>(_buffer.width(), _buffer.height(), _data, _useDataCopy);
    }
};

} // ZXing
