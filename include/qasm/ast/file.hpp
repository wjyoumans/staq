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
  class file : public ast_node {
  public:
    file* create(ast_context* ctx, uint32_t loc, std::string_view filename,
                 vector<stmt*>& body, bool output = false)
    {
      return new (*ctx) file(loc, filename, body, output);
    }

    std::string_view filename() const
    {
      return filename_;
    }

    bool output() const
    {
      return output_;
    }

    vector<stmt*>& body() const
    {
      return body_;
    }

  private:
    std::string_view filename_;
    bool output_;

    vector<stmt*> body_;

    file(uint32_t loc, std::string_view filename, vector<stmt*>& body, bool output)
      : ast_node(loc)
      , filename_(filename)
      , body_(body)
      , output_(output)
    {}

  };

}
}
