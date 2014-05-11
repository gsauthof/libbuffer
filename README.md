libbuffer - C++ library for managing buffers during lexing

2014-05-01, Georg Sauthoff <mail@georg.so>

## Example

    class Lexer {

      Memory::Buffer::Vector buffer;

      // ... additional state ...

      void number_begin(const char *p)
      {
        buffer.start(p);
      }
      void number_end(const char *p)
      {
        buffer.finish(p);
        // notifiy an observer that the buffer is ready
        // for consumption.
        // The observer can then access the contents via buffer.begin(), buffer.end()
      }

      void read_more(const char *begin, const char *end)
      {
        buffer.resume(begin);
        // ..
        // execute some lexing state machine
        // which may call number_begin()/number_end()
        // ..
        buffer.stop(end);
      }
    }

## Rationale

When you lex some input online - i.e. when it is read in blocks (e.g. from a
network stream) - you have to deal with the situation when a token spans over
multiple blocks.

The naive approach is to append character by character of a token to an additional
temporary buffer.

But the more efficient approach is to:

1. omit the copying when the token is located completely in the current read buffer,
   thus avoiding unnecessary double buffering
2. when a token continues to the next read buffer, copy the current part via
   an optimized function like `memcpy()`

Libbuffer implements this approach.

In addition it provides:

- a proxy buffer class for switching between multiple buffers
- a file buffer
- a null buffer that ignores all input (useful in combination with the proxy)


## Compile

    $ cmake ../CmakeLists.txt ..
    $ make

The unittest target is also built by defualt. You can execute the unittest then via:

    $ ./ut

## More examples

See also the unittest sources for more examples how to use the API.

## Related stuff

See also [Ragel][1], a powerful state machine generator. It does not prescribe
a certain buffering scheme, i.e. it doesn't generate code that manages buffers
in a certain way. Thus, one is flexible to use - depending on the use case - no
buffering or the scheme of ones choice.

## Licence

GPLv3

[1]: http://www.complang.org/ragel/

