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
#include <sstream>

namespace synthewareQ {
namespace ast {

  /*! \brief Minimal location information for debugging */
  class Location_minimal {
  private:
    const uint32_t line_;
    const uint32_t col_;
    const std::string file_;

  public:
    Location(std::string_view filename, uint32_t line, uint32_t column)
      : filename_(filename)
      , line_(line)
      , col_(column)
    {}

    std::string_view filename const { return file_; }
    uint32_t linenum() const { return line_; }
    uint32_t column() const { return col_; }

	std::string str() const
	{
		std::stringstream ss;
		ss << "<" << file_ << ":" << line_ << ":" << col_ << ">";
		return ss.str();
	}
  };

  typedef Location = uint32_t;

  // Forward declaration
  class Visitor;

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

    virtual void accept(Visitor& visitor) = 0;

    uint32_t uid() const { return uid_; }
    ast_nodes kind() const { return kind_; }
  };
  //AST_node::~AST_node() {}
  uint32_t AST_node::max_uid = 0;

}
}
