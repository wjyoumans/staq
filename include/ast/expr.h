/*
 * This file is part of synthewareQ.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * \file ast/expr.h
 * \brief openQASM expressions
 */
#pragma once

#include "base.h"

namespace synthewareQ {
namespace ast {

  /**
   * \brief Enum of binary operators
   */
  enum class BinaryOp { Plus, Minus, Times, Divide, Pow };
  std::ostream& operator<<(std::ostream& os, const BinaryOp& bop) {
      switch (bop) {
      case BinaryOp::Plus: os << "+"; break;
      case BinaryOp::Minus: os << "-"; break;
      case BinaryOp::Times: os << "*"; break;
      case BinaryOp::Divide: os << "/"; break;
      case BinaryOp::Pow: os << "^"; break;
      }
      return os;
  }

  /**
   * \brief Enum of unary operators
   */
  enum class UnaryOp { Neg, Sin, Cos, Tan, Ln, Sqrt, Exp };
  std::ostream& operator<<(std::ostream& os, const UnaryOp& uop) {
    switch (uop) {
    case UnaryOp::Neg: os << "-"; break;
    case UnaryOp::Sin: os << "sin"; break;
    case UnaryOp::Cos: os << "cos"; break;
    case UnaryOp::Tan: os << "tan"; break;
    case UnaryOp::Ln: os << "ln"; break;
    case UnaryOp::Sqrt: os << "sqrt"; break;
    case UnaryOp::Exp: os << "exp"; break;
    }
    return os;
  }

  /**
   * \class synthewareQ::ast::Expr
   * \brief Base class for openQASM expressions
   */
  class Expr : public ASTNode {
  public:
    Expr(parser::Position pos) : ASTNode(pos) {}
    virtual ~Expr() = default;
    virtual Expr* clone() const = 0;

    /**
     * \brief Internal pretty-printer with associative context 
     *
     * \param ctx Whether the current associative context is ambiguous
     */
    virtual std::ostream& pretty_print(std::ostream& os, bool ctx) const = 0;
    std::ostream& pretty_print(std::ostream& os) const override {
      return pretty_print(os, false);
    }
  };

  /**
   * \class synthewareQ::ast::BExpr
   * \brief Class for binary operator expressions
   * \see synthewareQ::ast::Expr
   */
  class BExpr final : public Expr {
    ptr<Expr> lexp_; ///< the left sub-expression
    BinaryOp op_; ///< the binary operator
    ptr<Expr> rexp_; ///< the right sub-expression

  public:
    BExpr(parser::Position pos, ptr<Expr> lexp, BinaryOp op, ptr<Expr> rexp)
      : Expr(pos)
      , lexp_(std::move(lexp))
      , op_(op)
      , rexp_(std::move(rexp))
    {}

    BinaryOp op() const { return op_; }
    Expr& lexp() { return *lexp_; }
    Expr& rexp() { return *rexp_; }
    void set_lexp(ptr<Expr> exp) { lexp_ = std::move(exp); }
    void set_rexp(ptr<Expr> exp) { rexp_ = std::move(exp); }

    void accept(Visitor& visitor) override { visitor.visit(this); }
    std::ostream& pretty_print(std::ostream& os, bool ctx) const override {
      if (ctx) {
        os << "(";
        lexp_->pretty_print(os, true);
        os << op_;
        rexp_->pretty_print(os, true);
        os << ")";
      } else {
        lexp_->pretty_print(os, true);
        os << op_;
        rexp_->pretty_print(os, true);
      }

      return os;
    }
    BExpr* clone() const override {
      return new BExpr(pos_, ptr<Expr>(lexp_->clone()), op_, ptr<Expr>(rexp_->clone()));
    }

  };


  /**
   * \class synthewareQ::ast::UExpr
   * \brief Class for unary operator expressions
   * \see synthewareQ::ast::Expr
   */
  class UExpr final : public Expr {
    UnaryOp op_; ///< the unary operator
    ptr<Expr> exp_; ///< the sub-expression

  public:
    UExpr(parser::Position pos, UnaryOp op, ptr<Expr> exp)
      : Expr(pos)
      , op_(op)
      , exp_(std::move(exp))
    {}

	UnaryOp op() const { return op_; }
    Expr& exp() { return *exp_; }
    void set_exp(ptr<Expr> exp) { exp_ = std::move(exp); }
    
    void accept(Visitor& visitor) override { visitor.visit(this); }
    std::ostream& pretty_print(std::ostream& os, bool ctx) const override {
      (void)ctx;
      
      os << op_;
      if (op_ == UnaryOp::Neg)
        exp_->pretty_print(os, true);
      else {
        os << "(";
        exp_->pretty_print(os, false);
        os << ")";
      }

      return os;
    }
    UExpr* clone() const override {
      return new UExpr(pos_, op_, ptr<Expr>(exp_->clone()));
    }
  };


  /**
   * \class synthewareQ::ast::PiExpr
   * \brief Class for pi constants
   * \see synthewareQ::ast::Expr
   */
  class PiExpr final : public Expr {
    
  public:
    PiExpr(parser::Position pos) : Expr(pos) {}

    void accept(Visitor& visitor) override { visitor.visit(this); }
    std::ostream& pretty_print(std::ostream& os, bool ctx) const override {
      (void)ctx;
      
      os << "pi";
      return os;
    }
    PiExpr* clone() const override {
      return new PiExpr(pos_);
    }
  };


  /**
   * \class synthewareQ::ast::IntExpr
   * \brief Class for integer literal expressions
   * \see synthewareQ::ast::Expr
   */
  class IntExpr final : public Expr {
    int value_; ///< the integer value

  public:
    IntExpr(parser::Position pos, int value) : Expr(pos), value_(value) {}

    int value() const { return value_; }
    
    void accept(Visitor& visitor) override { visitor.visit(this); }
    std::ostream& pretty_print(std::ostream& os, bool ctx) const override {
      (void)ctx;
      
      os << value_;
      return os;
    }
    IntExpr* clone() const override {
      return new IntExpr(pos_, value_);
    }
  };


  /**
   * \class synthewareQ::ast::RealExpr
   * \brief Class for floating point literal expressions
   * \see synthewareQ::ast::Expr
   */
  class RealExpr final : public Expr {
    double value_; ///< the floating point value

  public:
    RealExpr(parser::Position pos, double value) : Expr(pos), value_(value) {}

    double value() const { return value_; }
    
    void accept(Visitor& visitor) override { visitor.visit(this); }
    std::ostream& pretty_print(std::ostream& os, bool ctx) const override {
      (void)ctx;
      
      os << value_;
      return os;
    }
    RealExpr* clone() const override {
      return new RealExpr(pos_, value_);
    }
  };

  /**
   * \class synthewareQ::ast::VarExpr
   * \brief Class for variable expressions
   * \see synthewareQ::ast::Expr
   */
  class VarExpr final : public Expr {
    symbol var_;                ///< the identifier
    std::optional<int> offset_; ///< optional offset into a register variable

  public:
    VarExpr(parser::Position pos, symbol var, std::optional<int> offset = std::nullopt)
      : Expr(pos)
      , var_(var)
      , offset_(offset)
    {}
    
    const symbol& var() const { return var_; }
    std::optional<int> offset() const { return offset_; }

    void accept(Visitor& visitor) override { visitor.visit(this); }
    std::ostream& pretty_print(std::ostream& os, bool ctx) const override {
      (void)ctx;
      
      os << var_;
      if (offset_) os << "[" << *offset_ << "]";
      return os;
    }
    VarExpr* clone() const override {
      return new VarExpr(pos_, var_, offset_);
    }
  };

}
}
