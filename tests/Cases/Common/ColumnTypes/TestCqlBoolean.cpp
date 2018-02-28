#include <CqlDriver/Common/Exceptions/CqlDecodeException.hpp>
#include <CqlDriver/Common/ColumnTypes/CqlBoolean.hpp>
#include <TestUtility/GTestUtils.hpp>

TEST(TestCqlBoolean, getset) {
	cql::CqlBoolean value(false);
	ASSERT_EQ(value.get(), false);
	value.set(true);
	ASSERT_EQ(value.get(), true);

	value = cql::CqlBoolean();
	ASSERT_EQ(value.get(), false);
}

TEST(TestCqlBoolean, encode) {
	{
		cql::CqlBoolean value(true);
		seastar::sstring data;
		value.encode(data);
		ASSERT_EQ(data, makeTestString("\x01"));
	}
	{
		cql::CqlBoolean value(false);
		seastar::sstring data;
		value.encode(data);
		ASSERT_EQ(data, makeTestString("\x00"));
	}
}

TEST(TestCqlBoolean, decode) {
	{
		cql::CqlBoolean value(true);
		auto data = makeTestString("");
		value.decode(data.data(), data.size());
		ASSERT_EQ(value.get(), false);
	}
	{
		cql::CqlBoolean value(true);
		auto data = makeTestString("\x00");
		value.decode(data.data(), data.size());
		ASSERT_EQ(value.get(), false);
	}
	{
		cql::CqlBoolean value(false);
		auto data = makeTestString("\x01");
		value.decode(data.data(), data.size());
		ASSERT_EQ(value.get(), true);
	}
}

TEST(TestCqlBoolean, decodeError) {
	{
		cql::CqlBoolean value(0);
		auto data = makeTestString("\x01\x00");
		ASSERT_THROWS(cql::CqlDecodeException, value.decode(data.data(), data.size()));
	}
}
