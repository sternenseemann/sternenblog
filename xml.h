/*!
 * @file xml.h
 * @brief Simple library for constructing XML documents
 *
 * This library provides a C interface for opening and closing
 * XML tags as well as filling them with content. It is mainly
 * intended for constructing XML/HTML documents by directly
 * writing them to `stdout`.
 *
 * It's main advantage over plain `printf()` is that it keeps
 * track of open tags, enabling it to automatically close
 * open tags (saving a few lines of code) using `xml_close_all()`
 * and `xml_close_including()` and/or to detect errors in the
 * programmer's XML nesting. For information on its sanity
 * checking abilities see the documentation of `xml_close_tag()`.
 *
 * Currently it has some limitations:
 *
 * * It does not give the calling code feedback if errors occurred
 * * It doesn't do validity checking of tags and attributes
 *   (legal characters etc.)
 * * It can't generate pretty output (i. e. properly indented),
 *   its output is currently always "minified".
 * * It currently has no satisfying support for `CDATA`,
 *   XML declarations and probably a lot more
 *
 * For handling arbitrary data this library is probably not a good
 * fit, it is mainly intended and tested for generating HTML and
 * RSS documents in a CGI-like environment from trusted data.
 *
 * An example application generating a HTML5 page looks like this:
 *
 * @include xml_example.c
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*!
 * @brief Internal linked list type
 *
 * Linked list used internally to keep track of tags to close.
 *
 * @see struct xml_context
 */
struct xml_stack {
    char *tag;               //!< tag name
    struct xml_stack *next;  //!< tag to be closed after the current one
};

/*!
 * @brief State and configuration of xml generation.
 *
 * Struct containing both state and configuration of this module.
 * See `new_xml_context()` for usage instructions.
 *
 * @see new_xml_context
 * @see del_xml_context
 * @see struct xml_stack
 */
struct xml_context {
    struct xml_stack *stack; //!< linked list used internally to keep track of open tags
    FILE *out;               //!< Where to write output, defaults to stdout
    FILE *warn;              //!< if not `NULL`, print warnings to handle warn, defaults to `NULL`
    bool closing_slash;      //!< whether to output a closing slash at the end of an empty tag
};

/*!
 * @brief Initialize the `xml_context` structure.
 *
 * Initialize a `struct xml_context` with default values:
 *
 * * empty stack
 * * output to `stdout`
 * * no warnings
 * * closing slashes enabled
 *
 * This function should always be called before any other functions of xml.
 * If you want to use different settings than the default ones, update the
 * struct after calling this function. This way your application won't break
 * if it gets extended.
 *
 * @see struct xml_context
 * @see del_xml_context
 */
void new_xml_context(struct xml_context *ctx);

/*!
 * @brief Clean up the `xml_context` structure.
 *
 * Frees any dynamically allocated data in a `struct xml_context`.
 * Should always be called before a `struct xml_context` goes out
 * of scope or the program terminates.
 *
 * If `ctx->warn` is not `NULL`, `del_xml_context()` will additionally
 * output a message about any remaining unclosed tags to `ctx->warn`.
 * For example:
 *
 * ```
 * Unclosed tags remaining: article main body html
 * ```
 *
 * @see struct xml_context
 * @see new_xml_context
 */
void del_xml_context(struct xml_context *ctx);

/*!
 * @brief Output a xml escaped string
 *
 * Outputs the given string escaped for use with XML. It only
 * does minimal-ish escaping, i. e. it escapes all characters
 * that have some syntactical meaning in XML. That includes:
 * Angled brackets (lower than and greater than), ampersand,
 * and single as well as double quotes. All other characters
 * are passed through as is and the caller is expected to
 * make sure they are correctly encoded, i. e. valid UTF-8
 * characters.
 *
 * The escaping is not as minimal as possible. In some cases
 * you can omit escaping all characters except for `<` and `&`,
 * but this would be context-sensitive and therefore
 * unnecessarily tedious to implement. Escaping all
 * syntactically significant characters has no real downsides
 * except maybe using a tiny bit more storage than absolutely
 * necessary.
 *
 * @see xml_raw
 */
void xml_escaped(struct xml_context *ctx, const char *str);

/*!
 * @brief Output a raw string.
 *
 * Output string to `ctx->out`, equivalent to `fputs(str, ctx.out)`.
 * If your string is not already XML, use xml_escaped() to output it
 * correcty escaped.
 *
 * @see struct xml_context
 * @see xml_escaped
 */
void xml_raw(struct xml_context *ctx, const char *str);

/*!
 * @brief Output an empty xml tag.
 *
 * Output an empty xml tag (i. e. a single tag that doesn't need to be closed).
 * This call does not change the provided context `ctx`.
 *
 * The call also outputs given attributes: For `attr_count` n, `xml_empty_tag()` expects
 * 2n additional arguments — for each attribute a name and a value. If value is `NULL`,
 * an attribute without a value will be output, i. e. just the name without the `="…"`.
 *
 * For example, `xml_empty_tag(&ctx, "my-tag", 2, "foo", "bar", "baz", NULL);` gives
 * `<my-tag foo="bar" baz/>` with default settings.
 *
 * The attributes' values are XML-escaped automatically. For details on how escaping
 * works in xml.h, see xml_escaped().
 *
 * If `closing_slash` is 0 in `ctx`, the slash before the closing ">" will be omitted.
 * This is useful for HTML5 where it is optional.
 *
 * @see struct xml_context
 */
