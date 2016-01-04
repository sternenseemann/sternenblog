# sternenblog
sternenblog is a blogging engine written in C intended to work with [CGI](https://de.wikipedia.org/wiki/Common_Gateway_Interface).

## How to blog with it?
* In `config.h` set the constants to your custom values
* In the Makefile set `TEMPLATE` to your template (see below)
* run `make`
* symlink/move `./blog` somewhere where your webserver will execute it as CGI script
* blogpost are stored in `BLOG_DIR` (defined in `config.h`) the must be named like this: `YYYY-MM-DD-HH-MM-title`
* the RSS Feed ist located at `http://your-server.com/cgi-bin/blog/rss.xml`

## writing templates
See [template.h](./template.h), [blogdefs.h](./blogdefs.h) and [simple.c](./templates/simple.c).
(more detailed documentation is coming soon)

## Is it anyhow useful?
Oh yes. And it will take you to the stars! :stars:
