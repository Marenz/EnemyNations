#pragma once

#include <algorithm>
#include <cstdint>

namespace enations::sprite_detail {

inline void CopyVerticalColumn(
        std::uint8_t* destination,
        int destinationPitch,
        const std::uint8_t* sourceColumn,
        int sourceStride,
        int bytesPerPixel,
        int pixelCount,
        int startFixV,
        int deltaFixV) noexcept
{
    if (!destination || !sourceColumn || pixelCount <= 0 || bytesPerPixel <= 0) {
        return;
    }

    int currentFixV = startFixV;

    for (int index = 0; index < pixelCount; ++index) {
        const int rowIndex = currentFixV >> 16;
        const std::uint8_t* sourcePixel = sourceColumn + static_cast<std::ptrdiff_t>(rowIndex) * sourceStride;
        std::copy_n(sourcePixel, static_cast<std::size_t>(bytesPerPixel), destination);

        currentFixV += deltaFixV;
        destination += destinationPitch;
    }
}

} // namespace enations::sprite_detail
