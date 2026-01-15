#ifndef PKG_PKG_HPP
#define PKG_PKG_HPP

#include <iostream>
#include <glaze/glaze.hpp>
#include "pkg/observe/logger.hpp"

using VecU8 = std::vector<uint8_t>;

namespace pkg {
    namespace observe {
        class Logger;
        class Statistics;
    }
    namespace transform {
        class Conversion;
    }
    namespace element {
        class Checksum;
    }
    namespace transport {
        class Simulation;
    }
}

#endif  // PKG_PKG_HPP