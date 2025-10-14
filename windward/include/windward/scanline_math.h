#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace windward::scanlist_detail {

inline void FillConstant(int* destination, std::size_t count, int value) noexcept
{
    if (!destination || count == 0U) {
        return;
    }

    std::fill_n(destination, count, value);
}

inline void FillInterpolatedIntegers(
        int* destination,
        std::size_t count,
        int startX,
        int deltaX,
        int deltaY) noexcept
{
    if (!destination || count == 0U || deltaY == 0) {
        return;
    }

    const std::int64_t step = (static_cast<std::int64_t>(deltaX) << 16) / deltaY;
    std::int64_t current = (static_cast<std::int64_t>(startX) << 16) + 0x00008000;

    for (std::size_t index = 0; index < count; ++index) {
        destination[index] = static_cast<int>(current >> 16);
        current += step;
    }
}

inline void FillInterpolatedFixed(
        int* destination,
        std::size_t count,
        int startFixX,
        int deltaX,
        int deltaY) noexcept
{
    if (!destination || count == 0U || deltaY == 0) {
        return;
    }

    const std::int64_t step = (static_cast<std::int64_t>(deltaX) << 16) / deltaY;
    std::int64_t current = static_cast<std::int64_t>(startFixX);

    for (std::size_t index = 0; index < count; ++index) {
        destination[index] = static_cast<int>(current);
        current += step;
    }
}

} // namespace windward::scanlist_detail
