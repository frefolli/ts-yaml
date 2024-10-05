#include <ts-yaml.h>
#include <string.h>

char* read_source_code(const char* filepath) {
    char* text = NULL;
    FILE* file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    text = (char*) malloc(fsize + 1);
    fsize = fread(text, fsize, 1, file);
    fclose(file);
    return text;
}

int main(int argc, char** args) {
  const char* filepath = "example.yaml";
  if (argc > 1) {
    filepath = args[1];
  }

  TSParser* parser = ts_parser_new();
  const TSLanguage* language = tree_sitter_yaml();
  ts_parser_set_language(parser, language);
  char* source_code = read_source_code(filepath);
  uint64_t source_length = strlen(source_code);
  TSTree* tree = ts_parser_parse_string(parser, NULL, source_code, source_length);
  TSNode root_node = ts_tree_root_node(tree);

  char* repr = ts_node_string(root_node);
  fprintf(stdout, "%s\n", repr);
  free(repr);

  YamlObjectp value = parse_yaml(language, root_node, source_code);
  YamlObject__fprint(stdout, value, 0);
  fputc('\n', stdout);
  YamlObject__delete(value);

  ts_tree_delete(tree);
  ts_parser_delete(parser);
  free(source_code);
}
