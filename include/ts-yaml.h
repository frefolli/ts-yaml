#ifndef TS_YAML_H
#define TS_YAML_H
#include <assert.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-yaml.h>
#include <string.h>

struct YamlObject;
typedef struct YamlObject* YamlObjectp;

#define T YamlObjectp
#include <c-templates/vector.h>
#undef T

typedef char* string;
typedef const char* c_string;
#define K string
#define const_K const c_string
#define V YamlObjectp
#define K_Equals(Ka, Kb) strcmp(Ka, Kb) == 0
#include <c-templates/map.h>
#undef V
#undef K
typedef Pair_string_YamlObjectp YamlField;

enum yaml_object_t {
  INTEGER_YO,
  STRING_YO,
  LIST_YO,
  MAP_YO
};

struct YamlObject {
  enum yaml_object_t kind;
  union {
    int integer;
    char* string;
    Vector_YamlObjectp list;
    Map_string_YamlObjectp map;
  };
};

YamlObjectp YamlObject__new(enum yaml_object_t kind);
void YamlObject__delete(YamlObjectp value);
void YamlObject__fprint(FILE* fd, YamlObjectp value, uint64_t tab);

static inline bool YamlObject__is_map(YamlObjectp value) {
  return value->kind == MAP_YO;
}
static inline bool YamlObject__is_list(YamlObjectp value) {
  return value->kind == LIST_YO;
}

YamlObjectp YamlObject__get(YamlObjectp map, const char* field);
size_t YamlObject__size(YamlObjectp list);
YamlObjectp YamlObject__at(YamlObjectp list, size_t idx);

YamlObjectp parse_yaml(const TSLanguage* language, TSNode node, const char* source_code);
YamlObjectp parse_yaml_file(const char* filepath);
#endif
