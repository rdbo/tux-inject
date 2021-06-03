OUTPUT_DIR=./bin
OUTPUT=$(OUTPUT_DIR)/tux-inject
SRC_DIR=./src
SRCS=$(SRC_DIR)/main.c $(SRC_DIR)/proc/proc.c $(SRC_DIR)/ptrace/ptrace.c $(SRC_DIR)/maps/maps.c
LIBS=-ldl
CFLAGS=-std=c89 -g -Wall -Wextra -pedantic

install: all
	@echo "[*] Creating Desktop Icon"
	> tux-inject.desktop
	echo "[Desktop Entry]" >> tux-inject.desktop
	echo "Name=Tux Inject" >> tux-inject.desktop
	echo "Type=Application" >> tux-inject.desktop
	echo "Icon=`pwd`/LOGO.png" >> tux-inject.desktop
	echo "Exec=`pwd`/$(OUTPUT)" >> tux-inject.desktop
	if [ -d ~/.local/share/applications ]; then cp tux-inject.desktop ~/.local/share/applications/; fi
	rm tux-inject.desktop
	@echo "[+] Done"

all: clean
	mkdir $(OUTPUT_DIR)
	@echo "[-] Compiling..."
	$(CC) -o $(OUTPUT) $(CFLAGS) $(SRCS) $(LIBS)
	@echo "[+] Done"
clean:
	@echo "[-] Cleaning"
	if [ -d $(OUTPUT_DIR) ]; then rm -rf $(OUTPUT_DIR); fi
	@echo "[+] Done"