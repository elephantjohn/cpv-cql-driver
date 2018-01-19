#include <LowLevel/ResponseMessages/CqlEventMessage.hpp>
#include <LowLevel/ResponseMessages/CqlResponseMessageFactory.hpp>
#include <TestUtility/GTestUtils.hpp>

TEST(TestCqlEventMessage, decode) {
	cql::CqlConnectionInfo info;
	for (std::size_t i = 0; i < 3; ++i) {
		auto headerData = makeTestString("\x04\x00\x00\x00\x0c\x00\x00\x00\x00");
		const char* headerDataPtr = headerData.data();
		const char* headerDataEnd = headerDataPtr + headerData.size();
		cql::CqlMessageHeader header;
		header.decodeHeader(info, headerDataPtr, headerDataEnd);

		auto baseMessage = cql::CqlResponseMessageFactory::makeResponseMessage(std::move(header));
		ASSERT_EQ(baseMessage->getHeader().getOpCode(), cql::CqlMessageType::Event);
		cql::CqlObject<cql::CqlEventMessage> message(std::move(baseMessage));
 		auto bodyData = makeTestString(
			"\x00\x0d""SCHEMA_CHANGE"
			"\x00\x07""CREATED"
			"\x00\x08""KEYSPACE"
			"\x00\x03""abc");
		const char* bodyDataPtr = bodyData.data();
		const char* bodyDataEnd = bodyDataPtr + bodyData.size();
		message->decodeBody(info, bodyDataPtr, bodyDataEnd);
		ASSERT_TRUE(bodyDataPtr == bodyDataEnd);
		ASSERT_EQ(message->getType().get(), "SCHEMA_CHANGE");
		ASSERT_EQ(message->getData(), makeTestString(
			"\x00\x07""CREATED"
			"\x00\x08""KEYSPACE"
			"\x00\x03""abc"));
	}
}

