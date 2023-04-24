#ifndef FAST_WFC_UTILS_COLOR_HPP_
#define FAST_WFC_UTILS_COLOR_HPP_

#include <functional>

/**
 * Represent a 24-bit rgb WFCColor.
 */
struct WFCColor {
  unsigned char r, g, b;

  bool operator==(const WFCColor &c) const noexcept {
    return r == c.r && g == c.g && b == c.b;
  }

  bool operator!=(const WFCColor &c) const noexcept { return !(c == *this); }
};

/**
 * Hash function for WFCColor.
 */
namespace std {
template <> class hash<WFCColor> {
public:
  size_t operator()(const WFCColor &c) const {
    return (size_t)c.r + (size_t)256 * (size_t)c.g +
           (size_t)256 * (size_t)256 * (size_t)c.b;
  }
};
} // namespace std

#endif // FAST_WFC_UTILS_WFCColor_HPP_
