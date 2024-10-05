#include <ts-yaml.h>
#include <stdlib.h>
#include <string.h>

static inline void tabulate(FILE* fd, uint64_t tab) {
  for (uint64_t i = 0; i < tab; ++i)
    fputs("  ", fd);
}

static inline void assert_eq_str(const char* actual, const char* expected) {
  if (strcmp(actual, expected) != 0) {
    fprintf(stderr, "'%s' != '%s'\n", actual, expected);
    assert(false);
  }
}

static inline void assert_not_null(const void* ptr) {
  if (ptr == NULL) {
    fprintf(stderr, "NULL ptr encountered\n");
    assert(false);
  }
}

static inline TSNode ts_node_named_child_by_field_name(TSNode self, const char* field_name) {
  return ts_node_child_by_field_name(self, field_name, strlen(field_name));
}

static inline char* ts_node_source_code(TSNode node, const char* source_code) {
  const char* src = source_code + ts_node_start_byte(node);
  uint64_t length = ts_node_end_byte(node) - ts_node_start_byte(node);
  char* dst = malloc(sizeof(char) * length + 1);
  memcpy(dst, src, length);
  dst[length] = '\0';
  return dst;
}

YamlFieldp YamlField__new() {
  YamlFieldp field = malloc(sizeof(struct YamlField));
  field->key = NULL;
  field->value = NULL;
  return field;
}

YamlObjectp YamlObject__new(enum yaml_object_t kind) {
  YamlObjectp value = malloc(sizeof(struct YamlObject));
  value->kind = kind;
  switch (kind) {
    case INTEGER_YO:
      {
        value->integer = 0;
        break;
      };
    case STRING_YO:
      {
        value->string = NULL;
        break;
      };
    case LIST_YO:
      {
        Vector_YamlObjectp__init(&value->list);
        break;
      };
    case MAP_YO:
      {
        Vector_YamlFieldp__init(&value->map);
        break;
      };
  };
  return value;
}

void YamlObject__delete(YamlObjectp value);

void YamlField__delete(YamlFieldp value) {
  if (value != NULL) {
    if (value->key != NULL) {
      free(value->key);
      value->key = NULL;
    }
    if (value->value != NULL) {
      YamlObject__delete(value->value);
      value->value = NULL;
    }
    free(value);
  }
}

void YamlObject__delete(YamlObjectp value) {
  switch (value->kind) {
    case INTEGER_YO:
      {
        value->integer = 0;
        break;
      };
    case STRING_YO:
      {
        if (value->string != NULL) {
          free(value->string);
          value->string = NULL;
        }
        break;
      };
    case LIST_YO:
      {
        YamlObjectp* it = Vector_YamlObjectp__begin(&value->list);
        YamlObjectp* end = Vector_YamlObjectp__end(&value->list);
        while (it != end) {
          YamlObject__delete(*it);
          *it = NULL;
          ++it;
        }
        Vector_YamlObjectp__clean(&value->list);
        break;
      };
    case MAP_YO:
      {
        YamlFieldp* it = Vector_YamlFieldp__begin(&value->map);
        YamlFieldp* end = Vector_YamlFieldp__end(&value->map);
        while (it != end) {
          YamlField__delete(*it);
          *it = NULL;
          ++it;
        }
        Vector_YamlFieldp__clean(&value->map);
        break;
      };
  };
}

void YamlObject__fprint(FILE* fd, YamlObjectp value, uint64_t tab);

void YamlField__fprint(FILE* fd, YamlFieldp field, uint64_t tab) {
  tabulate(fd, tab);
  fprintf(fd, "%s:", field->key);
  if ((field->value->kind == MAP_YO) || (field->value->kind == LIST_YO)) {
    tabulate(fd, tab);
    fputc('\n', fd);
    YamlObject__fprint(fd, field->value, tab + 1);
  } else {
    fputc(' ', fd);
    YamlObject__fprint(fd, field->value, tab);
  }
}

void YamlObject__fprint(FILE* fd, YamlObjectp value, uint64_t tab) {
  switch (value->kind) {
    case INTEGER_YO:
      {
        fprintf(fd, "%d", value->integer);
        break;
      };
    case STRING_YO:
      {
        fprintf(fd, "%s", value->string);
        break;
      };
    case LIST_YO:
      {
        YamlObjectp* begin = Vector_YamlObjectp__begin(&value->list);
        YamlObjectp* it = begin;
        YamlObjectp* end = Vector_YamlObjectp__end(&value->list);
        while (it != end) {
          if (it != begin) {
            fputc('\n', fd);
          }
          tabulate(fd, tab);
          fprintf(fd, "- ");
          YamlObject__fprint(fd, *it, tab);
          ++it;
        }
        break;
      };
    case MAP_YO:
      {
        YamlFieldp* begin = Vector_YamlFieldp__begin(&value->map);
        YamlFieldp* it = begin;
        YamlFieldp* end = Vector_YamlFieldp__end(&value->map);
        while (it != end) {
          if (it != begin) {
            fputc('\n', fd);
          }
          YamlField__fprint(fd, *it, tab);
          ++it;
        }
        break;
      };
  }
}

