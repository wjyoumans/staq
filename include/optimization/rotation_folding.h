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
 * \file optimization/rotation_folding.h
 * \brief Rotation folding algorithm
 */
#pragma once

#include "ast/visitor.h"
#include "circuits/channel.h"

#include <list>
#include <unordered_map>
#include <sstream>

namespace synthewareQ {
namespace optimization {

  /** 
   * \brief Rotation gate merging algorithm based on arXiv:1903.12456 
   *
   *  Returns a replacement list giving the nodes to the be replaced (or erased)
   */

  // TODO: Add option for global phase correction
  class RotationOptimizer final : public Visitor<RotationOptimizer> {
    using Gatelib = gates::ChannelRepr<VarAccess>;

  public:
    struct config {
      bool correct_global_phase;
    };

    RotationOptimizer() = default;
    RotationOptimizer(const config& params) : Visitor(), config_(params) {}
    ~RotationOptimizer() = default;

    std::unordered_map<int, std::vector<ast::ptr<ast::Gate> > >
    run(ast::ASTNode& node) {
      reset();
      node.accept(*this);
      return replacement_list_;
    }

    /* Variables */
    void visit(ast::VarAccess&) {}

    /* Expressions */
    void visit(ast::BExpr&) {}
    void visit(ast::UExpr&) {}
    void visit(ast::PiExpr&) {}
    void visit(ast::IntExpr&) {}
    void visit(ast::RealExpr&) {}
    void visit(ast::VarExpr&) {}

    /* Statements */
    void visit(ast::MeasureStmt& stmt) {
      auto arg = stringify(stmt.q_arg());
      push_uninterp(UninterpOp({ arg }));
    }
    void visit(ast::ResetStmt& stmt) {
      auto arg = stringify(stmt.arg());
      push_uninterp(UninterpOp({ arg }));
    }
    void visit(ast::IfStmt& stmt) {
      mergeable_ = false;
      stmt.then().accept(*this);
      mergeable_ = true;
    }

    /* Gates */
    void visit(ast::UGate& gate) {
      auto arg = stringify(gate.arg());
      push_uninterp(UninterpOp({ arg }));
    }
    void visit(ast::CNOTGate& gate) {
      auto ctrl = stringify(gate.ctrl());
      auto tgt = stringify(gate.tgt());
      if (mergeable_) {
        current_clifford_ *= CliffordOp::cnot_gate(ctrl, tgt);
      } else {
        push_uninterp(UninterpOp({ ctrl, tgt }));
      }
    }
    void visit(ast::BarrierGate& gate) {
      std::vector<std::string> args;
      gate.foreach_arg([&args, this](auto& arg){ args.emplace_back(stringify(arg)); });

      push_uninterp(UninterpOp(args));
    }
    void visit(ast::DeclaredGate& gate) {
      // TODO: deal with other standard library operations
      auto name = gate.name();
      std::vector<std::string> args;
      gate.foreach_qarg([&args, this](auto& arg){ args.emplace_back(stringify(arg)); });

      if (mergeable_) {
        auto it = args.begin();
        if (name == "cx") current_clifford_ *= CliffordOp::cnot_gate(*it, *(std::next(it)));
        else if (name == "h") current_clifford_ *= CliffordOp::h_gate(*it);
        else if (name == "x") current_clifford_ *= CliffordOp::x_gate(*it);
        else if (name == "y") current_clifford_ *= CliffordOp::y_gate(*it);
        else if (name == "z") current_clifford_ *= CliffordOp::z_gate(*it);
        else if (name == "s") current_clifford_ *= CliffordOp::sdg_gate(*it);
        else if (name == "sdg") current_clifford_ *= CliffordOp::s_gate(*it);
        else if (name == "t") {
          auto gate = RotationOp::t_gate(*it);
          rotation_info info{ gate.uid(), rotation_info::axis::z, &gate.qarg(0) };
          
          accum_.push_back(std::make_pair(info, gate.commute_left(current_clifford_)));
        } else if (name == "tdg") {
          auto gate = RotationOp::tdg_gate(*it);
          rotation_info info{ node, rotation_info::axis::z, &gate.qarg(0) };

          accum_.push_back(std::make_pair(info, gate.commute_left(current_clifford_)));
        } else push_uninterp(UninterpOp(args));
      } else {
        push_uninterp(UninterpOp(args));
      }
    }

    /* Declarations */
    void visit(GateDecl& decl) {
      // Initialize a new local state
      circuit_callback local_state;
      CliffordOp local_clifford;
      std::swap(accum_, local_state);
      std::swap(current_clifford_, local_clifford);

      // Process gate body
      gate.foreach_stmt([this](auto& stmt){ stmt.accept(*this); });
      accum_.push_back(current_clifford_);

      // Fold the gate body
      fold(accum_);

      // Reset the state
      std::swap(accum_, local_state);
      std::swap(current_clifford_, local_clifford);
    }
    void visit(ast::OracleDecl&) {}
    void visit(ast::RegisterDecl&) {}
    void visit(ast::AncillaDecl&) {}

