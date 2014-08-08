#include "blogdefs.h"
/* this function should print out the
 * top part of the blog's source code */
void template_header(void);

/* this functions should print out the
 * bottom part of the blog's source code */
void template_footer(void);

/* this function should print out the source
 * code for a index entry for a blog post */
void template_post_index_entry(struct blogpost post);

/* this function should generate the source code
 * for a single entry */
void template_post_single_entry(struct blogpost post);

/* this functions should return a 404 error page */
void template_error_404(void);
