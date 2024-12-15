#pragma once

#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>

namespace pg_service_template {

std::string Convert(std::string_view from_currency, std::string_view to_currency, double amount);

void AppendCurrencyConverter(userver::components::ComponentList& component_list);

}  // namespace pg_service_template
