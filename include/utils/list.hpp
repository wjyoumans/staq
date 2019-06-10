/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Matthew Amy
*------------------------------------------------------------------------------------------------*/
#pragma once

#include <vector>
#include <memory>

namespace synthewareQ {
namespace utils {

  /*! \brief Extensible lists of managed-memory objects */

  template<typename T>
  class unique_list {
  private:
    using list_t = std::vector<std::unique_ptr<T> >;
    list_t list_;

  public:
    unique_list() {}
    
    // Assumes ownership of a vector of pointers
    unique_list(std::vector<T*> vec) {
      for (auto item : vec) {
        list_.push_back(std::unique_ptr<T>(item));
      }
    }

    uint32_t size() { return list_.size(); }

    class iterator : list_t::iterator {
    friend class Ulist;
    private:
      using internal_t = typename list_t::iterator;
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using const_reference = const T&;
      
    public:
      
      iterator(const internal_t &it) : internal_t(it) {}

      const_reference operator*() const {
        return *(this->internal_t::operator*());
      }

      pointer operator->() const {
        return this->internal_t::operator*().get();
      }

      iterator& operator++() {
        this->internal_t::operator++();
        return *this;
      }

      iterator operator++(int) {
        auto tmp = *this;
        this->internal_t::operator++();
        return tmp;
      }
    };
      
    iterator begin() const {
      return iterator(list_.begin());
    }

    iterator end() const {
      return iterator(list_.end());
    }

    void push_back(T* item) {
      list_.push_back(std::unique_ptr<T>(item));
    }

    void insert(iterator it, T* item) {
      list_.insert(static_cast<typename list_t::iterator>(it), std::unique_ptr<T>(item));
    }

    void emplace_back(T* item) {
      list_.emplace_back(item);
    }

    void emplace(iterator it, T* item) {
      list_.emplace(static_cast<typename list_t::iterator>(it), item);
    }

    void set(iterator it, T* item) {
      static_cast<typename list_t::iterator>(it)->reset(item);
    }
  };

}
}
