#include <gtest/gtest.h>

#include "Interpreter.h"

TEST(Arithmetic, Plus)
{
	EXPECT_EQ(Interpreter::Create("2+2")->Interpret(), 4);
	EXPECT_EQ(Interpreter::Create("1+3421+213+0+3+01")->Interpret(), 3639);
	EXPECT_EQ(Interpreter::Create("1")->Interpret(), 1);
	EXPECT_EQ(Interpreter::Create("31231")->Interpret(), 31231);
}

TEST(Arithmetic, Minus)
{
	EXPECT_EQ(Interpreter::Create("-321-969-123-414")->Interpret(), -1827);
	EXPECT_EQ(Interpreter::Create("-5")->Interpret(), -5);
	EXPECT_EQ(Interpreter::Create("-323")->Interpret(), -323);
}

TEST(Arithmetic, Div)
{
	EXPECT_EQ(Interpreter::Create("20/2/1/1/1/1/1/1/2/5")->Interpret(), 1);
}

TEST(Arithmetic, Mul)
{
	EXPECT_EQ(Interpreter::Create("1*1*1*1*1*1")->Interpret(), 1);
	EXPECT_EQ(Interpreter::Create("1*0")->Interpret(), 0);
}

TEST(Arithmetic, Unary)
{
	EXPECT_EQ(Interpreter::Create("-22")->Interpret(), -22);
	EXPECT_EQ(Interpreter::Create("++++++++2+++++++2")->Interpret(), 4);
	EXPECT_EQ(Interpreter::Create("5 - - - + - 3")->Interpret(), 8);
}

TEST(Arithmetic, Complex)
{
	EXPECT_EQ(Interpreter::Create("5 - --+-(3 + 4) - +2")->Interpret(), 10);
	EXPECT_EQ(Interpreter::Create("14 + 2 * 3 - 6 / 2")->Interpret(), 17);
	EXPECT_EQ(Interpreter::Create("7 + 3 * (10 / (12 / (3 + 1) - 1)) / (2 + 3) - 5 - 3 + (8)")->Interpret(), 10);
}