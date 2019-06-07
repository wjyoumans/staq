/*-------------------------------------------------------------------------------------------------
  | This file is distributed under the MIT License.
  | See accompanying file /LICENSE for details.
  | Author(s): Matthew Amy
  *------------------------------------------------------------------------------------------------*/

#include <unordered_map>

namespace synthewareQ {
namespace channel {

  /*! \brief Optimizations based on the channel representation of Clifford + single qubit gates */

  enum class Pauli { i, x, y, z };


  /*! \brief Class storing a rotation of some angle around a pauli
   *
   *  (1 + e^i\theta)/2 I + (1 - e^i\theta) P
   */
  class Rotation {
  public:

  protected:

  private:

    tweedledum::angle theta;
    std::unordered_map<std::string_view, Pauli> pauli;

  }


  class Circuit {
  public:

  protected:

  private:

    std::
