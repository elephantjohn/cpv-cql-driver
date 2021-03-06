#include <seastar/core/app-template.hh>
#include <seastar/core/thread.hh>
#include <seastar/core/sleep.hh>
#include "./GTestUtils.hpp"

namespace cql::gtest {
	int runAllTests(int argc, char** argv) {
		::testing::InitGoogleTest(&argc, argv);
		seastar::app_template app;
		int returnValue(0);
		app.run(argc, argv, [&returnValue] {
			return seastar::async([] {
				return RUN_ALL_TESTS();
			}).then([&returnValue] (int result) {
				returnValue = result;
				// wait for internal cleanup to make leak sanitizer happy
				return seastar::sleep(std::chrono::seconds(1));
			});
		});
		return returnValue;
	}
}

