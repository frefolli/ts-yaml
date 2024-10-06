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
  printf("reading file: %s\n", filepath);

  TSParser* parser = ts_parser_new();
  const TSLanguage* language = tree_sitter_yaml();
  ts_parser_set_language(parser, language);
  char* source_code = read_source_code(filepath);
  uint64_t source_length = strlen(source_code);
  TSTree* tree = ts_parser_parse_string(parser, NULL, source_code, source_length);
  TSNode root_node = ts_tree_root_node(tree);

  char* repr = ts_node_string(root_node);
  printf("CST of file: %s\n", repr);
  free(repr);

  printf("Parsing yaml document ...\n");
  YamlObjectp value = parse_yaml(language, root_node, source_code);
  printf("Dumping yaml document ...\n");
  YamlObject__fprint(stdout, value, 0);
  fputc('\n', stdout);
  printf("Cleaning yaml document ...\n");
  YamlObject__delete(value);

  printf("Terminating\n");
  ts_tree_delete(tree);
  ts_parser_delete(parser);
  free(source_code);
}
