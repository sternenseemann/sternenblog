# sternenblog
sternenblog is a blogging engine written in C intended to work with [CGI](de.wikipedia.org/wiki/Common_Gateway_Interface).

## How to blog with it?
* In `main.c` set `BLOGDIR` to the directory where your posts should be stored
* In the Makefile set `TEMPLATE` to your template (see below)
* run `make`
* symlink/move `./blog` somewhere where your webserver will execute it as CGI script

## writing templates
See [template.h](./template.h) and [simple.c](./templates/simple.c).

## Is it anyhow useful?
Oh yes. And it will take you to the stars!