    /* Program */
    void visit(ast::Program& prog) {
      prog.foreach_stmt([this](auto& stmt){ stmt.accept(*this); });
      accum_.push_back(current_clifford_);

      fold(accum_);
    }

  private:
    // Store information necessary for generating a replacement of <node> with a
    // different angle
    struct rotation_info {
      enum class axis {x, y, z};
      
      int uid;
      axis rotation_axis;
      VarExpr* arg;
    };

    using circuit_callback =
      std::list<std::variant<UninterpOp, CliffordOp, std::pair<rotation_info, RotationOp> > >;

    std::unordered_map<ast_node*, ast_node_list> replacement_list_;

    /* Algorithm state */
    circuit_callback accum_;       // The currently accumulating circuit (in channel repr.)
    bool mergeable_ = true;        // Whether we're in a context where a gate can be merged
    CliffordOp current_clifford_; // The current clifford operator
    // Note: current clifford is stored as the dagger of the actual Clifford gate
    // this is so that left commutation (i.e. conjugation) actually right-commutes
    // the rotation gate, allowing us to walk the circuit forwards rather than backwards
    // That is, we want to end up with the form
    //   C_0R_1R_2R_3...R_n
    // rather than the form
    //   R_n...R_3R_2R_1C_0
    // as in the paper

    void reset() {
      replacement_list_.clear();
      accum_.clear();
      mergeable_ = true;
      current_clifford = CliffordOp();
    }

    /* Phase two of the algorithm */
    td::angle fold(circuit_callback& circuit) {
      auto phase = td::angles::zero;

      for (auto it = circuit.rbegin(); it != circuit.rend(); it++) {
        auto& op = *it;
        if (auto tmp = std::get_if<std::pair<rotation_info, RotationOp> >(&op)) {
          auto [new_phase, new_R] = fold_forward(circuit, std::next(it), tmp->second);

          phase += new_phase;
          if (!(new_R == tmp->second)) {
            auto node = tmp->first.node;
            ast_node_list xs;
            auto new_node = new_rotation(tmp->first, new_R.rotation_angle());
            if (new_node) xs.push_back(&node->parent(), new_node);
            replacement_list_[node] = xs;
          }
        }
      }

      return phase;
    }

    std::pair<td::angle, RotationOp>
    fold_forward(circuit_callback& circuit, circuit_callback::reverse_iterator it, RotationOp R)
    {
      // Tries to commute op backward as much as possible, merging with applicable
      // gates and deleting them as it goes
      // Note: We go backwards so that we only commute **left** past C^*/**right** past C

      auto phase = td::angles::zero;
      bool cont = true;

      for (; cont && it != circuit.rend(); it++) {
        auto visitor = utils::overloaded {
          [this, it, &R, &phase, &circuit](std::pair<rotation_info, RotationOp>& Rop) {
              auto res = R.try_merge(Rop.second);
              if (res.has_value()) {
                auto &[new_phase, new_R] = res.value();
                phase += new_phase;
                R = new_R;

                // Delete R in circuit & the node
                replacement_list_[Rop.first.node] = ast_node_list();
                circuit.erase(std::next(it).base());

                return false;
              } else if (R.commutes_with(Rop.second)) {
                return true;
              } else {
                return false;
              }
            },
            [&R](CliffordOp& C) {
              R = R.commute_left(C); return true;
            },
            [&R](UninterpOp& U) {
              if (!R.commutes_with(U)) return false;
              else return true;
            }
        };

        cont = std::visit(visitor, *it);
      }

      return std::make_pair(phase, R);
    }

    /* Utilities */
    void push_uninterp(UninterpOp op) {
      accum_.push_back(current_clifford_);
      accum_.push_back(op);
      // Clear the current clifford
      current_clifford_ = CliffordOp();
    }

    /*! \brief Returns a qasm expr node with the value of the given angle */
    // TODO: Find a better home for this, it's duplicated in logic synthesis
    ast_node* angle_to_expr(ast_context* ctx_, uint32_t location, tweedledum::angle theta) {
      auto sval = theta.symbolic_value();
      if (sval == std::nullopt) {
        // Angle is real-valued
        return expr_real::create(ctx_, location, theta.numeric_value());
      } else {
        // Angle is of the form pi*(a/b) for a & b integers
        // NOTE: tweedledum::gate base and tweedledum::angle contradict -- the former defines t as
        //       an angle of 1/4, while the latter gives it as an angle of 1/8.
        auto [a, b] = sval.value();

        if (a == 0) {
          return expr_integer::create(ctx_, location, 0);
        } else if (a == 1) {
          auto total = expr_binary_op::builder(ctx_, location, binary_ops::division);
          total.add_child(expr_pi::create(ctx_, location));
          total.add_child(expr_integer::create(ctx_, location, b));

          return total.finish();
        } else if (a == -1) {
          auto numerator = expr_unary_op::builder(ctx_, location, unary_ops::minus);
          numerator.add_child(expr_pi::create(ctx_, location));

          auto total = expr_binary_op::builder(ctx_, location, binary_ops::division);
          total.add_child(numerator.finish());
          total.add_child(expr_integer::create(ctx_, location, b));

          return total.finish();
        } else {
          auto numerator = expr_binary_op::builder(ctx_, location, binary_ops::multiplication);
          numerator.add_child(expr_integer::create(ctx_, location, a));
          numerator.add_child(expr_pi::create(ctx_, location));

          auto total = expr_binary_op::builder(ctx_, location, binary_ops::division);
          total.add_child(numerator.finish());
          total.add_child(expr_integer::create(ctx_, location, b));

          return total.finish();
        }
      }
    }

