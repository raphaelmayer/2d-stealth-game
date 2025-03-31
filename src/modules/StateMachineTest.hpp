#pragma once

# include "include/boost-ext/sml.hpp"

namespace sml = boost::sml;

//states
struct idle {};
struct walking {};

//events
struct button_press {};

auto operator()() const
{
	using namespace sml;

	return make_transition_table(*state<idle> + event<button_press> = state<walking>);
}