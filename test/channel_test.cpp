#include "circuits/channel.h"
#include <unordered_map>
#include <set>
#include <iostream>

using namespace synthewareQ::circuits;

void print_merge(const RotationOp& R1, const RotationOp& R2) {
  auto merged = R1.try_merge(R2);
  if (merged.has_value()) {
    auto& [phase, R] = merged.value();
    std::cout << "e^i(" << phase << ")" << R;
  } else {
    std::cout << R1 << R2;
  }
}


int main() {
  PauliOp empty;
  auto a = PauliOp::i_gate("x1");
  auto b = PauliOp::x_gate("x1");
  auto c = PauliOp::z_gate("x1");
  auto d = PauliOp::y_gate("x1");

  std::cout << a << "*" << b << "*" << c << "*" << d << "=" << a*b*c*d << "\n";
  std::cout << "\n";

  std::cout << "X = I? " << (b == empty ? "true" : "false") << "\n";
  std::cout << "XX = I? " << ((b * b) == empty ? "true" : "false") << "\n";
  std::cout << "XZY = I? " << ((b * c * d) == empty ? "true" : "false") << "\n";

  std::cout << "\n";

  auto x1 = PauliOp::x_gate("x1");
  auto x2 = PauliOp::x_gate("x2");
  auto z1 = PauliOp::z_gate("x1");
  auto z2 = PauliOp::z_gate("x2");
  std::cout << "[" << x1 << ", " << x1 << "] = " << (x1.commutes_with(x1) ? "1" : "-1") << "\n";
  std::cout << "[" << x1 << ", " << z1 << "] = " << (x1.commutes_with(z1) ? "1" : "-1") << "\n";
  std::cout << "[" << x1 << ", " << z2 << "] = " << (x1.commutes_with(z2) ? "1" : "-1") << "\n";
  std::cout << "[" << x1*z2 << ", " << z1*x2 << "] = " << ((x1*z2).commutes_with(z1*x2) ? "1" : "-1") << "\n";
  std::cout << "\n";

  auto h1 = CliffordOp::h_gate("x1");
  auto s1 = CliffordOp::s_gate("x1");
  auto cnot12 = CliffordOp::cnot_gate("x1", "x2");

  std::cout << "H: " << h1 << "\n"; 
  std::cout << "HH: " << h1 * h1 << "\n"; 
  std::cout << "S: " << s1 << "\n";
  std::cout << "SS: " << s1 * s1 << "\n";
  std::cout << "SS*: " << s1 * CliffordOp::sdg_gate("x1") << "\n";
  std::cout << "CNOT: " << cnot12 << "\n";
  std::cout << "CNOTCNOT: " << cnot12 * cnot12 << "\n";
  std::cout << "HSH: " << h1 * s1 * h1 << "\n";
  std::cout << "(I H)CNOT(I H): " << CliffordOp::h_gate("x2") * cnot12 * CliffordOp::h_gate("x2") << "\n";
  std::cout << "\n";

  std::cout << "H X(x1) H = " << h1.conjugate(b) << "\n";
  std::cout << "H Z(x1) H = " << h1.conjugate(c) << "\n";
  std::cout << "H Y(x1) H = " << h1.conjugate(d) << "\n";
  std::cout << "CNOT X(x1) CNOT = " << cnot12.conjugate(b) << "\n";
  std::cout << "CNOT X(x2) CNOT = " << cnot12.conjugate(PauliOp::x_gate("x2")) << "\n";
  std::cout << "\n";

  auto t1 = RotationOp::t_gate("x1");
  auto tdg1 = RotationOp::tdg_gate("x1");
  auto t2 = RotationOp::t_gate("x2");
  auto u1 = UninterpOp({ "x1" });

  std::cout << h1 << t1 << " = " << t1.commute_left(h1) << h1 << "\n";
  std::cout << t1 << t1 << " = "; print_merge(t1, t1); std::cout << "\n";
  std::cout << t1 << tdg1 << " = "; print_merge(t1, tdg1); std::cout << "\n";
  std::cout << t1 << t2 << " = "; print_merge(t1, t2); std::cout << "\n";
  std::cout << "\n";

  std::cout << "[" << t1 << ", " << tdg1 << "] = 1? " << (t1.commutes_with(tdg1) ? "yes" : "no") << "\n";
  std::cout << "[" << t1 << ", " << u1 << "] = 1? " << (t1.commutes_with(u1) ? "yes" : "no") << "\n";
  std::cout << "[" << t2 << ", " << u1 << "] = 1? " << (t2.commutes_with(u1) ? "yes" : "no") << "\n";

  return 1;
}
