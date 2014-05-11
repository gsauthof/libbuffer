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
#ifndef GMS_BUFFER_H
#define GMS_BUFFER_H

#include <vector>
#include <cstddef>
#include <stdio.h>
#include <string>

namespace Memory {

  namespace Buffer {

    class Base {
      private:
      public:
        virtual ~Base();

        virtual void start(const char *p) = 0;
        virtual void stop(const char *p) = 0;
        virtual void resume(const char *p) = 0;
        virtual void finish(const char *p) = 0;
        virtual void finish() = 0;

        virtual void buffer_copy(const char *begin, const char *end,
            bool last) = 0;

    };

    class Proxy : public Base {
      private:
        Base *b { nullptr };
      public:
        Proxy(Base *b);
        Proxy();
        void set(Base *b);

        void start(const char *p) override;
        void stop(const char *p) override;
        void resume(const char *p) override;
        void finish(const char *p) override;
        void finish() override;

        void buffer_copy(const char *begin, const char *end,
            bool last) override;

    };

    class Caller : public Base {
      private:
        const char* first { nullptr };
      public:
        void start(const char *p) override;
        void stop(const char *p) override;
        void resume(const char *p) override;
        void finish(const char *p) override;
        void finish() override;;
    };

    class Vector : public Caller {
      private:
        std::vector<char> v;
        std::pair<const char*, const char*> range_ {nullptr, nullptr};
      public:
        Vector(const Vector &) =delete;
        Vector &operator=(const Vector &) =delete;

        Vector();
        Vector(Vector &&o);
        Vector &operator=(Vector &&o);

        void start(const char *p) override;
        void commit();

        void buffer_copy(const char *begin, const char *end, bool last)
          override;

        void clear() ;
        const char *data() const ;
        size_t size() const ;
        typedef const char * const_iterator ;
        const_iterator begin() const ;
        const_iterator end() const ;
        std::pair<const char*, const char*> range() const;
    };
  }
}


#endif
