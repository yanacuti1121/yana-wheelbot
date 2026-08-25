// Usage: openscad -D 'model_file="/path/to/model.stl"' ... preview_stl.scad
model_file = "";

assert(model_file != "", "Pass model_file with -D");
color([0.72, 0.72, 0.74]) import(model_file, convexity = 10);
