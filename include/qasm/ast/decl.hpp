/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "../ast_context.hpp"
#include "../ast_node.hpp"
#include "../ast_node_kinds.hpp"

#include "stmt.hpp"
#include "types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace tweedledee {
namespace qasm {

  enum class register_type : unsigned short {
    classical,
    quantum,
  };

  /*! \brief Declaration base class */
  class stmt_decl : public stmt {
  public:
    stmt_decl* create(ast_context* ctx, uint32_t location, std::string_view identifier)
    {
      return new (*ctx) stmt_decl(location, identifier);
    }

    std::string_view identifier const
    {
      return identifier_;
    }

  protected:
    std::string_view identifier_;

    stmt_decl(uint32_t location, std::string_view identifier)
      : stmt(location)
      , identifier_(identifier)
    {}
  };


  /*! \brief Register declaration */
  class decl_register : public decl {
  public:
    decl_register* create(ast_context* ctx, uint23_t location, register_type type,
                          std::string_view identifier, uint32_t size)
    {
      return new (*ctx) decl_register(location, type, identifier, size);
    }

    bool is_quantum() const
    {
      return type_ == register_type::quantum;
    }

    uint32_t size() const
    {
      return size_;
    }

  protected:
    register_type type_;
    uint32_t size_;

    decl_register(uint32_t location, register_type type, std::string_view identifier,
                  uint32_t size)
      : stmt_decl(location, identifier)
      , type_(type)
      , size_(size)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::decl_register;
    }

  };


  /*! \brief Ancilla declaration */
  class decl_ancilla final : public decl_register {
  public:
    static decl_ancilla* create(ast_context* ctx, uint32_t location,
                               std::string_view identifier, uint32_t size, bool dirty)
    {
      return new (*ctx) decl_ancilla(location, identifier, size, dirty);
    }

    bool is_dirty() const
    {
      return dirty_;
    }

  protected:
    bool dirty_;

    decl_ancilla(uint32_t location, std::string_view identifier, uint32_t size, bool dirty)
      : decl_register(location, identifier, size)
      , dirty_(dirty)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::decl_ancilla;
    }

  };


  /*! \brief Parameter declaration */
  // TODO: move into a single variable declaration
  class decl_param final : public stmt_decl {
  public:
    static decl_param* create(ast_context* ctx, uint32_t location, std::string_view identifier)
    {
      return new (*ctx) decl_param(location, identifier);
    }

  protected:
    decl_param(uint32_t location, std::string_view identifier)
      : stmt_decl(location, identifier)
    {}

    ast_node_kinds do_get_kind() const override
    {
      return ast_node_kinds::decl_param;
    }

  };

  /*! \brief Gate declaration */
  class decl_gate : public stmt_decl {

  public:
    static decl_gate* create(ast_context* ctx, uint32_t location, std::string_view identifier,
                             vector<decl_param*>& parameters, vector<std::string_view>& formals)
                             
    {
      return new (*ctx) decl_gate(location, identifier, parameters, formals);
    }

    /* Gate type */
    virtual bool is_opaque() const;
    virtual bool is_oracle() const;

    /* Parameters */
    bool has_parameters() const
    {
      return parameters_.size() > 0;
    }
    
    uint32_t num_parameters() const
    {
      return parameters_.size();
    }

    vector<decl_param*>& parameters()
    {
      return parameters_;
    }

    /* Formals */
    uint32_t num_formals() const
    {
      return formals_.size();
    }

    vector<std::string_view>& formals()
    {
      return formals_;
    }

  protected:
    decl_gate(uint32_t location, std::string_view identifier,
              vector<decl_param*>& parameters, vector<std::string_view>& formals)
      : stmt_decl(location, identifier)
      , parameters_(parameters)
      , formals_(formals)
	{}

	ast_node_kinds do_get_kind() const override
	{
      return ast_node_kinds::decl_gate;
	}

  };


  /*! \brief Regular gate declaration */
  class decl_unitary_gate : public decl_gate {

  public:
    static decl_unitary_gate* create(ast_context* ctx, uint32_t location, std::string_view identifier,
                                     vector<decl_param*>& parameters, vector<std::string_view>& formals,
                                     vector<stmt_unitary*>& body)
                             
    {
      return new (*ctx) decl_unitary_gate(location, identifier, parameters, formals, body);
    }

    bool is_opaque const
    {
      return false;
    }

    bool is_oracle const
    {
      return false;
    }

    vector<stmt_unitary*>& body()
    {
      return body_;
    }

  protected:
    vector<stmt_unitary*> body_;
    
    decl_unitary_gate(uint32_t location, std::string_view identifier,
                      vector<decl_param*>& parameters, vector<std::string_view>& formals,
                      vector<stmt_unitary*>& body)
      : decl_gate(location, identifier, parameters, formals)
      , body_(body)
	{}

  };


  /*! \brief Opaque gate declaration */
  class decl_opaque_gate : public decl_gate {

  public:
    static decl_opaque_gate* create(ast_context* ctx, uint32_t location, std::string_view identifier,
                                    vector<decl_param*>& parameters, vector<std::string_view>& formals)
    {
      return new (*ctx) decl_opaque_gate(location, identifier, parameters, formals);
    }

    bool is_opaque const
    {
      return true;
    }

    bool is_oracle const
    {
      return false;
    }

  protected:
    decl_opaque_gate(uint32_t location, std::string_view identifier,
                     vector<decl_param*>& parameters, vector<std::string_view>& formals)
      : decl_gate(location, identifier, parameters, formals)
	{}

  };


  /*! \brief Oracle gate declaration */
  class decl_oracle_gate : public decl_gate {

  public:
    static decl_oracle_gate* create(ast_context* ctx, uint32_t location, std::string_view identifier,
                                    vector<decl_param*>& parameters, vector<std::string_view>& formals,
                                    std::string_view filename)
    {
      return new (*ctx) decl_oracle_gate(location, identifier, parameters, formals, filename);
    }

    bool is_opaque const
    {
      return false;
    }

    bool is_oracle const
    {
      return true;
    }

    std::string_view filename() const
    {
      return filename_;
    }

  protected:
    std::string_view filename_;
    
    decl_opaque_gate(uint32_t location, std::string_view identifier,
                     vector<decl_param*>& parameters, vector<std::string_view>& formals,
                     std::string_view filename)
      : decl_gate(location, identifier, parameters, formals)
      , filename_(filename)
	{}

  };

} // namespace qasm
} // namespace tweedledee
