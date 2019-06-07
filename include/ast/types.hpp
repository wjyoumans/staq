/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ast/kinds.hpp"
#include "ast/ast_node.hpp"

#include <memory>
#include <string>
#include <vector>

namespace synthewareQ {
namespace ast {

  /*! \brief Types
   *
   *  Types aren't stratified into base and compound types to simplify the class hierarchy.
   *  Syntactic constraints restrict the creation of semantically undefined types like
   *  registers of registers.
   */

  /*! \brief Base class of types */
  class Type : public AST_node {
  public:
    Type(Location loc, ast_nodes kind)
      : AST_node(loc, kind)
    {}

    virtual ~Type() = 0;
  };
  Type::~Type() {}


  /*! \brief Register types */
  class Type_register : public Type {
  private:
    std::unique_ptr<Type> base_;
    uint32_t size_;


    Type_register(Location loc, Type* base, uint32_t size)
      : Type(loc, ast_nodes::type_register)
      , base_(base)
      , size_(size)
    {}
    
  public:
    static Type_register* create(Location loc, Type* base, uint32_t size) {
      return new Type_register(loc, base, size);
    }

    Type* get_base() {
      return base_.get();
    }

    void set_base(Type* base) {
      base_.reset(base);
    }

    uint32_t get_size() const {
      return size_;
    }

    void resize(uint32_t size) {
      size_ = size;
    }
      

  };


  /*! \brief Base types */
  class Type_qubit : public Type {
  private:
    Type_qubit(Location loc)
      : Type(loc, ast_nodes::type_qbit)
    {}
    
  public:
    static Type_qubit* create(Location loc) { return new Type_qubit(loc); }
    
  };

  class Type_cbit : public Type {
  private:
    Type_cbit(Location loc)
      : Type(loc, ast_nodes::type_cbit)
    {}
    
  public:
    static Type_cbit* create(Location loc) { return new Type_cbit(loc); }
    
  };

}
}
