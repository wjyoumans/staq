/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ast/kinds.hpp"
#include "ast/ast_node.hpp"
#include "ast/decl.hpp"

#include "ast/visitor.hpp"

#include <memory>
#include <string>

namespace synthewareQ {
namespace ast {

  /*! \brief Holds information about an individual source unit
   *
   *  To keep preprocessor inclusion semantics, all definitions
   *  in a file are automatically promoted to the global scope
   */
  class Translation_unit : public AST_node {
  private:
    std::string_view filename_;
    bool writeout_;
    std::unique_ptr<Stmt_list> body_;
    
    Translation_unit(Location loc, std::string_view filename, Stmt_list* body, bool writeout)
      : AST_node(loc, ast_nodes::trans_unit)
      , filename_(filename)
      , body_(body)
      , writeout_(writeout)
    {}

  public:

    Translation_unit* create(Location loc, std::string_view filename, Stmt_list* body, bool writeout = false) {
      return new Translation_unit(loc, filename, body, writeout);
    }

    std::string_view filename() {
      return filename_;
    }

    Stmt_list* get_body() {
      return body_.get();
    }

    bool do_write() {
      return writeout_;
    }

    void set_write(bool writeout) {
      writeout_ = writeout;
    }

    void accept(Visitor& visitor) {
      visitor.visit(this);
    }

  };

}
}
