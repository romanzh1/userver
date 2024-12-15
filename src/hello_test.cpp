#include <userver/utest/utest.hpp>
#include "hello.hpp"

UTEST(ConvertFunction, BasicConversion) {
  using pg_service_template::Convert;

  EXPECT_EQ(Convert("USD", "EUR", 100.0), "Converted amount: 85.00 EUR");
  EXPECT_EQ(Convert("USD", "GBP", 200.0), "Converted amount: 150.00 GBP");
}

UTEST(ConvertFunction, MissingParameters) {
    using pg_service_template::Convert;

  EXPECT_THROW(Convert("", "EUR", 100.0), std::invalid_argument);
  EXPECT_THROW(Convert("USD", "", 100.0), std::invalid_argument);
  EXPECT_THROW(Convert("USD", "EUR", -100.0), std::invalid_argument);
}

UTEST(ConvertFunction, RateNotFound) {
    using pg_service_template::Convert;

  EXPECT_THROW(Convert("USD", "INR", 100.0), std::runtime_error);
}
