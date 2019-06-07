/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "../ast_context.hpp"
#include "../ast_node.hpp"
#include "../ast_node_kinds.hpp"

#include <memory>
#include <string>

namespace tweedledee {
namespace qasm {

  /*! \brief Holds information about an individual source unit
   *
   *  To keep preprocessor inclusion semantics, all definitions
   *  in a file are automatically promoted to the global scope
   */
  class source_unit : public ast_node {
  public:
    source_unit(uint32_t loc, std::string_view filename, vector<stmt*>& body, bool output)
      : ast_node(loc)
      , filename_(filename)
      , body_(body)
      , output_(output)
    {}

    std::string_view filename;
    bool output;
    vector<stmt*> body;

  protected:
	ast_node_kinds get_kind() const override
	{
      return ast_node_kinds::source_unit;
	}
  };

}
}
