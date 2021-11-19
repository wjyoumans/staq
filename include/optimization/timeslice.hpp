/*
 * This file is part of staq.
 *
 * Copyright (c) 2019 - 2021 softwareQ Inc. All rights reserved.
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
 * \file optimization/timeslice.hpp
 * \brief Gate reorganization using commutativity rules
 */

#pragma once

#include "qasmtools/ast/visitor.hpp"
#include "qasmtools/ast/replacer.hpp"
//#include "qasmtools/parser/position.hpp"

#include <tuple>

namespace staq {
namespace optimization {

namespace ast = qasmtools::ast;

/**
 * \brief Gate reorganization using commutativity rules
 */

class Timeslicer final : public ast::Visitor {
  public:
    struct config {
        bool simplify = true;
    };

    Timeslicer() = default;
    Timeslicer(const config& params) : Visitor(), config_(params) {}
    ~Timeslicer() = default;

    // TODO
    void run(ast::ASTNode& node) {
    	reset();
        node.accept(*this);
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
    // TODO
    void visit(ast::MeasureStmt&) {}
    // TODO
    void visit(ast::ResetStmt&) {}
    // TODO: is this correct?
    void visit(ast::IfStmt& stmt) {
        mergeable_ = false;
        stmt.then().accept(*this);
        mergeable_ = true;
    }

    /* Gates */
    // TODO: Special rules for UGates with parameters that match named gates (x gate etc.)?
    void visit(ast::UGate&) {}
    void visit(ast::CNOTGate& gate) {
        auto ctrl = gate.ctrl();
        auto tgt = gate.tgt();

        if (mergeable_) {
            // loop through timeslices in reverse looking for best fit
            int best = -1;
            int uid;

            for (int i = timeslices_.size() - 1; i >= 0; i--) {
                // no conflict by default
                int conflict = 0;

                // conflict needs investigation
                if (timeslices_[i].find(ctrl) != timeslices_[i].end() ||
                        timeslices_[i].find(tgt) != timeslices_[i].end()) {
                    
                    // unresolveable conflict by default
                    conflict = 1;

                    auto [name1, args1, pos1, uid1] = timeslices_[i][ctrl];
                    auto [name2, args2, pos2, uid2] = timeslices_[i][tgt];

                    // gates commute, resolveable conflict
                    if (uid1 == uid2 && name1 == "cx" && 
                        args1 == std::vector<ast::VarAccess>({tgt, ctrl})) {
                        conflict = -1;
                    }
                }

                if (conflict == 0) { 
                    best=i; 
                } else if (conflict == 1) {
                    break;
                }
                // if conflict == -1 we keep looking
            }

            // move gate to best timeslice
            if (best >= 0) {
                timeslices_[best][ctrl] = {"cx", {ctrl, tgt}, gate.pos(), gate.uid()};
                timeslices_[best][tgt] = {"cx", {ctrl, tgt}, gate.pos(), gate.uid()};
                
                /* How to update the AST? This is an attempt to replace an arbitrary gate in the 
                 * 'best' timeslice with itself plus the new CX gate.

                auto [name, args, pos, uid] = std::begin(timeslices_[best])->second;
               
                std::list<ast::ptr<ast::Gate>> tmp;
                tmp.emplace_back(
                    new ast::CNOTGate(pos, ctrl, tgt));
                tmp.emplace_back(std::move(gate));
                
                replacement_list_[gate.uid()] = std::move(std::list<ast::ptr<ast::Gate>>());
                replacement_list_[uid] = std::move(tmp);
                */
                return;
            }
        }

        // put gate in new timeslice
        std::unordered_map<
            ast::VarAccess, 
            std::tuple<
                std::string, 
                std::vector<ast::VarAccess>, 
                qasmtools::parser::Position, 
                int>> temp = {
            {ctrl, {"cx", {ctrl, tgt}, gate.pos(), gate.uid()}},
            {tgt, {"cx", {ctrl, tgt}, gate.pos(), gate.uid()}},
        };
        timeslices_.push_back(temp);
    }
	
    void visit(ast::BarrierGate& gate) {}
    void visit(ast::DeclaredGate& gate) {}

    /* Declarations */
    void visit(ast::GateDecl& decl) {}
    void visit(ast::OracleDecl&) {}
    void visit(ast::RegisterDecl&) {}
    void visit(ast::AncillaDecl&) {}

    /* Program */
    void visit(ast::Program& prog) {
        prog.foreach_stmt([this](auto& stmt) { stmt.accept(*this); });
    }

  private:
    config config_;
    bool mergeable_;
    std::vector<std::unordered_map<
        ast::VarAccess,
        std::tuple<std::string, std::vector<ast::VarAccess>, qasmtools::parser::Position, int>>>
        timeslices_;
    std::unordered_map<int, std::list<ast::ptr<ast::Gate>>> replacement_list_;

    void reset() {
        replacement_list_.clear();
        timeslices_.clear();
        mergeable_ = true;
    }

    /* reconstruct ptr<Gate> with something like this?
    ast::ptr<ast::DeclaredGate> generate_cnot(int i, int j) {
        parser::Position pos;
        std::string name = "cx";
        std::vector<ast::ptr<ast::Expr>> cargs;
        std::vector<ast::VarAccess> qargs{(map_qubit_.find(i))->second,
                                          (map_qubit_.find(j))->second};

        return std::make_unique<ast::DeclaredGate>(pos, name, std::move(cargs),
                                                   std::move(qargs));
    }*/
};

inline void timeslice(ast::ASTNode& node) {
    Timeslicer optimizer;
    optimizer.run(node);
}

inline void timeslice(ast::ASTNode& node, const Timeslicer::config& params) {
    Timeslicer optimizer(params);
    optimizer.run(node);
}

} // namespace optimization
} // namespace staq