YamlObjectp parse_yaml(const TSLanguage* language, TSNode node, const char* source_code);

YamlObjectp parse_yaml_flow_node(const TSLanguage* language, TSNode node, const char* source_code) {
  YamlObjectp value = NULL;

  TSNode child = ts_node_named_child(node, 0);
  const char* symbol_name = ts_language_symbol_name(language, ts_node_symbol(child));

  if (strcmp(symbol_name, "plain_scalar") == 0) {
    TSNode subchild = ts_node_named_child(child, 0);
    const char* symbol_name = ts_language_symbol_name(language, ts_node_symbol(subchild));
    if (strcmp(symbol_name, "integer_scalar") == 0) {
      value = YamlObject__new(INTEGER_YO);
      char* str_value = ts_node_source_code(subchild, source_code);
      sscanf(str_value, "%d", &value->integer);
      free(str_value);
    } else if (strcmp(symbol_name, "string_scalar") == 0) {
      value = YamlObject__new(STRING_YO);
      value->string = ts_node_source_code(subchild, source_code);
    } else {
      fprintf(stderr, "symbol_name: '%s' not mapped\n", symbol_name);
      exit(1);
    }
  } else  if (strcmp(symbol_name, "double_quote_scalar") == 0) {
    value = YamlObject__new(STRING_YO);
    value->string = ts_node_source_code(child, source_code);
  } else if (strcmp(symbol_name, "flow_sequence") == 0) {
    value = YamlObject__new(LIST_YO);
    uint64_t child_count = ts_node_named_child_count(child);
    for (uint64_t i = 0; i < child_count; ++i) {
      TSNode subchild = ts_node_named_child(child, i);
      Vector_YamlObjectp__push_back(&value->list, parse_yaml(language, subchild, source_code));
    }
  } else {
    fprintf(stderr, "symbol_name: '%s' not mapped\n", symbol_name);
    exit(1);
  }

  assert(value != NULL);
  return value;
}

YamlObjectp parse_yaml_block_mapping(const TSLanguage* language, TSNode node, const char* source_code) {
  YamlObjectp value = YamlObject__new(MAP_YO);
  assert(value != NULL);
  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t i = 0; i < child_count; ++i) {
    TSNode child = ts_node_named_child(node, i);
    const char* symbol_name = ts_language_symbol_name(language, ts_node_symbol(child));
    assert_eq_str(symbol_name, "block_mapping_pair");

    TSNode key_node = ts_node_named_child_by_field_name(child, "key");
    assert_not_null(key_node.id);
    TSNode value_node = ts_node_named_child_by_field_name(child, "value");
    assert_not_null(value_node.id);
    
    YamlFieldp field = YamlField__new();
    field->key = ts_node_source_code(key_node, source_code);
    field->value = parse_yaml(language, value_node, source_code);
    Vector_YamlFieldp__push_back(&value->map, field);
  }
  return value;
}

YamlObjectp parse_yaml(const TSLanguage* language, TSNode node, const char* source_code) {
  YamlObjectp result = NULL;
  const char* symbol_name = ts_language_symbol_name(language, ts_node_symbol(node));

  if (strcmp(symbol_name, "stream") == 0) {
    result = parse_yaml(language, ts_node_named_child(node, 0), source_code);
  } else if (strcmp(symbol_name, "document") == 0) {
    result = parse_yaml(language, ts_node_named_child(node, 0), source_code);
  } else if (strcmp(symbol_name, "block_node") == 0) {
    result = parse_yaml(language, ts_node_named_child(node, 0), source_code);
  } else if (strcmp(symbol_name, "block_mapping") == 0) {
    result = parse_yaml_block_mapping(language, node, source_code);
  } else if (strcmp(symbol_name, "flow_node") == 0) {
    result = parse_yaml_flow_node(language, node, source_code);
  }  else {
    fprintf(stderr, "symbol_name: '%s' not mapped\n", symbol_name);
    exit(1);
  }

  assert(result != NULL);
  return result;
}
