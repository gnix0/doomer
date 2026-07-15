#include <stdio.h>
#include <stdlib.h>

typedef struct {
  double min_scale;
  double scroll_speed;
  double drag_friction;
  double scale_friction;
} Config;

const Config defaultConfig = {
  .min_scale = 0.01,
  .scroll_speed = 1.5,
  .drag_friction = 6.0,
  .scale_friction = 4.0,
};

Config loadConfig(const char *filePath)
{
  Config config = defaultConfig;
  
  FILE *file = fopen(filePath, "r");
  if (file == NULL) {
    perror("fopen");
    return config;
  }

  char line[256];
  while (fgets(line, sizeof line, file) != NULL) {
    // TODO: process lines
  }

  fclose(file);
  return config;

}
