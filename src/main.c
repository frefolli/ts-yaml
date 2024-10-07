#include <ts-yaml.h>
#include <string.h>

int main(int argc, char** args) {
  const char* filepath = "example.yaml";
  for (int64_t i = 1; i < argc; ++i) {
    filepath = args[i];
    printf("reading file: %s\n", filepath);

    YamlObjectp value = parse_yaml_file(filepath);
    if (value != NULL) {
      YamlObject__fprint(stdout, value, 0);
      fprintf(stdout, "\n");
      YamlObject__delete(value);
    } else {
      fprintf(stderr, "ERROR: unable to read\n");
      return 1;
    }
  }
  return 0;
}
