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
      protected:
        bool active_ {false};
      public:
        virtual ~Base();

        virtual void clear() = 0;

        virtual void start(const char *p) = 0;
        virtual void cont(const char *p) = 0;

        virtual void stop(const char *p) = 0;
        virtual void finish(const char *p) = 0;
        virtual void finish() = 0;

        virtual void resume(const char *p) = 0;
        virtual void pause(const char *p) = 0;

        virtual void buffer_copy(const char *begin, const char *end,
            bool last) = 0;

    };

    class Resume {
      private:
        Base &b;
        //const char *p {nullptr};
        const char *&pe;
      public:
        Resume(Base &b, const char *p, const char *&pe);
        ~Resume();
    };

    class Proxy : public Base {
      private:
        Base *b { nullptr };
      public:
        Proxy(Base *b);
        Proxy();
        void set(Base *b);

        void clear() override;

        void start(const char *p) override;
        void cont(const char *p) override;

        void stop(const char *p) override;
        void finish(const char *p) override;
        void finish() override;

        void resume(const char *p) override;
        void pause(const char *p) override;

        void buffer_copy(const char *begin, const char *end,
            bool last) override;

    };

    class Caller : public Base {
      private:
        const char* first { nullptr };
      public:
        void clear() override;

        void start(const char *p) override;
        void cont(const char *p) override;

        void stop(const char *p) override;
        void finish(const char *p) override;
        void finish() override;

        void resume(const char *p) override;
        void pause(const char *p) override;
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

        void clear() override;
        const char *data() const ;
        size_t size() const ;
        bool empty() const ;
        typedef const char * const_iterator ;
        const_iterator begin() const ;
        const_iterator end() const ;
        std::pair<const char*, const char*> range() const;
    };
  }
}


#endif
