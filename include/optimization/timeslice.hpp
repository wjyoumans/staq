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
 * \brief Gate reorganization optimization using commutativity rules
 */

#pragma once

#include "qasmtools/ast/visitor.hpp"
#include "qasmtools/ast/replacer.hpp"

#include <tuple>

namespace staq {
namespace optimization {

namespace ast = qasmtools::ast;

/**
 * \brief Gate reorganization optimization using commutativity rules
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
    void visit(ast::CNOTGate& gate) {}
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
    std::unordered_map<
        ast::VarAccess,
        std::tuple<std::string, std::vector<ast::VarAccess>, int>>
        last_;

    void reset() {
        last_.clear();
        mergeable_ = true;
    }
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
