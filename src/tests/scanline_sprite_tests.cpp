#include <cassert>
#include <cstdint>
#include <vector>

#include "windward/scanline_math.h"
#include "detail/sprite_sampling.h"

int main()
{
    using windward::scanlist_detail::FillConstant;
    using windward::scanlist_detail::FillInterpolatedFixed;
    using windward::scanlist_detail::FillInterpolatedIntegers;

    {
        std::vector<int> values(5);
        FillConstant(values.data(), values.size(), 7);
        for (int value : values)
        {
            assert(value == 7);
        }
    }

    {
        std::vector<int> values(5);
        FillInterpolatedIntegers(values.data(), values.size(), 2, 4, 4);
        const std::vector<int> expected{2, 3, 4, 5, 6};
        assert(values == expected);
    }

    {
        std::vector<int> values(5);
        FillInterpolatedIntegers(values.data(), values.size(), -2, -4, 4);
        const std::vector<int> expected{-2, -3, -4, -5, -6};
        assert(values == expected);
    }

    {
        const int startFix = (3 << 16) + 0x00008000;
        std::vector<int> fixedValues(4);
        FillInterpolatedFixed(fixedValues.data(), fixedValues.size(), startFix, 4, 4);

        std::vector<int> rounded(4);
        for (std::size_t index = 0; index < fixedValues.size(); ++index)
        {
            rounded[index] = fixedValues[index] >> 16;
        }

        const std::vector<int> expected{3, 4, 5, 6};
        assert(rounded == expected);
    }

    {
        const int bytesPerPixel = 1;
        const int stride = 4;
        std::vector<std::uint8_t> source(stride * 5);
        for (int row = 0; row < 5; ++row)
        {
            source[row * stride] = static_cast<std::uint8_t>(10 * row);
        }

        std::vector<std::uint8_t> destination(bytesPerPixel * 3, 0);
        enations::sprite_detail::CopyVerticalColumn(
                destination.data(),
                bytesPerPixel,
                source.data(),
                stride,
                bytesPerPixel,
                3,
                0x00008000,
                0x00010000);

        const std::vector<std::uint8_t> expected{0, 10, 20};
        assert(destination == expected);
    }

    {
        const int bytesPerPixel = 3;
        const int stride = 6;
        std::vector<std::uint8_t> source(stride * 4);
        for (int row = 0; row < 4; ++row)
        {
            std::uint8_t base = static_cast<std::uint8_t>(row * 3);
            for (int column = 0; column < bytesPerPixel; ++column)
            {
                source[row * stride + column] = static_cast<std::uint8_t>(base + column);
            }
        }

        std::vector<std::uint8_t> destination(bytesPerPixel * 2, 0);
        enations::sprite_detail::CopyVerticalColumn(
                destination.data(),
                bytesPerPixel,
                source.data(),
                stride,
                bytesPerPixel,
                2,
                0x00018000,
                0x00018000);

        const std::vector<std::uint8_t> expected{
                source[1 * stride + 0], source[1 * stride + 1], source[1 * stride + 2],
                source[2 * stride + 0], source[2 * stride + 1], source[2 * stride + 2]};
        assert(destination == expected);
    }

    return 0;
}
