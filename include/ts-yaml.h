#ifndef TS_YAML_H
#define TS_YAML_H
#include <assert.h>
#include <stdio.h>
#include <tree_sitter/api.h>
#include <tree_sitter/tree-sitter-yaml.h>

struct YamlObject;
typedef struct YamlObject* YamlObjectp;

#define T YamlObjectp
#include <c-templates/vector.h>
#undef T

struct YamlField;
typedef struct YamlField* YamlFieldp;

#define T YamlFieldp
#include <c-templates/vector.h>
#undef T

struct YamlField {
  char* key;
  YamlObjectp value;
};

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
    Vector_YamlFieldp map;
  };
};

YamlFieldp YamlField__new();
YamlObjectp YamlObject__new(enum yaml_object_t kind);
void YamlObject__delete(YamlObjectp value);
void YamlField__delete(YamlFieldp value);
void YamlObject__delete(YamlObjectp value);
void YamlObject__fprint(FILE* fd, YamlObjectp value, uint64_t tab);
void YamlField__fprint(FILE* fd, YamlFieldp field, uint64_t tab);
void YamlObject__fprint(FILE* fd, YamlObjectp value, uint64_t tab);
YamlObjectp parse_yaml(const TSLanguage* language, TSNode node, const char* source_code);
YamlObjectp parse_yaml_flow_node(const TSLanguage* language, TSNode node, const char* source_code);
YamlObjectp parse_yaml_block_mapping(const TSLanguage* language, TSNode node, const char* source_code);
YamlObjectp parse_yaml(const TSLanguage* language, TSNode node, const char* source_code);
#endif
