/*
  g++ -I/opt/local/include -L/opt/local/lib OscMessageTest.cpp -lboost_unit_test_framework-mt -o OscMessageTest && ./OscMessageTest
*/

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Test
#include <boost/test/unit_test.hpp>

#include "OscMessage.hpp"

#define FLOAT_TEST_TOLERANCE 0.00001 // percent tolerance when comparing floats

BOOST_AUTO_TEST_CASE(universeInOrder){
    BOOST_CHECK(2+2 == 4);
}

BOOST_AUTO_TEST_CASE(testDefaults){
  OscMessage msg;
  BOOST_CHECK_EQUAL(msg.getSize(), 0);
  BOOST_CHECK_EQUAL(msg.getPrefixLength(), 0);
  BOOST_CHECK_EQUAL(msg.getMessageLength(), 0);
}

BOOST_AUTO_TEST_CASE(testSetPrefix){
  uint8_t buffer[64];
  OscMessage msg(buffer, sizeof(buffer));
  msg.setPrefix("/foo/bar", ",i");
  BOOST_CHECK_EQUAL(msg.getSize(), 1);
  BOOST_CHECK_EQUAL(msg.getAddressLength(), 12);
  BOOST_CHECK_EQUAL(msg.getPrefixLength(), 16);
  BOOST_CHECK_EQUAL(msg.getMessageLength(), 64);
  BOOST_CHECK_EQUAL(msg.calculateMessageLength(), 20);
}

BOOST_AUTO_TEST_CASE(testParse){
  char data[] = "/foo/bar/\0\0\0,i\0\0\0\0\0\120";
  OscMessage msg;
  msg.setBuffer(data, sizeof(data));
  msg.parse();
  BOOST_CHECK_EQUAL(msg.getSize(), 1);
  BOOST_CHECK_EQUAL(msg.getAddressLength(), 12);
  BOOST_CHECK_EQUAL(msg.getPrefixLength(), 16);
  BOOST_CHECK_EQUAL(msg.getMessageLength(), sizeof(data));
  BOOST_CHECK_EQUAL(msg.calculateMessageLength(), 20);
  BOOST_CHECK_EQUAL(msg.getDataType(0), 'i');
  BOOST_CHECK_EQUAL(msg.getInt(0), 80);
}

BOOST_AUTO_TEST_CASE(testStrings){
  uint8_t buffer[64];
  OscMessage msg(buffer, sizeof(buffer));
  msg.setPrefix("osc", "");
  BOOST_CHECK_EQUAL(msg.getAddressLength(), 4);
  msg.setPrefix("data", "");
  BOOST_CHECK_EQUAL(msg.getAddressLength(), 8);
}

BOOST_AUTO_TEST_CASE(testSetFloatData){
  uint8_t buffer[64];
  OscMessage msg(buffer, sizeof(buffer));
  msg.setPrefix("/yoyo", ",ff");
  BOOST_CHECK_EQUAL(msg.getAddressLength(), 8);
  BOOST_CHECK_EQUAL(msg.getPrefixLength(), 12);
  BOOST_CHECK_EQUAL(msg.calculateMessageLength(), 20);
  msg.setFloat(1, -43120.43210);
  msg.setFloat(0, 9786.6789);
  BOOST_CHECK_CLOSE(msg.getFloat(1), -43120.43210, FLOAT_TEST_TOLERANCE);
  BOOST_CHECK_CLOSE(msg.getFloat(0), 9786.6789, FLOAT_TEST_TOLERANCE);

}

