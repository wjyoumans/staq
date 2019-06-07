/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Bruno Schmitt, Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ast_node_kinds.hpp"

#include <array>
#include <memory>

namespace tweedledee {
namespace qasm {

// Base class for all QASM AST nodes
class ast_node {
public:
	ast_node(const ast_node&) = delete;
	ast_node& operator=(const ast_node&) = delete;

	virtual ~ast_node() = default;

	ast_node_kinds kind() const
	{
		return do_get_kind();
	}

	uint32_t location() const
	{
		return location_;
	}

protected:
	ast_node(uint32_t location)
	    : location_(location)
	{}

	uint32_t config_bits_ = 0;

private:
	virtual ast_node_kinds do_get_kind() const = 0;

private:
	uint32_t location_;

};

} // namespace qasm
} // namespace tweedledee
