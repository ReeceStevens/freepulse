#include "gtest/gtest.h"
#include "CircleFifo.h"

namespace {

// The fixture for testing class Foo.
class CircleFifoTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  CircleFifoTest() {
    // You can do set-up work for each test here.
  }

  virtual ~CircleFifoTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(CircleFifoTest, SimpleAdd) {
	CircleFifo<int> a = CircleFifo<int>();
	int payload = 1;
	a.add(payload);
	EXPECT_EQ(a[0], payload);
	EXPECT_EQ(a[0], a.newest());
}

TEST_F(CircleFifoTest, ModBehavior) {
	EXPECT_EQ(CircleFifo<int>::mod(4,5), 4);
	EXPECT_EQ(CircleFifo<int>::mod(5,4), 1);
	EXPECT_EQ(CircleFifo<int>::mod(10,4), 2);
	EXPECT_EQ(CircleFifo<int>::mod(-1,4), 3);
	EXPECT_EQ(CircleFifo<int>::mod(-3,4), 1);
}

TEST_F(CircleFifoTest, AddLotsOfElements) {
	CircleFifo<int> a = CircleFifo<int>(5);
	for (int i = 0; i < 6; i ++) {
		a.add(i);
	}
	EXPECT_EQ(a[0], 5);
	EXPECT_EQ(a.newest(), 5);
	EXPECT_EQ(a[4], 1);
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
