// ConstructorTest.cpp : Defines the entry point for the console application.
//

#include <gtest/gtest.h>


int main(int argc, char** argv)
{
	// Git push check;
    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}