    // Assumes basic gates (x, y, z, s, sdg, t, tdg, rx, ry, rz) are defined
    ast_node* new_rotation(const rotation_info& rinfo, const td::angle& theta) {
      auto location = rinfo.node->location();

      // Determine the gate
      std::string name;
      list_exprs* cargs = nullptr;
      
      auto sval = theta.symbolic_value();
      if (sval == std::nullopt) {
        // Angle is real-valued
        auto val = theta.numeric_value();

        if (val == 0) return nullptr;

        switch (rinfo.rotation_axis) {
        case rotation_info::axis::x:
          name = "rx";
          break;
        case rotation_info::axis::y:
          name = "ry";
          break;
        case rotation_info::axis::z:
          name = "rz";
          break;
        }

        auto carg_builder = list_exprs::builder(ctx_, location);
        carg_builder.add_child(angle_to_expr(ctx_, location, theta));
        cargs = carg_builder.finish();
      } else {
        // Angle is of the form pi*(a/b) for a & b integers
        auto [num, denom] = sval.value();

        if (num == 0) return nullptr;
        switch (rinfo.rotation_axis) {
        case rotation_info::axis::x:
          if ((num == 1) && (denom == 1)) {
            // X gate
            name = "x";
          } else {
            // Rx gate
            name = "rx";

            auto carg_builder = list_exprs::builder(ctx_, location);
            carg_builder.add_child(angle_to_expr(ctx_, location, theta));
            cargs = carg_builder.finish();
          }
          break;
        case rotation_info::axis::y:
          if ((num == 1) && (denom == 1)) {
            // Y gate
            name = "y";
          } else {
            // Ry gate
            name = "ry";

            auto carg_builder = list_exprs::builder(ctx_, location);
            carg_builder.add_child(angle_to_expr(ctx_, location, theta));
            cargs = carg_builder.finish();
          }
          break;
        case rotation_info::axis::z:
          if ((num == 1) && (denom == 1)) {
            // Z gate
            name = "z";
          } else if (((num == 1) || (num == -3)) && (denom == 2)) {
            // S gate
            name = "s";
          } else if (((num == -1) || (num == 3)) && (denom == 2)) {
            // Sdg gate
            name = "sdg";
          } else if (((num == 1) || (num == -7)) && (denom == 4)) {
            // T gate
            name = "t";
          } else if (((num == -1) || (num == 7)) && (denom == 4)) {
            // Tdg gate
            name = "tdg";
          } else {
            // Rz gate
            name = "rz";

            auto carg_builder = list_exprs::builder(ctx_, location);
            carg_builder.add_child(angle_to_expr(ctx_, location, theta));
            cargs = carg_builder.finish();
          }
          break;
        }
      }
      auto stmt_builder = stmt_gate::builder(ctx_, location, name);
      if (cargs != nullptr) stmt_builder.add_cargs(cargs);

      // Quantum argument
      auto qarg_builder = list_aps::builder(ctx_, location);
      if (rinfo.arg->kind() == ast_node_kinds::expr_var) {
        auto var = static_cast<expr_var*>(rinfo.arg);
        
        qarg_builder.add_child(expr_var::build(ctx_, var->location(), var->id()));
      } else if (rinfo.arg->kind() == ast_node_kinds::expr_reg_offset) {
        auto expr = static_cast<expr_reg_offset*>(rinfo.arg);

        // Get numeric value of index
        if (expr->index().kind() != ast_node_kinds::expr_integer) {
          throw std::logic_error("AST invalid -- deference offset not an integer");
        }
        auto index = static_cast<expr_integer*>(&expr->index());
        auto index_copy = expr_integer::create(ctx_, index->location(), index->evaluate());
        
        qarg_builder.add_child(expr_reg_offset::build(ctx_, expr->location(), expr->id(), index_copy));
      }
      stmt_builder.add_qargs(qarg_builder.finish());

      return stmt_builder.finish();
    }

    std::string stringify(VarExpr& var) {
      std::stringstream ss;
      ss << var;

      return ss.str();
    }

  };

  void fold_rotations(ASTNode& node) {
    RotationOptimizer optimizer;

    auto res = alg.run(ctx);
    bulk_replace(ctx, res);
  }

}
