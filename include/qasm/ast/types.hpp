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
#include <vector>

namespace tweedledee {
namespace qasm {

  /*! \brief Types
   *
   *  Types aren't stratified into base and compound types to simplify the class hierarchy.
   *  Syntactic constraints restrict the creation of semantically undefined types like
   *  registers of registers.
   */

  class type : public ast_node {
  protected:
    type(uint32_t loc)
      : ast_node(loc)
    {}
  };


  /*! \brief Register types */
  class t_register : public type {
  public:
    t_register* make(ast_context* ctx, uint32_t loc, type* base, uint32_t size)
    {
      return new (*ctx) t_register(loc, base_type, size);
    }

    type& base() const
    {
      return *base_;
    }

    uint32_t size() const
    {
      return size_;
    }
      
  protected:
    type* base_;
    uint32_t size_;
      
    t_register(uint32_t loc, type* base_type, uint32_t size)
      : type(loc)
      , base_(base)
      , size_(size)
    {}

	ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::t_register;
	}
  };


  /*! \brief Base types */
  class t_qubit : public type {
  public:
    t_qubit* create(ast_context* ctx, uint32_t loc)
    {
      return new (*ctx) t_qubit(loc);
    }
    
  protected:
    t_qubit(uint32_t loc)
      : type(loc)
    {}

	ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::t_qubit;
	}
  };

  class t_cbit : public type {
  public:
    t_cbit* create(ast_context* ctx, uint32_t loc)
    {
      return new (*ctx) t_cbit(loc);
    }
    
  protected:
    t_cbit(uint32_t loc)
      : type(loc)
    {}

	ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::t_cbit;
	}
  };

}
}
