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
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <buffer/buffer.h>
#include <buffer/file.h>

#include <array>
#include <set>
#include <fstream>
using namespace std;

BOOST_AUTO_TEST_SUITE( buffer )


  BOOST_AUTO_TEST_SUITE( basic )

    BOOST_AUTO_TEST_CASE( proxy )
    {
      using namespace Memory;
      Buffer::Vector v;
      Buffer::Vector w;
      Buffer::Proxy p;
      const char *inp[] = {
        "ign1",
        "foo",
        "ign2",
        "bar"
      };
      p.start(inp[0]);
      p.finish(inp[0] + strlen(inp[0]));
      p.set(&v);
      p.start(inp[1]);
      p.finish(inp[1] + strlen(inp[1]));
      {
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, inp[1]);
      }
      p.set(0);
      p.start(inp[2]);
      p.finish(inp[2] + strlen(inp[2]));
      {
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, inp[1]);
      }
      p.set(&w);
      p.start(inp[3]);
      p.finish(inp[3] + strlen(inp[3]));
      {
      string s(w.begin(), w.end());
      BOOST_CHECK_EQUAL(s, inp[3]);
      }
    }

  BOOST_AUTO_TEST_SUITE_END()

  BOOST_AUTO_TEST_SUITE( vector )

    BOOST_AUTO_TEST_CASE( move )
    {
      using namespace Memory;
      Buffer::Vector v;
      Buffer::Vector w;
      const char inp[] = "xyz";
      v.start(inp);
      v.finish(inp+sizeof(inp)-1);
      w = std::move(v);
      BOOST_CHECK(v.begin() == nullptr);
      BOOST_CHECK(v.end() == nullptr);
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, "");
      string t(w.begin(), w.end());
      BOOST_CHECK_EQUAL(t, inp);
    }
    BOOST_AUTO_TEST_CASE( move_commit )
    {
      using namespace Memory;
      Buffer::Vector v;
      Buffer::Vector w;
      const char inp[] = "xyz";
      v.start(inp);
      v.finish(inp+sizeof(inp)-1);
      v.commit();
      w = std::move(v);
      BOOST_CHECK(v.begin() == nullptr);
      BOOST_CHECK(v.end() == nullptr);
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, "");
      string t(w.begin(), w.end());
      BOOST_CHECK_EQUAL(t, inp);
    }

    BOOST_AUTO_TEST_CASE( move_constr )
    {
      using namespace Memory;
      Buffer::Vector v;
      const char inp[] = "xyz";
      v.start(inp);
      v.finish(inp+sizeof(inp)-1);
      Buffer::Vector w(std::move(v));
      BOOST_CHECK(v.begin() == nullptr);
      BOOST_CHECK(v.end() == nullptr);
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, "");
      string t(w.begin(), w.end());
      BOOST_CHECK_EQUAL(t, inp);
    }

    BOOST_AUTO_TEST_CASE( commit )
    {
      using namespace Memory;
      Buffer::Vector v;
      const char inp[] = "xyz";
      v.start(inp);
      v.finish(inp+sizeof(inp)-1);
      BOOST_CHECK(v.begin() == inp);
      BOOST_CHECK(v.end() == inp+sizeof(inp)-1);
      v.commit();
      BOOST_CHECK(v.begin() != inp);
      BOOST_CHECK(v.end() != inp+sizeof(inp)-1);
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, inp);
    }

  BOOST_AUTO_TEST_SUITE_END()

  BOOST_AUTO_TEST_SUITE( file  )

    BOOST_AUTO_TEST_CASE( exclusive )
    {
      using namespace Memory;
      const char filename[] = "tmp/exclusive";
      const char dir[] = "tmp";
      const char file[] = "exclusive";
      fs::remove(filename);
      fs::create_directory(dir);
      {
      ofstream f(filename, ofstream::out | ofstream::binary);
      f << "foo bar\n";
      }
      BOOST_CHECK_EQUAL(fs::exists(filename), true);
      bool caught = false;
      try {
        Buffer::File f(dir, file);
      } catch (const runtime_error &e) {
        //cout << "what: " << e.what() << '\n';
        caught = true;
        BOOST_CHECK(strcmp(e.what(), "open: ") != 0);
      }
      BOOST_CHECK_EQUAL(caught, true);

    }

    BOOST_AUTO_TEST_CASE( nonexclusive )
    {
      using namespace Memory;
      const char filename[] = "tmp/nonexclusive";
      const char dir[] = "tmp";
      const char file[] = "nonexclusive";
      fs::remove(filename);
      fs::create_directory(dir);
      {
      ofstream f(filename, ofstream::out | ofstream::binary);
      f << "foo bar\n";
      }
      BOOST_CHECK_EQUAL(fs::exists(filename), true);
      bool caught = false;
      try {
        Buffer::File f(dir, file, false);
        const char inp[] = "hello world";
        f.start(inp);
        f.finish(inp+sizeof(inp)-1);
      } catch (const runtime_error &e) {
        cout << "what: " << e.what() << '\n';
        caught = true;
      }
      BOOST_CHECK_EQUAL(caught, false);
      ifstream f(filename, ofstream::in | ofstream::binary);
      array<char, 32> b = {0};
      f.read(b.data(), b.size()-1);
      BOOST_CHECK_EQUAL(b.data(), "hello world");

    }

    BOOST_AUTO_TEST_CASE( openat )
    {
      using namespace Memory;
      const char path[] = "tmp/openat";
      fs::remove_all(path);
      fs::create_directories(path);
      {
        Dir d(path);
        Buffer::File f(d, "foo");
        Buffer::File g(d, "bar");
      }
      {
        fs::directory_iterator begin(path), end;
        unsigned count = std::distance(begin ,end);
        BOOST_REQUIRE_EQUAL(count, 2);
      }
      {
        fs::directory_iterator begin(path), end;
        set<string> names;
        for (auto i = begin; i != end; ++i)
          names.insert((*i).path().filename().generic_string());
        const array<const char*, 2> ref = { "bar", "foo" };
        BOOST_CHECK_EQUAL_COLLECTIONS(names.begin(), names.end(),
            ref.begin(), ref.end());
      }

    }

  BOOST_AUTO_TEST_SUITE_END()

  BOOST_AUTO_TEST_SUITE( opportune )

    BOOST_AUTO_TEST_CASE( lazy )
    {
      const char i[] = "Hello World!";
      pair<const char*, const char*> inp(i, i+sizeof(i)-1);
      Memory::Buffer::Vector v;
      v.start(inp.first);
      v.finish(inp.second);
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, "Hello World!");
      BOOST_CHECK(v.begin() == inp.first);
      BOOST_CHECK(v.end()   == inp.second);
    }

    BOOST_AUTO_TEST_CASE( copy )
    {
      const char i[] = "Hello World!";
      pair<const char*, const char*> inp(i, i+sizeof(i)-1);
      Memory::Buffer::Vector v;
      v.start(inp.first);
      v.stop(inp.first+5);
      v.cont(inp.first+5);
      v.finish(inp.second);
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, "Hello World!");
      BOOST_CHECK(    (v.begin() < inp.first && v.end() <= inp.first)
                   || (v.begin() >= inp.second && v.end() >= inp.second) );
    }

    BOOST_AUTO_TEST_CASE( pause )
    {
      const char i[] = "Hello World!";
      pair<const char*, const char*> inp(i, i+sizeof(i)-1);
      Memory::Buffer::Vector v;
      v.resume(inp.first);
      v.cont(inp.first);
      v.pause(inp.first+5);
      v.resume(inp.first+5);
      v.finish(inp.second);
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, "Hello World!");
      BOOST_CHECK(    (v.begin() < inp.first && v.end() <= inp.first)
                   || (v.begin() >= inp.second && v.end() >= inp.second) );
    }

    BOOST_AUTO_TEST_CASE( resume )
    {
      const char i[] = "Hello World!";
      pair<const char*, const char*> inp(i, i+sizeof(i)-1);
      Memory::Buffer::Vector v;
      {
        const char *pe = inp.first+5;
        Memory::Buffer::Resume r(v, inp.first, pe);
        v.cont(inp.first);
      }
      {
        Memory::Buffer::Resume r(v, inp.first+5, inp.second);
        v.finish(inp.second);
      }
      string s(v.begin(), v.end());
      BOOST_CHECK_EQUAL(s, "Hello World!");
      BOOST_CHECK(    (v.begin() < inp.first && v.end() <= inp.first)
                   || (v.begin() >= inp.second && v.end() >= inp.second) );
    }

  BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
