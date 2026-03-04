-include scripts/emdawnwebgpu.env

BUILD_DIR      := build
WEB_BUILD_DIR  := build-web
BUILD_TYPE     := Release
WEB_PORT       ?= 8080
NUM_JOBS       := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

.DEFAULT_GOAL := run
.PHONY: configure build rebuild clean install run all \
	configure-web build-web web

# ARGS for native executable
ARGS := -v="$(CURDIR)/dataset/video.mp4" -c="$(CURDIR)/dataset/camera.yaml"

configure:
	@echo "→ Configuring native ($(BUILD_TYPE))..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_POLICY_VERSION_MINIMUM=4.2.0 \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure
	@echo "→ Building native..."
	@cmake --build $(BUILD_DIR) -j$(NUM_JOBS)
	@echo "✓ Native build complete"

run: build
	@echo "→ Running studio (native)..."
	@cd $(BUILD_DIR)/bin && ./studio $(ARGS)

rebuild: clean build

debug:
	@$(MAKE) run BUILD_TYPE=Debug

release:
	@$(MAKE) run BUILD_TYPE=Release

all: build

configure-web:
	@echo "→ Configuring web ($(BUILD_TYPE))..."
	@mkdir -p $(WEB_BUILD_DIR)
	@cd $(WEB_BUILD_DIR) && emcmake cmake .. \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DEMDAWNWEBGPU_PORT="$(EMDAWNWEBGPU_PORT)"



build-web: configure-web
	@echo "→ Building web..."
	@cmake --build $(WEB_BUILD_DIR) -j$(NUM_JOBS)
	@echo "✓ Web build complete"

web: build-web
	@echo "→ Running web (emrun)..."
	@emrun --no_browser --hostname 0.0.0.0 --port $(WEB_PORT) build-web/studio.html
	@echo "Open: http://<your-local-ip>:$(WEB_PORT)/studio.html"

clean:
	@rm -rf $(BUILD_DIR) $(WEB_BUILD_DIR)

