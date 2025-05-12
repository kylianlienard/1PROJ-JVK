import subprocess

folderPath = "C:/Users/kylia/OneDrive - SUPINFO/Documents/GitHub/1PROJ-JVK"
subprocess.run(["powershell", "-Command", f"gcc '{folderPath}/src/main.c' -o '{folderPath}/main.exe' -I'{folderPath}/SDL2/include' -L'{folderPath}/SDL2/lib' -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer"])
print("# ~~~ Compilation ended ~~~ #")