void xml_empty_tag(struct xml_context *ctx, const char *tag, size_t attr_count, ...);

/*!
 * @brief Output an opening tag with attributes.
 *
 * Output an opening tag with attributes and add it to `ctx->stack` for future reference.
 *
 * Attributes work exactly like in `xml_empty_tag()`.
 * @see xml_empty_tag
 */
void xml_open_tag_attrs(struct xml_context *ctx, const char *tag, size_t attr_count, ...);

/*!
 * @brief Output an opening tag without any attributes.
 *
 * Shorthand for `xml_open_tag_attrs(ctx, tag, 0)`.
 * @see xml_open_tag_attrs
 */
void xml_open_tag(struct xml_context *ctx, const char *tag);

/*!
 * @brief Close a previously opened tag.
 *
 * `xml_close_tag()` first checks the head of the current `xml_stack`
 * if the provided `tag` is in fact the current innermost opened tag.
 *
 * If this is true, it outputs the closing tag, removes the reference
 * to the tag on top of the `xml_stack` and frees this part of the
 * structure.
 *
 * If it isn't true, it does nothing and outputs an appropriate warning
 * to `ctx->warn` if it is not `NULL`:
 *
 * * `Refusing to close tag xyz, unclosed tags remaining`
 * * `Refusing to close tag zyx, no tags left to be closed`
 *
 * This sanity checking of tag closing ensures that a xml document
 * constructed by this module has no nesting errors, i. e. every tag
 * is closed at the proper nesting level. Because it is only simple
 * runtime checking in specific calls it can't prevent / detect the
 * following errors:
 *
 * * It can't prevent unclosed tags remaining at the end. You can
 *   however prevent this by calling `xml_close_all()` at the end of
 *   your XML outputting code.
 * * It can worsen a situation with remaining unclosed tags: If
 *   an inner tag is left unclosed, it will refuse to close all
 *   outer tags, leaving a trail of unclosed tags behind.
 * * It will always attribute the error to closing: Some errors
 *   will be caused by missing an `xml_open_tag()` somewhere, but
 *   `xml_close_tag()` will think the closing is erroneous. Of course
 *   it's also unable to resolve the error.
 * * It can't compare against the intended XML structure: Sometimes
 *   a programming error will result in an "wrong" XML structure
 *   which is still completely valid to `xml_close_tag()`, i. e.
 *   correctly nested.
 *
 * Overall the sanity checking is limited, as you can see, but
 * it should generate *some* warning if a detectable issue is
 * present (invalid XML nesting) and never actively output
 * a tag that makes the XML nesting invalid.
 *
 * These properties should however be enough to detect issues
 * quickly in development. Additionally the sanity checking is
 * cheap enough to be feasible in production. `xml_close_tag()`
 * only needs to call `strcmp` once per invocation.
 *
 * @see xml_open_tag_attrs
 * @see xml_open_tag
 * @see struct xml_stack
 * @see xml_close_all
 * @see xml_close_including
 */
void xml_close_tag(struct xml_context *ctx, const char *tag);

/*!
 * @brief Close all remaining unclosed tags
 *
 * `xml_close_all()` iterates through the `xml_stack` and calls
 * `xml_close_tag()` for every tag in it. A call to it will thus
 * result in an empty `xml_stack` and all previously opened tags
 * being closed correctly.
 *
 * Note that `xml_close_all()` will limit error checking, since it
 * (by nature) always succeeds and has no insight into what the
 * programmer thinks needs to be closed.
 *
 * @see xml_close_tag
 * @see xml_close_including
 * @see struct xml_stack
 */
void xml_close_all(struct xml_context *ctx);

/*!
 * @brief Close all unclosed tags until a given one.
 *
 * `xml_close_including()` works like `xml_close_all()`, but
 * will stop after it hits a tag of the given name.
 * If the given tag is not present in the stack, it behaves
 * like `xml_close_all()`.
 *
 * Be aware that it might lead to unexpected results if
 * multiple tags of the same are nested. Consider the
 * following snippet.
 *
 * ```c
 * xml_open_tag(&ctx, "a");
 * xml_open_tag(&ctx, "b");
 * xml_close_tag(&ctx, "a");
 * xml_open_tag(&ctx, "c");
 * xml_raw(&ctx, "value");
 * xml_close_including(&ctx, "a");
 * ```
 *
 * `xml_close_including()` will stop as soon as it hits the first
 * tag "a", although it might be intended to keep going until the
 * outermost one. The result would be:
 *
 * ```xml
 * <a><b><a><c>value</c></a>
 * ```
 *
 * This is the behavior of `xml_close_including()`: It closes all
 * unclosed tags until it hits the first instance of the specified
 * tag which is also closed.
 *
 * This function will also limit error detection like `xml_close_all()`.
 * For an explanation of this, see its documentation.
 *
 * @see xml_close_all
 * @see xml_close_tag
 * @see struct xml_stack
 */
void xml_close_including(struct xml_context *ctx, const char *tag);
