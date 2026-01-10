#!/bin/bash

# Path Setup
TOOLS_DIR="$(cd "$(dirname "${BASH_SOURCE}")" && pwd)"
PROJECT_ROOT="$(dirname "$TOOLS_DIR")"
BASE_BUILD_DIR="$PROJECT_ROOT/build"

# Defaults
CONFIG="Debug"
EXPORT_COMMANDS="OFF"
CLEAN_BEFORE_GEN=false

# ANSI Color codes
NC='\033[0m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'

COMMAND=$1
shift

case $COMMAND in
    clean)
        if [ -d "$BASE_BUILD_DIR" ]; then
            echo "Removing entire build directory: $BASE_BUILD_DIR"
            rm -rf "$BASE_BUILD_DIR"
            echo -e "${GREEN}Cleaned${NC}."
        else
            echo -e "${YELLOW}Build directory does not exist. Nothing to clean.${NC}"
        fi
        ;;

    generate)
        while [[ $# -gt 0 ]]; do
            case $1 in
                -vsc_integration)
                    EXPORT_COMMANDS="ON"
                    shift
                    ;;
                -clean)
                    CLEAN_BEFORE_GEN=true
                    shift
                    ;;
                --configuration)
                    if [ "${2^^}" == "RELEASE" ]; then
                        CONFIG="Release"
                    else
                        CONFIG="Debug"
                    fi
                    shift 2
                    ;;
                *) shift ;;
            esac
        done

        BUILD_DIR="$BASE_BUILD_DIR/${CONFIG,,}"

        # Handle the -clean flag
        if [ "$CLEAN_BEFORE_GEN" = true ]; then
            if [ -d "$BUILD_DIR" ]; then
                echo "Cleaning $CONFIG build folder $BUILD_DIR before generation..."
                rm -rf "$BUILD_DIR"
                echo "Cleaned."
            fi
        fi

        echo "Generating $CONFIG build files in: $BUILD_DIR"
        cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
            -DCMAKE_BUILD_TYPE="$CONFIG" \
            -DCMAKE_EXPORT_COMPILE_COMMANDS="$EXPORT_COMMANDS"
        echo -e "${GREEN}Success${NC}"
        ;;

    build)
        TARGET_CONFIG="debug"
        if [ "${1^^}" == "RELEASE" ]; then
            TARGET_CONFIG="release"
        fi

        BUILD_DIR="$BASE_BUILD_DIR/$TARGET_CONFIG"

        if [ -d "$BUILD_DIR" ]; then
            echo "Compiling $TARGET_CONFIG configuration..."
            cmake --build "$BUILD_DIR"
        else
            echo -e "${RED}Error: Build directory '$BUILD_DIR' not found.${NC}"
            echo -e "${YELLOW}Make sure to run generate first before this: $0 generate --configuration ${TARGET_CONFIG^^}${NC}"
            exit 1
        fi
        echo -e "${GREEN}Success${NC}"
        ;;

    test)
        TARGET_CONFIG="debug"
        if [ "${1^^}" == "RELEASE" ]; then
            TARGET_CONFIG="release"
        fi

        BUILD_DIR="$BASE_BUILD_DIR/$TARGET_CONFIG"

        if [ -d "$BUILD_DIR" ]; then
            echo "Running Tests..."
            ctest --test-dir $BUILD_DIR -V
        else
            echo -e "${RED}Error: Build directory '$BUILD_DIR' not found.${NC}"
            echo -e "${YELLOW}Make sure to run generate first before this: $0 generate --configuration ${TARGET_CONFIG^^}${NC}"
            exit 1
        fi
        echo -e "${GREEN}Tests Finished${NC}"
        ;;

    *)
        echo "Usage: $0 {generate|build|clean} [options]"
        echo "  clean: Deletes the entire /build folder"
        echo "  generate [-vsc_integration] [-clean] [--configuration RELEASE|DEBUG]: Generate build files"
        echo "    -vsc_integration: Generates a "compile_commands.json" file for use integrating with VS Code"
        echo "    -clean: Deletes the /build/<configuration> folder before generating a new one"
        echo "    --configuration [RELEASE|DEBUG]: Specifies the configuration to generate for. Defaults to DEBUG"
        echo "  build [RELEASE|DEBUG]: Compile the project from the build files"
        echo "  test [RELEASE|DEBUG]: Run the test suite on the compiled configuration"
        exit 1
        ;;
esac
