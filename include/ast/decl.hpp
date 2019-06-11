/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "utils/list.hpp"

#include "ast/kinds.hpp"
#include "ast/ast_node.hpp"
#include "ast/types.hpp"
#include "ast/stmt.hpp"

#include "ast/visitor.hpp"

#include <memory>
#include <string>
#include <vector>
#include <set>

namespace synthewareQ {
namespace ast {

  enum class qualifier {
    dirty
  };

  /*! \brief Declaration base class */
  class Decl : public AST_node {
  private:
    std::string_view identifier_;
    
  public:
    Decl(Location loc, ast_nodes kind, std::string_view identifier)
      : AST_node(loc, kind)
      , identifier_(identifier)
    {}

    virtual ~Decl() = 0;

    std::string_view identifier() const {
      return identifier_;
    }
  };
  Decl::~Decl() {}


  /*! \brief Register declaration */
  class Decl_var final : public Decl {
  private:
    std::unique_ptr<Type> type_;
    std::set<qualifier> qualifiers_;

    Decl_var(Location loc, std::string_view identifier, Type* type, std::set<qualifier> qualifiers)
      : Decl(loc, ast_nodes::decl_var, identifier)
      , type_(type)
      , qualifiers_(qualifiers)
    {}

  public:
    Decl_var* create(Location loc, std::string_view identifier, Type* type, std::set<qualifier> qualifiers) {
      return new Decl_var(loc, identifier, type, qualifiers);
    }

    Decl_var* create(Location loc, std::string_view identifier, Type* type) {
      return new Decl_var(loc, identifier, type, { });
    }

    Decl_var* create_dirty(Location loc, std::string_view identifier, Type* type) {
      return new Decl_var(loc, identifier, type, { qualifier::dirty });
    }

    Type* get_type() const {
      return type_.get();
    }

    void set_type(Type* type) {
      type_.reset(type);
    }

    bool is_dirty() const {
      return qualifiers_.find(qualifier::dirty) != qualifiers_.end();
    }
    
    void accept(Visitor& visitor) {
      visitor.visit(this);
    }

  };

  using Decl_var_list = utils::unique_list<Decl_var>;


  /*! \brief Base class for different types of gate declarations */
  class Decl_circuit : public Decl {
  private:
    std::unique_ptr<Decl_var_list> params_;
    std::unique_ptr<Decl_var_list> formals_;

  public:
    Decl_circuit(Location loc,
                 ast_nodes kind,
                 std::string_view identifier,
                 Decl_var_list* params,
                 Decl_var_list* formals)
      : Decl(loc, kind, identifier)
      , params_(params)
      , formals_(formals)
    {}

    ~Decl_circuit() = 0;

    uint32_t num_params() const
    {
      return params_->size();
    }

    Decl_var_list* get_params() const {
      return params_.get();
    }

    uint32_t num_formals() const
    {
      return formals_->size();
    }

    Decl_var_list* get_formals() const {
      return formals_.get();
    }
  };
  Decl_circuit::~Decl_circuit() {};
    
        
    
  /*! \brief Gate declaration */
  class Decl_gate final : public Decl_circuit {
  private:
    std::unique_ptr<Unitary_list> body_;

    Decl_gate(Location loc,
              std::string_view identifier,
              Decl_var_list* params,
              Decl_var_list* formals,
              Unitary_list* body)
      : Decl_circuit(loc, ast_nodes::decl_gate, identifier, params, formals)
      , body_(body)
    {}

  public:
    static Decl_gate* create(Location loc,
                             std::string_view identifier,
                             Decl_var_list* params,
                             Decl_var_list* formals,
                             Unitary_list* body)
    {
      return new Decl_gate(loc, identifier, params, formals, body);
    }

    Unitary_list* get_body() const {
      return body_.get();
    }
    
    void accept(Visitor& visitor) {
      visitor.visit(this);
    }

  };


  /*! \brief Opaque gate declaration */
  class Decl_opaque final : public Decl_circuit {
  private:
    Decl_opaque(Location loc, std::string_view identifier, Decl_var_list* params, Decl_var_list* formals)
      : Decl_circuit(loc, ast_nodes::decl_opaque, identifier, params, formals)
    {}

  public:
    static Decl_opaque* create(Location loc,
                               std::string_view identifier,
                               Decl_var_list* params,
                               Decl_var_list* formals)
    {
      return new Decl_opaque(loc, identifier, params, formals);
    }
    
    void accept(Visitor& visitor) {
      visitor.visit(this);
    }

  };


  /*! \brief Oracle gate declaration */
  class Decl_oracle final : public Decl_circuit {
  private:
    std::string_view filename_;

    Decl_oracle(Location loc, std::string_view identifier, Decl_var_list* formals, std::string_view filename)
      : Decl_circuit(loc, ast_nodes::decl_oracle, identifier, nullptr, formals)
      , filename_(filename)
    {}

  public:
    static Decl_oracle* create(Location loc,
                               std::string_view identifier,
                               Decl_var_list* formals,
                               std::string_view filename)
    {
      return new Decl_oracle(loc, identifier, formals, filename);
    }

    std::string_view filename() const {
      return filename_;
    }
    
    void accept(Visitor& visitor) {
      visitor.visit(this);
    }

  };

} // namespace qasm
} // namespace tweedledee
