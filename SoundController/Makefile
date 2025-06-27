# SoundController Makefile
# Vereinfacht die Arbeit mit PlatformIO für das Arduino-Projekt

# Standard-Environment aus platformio.ini
ENV = nanoatmega328

# Farben für bessere Lesbarkeit
GREEN = \033[0;32m
YELLOW = \033[1;33m
RED = \033[0;31m
BLUE = \033[0;34m
NC = \033[0m # No Color

.PHONY: help build upload flash monitor clean show devices install deps

# Standard-Ziel
all: build

help:
	@echo "$(BLUE)SoundController - PlatformIO Makefile$(NC)"
	@echo "$(BLUE)=====================================$(NC)"
	@echo ""
	@echo "$(GREEN)Verfügbare Targets:$(NC)"
	@echo "  $(YELLOW)build$(NC)       - Projekt kompilieren"
	@echo "  $(YELLOW)upload$(NC)      - Code auf Board hochladen"
	@echo "  $(YELLOW)flash$(NC)       - Code hochladen + Monitor starten"
	@echo "  $(YELLOW)monitor$(NC)     - Seriellen Monitor starten"
	@echo "  $(YELLOW)clean$(NC)       - Build-Dateien löschen"
	@echo "  $(YELLOW)show$(NC)        - Angeschlossene Boards anzeigen"
	@echo "  $(YELLOW)devices$(NC)     - Detaillierte Geräte-Info"
	@echo "  $(YELLOW)install$(NC)     - PlatformIO installieren/aktualisieren"
	@echo "  $(YELLOW)deps$(NC)        - Abhängigkeiten installieren"
	@echo "  $(YELLOW)help$(NC)        - Diese Hilfe anzeigen"
	@echo ""
	@echo "$(GREEN)Beispiele:$(NC)"
	@echo "  make flash      - Hochladen und Monitor starten"
	@echo "  make show       - Verfügbare Boards anzeigen"
	@echo ""

# Projekt kompilieren
build:
	@echo "$(GREEN)Kompiliere Projekt...$(NC)"
	pio run -e $(ENV)

# Code auf Board hochladen
upload:
	@echo "$(GREEN)Lade Code auf Board hoch...$(NC)"
	pio run -e $(ENV) --target upload

# Code hochladen und Monitor starten
flash: upload
	@echo "$(GREEN)Starte Monitor...$(NC)"
	@echo "$(YELLOW)Drücke Ctrl+C um Monitor zu beenden$(NC)"
	pio device monitor -e $(ENV)

# Seriellen Monitor starten
monitor:
	@echo "$(GREEN)Starte seriellen Monitor...$(NC)"
	@echo "$(YELLOW)Drücke Ctrl+C um Monitor zu beenden$(NC)"
	pio device monitor -e $(ENV)

# Build-Dateien löschen
clean:
	@echo "$(GREEN)Lösche Build-Dateien...$(NC)"
	pio run --target clean

# Angeschlossene Boards anzeigen
show:
	@echo "$(BLUE)Angeschlossene serielle Geräte:$(NC)"
	@echo "$(BLUE)==============================$(NC)"
	@pio device list
	@echo ""
	@echo "$(YELLOW)Hinweis: Arduino Nano sollte als 'USB Serial' oder ähnlich erscheinen$(NC)"

# Detaillierte Geräte-Information
devices: show
	@echo ""
	@echo "$(BLUE)Board-Informationen:$(NC)"
	@echo "$(BLUE)====================$(NC)"
	@echo "$(GREEN)Environment:$(NC) $(ENV)"
	@echo "$(GREEN)Platform:$(NC) atmelavr"
	@echo "$(GREEN)Board:$(NC) nanoatmega328"
	@echo "$(GREEN)Framework:$(NC) arduino"
	@echo ""
	@echo "$(YELLOW)Falls Board nicht erkannt wird:$(NC)"
	@echo "  - USB-Kabel prüfen"
	@echo "  - Board-Treiber installieren (CH340/FTDI)"
	@echo "  - Port-Berechtigung prüfen: sudo usermod -a -G dialout $$USER"

# PlatformIO installieren/aktualisieren
install:
	@echo "$(GREEN)Installiere/Aktualisiere PlatformIO...$(NC)"
	@which pio > /dev/null || (echo "$(RED)PlatformIO nicht gefunden. Installiere...$(NC)" && curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py | python3)
	@echo "$(GREEN)PlatformIO ist bereit!$(NC)"

# Abhängigkeiten installieren
deps:
	@echo "$(GREEN)Installiere Projekt-Abhängigkeiten...$(NC)"
	pio lib install

# Erweiterte Targets für Entwicklung
debug:
	@echo "$(GREEN)Starte Debug-Build...$(NC)"
	pio run -e $(ENV) --verbose

# Bibliotheken aktualisieren
update-libs:
	@echo "$(GREEN)Aktualisiere Bibliotheken...$(NC)"
	pio lib update

# Projekt-Informationen anzeigen
info:
	@echo "$(BLUE)Projekt-Informationen:$(NC)"
	@echo "$(BLUE)=====================$(NC)"
	@pio project data
	@echo ""
	@echo "$(GREEN)Installierte Bibliotheken:$(NC)"
	@pio lib list

# Environment-spezifische Targets
upload-verbose:
	@echo "$(GREEN)Detaillierter Upload...$(NC)"
	pio run -e $(ENV) --target upload --verbose

# Test-Target (falls Tests vorhanden)
test:
	@echo "$(GREEN)Führe Tests aus...$(NC)"
	pio test -e $(ENV)

# Konfiguration prüfen
check:
	@echo "$(GREEN)Prüfe Projekt-Konfiguration...$(NC)"
	pio check -e $(ENV)

# Board-Reset (falls unterstützt)
reset:
	@echo "$(GREEN)Setze Board zurück...$(NC)"
	@echo "$(YELLOW)Drücke Reset-Taste am Board oder ziehe USB-Kabel$(NC)"

# Schnellhilfe für häufige Probleme
troubleshoot:
	@echo "$(BLUE)Problemlösung:$(NC)"
	@echo "$(BLUE)===============$(NC)"
	@echo "$(YELLOW)Board wird nicht erkannt:$(NC)"
	@echo "  - make show (zeigt verfügbare Ports)"
	@echo "  - USB-Kabel und -Port prüfen"
	@echo "  - sudo usermod -a -G dialout $$USER && logout"
	@echo ""
	@echo "$(YELLOW)Upload schlägt fehl:$(NC)"
	@echo "  - Board-Typ in platformio.ini prüfen"
	@echo "  - Reset-Taste während Upload drücken"
	@echo "  - Andere Programme schließen, die den Port verwenden"
	@echo ""
	@echo "$(YELLOW)Kompilierung schlägt fehl:$(NC)"
	@echo "  - make clean && make build"
	@echo "  - make deps (Abhängigkeiten neu installieren)"
