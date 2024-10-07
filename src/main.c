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

  YamlObjectp value = parse_yaml_file(filepath);
  if (value != NULL) {
    YamlObject__delete(value);
    return 0;
  } else {
    return 1;
  }
}
