/*
  g++ -I/opt/local/include -L/opt/local/lib OscMessageTest.cpp -lboost_unit_test_framework-mt -o OscMessageTest && ./OscMessageTest
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Test
#include <boost/test/unit_test.hpp>

#include "OscMessage.hpp"

BOOST_AUTO_TEST_CASE(universeInOrder){
    BOOST_CHECK(2+2 == 4);
}

BOOST_AUTO_TEST_CASE(testDefaults){
  OscMessage msg;
  BOOST_CHECK_EQUAL(msg.getSize(), 0);
  BOOST_CHECK_EQUAL(msg.getPrefixLength(), 0);
  BOOST_CHECK_EQUAL(msg.getMessageLength(), 0);
  BOOST_CHECK_EQUAL(msg.getTotalDataLength(), 0);
  // BOOST_CHECK(!clockIsHigh());
  // BOOST_CHECK_EQUAL(countFills(seqB), 1);
}
