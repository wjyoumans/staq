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
 * \file circuits/channel.h
 * \brief Gates in the channel representation
 */
#pragma once

#include "utils/angle.h"

#include <unordered_map>
#include <map>
#include <list>
#include <variant>
#include <iostream>

namespace synthewareQ {
namespace circuits {
  /*! \brief Utilities for the channel representation of Clifford + single qubit gates */

  using symbol = std::string;
  
  /* Main declarations */
  class RotationOp;
  class CliffordOp;
  class UninterpOp;
  typedef std::variant<RotationOp, CliffordOp, UninterpOp> channel_op;


  /* The single qubit Pauli group and operations on it */
  enum class Pauli  : unsigned short { i = 0, x = 1, z = 2, y = 3 };
  enum class IPhase : unsigned short { zero = 0, one = 1, two = 2, three = 3 };


  inline Pauli operator*(const Pauli& p, const Pauli& q) {
    return static_cast<Pauli>(static_cast<unsigned short>(p) ^ static_cast<unsigned short>(q));
  }
  inline Pauli& operator*=(Pauli& p, const Pauli& q) {
    p = p * q;
    return p;
  }
  std::ostream& operator<<(std::ostream& os, const Pauli& p) {
    switch(p) {
    case Pauli::i: os << "I"; break;
    case Pauli::x: os << "X"; break;
    case Pauli::z: os << "Z"; break;
    case Pauli::y: os << "Y"; break;
    }

    return os;
  }

  inline IPhase operator*(const IPhase& a, const IPhase& b) {
    return static_cast<IPhase>((static_cast<unsigned short>(a) + static_cast<unsigned short>(b)) % 4);
  }
  inline IPhase operator*=(IPhase& a, const IPhase& b) {
    a = a * b;
    return a;
  }
  std::ostream& operator<<(std::ostream& os, const IPhase& p) {
    switch(p) {
    case IPhase::zero: os << ""; break;
    case IPhase::one: os << "i"; break;
    case IPhase::two: os << "-"; break;
    case IPhase::three: os << "-i"; break;
    }

    return os;
  }

  inline IPhase normal_phase(const Pauli& p, const Pauli& q) {
    static IPhase phase_mult_table[16] = {
      IPhase::zero, // II
      IPhase::zero, // XI
      IPhase::zero, // ZI
      IPhase::zero, // YI
      IPhase::zero, // IX
      IPhase::zero, // XX
      IPhase::one, // ZX
      IPhase::three, // YX
      IPhase::zero, // IZ
      IPhase::three, // XZ
      IPhase::zero, // ZZ
      IPhase::one, // YZ
      IPhase::zero, // IY
      IPhase::one, // XY
      IPhase::three, // ZY
      IPhase::zero // YY
    };
    
    auto idx = (static_cast<unsigned short>(p) | (static_cast<unsigned short>(q) << 2));
    return phase_mult_table[idx % 16];
  }

  inline bool paulis_commute (const Pauli& p, const Pauli& q) {
    static bool commute_table[16] = {
      true, // II
      true, // XI
      true, // ZI
      true, // YI
      true, // IX
      true, // XX
      false, // ZX
      false, // YX
      true, // IZ
      false, // XZ
      true, // ZZ
      false, // YZ
      true, // IY
      false, // XY
      false, // ZY
      true // YY
    };
    
    auto idx = (static_cast<unsigned short>(p) | (static_cast<unsigned short>(q) << 2));
    return commute_table[idx % 16];
  }

  /*! \brief Class representing an n-qubit pauli operator */
  class PauliOp {
  public:
    PauliOp() {}
    PauliOp(std::pair<symbol, Pauli> gate) { pauli_[gate.first] = gate.second; }
    PauliOp(std::unordered_map<symbol, Pauli> pauli) : pauli_(pauli) {}

    /* Gate constructors */
    static PauliOp i_gate(symbol q) { return PauliOp({{ q, Pauli::i }}); }
    static PauliOp x_gate(symbol q) { return PauliOp({{ q, Pauli::x }}); }
    static PauliOp z_gate(symbol q) { return PauliOp({{ q, Pauli::z }}); }
    static PauliOp y_gate(symbol q) { return PauliOp({{ q, Pauli::y }}); }

    /* Accessors */
    IPhase phase() const { return phase_; }

    template<typename Fn>
    void for_each(Fn&& fn) const {
      static_assert(std::is_invocable_r_v<void, Fn, std::pair<symbol, Pauli> const&>);
      for (auto& p : pauli_) fn(p);
    }

    /* Operators */
    PauliOp& operator*=(const IPhase& phase) {
      phase_ *= phase;
      return *this;
    }
    PauliOp operator*(const IPhase& phase) const {
      auto tmp_(*this);
      tmp_ *= phase;
      return tmp_;
    }

