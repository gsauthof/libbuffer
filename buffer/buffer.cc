// Copyright 2014, Georg Sauthoff <mail@georg.so>

/* {{{ GPLv3

    This file is part of libbuffer.

    libbuffer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libbuffer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libbuffer.  If not, see <http://www.gnu.org/licenses/>.

}}} */
#include "buffer.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <array>
#include <utility>

using namespace std;

namespace Memory {
  namespace Buffer {

    Base::~Base()
    {
    }

    Resume::Resume(Base &b, const char *p, const char *&pe)
      :
        b(b),
        //p(p),
        pe(pe)
    {
      b.resume(p);
    }
    Resume::~Resume()
    {
      b.pause(pe);
    }

    Proxy::Proxy()
    {
    }
    void Proxy::set(Base *b)
    {
      this->b = b;
    }
    void Proxy::clear()
    {
      if (b)
        b->clear();
    }
    void Proxy::start(const char *p)
    {
      if (b)
        b->start(p);
    }
    void Proxy::cont(const char *p)
    {
      if (b)
        b->cont(p);
    }
    void Proxy::stop(const char *p)
    {
      if (b)
        b->stop(p);
    }
    void Proxy::finish(const char *p)
    {
      if (b)
        b->finish(p);
    }
    void Proxy::finish()
    {
      if (b)
        b->finish();
    }
    void Proxy::resume(const char *p)
    {
      if (b)
        b->resume(p);
    }
    void Proxy::pause(const char *p)
    {
      if (b)
        b->pause(p);
    }
    void Proxy::buffer_copy(const char *begin, const char *end,
            bool last)
    {
      if (b)
        b->buffer_copy(begin, end, last);
    }

    void Caller::clear()
    {
      throw logic_error("clear not implemented");
    }
    void Caller::start(const char *p)
    {
      assert(p);
      first = p;
      active_ = true;
    }
    // two methods such that sub-classes
    // can something different
    void Caller::cont(const char *p)
    {
      assert(p);
      first = p;
      active_ = true;
    }
    void Caller::stop(const char *p)
    {
      if (!first)
        return;
      buffer_copy(first, p, false);
      first = nullptr;
      active_ = false;
    }
    void Caller::finish(const char *p)
    {
      if (!first)
        return;
      buffer_copy(first, p, true);
      first = nullptr;
      active_ = false;
    }
    void Caller::finish()
    {
      if (!first)
        return;
      buffer_copy(nullptr, nullptr, true);
      first = nullptr;
      active_ = false;
    }
    void Caller::resume(const char *p)
    {
      assert(p);
      if (!active_)
        return;
      first = p;
    }
    void Caller::pause(const char *p)
    {
      if (!active_)
        return;
      buffer_copy(first, p, false);
      first = nullptr;
    }


    Vector::Vector()
    {
    }
    Vector::Vector(Vector &&o)
      : v(std::move(o.v)), range_(o.range_)
    {
      o.range_.first = nullptr;
      o.range_.second = nullptr;
    }
    Vector &Vector::operator=(Vector &&o)
    {
      v = std::move(o.v);
      range_.first = o.range_.first;
      o.range_.first = nullptr;
      range_.second = o.range_.second;
      o.range_.second = nullptr;
      return *this;
    }

    void Vector::start(const char *p)
    {
      Caller::start(p);
      clear();
    }
    void Vector::commit()
    {
      if (range_.first) {
        v.insert(v.end(), range_.first, range_.second);
        range_.first = nullptr;
        range_.second = nullptr;
      }
    }
    void Vector::buffer_copy(const char *begin, const char *end, bool last)
    {
      assert(begin<=end);
      if (begin == end)
        return;

      if (last) {
        if (v.empty()) {
          if (range_.first)
            throw logic_error("Vector::buffer_copy(..., last=true) called a 2nd time?");
          range_.first = begin;
          range_.second = end;
        } else {
          v.insert(v.end(), begin, end);
        }
      } else {
        v.insert(v.end(), begin, end);
      }
    }
    void Vector::clear()
    {
      v.clear();
      range_.first = nullptr;
      range_.second = nullptr;
    }
    const char *Vector::data() const
    {
      if (v.empty())
        return range_.first;
      else
        return v.data();
    }
    size_t Vector::size() const
    {
      if (v.empty())
        return range_.second - range_.first;
      else
        return v.size();
    }
    bool Vector::empty() const
    {
      if (v.empty())
        return range_.second - range_.first == 0;
      else
        return false;
    }
    Vector::const_iterator Vector::begin() const
    {
      if (v.empty())
        return range_.first;
      else
        return v.data();
    }
    Vector::const_iterator Vector::end() const
    {
      if (v.empty())
        return range_.second;
      else
        return v.data() + v.size();
    }
    std::pair<const char*, const char*> Vector::range() const
    {
      if (v.empty())
        return range_;
      else
        return make_pair(begin(), end());
    }

  }
}

