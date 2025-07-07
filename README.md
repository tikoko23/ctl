> The project is in very early stages. Expect lots of bugs.
> This library was created to bundle common functionality needed for my C software. It is purely a hobby project.

# CTL
CTL is a C library aiming to provide data structures in a template-like manner using the C preprocessor, while trying to be flexible without sacrificing simplicity.

# Documentation
Documentation is added to make the library more accessible, and potentially useful, to other people.
Tools such as [Doxygen](https://www.doxygen.nl/) can be used to generate documentation pages from comments.

To generate doxygen documentation with the default configuration:
```sh
# Ensure `doxygen` is installed and accessible
mkdir -p local/docs
doxygen doxygen.cfg
```

## General Style
For template-like macros, `$` refers to `prefix` unless specified otherwise.
##### Example:
Assuming `prefix` is `name`, `$New()` is equivalent `nameNew()`.