    PauliOp& operator*=(const PauliOp& P) {
      phase_ *= P.phase_;
      for (auto& [q, p] : P.pauli_) {
        phase_ *= normal_phase(pauli_[q], p);
        pauli_[q] *= p;
      }
      return *this;
    }
    PauliOp operator*(const PauliOp& P) const {
      auto tmp_(*this);
      tmp_ *= P;
      return tmp_;
    }

    PauliOp operator-() const {
      return (*this) * IPhase::two;
    }

    bool operator==(const PauliOp& P) const {
      if (phase_ != P.phase_) return false;
      
      for (auto& [q, p] : P.pauli_) {
        auto it = pauli_.find(q);
        auto tmp = it == pauli_.end() ? Pauli::i : it->second;

        if (tmp != p) return false;
      }

      for (auto& [q, p] : pauli_) {
        auto it = P.pauli_.find(q);
        auto tmp = it == P.pauli_.end() ? Pauli::i : it->second;

        if (tmp != p) return false;
      }

      return true;
    }

    bool commutes_with(const PauliOp& P) const {
      uint32_t tot_anti = 0;
      
      for (auto& [q, p] : P.pauli_) {
        auto it = pauli_.find(q);
        if (it != pauli_.end() && !paulis_commute(it->second, p)) tot_anti++;
      }

      return (tot_anti % 2) == 0;
    }

    bool trivial_on(const symbol q) const {
      auto it = pauli_.find(q);
      if (it == pauli_.end() || it->second == Pauli::i) return true;
      return false;
    }

    bool is_z() const {
      for (auto& [q, p] : pauli_) {
        if ((p != Pauli::i) && (p != Pauli::z)) return false;
      }
      return true;
    }

    /* Printing */
    std::ostream& print(std::ostream& os) const {
      os << phase_;
      for (auto& [q, p] : pauli_) {
        os << p << "(" << q << ")";
      }
      return os;
    }

  private:
    std::unordered_map<symbol, Pauli> pauli_;
    IPhase phase_ = IPhase::zero;

  };

  std::ostream& operator<<(std::ostream& os, const PauliOp& P) { return P.print(os); }


  /*! \brief Class representing an n-qubit Clifford operator as the normalizer of the Pauli group
   *
   *  Cliffords are represented via a sparse mapping from a (non-minimal) set of generators of 
   *  the n-qubit Pauli group to an n-qubit Pauli operator, defined by permutation of the Pauli
   *  group under conjugation -- i.e. CPC^* = CP_1C^*CP_2C^*... 
   *  
   *  Note: no mapping means the operator acts trivially on that generator */
  class CliffordOp {
  public:
    CliffordOp() {}
    CliffordOp(std::map<std::pair<symbol, Pauli>, PauliOp> perm) : perm_(perm) {}

    /* Gate constructors */
    static CliffordOp h_gate(symbol q) {
      return CliffordOp(
        { { std::make_pair(q, Pauli::x), PauliOp::z_gate(q) },
          { std::make_pair(q, Pauli::z), PauliOp::x_gate(q) },
          { std::make_pair(q, Pauli::y), -(PauliOp::y_gate(q)) } });
    }
    static CliffordOp s_gate(symbol q) {
      return CliffordOp(
        { { std::make_pair(q, Pauli::x), PauliOp::y_gate(q) },
          { std::make_pair(q, Pauli::y), -(PauliOp::x_gate(q)) } });
    }
    static CliffordOp sdg_gate(symbol q) {
      return CliffordOp(
        { { std::make_pair(q, Pauli::x), -(PauliOp::y_gate(q)) },
          { std::make_pair(q, Pauli::y), PauliOp::x_gate(q) } });
    }
    static CliffordOp cnot_gate(symbol q1, symbol q2) {
      return CliffordOp(
        { { std::make_pair(q1, Pauli::x), PauliOp::x_gate(q1) * PauliOp::x_gate(q2) },
          { std::make_pair(q2, Pauli::z), PauliOp::z_gate(q1) * PauliOp::z_gate(q2) },
          { std::make_pair(q1, Pauli::y), PauliOp::y_gate(q1) * PauliOp::x_gate(q2) },
          { std::make_pair(q2, Pauli::y), PauliOp::z_gate(q1) * PauliOp::y_gate(q2) } });
    }
    static CliffordOp x_gate(symbol q) {
      return CliffordOp(
        { { std::make_pair(q, Pauli::z), -(PauliOp::z_gate(q)) },
          { std::make_pair(q, Pauli::y), -(PauliOp::y_gate(q)) } });
    }
    static CliffordOp z_gate(symbol q) {
      return CliffordOp(
        { { std::make_pair(q, Pauli::x), -(PauliOp::x_gate(q)) },
          { std::make_pair(q, Pauli::y), -(PauliOp::y_gate(q)) } });
    }
    static CliffordOp y_gate(symbol q) {
      return CliffordOp(
        { { std::make_pair(q, Pauli::x), -(PauliOp::x_gate(q)) },
          { std::make_pair(q, Pauli::z), -(PauliOp::z_gate(q)) } });
    }