BOOST_AUTO_TEST_CASE(testExample1){
/*
The OSC Message with the OSC Address Pattern "/oscillator/4/frequency"
and the floating point number 440.0 as the single argument would be represented
by the following 32-byte message:
 2f (/)  6f (o)  73 (s)  63 (c)
 69 (i)  6c (l)  6c (l)  61 (a)
 74 (t)  6f (o)  72 (r)  2f (/)
 34 (4)  2f (/)  66 (f)  72 (r)
 65 (e)  71 (q)  75 (u)  65 (e)
 6e (n)  63 (c)  79 (y)  0 ()
 2c (,)  66 (f)  0 ()    0 ()
 43 (C)  dc (Ü)  0 ()    0 ()
*/
  uint8_t buffer[] = {
    0x2f, 0x6f, 0x73, 0x63,
    0x69, 0x6c, 0x6c, 0x61,
    0x74, 0x6f, 0x72, 0x2f,
    0x34, 0x2f, 0x66, 0x72,
    0x65, 0x71, 0x75, 0x65,
    0x6e, 0x63, 0x79, 0x0,
    0x2c, 0x66, 0x0, 0x0,
    0x43, 0xdc, 0x0, 0x0    
  };
  OscMessage msg(buffer, sizeof(buffer));
  msg.parse();
  BOOST_CHECK_EQUAL(msg.getSize(), 1);
  BOOST_CHECK_EQUAL(msg.calculateMessageLength(), sizeof(buffer));
  BOOST_CHECK_EQUAL(msg.getDataType(0), 'f');
  BOOST_CHECK_EQUAL(msg.getFloat(0), 440.0);
  BOOST_CHECK(strcmp(msg.getAddress(), "/oscillator/4/frequency") == 0);
  BOOST_CHECK(msg.matches("/oscillator/4/frequency"));
}

BOOST_AUTO_TEST_CASE(testExample2){
/*
The next example shows the 40 bytes in the representation of the OSC Message
with OSC Address Pattern "/foo" and 5 arguments:

The int32 1000
The int32 -1
The string "hello"
The float32 1.234
The float32 5.678

 2f (/)  66 (f)  6f (o)  6f (o)
 0 ()    0 ()    0 ()    0 ()
 2c (,)  69 (i)  69 (i)  73 (s)
 66 (f)  66 (f)  0 ()    0 ()
 0 ()    0 ()    3 ()    e8 (è)
 ff (ÿ)  ff (ÿ)  ff (ÿ)  ff (ÿ)
 68 (h)  65 (e)  6c (l)  6c (l)
 6f (o)  0 ()    0 ()    0 ()
 3f (?)  9d ()   f3 (ó)  b6 (¶)
 40 (@)  b5 (µ)  b2 (”)  2d (-)

*/
  uint8_t buffer[] = {
    0x2f, 0x66, 0x6f, 0x6f,
    0x0, 0x0, 0x0, 0x0,
    0x2c, 0x69, 0x69, 0x73,
    0x66, 0x66, 0x0, 0x0,
    0x0, 0x0, 0x3, 0xe8,
    0xff, 0xff, 0xff, 0xff,
    0x68, 0x65, 0x6c, 0x6c,
    0x6f, 0x0, 0x0, 0x0,
    0x3f, 0x9d, 0xf3, 0xb6,
    0x40, 0xb5, 0xb2, 0x2d
  };
  OscMessage msg(buffer, sizeof(buffer));
  msg.parse();
  BOOST_CHECK_EQUAL(msg.getSize(), 5);
  BOOST_CHECK_EQUAL(msg.calculateMessageLength(), sizeof(buffer));
  BOOST_CHECK_EQUAL(msg.getDataType(0), 'i');
  BOOST_CHECK_EQUAL(msg.getInt(0), 1000);
  BOOST_CHECK_EQUAL(msg.getDataType(1), 'i');
  BOOST_CHECK_EQUAL(msg.getInt(1), -1);
  BOOST_CHECK_EQUAL(msg.getDataType(2), 's');
  BOOST_CHECK_EQUAL(strcmp(msg.getString(2), "hello"), 0);
  BOOST_CHECK_EQUAL(msg.getDataType(3), 'f');
  BOOST_CHECK_CLOSE(msg.getFloat(3), 1.234, FLOAT_TEST_TOLERANCE);
  BOOST_CHECK_EQUAL(msg.getDataType(4), 'f');  
  BOOST_CHECK_CLOSE(msg.getFloat(4), 5.678, FLOAT_TEST_TOLERANCE);
  BOOST_CHECK(strcmp(msg.getAddress(), "/foo") == 0);
  BOOST_CHECK(msg.matches("/foo"));  
}
