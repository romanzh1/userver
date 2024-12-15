#include "hello.hpp"

#include <fmt/format.h>
#include <stdexcept>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>

namespace pg_service_template {

namespace {

class CurrencyConverter final : public userver::server::handlers::HttpHandlerBase {
 public:
  static constexpr std::string_view kName = "handler-currency-converter";

  CurrencyConverter(const userver::components::ComponentConfig& config,
                    const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext&) const override {
    const auto& from_currency = request.GetArg("from_currency");
    const auto& to_currency = request.GetArg("to_currency");
    const auto& amount_str = request.GetArg("amount");

    if (from_currency.empty() || to_currency.empty() || amount_str.empty()) {
      return "Missing required parameters: from_currency, to_currency, amount\n";
    }

    double amount;
    try {
      amount = std::stod(amount_str);
    } catch (const std::exception&) {
      return "Invalid amount parameter\n";
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT rate FROM currency_rates WHERE from_currency = $1 AND to_currency = $2",
        from_currency, to_currency);

    if (result.IsEmpty()) {
      return fmt::format("Conversion rate not found for {} to {}\n", from_currency, to_currency);
    }

    double rate = result.AsSingleRow<double>();
    double converted_amount = amount * rate;

    return fmt::format("Converted amount: {:.2f} {}\n", converted_amount, to_currency);
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

std::string Convert(std::string_view from_currency, std::string_view to_currency, double amount) {
  if (from_currency.empty() || to_currency.empty()) {
    throw std::invalid_argument("Missing currency parameters");
  }
  if (amount <= 0) {
    throw std::invalid_argument("Amount must be positive");
  }

  if (from_currency == "USD" && to_currency == "EUR") {
    return fmt::format("Converted amount: {:.2f} EUR", amount * 0.85);
  } else if (from_currency == "USD" && to_currency == "GBP") {
    return fmt::format("Converted amount: {:.2f} GBP", amount * 0.75);
  } else {
    throw std::runtime_error(fmt::format("Conversion rate not found for {} to {}", from_currency, to_currency));
  }
}
void AppendCurrencyConverter(userver::components::ComponentList& component_list) {
  component_list.Append<CurrencyConverter>();
  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();
}

}  // namespace pg_service_template