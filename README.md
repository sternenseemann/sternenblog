# sternenblog

🌟blog — file based CGI blog software

## user documentation

sternenblog is documented in `sternenblog.cgi(1)` which you can read using
`man -l doc/man/man1/sternenblog.cgi.1`. A rendered version for your
web browser will be added soon™.

## configuration and building

configuration of sternenblog is static and done in
`config.h` and `config.mk`. After configuring you
can compile the CGI application and (optionally)
install it:

```
make
make install # default installation to /usr/local/share/sternenblog
```

the possible customizations are detailed in the
[user documentation](#user%20documentation).

## templating and development

templating is unstable at the moment. for the current state
you have to refer to `core.h` and `template.h`.

you can generate documentation from all source files using `make doc`
which requires `doxygen` to be installed.

outstanding tasks are managed using [t](https://hg.stevelosh.com/t)
and stored in [`TODO`](./TODO). The format is also text editor friendly.
