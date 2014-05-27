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
#ifndef GMS_BUFFER_FILE_H
#define GMS_BUFFER_FILE_H

#include <buffer/buffer.h>

namespace Memory {

  class Dir {
    private:
      std::string path_;
      int fd_ {-1};
    public:
      Dir(const Dir &) =delete;
      Dir &operator=(Dir &) =delete;
      Dir();
      Dir(int fd);
      Dir(Dir &&o);
      Dir &operator=(Dir &&o);
      Dir(const std::string &path);
      ~Dir();
      int fd() const;
      const std::string &path() const;
      void fsync();
      void open(const std::string &path);
      void close();
  };

  namespace Buffer {

    class File : public Caller {
      private:
        Dir *dir_ {nullptr};
        std::string dir_path_;
        int fd {-1};
        FILE *f {nullptr};
        std::string filename_;
        bool sync_ {true};
      public:
        File(const File &) =delete;
        File &operator=(const File &) =delete;

        File();
        File(const std::string &dir, const std::string &filename, bool exclusive = true);
        File(Dir &dir, const std::string &filename, bool exclusive = true);
        File(File &&o);
        File &operator=(File &&o);
        ~File();
        void open(const std::string &dir, const std::string &filename, bool exclusive = true);
        void open(const std::string &dir, const char *filename, bool exclusive = true);
        void open(const std::string &filename, bool exclusive = true);
        void close();
        void set_sync(bool b);

        void clear() override;

        void buffer_copy(const char *begin, const char *end, bool last)
          override;

    };

  }
}

#endif