    /* Operators */
    PauliOp conjugate(const PauliOp& P) const {
      PauliOp ret;
      ret *= P.phase();
      
      P.for_each([&ret, this](auto& p) {
          auto it = this->perm_.find(p);
          if (it == perm_.end()) {
            ret *= PauliOp(p);
          } else {
            ret *= it->second;
          }
        });

      return ret;
    }

    CliffordOp& operator*=(const CliffordOp& C) {
      *this = *this * C;
      return *this;
    }
    CliffordOp operator*(const CliffordOp& C) {
      CliffordOp ret(*this);
      for (auto& [pauli_in, pauli_out] : C.perm_) {
        ret.perm_[pauli_in] = conjugate(pauli_out);
      }
      return ret;
    }

    /* Printing */
    std::ostream& print(std::ostream& os) const {
      os << "{ ";
      for (auto& [pauli_in, pauli_out] : perm_) {
        os << PauliOp(pauli_in) << " --> " << pauli_out << ", ";
      }
      os << "}";

      return os;
    }

  private:
    std::map<std::pair<symbol, Pauli>, PauliOp> perm_;

  };

  std::ostream& operator<<(std::ostream& os, const CliffordOp& P) { return P.print(os); }


  /*! \brief Class storing an uninterpreted operation on some set of qubits */
  class UninterpOp {
  public:
    UninterpOp(std::list<symbol> qubits) : qubits_(qubits) {}

    template<typename Fn>
    void for_each_qubit(Fn&& fn) const {
      static_assert(std::is_invocable_r_v<void, Fn, symbol const&>);
      for (auto& q : qubits_) fn(q);
    }

    /* Printing */
    std::ostream& print(std::ostream& os) const {
      os << "U(";
      for (auto& q : qubits_) os << q << ",";
      os << ")";

      return os;
    }

  private:
    std::list<symbol> qubits_;
  };
    
  std::ostream& operator<<(std::ostream& os, const UninterpOp& P) { return P.print(os); }


  /*! \brief Class storing a rotation of some angle around a pauli
   *
   *  (1 + e^i\theta)/2 I + (1 - e^i\theta) P
   */
  class RotationOp {
  public:
    RotationOp() : theta_(utils::angles::zero) {}
    RotationOp(utils::Angle theta, PauliOp pauli) : theta_(theta), pauli_(pauli) {}

    /* Gate constructors */
    static RotationOp t_gate(symbol q) { return RotationOp(utils::angles::pi_quarter, PauliOp::z_gate(q)); }
    static RotationOp tdg_gate(symbol q) { return RotationOp(-utils::angles::pi_quarter, PauliOp::z_gate(q)); }
    static RotationOp rz_gate(utils::Angle theta, symbol q) { return RotationOp(theta, PauliOp::z_gate(q)); }
    static RotationOp rx_gate(utils::Angle theta, symbol q) { return RotationOp(theta, PauliOp::x_gate(q)); }
    static RotationOp ry_gate(utils::Angle theta, symbol q) { return RotationOp(theta, PauliOp::y_gate(q)); }

    /* Accessors */
    utils::Angle rotation_angle() { return theta_; }

    /* Operators */

    // CR(theta, P) == R(theta, P')C
    RotationOp commute_left(const CliffordOp& C) const {
      auto tmp = RotationOp(*this);
      tmp.pauli_ = C.conjugate(tmp.pauli_);
      return tmp;
    }

    bool operator==(const RotationOp& R) const {
      return (theta_ == R.theta_) && (pauli_ == R.pauli_);
    }

    bool commutes_with(const RotationOp& R) const {
      return pauli_.commutes_with(R.pauli_);
    }

    bool commutes_with(const UninterpOp& U) const {
      auto tmp = true;

      U.for_each_qubit([&tmp, this](symbol q) {
          tmp &= pauli_.trivial_on(q);
        });

      return tmp;
    }

    std::optional<std::pair<utils::Angle, RotationOp> > try_merge(const RotationOp& R) const {
      if (pauli_ == R.pauli_) {
        auto phase = utils::angles::zero;
        auto rotation = RotationOp(theta_ + R.theta_, pauli_);
        return std::make_optional(std::make_pair(phase, rotation));
      } else if (pauli_ == -(R.pauli_)) {
        auto phase = R.theta_;
        auto rotation = RotationOp(theta_ + -R.theta_, pauli_);
        return std::make_optional(std::make_pair(phase, rotation));
      } else {
        return std::nullopt;
      }
    }

    bool is_z_rotation() const {
      return pauli_.is_z();
    }

    /* Printing */
    std::ostream& print(std::ostream& os) const {
      os << "R(" << theta_ << ", " << pauli_ << ")";

      return os;
    }

  private:
    utils::Angle theta_;
    PauliOp pauli_;

  };

  std::ostream& operator<<(std::ostream& os, const RotationOp& P) { return P.print(os); }

}
}
