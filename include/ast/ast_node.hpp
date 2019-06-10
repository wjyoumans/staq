/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ast/kinds.hpp"

#include <memory>
#include <string>
#include <vector>

#include <iostream>

namespace synthewareQ {
namespace ast {

  // TEMPORARY
  class Location {
  private:
    const uint32_t linenum_;

  public:
    Location(uint32_t linenum) : linenum_(linenum) {}

    uint32_t linenum() const { return linenum_; }
  };
    

  /*! \brief Base AST node class */
  class AST_node {
  private:
    static uint32_t max_uid;
    
    const uint32_t uid_;
    const Location loc_;
    const ast_nodes kind_;

  public:
    AST_node(Location loc, ast_nodes kind)
      : uid_(max_uid++)
      , loc_(loc)
      , kind_(kind)
    { std::cout << "Creating node #" << uid_ << std::endl; }

    //virtual ~AST_node() = 0;
    ~AST_node() { std::cout << "Destroying node #" << uid_ << std::endl; }

    uint32_t uid() const { return uid_; }
    ast_nodes kind() const { return kind_; }
  };
  //AST_node::~AST_node() {}
  uint32_t AST_node::max_uid = 0;

}
}
