#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <regex>
#include <algorithm>
#include <unordered_set>

#include "FileReader.h"

using namespace std;

// lista de todas las etiquetas HTML validas que puedo econtrar en paginas web normales
const unordered_set< string > validHtmlTags = {

    // document metadata
    "html", "head", "title", "base", "link", "meta", "style",

    // sectioning
    "body", "header", "nav", "main", "section", "article", "aside", "footer",

    // grouping content
    "p", "hr", "pre", "blockquote", "ol", "ul", "li", "dl", "dt", "dd", "figure", "figcaption", "div",

    // text semantics
    "a", "em", "strong", "small", "s", "cite", "q", "dfn", "abbr", "data", "time", "code", "var", "samp", "kbd", "sub", "sup", "i", "b", "u", "mark", "ruby", "rt", "rp", "bdi", "bdo", "span", "br", "wbr",

    // edits
    "ins", "del",

    // embedded content
    "img", "iframe", "embed", "object", "param", "video", "audio", "source", "track", "canvas", "map", "area", "svg", "math",

    // tabular data
    "table", "caption", "colgroup", "col", "tbody", "thead", "tfoot", "tr", "td", "th",

    // forms
    "form", "fieldset", "legend", "label", "input", "button", "select", "datalist", "optgroup", "option", "textarea", "keygen", "output", "progress", "meter",

    // interactive elements
    "details", "summary", "menu", "menuitem", "dialog",

    // scripting
    "script", "noscript", "template", "slot"
};

void countTagsBuffer( const string &buffer, map< string, int > &localCounts, string &carryOver );

bool isValidHtmlTag( const string &tag );
