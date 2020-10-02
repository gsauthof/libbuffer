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
#include "file.h"
#include <ixxx/ixxx.h>
using namespace ixxx;

#include <stdexcept>
using namespace std;

namespace Memory {

  Dir::Dir()
  {
  }
  Dir::Dir(int fd)
    : fd_(fd)
  {
  }

  Dir::Dir(Dir &&o)
    : path_(std::move(o.path_)),
    fd_(o.fd_)
  {
    o.path_.clear();
    o.fd_ = -1;
  }
  Dir &Dir::operator=(Dir &&o)
  {
    path_ = o.path_;
    o.path_.clear();
    fd_ = o.fd_;
    o.fd_ = -1;
    return *this;
  }
  Dir::Dir(const std::string &path)
  {
    open(path);
  }
  Dir::~Dir()
  {
    try {
      close();
    } catch (const exception&) {
    }
  }
  int Dir::fd() const
  {
    if (fd_ == -1)
      throw logic_error("Dir::fd() - directory not opened");
    return fd_;
  }
  const std::string &Dir::path() const
  {
    if (fd_ == -1)
      throw logic_error("Dir::path() - directory not opened");
    return path_;
  }
  void Dir::fsync()
  {
    if (fd_ == -1)
      throw logic_error("Dir::fsync() - directory not opened");
    // such that new file directory entries are sync'd as well
    // cf. linux man page of fsync()
    posix::fsync(fd_);
  }
  void Dir::open(const std::string &path)
  {
    close();
    fd_ = posix::open(path.c_str(), O_RDONLY);
    path_ = path;
  }
  void Dir::close()
  {
    // when fd was supplied via constructor
    if (path_.empty())
      return;
    if (fd_ == -1)
      return;
    posix::close(fd_);
  }


  namespace Buffer {

    File::File()
    {
    }
    File::File(const string &dir, const string &filename, bool exclusive)
    {
      open(dir, filename, exclusive);
    }
    File::File(Dir &dir, const std::string &filename, bool exclusive)
      : dir_(&dir)
    {
      open(filename, exclusive);
    }
    File::File(File &&o)
      : dir_(o.dir_), dir_path_(std::move(o.dir_path_)),
          fd(o.fd), f(o.f), filename_(std::move(o.filename_)),
          sync_(o.sync_)
    {
      o.dir_ = nullptr;
      o.dir_path_.clear();
      o.fd = -1;
      o.f = nullptr;
      o.filename_.clear();
      o.sync_ = false;
    }
    File &File::operator=(File &&o)
    {
      dir_ = o.dir_;
      o.dir_ = nullptr;
      dir_path_ = std::move(o.dir_path_);
      o.dir_path_.clear();
      fd = o.fd;
      o.fd = -1;
      f = o.f;
      o.f = nullptr;
      filename_ = std::move(o.filename_);
      o.filename_.clear();
      sync_ = o.sync_;
      o.sync_ = false;
      return *this;
    }
    File::~File()
    {
      try {
        close();
      } catch(const exception &)
      {
        // we must not throw an exception from a destructor
        (void)0;
      }
    }
    void File::set_sync(bool b)
    {
      sync_ = b;
    }
    void File::open(const std::string &dir, const char *filename, bool exclusive)
    {
      return open(dir, string(filename), exclusive);
    }
    void File::open(const string &dir, const string &filename, bool exclusive)
    {
      close();
      dir_path_ = dir;
      filename_ = filename;
      string fn(dir);
      // this path sep should also work for windows
      fn += '/';
      fn += filename;

      int flags = O_CREAT | O_WRONLY;
      if (exclusive)
        flags |= O_EXCL;
      fd = posix::open(fn.c_str(), flags, 0666);
      f = posix::fdopen(fd, "wb");
    }
    void File::open(const string &filename, bool exclusive)
    {
      close();
      filename_ = filename;
      int flags = O_CREAT | O_WRONLY;
      if (exclusive)
        flags |= O_EXCL;
      fd = posix::openat(dir_->fd(), filename.c_str(), flags, 0666);
      f = posix::fdopen(fd, "wb");
    }
    void File::close()
    {
      if (!f)
        return;
      ansi::fflush(f);
      if (sync_) {
        int fd = posix::fileno(f);
        posix::fsync(fd);
      }
      ansi::fclose(f);
      f = nullptr;
      if (sync_) {
        if (dir_) {
          dir_->fsync();
        } else {
          // such that new file directory entries are sync'd as well
          // cf. linux man page of fsync()
          int dir_fd = posix::open(dir_path_.c_str(), O_RDONLY);
          posix::fsync(dir_fd);
          posix::close(dir_fd);
        }
      }
    }
    void File::clear()
    {
    }

    void File::buffer_copy(const char *begin, const char *end, bool /* last */)
    {
      ansi::fwrite(begin, 1, end-begin, f);
    }


  }
}
