import os

folderPath = "C:/Users/kylia/OneDrive - SUPINFO/Documents/GitHub/1PROJ-JVK"

checks = [
    ("main.c", os.path.join(folderPath, "1.0", "main.c")),
    ("SDL.h", os.path.join(folderPath, "SDL_2.30.4", "include", "SDL.h")),
    ("libSDL2.a", os.path.join(folderPath, "SDL_2.30.4", "lib", "libSDL2.a")),
    ("libSDL2main.a", os.path.join(folderPath, "SDL_2.30.4", "lib", "libSDL2main.a")),
]

print("Checking critical files:")
for name, path in checks:
    if os.path.isfile(path):
        print(f"{name}: {path}")
    else:
        print(f"{name} MISSING: {path}")

# List all .a files in lib directory
lib_dir = os.path.join(folderPath, "SDL_2.30.4", "lib")
if os.path.isdir(lib_dir):
    print("\nLibrary files present in lib directory:")
    for f in os.listdir(lib_dir):
        if f.endswith(".a"):
            print(f"  {f}")
else:
    print("lib directory missing")