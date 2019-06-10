/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "utils/list.hpp"

#include "ast/kinds.hpp"
#include "ast/ast_node.hpp"
#include "ast/expr.hpp"

#include <memory>
#include <string>

namespace synthewareQ {
namespace ast {

  /* Forward declaration */
  class Decl;

  /*! \brief Statement base class */
  class Stmt : public AST_node {
  public:
    Stmt(Location loc, ast_nodes kind)
      : AST_node(loc, kind)
    {}

    virtual ~Stmt() = 0;
  };
  Stmt::~Stmt() {}


  /*! \brief Conditional statement */
  class Stmt_if final : public Stmt {
  private:
    std::unique_ptr<Expr> cond_;
    std::unique_ptr<Stmt> then_;

    Stmt_if(Location loc, Expr* cond, Stmt* then)
      : Stmt(loc, ast_nodes::stmt_if)
      , cond_(cond)
      , then_(then)
    {}

  public:
    static Stmt_if* create(Location loc, Expr* cond, Stmt* then) {
      return new Stmt_if(loc, cond, then);
    }

    Expr* get_cond() const {
      return cond_.get();
    }

    void set_cond(Expr* cond) {
      cond_.reset(cond);
    }

    Stmt* get_then() const {
      return then_.get();
    }

    void set_then(Stmt* then) {
      then_.reset(then);
    }

  };


  /*! \brief Measurement statement */
  class Stmt_measure final : public Stmt {
  private:
    std::unique_ptr<Expr_AP> src_;
    std::unique_ptr<Expr_AP> dest_;

    Stmt_measure(Location loc, Expr_AP* src, Expr_AP* dest)
      : Stmt(loc, ast_nodes::stmt_measure)
      , src_(src)
      , dest_(dest)
    {}

  public:
    static Stmt_measure* create(Location loc, Expr_AP* src, Expr_AP* dest) {
      return new Stmt_measure(loc, src, dest);
    }

    Expr_AP* get_src() const {
      return src_.get();
    }

    void set_src(Expr_AP* src) {
      src_.reset(src);
    }

    Expr_AP* get_dest() const {
      return dest_.get();
    }

    void set_dest(Expr_AP* dest) {
      dest_.reset(dest);
    }
  };


  /*! \brief Reset statement */
  class Stmt_reset final : public Stmt {
  private:
    std::unique_ptr<Expr_AP> arg_;
    
    Stmt_reset(Location loc, Expr_AP* arg)
      : Stmt(loc, ast_nodes::stmt_reset)
      , arg_(arg)
    {}

  public:
    static Stmt_reset* create(Location loc, Expr_AP* arg) {
      return new Stmt_reset(loc, arg);
    }

    Expr_AP* get_arg() const {
      return arg_.get();
    }

    void set_arg(Expr_AP* arg) {
      arg_.reset(arg);
    }

  };

  /*! \brief Declaration statement */
  class Stmt_decl final : public Stmt {
  private:
    std::unique_ptr<Decl> decl_;

    Stmt_decl(Location loc, Decl* decl)
      : Stmt(loc, ast_nodes::stmt_decl)
      , decl_(decl)
    {}

  public:
    static Stmt_decl* create(Location loc, Decl* decl) {
      return new Stmt_decl(loc, decl);
    }

    Decl* get_decl() const {
      return decl_.get();
    }

    void set_decl(Decl* decl) {
      decl_.reset(decl);
    }

  };


  /*! \brief Unitary sub-class */
  class Unitary : public Stmt {
  public:
    Unitary(Location loc, ast_nodes kind)
      : Stmt(loc, kind)
    {}

    virtual ~Unitary() = 0;
  };
  Unitary::~Unitary() {}


  /*! \brief Barrier */
  class Stmt_barrier final : public Unitary {
  private:
    std::unique_ptr<AP_list> args_;

    Stmt_barrier(Location loc, AP_list* args)
      : Unitary(loc, ast_nodes::stmt_barrier)
      , args_(args)
    {}

  public:
    static Stmt_barrier* create(Location loc, AP_list* args) {
      return new Stmt_barrier(loc, args);
    }

    AP_list* get_args() const {
      return args_.get();
    }

  };


  /*! \brief Single-qubit unitary */
  class Stmt_unitary final : public Unitary {
  private:
    std::unique_ptr<Expr> theta_;
    std::unique_ptr<Expr> phi_;
    std::unique_ptr<Expr> lambda_;
    std::unique_ptr<Expr_AP> arg_;

    Stmt_unitary(Location loc, Expr* theta, Expr* phi, Expr* lambda, Expr_AP* arg)
      : Unitary(loc, ast_nodes::stmt_unitary)
      , theta_(theta)
      , phi_(phi)
      , lambda_(lambda)
      , arg_(arg)
    {}

  public:
    static Stmt_unitary* create(Location loc, Expr* theta, Expr* phi, Expr* lambda, Expr_AP* arg)
    {
      return new Stmt_unitary(loc, theta, phi, lambda, arg);
    }

    Expr* get_theta() const {
      return theta_.get();
    }

    void set_theta(Expr* theta) {
      theta_.reset(theta);
    }

    Expr* get_phi() const {
      return phi_.get();
    }

    void set_phi(Expr* phi) {
      phi_.reset(phi);
    }

    Expr* get_lambda() const {
      return lambda_.get();
    }

    void set_lambda(Expr* lambda) {
      lambda_.reset(lambda);
    }

    Expr_AP* get_arg() const {
      return arg_.get();
    }

    void set_arg(Expr_AP* arg) {
      arg_.reset(arg);
    }

  };


  /*! \brief CNOT gate */
  class Stmt_cnot final : public Unitary {
  private:
    std::unique_ptr<Expr_AP> control_;
    std::unique_ptr<Expr_AP> target_;

    Stmt_cnot(Location loc, Expr_AP* control, Expr_AP* target)
      : Unitary(loc, ast_nodes::stmt_cnot)
      , control_(control)
      , target_(target)
    {}
    
  public:
    static Stmt_cnot* create(Location loc, Expr_AP* control, Expr_AP* target) {
      return new Stmt_cnot(loc, control, target);
    }

    Expr_AP* get_control() const {
      return control_.get();
    }

    void set_control(Expr_AP* control) {
      control_.reset(control);
    }

    Expr_AP* get_target() const {
      return target_.get();
    }

    void set_target(Expr_AP* target) {
      target_.reset(target);
    }

  };


  /*! \brief Defined gate */
  class Stmt_gate final : public Unitary {
  private:
    std::string_view name_;
    std::unique_ptr<Expr_list> cargs_;
    std::unique_ptr<AP_list> qargs_;

    Stmt_gate(Location loc, std::string_view name, Expr_list* cargs, AP_list* qargs)
      : Unitary(loc, ast_nodes::stmt_gate)
      , name_(name)
      , cargs_(cargs)
      , qargs_(qargs)
    {}

  public:
    static Stmt_gate* create(Location loc, std::string_view name, Expr_list* cargs, AP_list* qargs) {
      return new Stmt_gate(loc, name, cargs, qargs);
    }

    std::string_view gate_name() const {
      return name_;
    }

    Expr_list* get_cargs() const {
      return cargs_.get();
    }

    AP_list* get_qargs() const {
      return qargs_.get();
    }
    
  };

  using Stmt_list = utils::unique_list<Stmt>;
  using Unitary_list = utils::unique_list<Unitary>;

}
}
