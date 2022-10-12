#ifndef TESTS_HPP
#define TESTS_HPP
#include <gtest/gtest.h>
#include "Client.hpp"

namespace es {
    namespace test {
        class ServerTest: public ::testing::Test {
            protected:
                Client _client;
        };
    }
}



#endif