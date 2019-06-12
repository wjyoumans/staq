/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ast/kinds.hpp"
#include "ast/ast_node.hpp"
#include "ast/file.hpp"

#include <memory>
#include <string>

namespace synthewareQ {
namespace ast {

  /*! \brief Packages an AST for each translation unit with other utilities */
  class QASM_program {
  private:
    /*! \brief The associated source manager. */
    source_manager& source_manager_;

    /*! \brief The associated diagnostic engine. */
    diagnostic_engine& diagnostic_;

    /*! \brief A list of translation units in inclusion order. */
    std::unique_ptr<File_list> trans_units_;

  public:
    QASM_program(source_manager& source_manager, diagnostic_engine& diagnostic)
      : source_manager_(source_manager)
      , diagnostic_(diagnostic)
      , trans_units_(new File_list)
    {}

    void add_translation_unit(Translation_unit * unit) {
      trans_unit_.push_back(unit);
    }
  };

}
}
