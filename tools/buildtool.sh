#!/bin/bash

# Generic Return Value
RC=0

# ANSI Color codes
NC='\033[0m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'

# Error Codes
ERR_SAFETY_FAILED=1
ERR_BAD_USAGE=2
ERR_NO_BUILD=3

# Path Setup
TOOLS_DIR="$(cd "$(dirname "${BASH_SOURCE}")" && pwd)"
PROJECT_ROOT="$(dirname "$TOOLS_DIR")"
PROJECT_DIR_NAME=$(basename "$PROJECT_ROOT")
BASE_BUILD_DIR="$PROJECT_ROOT/build"

function show_usage_clean {
    cat << EOF
================================================================================

----- Clean Command -----
Usage: buildtool.sh clean [configuration]
    
Removes build artifacts. Effectively recursively deletes the selected build
directory. Parent build directory 'GBoy/build' is targeted if no configuration
is given. Below are some examples:

Select all build fragments:
./buildtool.sh clean

Select DEBUG build fragments:
./buildtool.sh clean debug

EOF

    return 0;
}

function show_usage_build {
    cat << EOF
================================================================================

----- Build Command -----
Usage: buildtool.sh build [configuration] [options]

Builds the project. Will automatically generate build files if out of date or
missing. Defaults to a DEBUG configuration if no configuration is given.

Options:
  -c | --clean     Performs a clean build by removing configuration build artifacts and regenerating before building.
  --parallel       Enables parallel processing during build. Can dramatically speed up build process.
  -g | --generate  Only generates the build files. Does nothing if build files are already up to date.
  --vsc            Enables Visual Studio Code support by generating the compile_commands.json file.
  -t | --test      Runs CTest after build finishes. Note: cannot be used with -g or --generate.

EOF

    return 0;
}

function show_usage_test {
    cat << EOF
================================================================================

----- Test Command -----
Usage: buildtool.sh test [configuration]

Runs CTest on a given built configuration. If a test fails, related logs during
test are also printed. Defaults to a DEBUG configuration if no configuration is
given.

EOF

    return 0;
}

function show_usage_run {
    cat << EOF
================================================================================

----- Run Command -----
Usage: buildtool.sh run [configuration] -- [GBoy Args]

Runs a built configuration of GBoy. Arguments can be passed directly to GBoy
following a "--" token. Defaults to a DEBUG configuration if no configuration
is given.

EOF

    return 0;
}

function show_usage {
    echo -e "${BLUE}GBoy Build & Management Tools${NC}"
    cat << EOF
Usage: buildtool.sh <command> [configuration] [options]

Commands:
  clean [configuration]            Removes build artifacts. Wipes all builds if no configuration is given.
  build [configuration] [options]  Builds the project. If configuration hasn't been generated, automatically generates build files.
  test  [configuration]            Runs CTest on the project.
  run   [configuration]            Runs a built configuration of GBoy.
  help  [command]                  Displays this message. Can display more detailed information about specific commands.

Configurations:
  debug      Debug Configuration. Builds with debug symbols included and optimization off.
  release    Release Configuration. Builds with debug symbols stripped and optimization on.
  asan       ASan Configuration. Debug build with Address Sanitizer enabled.

EOF

    while [[ $# -gt 0 ]]; do
        case "$1" in
            clean)
                show_usage_clean
                shift
                ;;
            build)
                show_usage_build
                shift
                ;;
            test)
                show_usage_test
                shift
                ;;
            run)
                show_usage_run
                shift
                ;;
        esac
    done

    return 0
}

function update_vsc_link {
    local CONFIG="$1"
    local SOURCE="$BASE_BUILD_DIR/$CONFIG/compile_commands.json"
    local TARGET="$BASE_BUILD_DIR/compile_commands.json"

    # Only link if the file actually exists
    if [[ -f "$SOURCE" ]]; then
        ln -sf "$SOURCE" "$TARGET"
        echo -e "${GREEN}Active IntelliSense synced to [${CONFIG^^}]${NC}"
    fi
}

function clean {
    local TARGET_CONFIG="$1"
    local TARGET_DIR=""

    if (( $# > 1 )); then
        echo -e "${RED}Error: Too many arguments. Expected 0 or 1.${NC}"
        return $ERR_BAD_USAGE
    fi

    if (( $# == 0 )); then
        echo -e "${YELLOW}Cleaning all build configurations...${NC}"
        TARGET_DIR="$BASE_BUILD_DIR"
    else
        case "$TARGET_CONFIG" in
            "debug" | "release" | "asan")
                echo -e "${YELLOW}Cleaning [${TARGET_CONFIG^^}] configuration...${NC}"
                TARGET_DIR="$BASE_BUILD_DIR/$1"
                ;;
            *)
                echo -e "${RED}Error: Invalid Configuration '${TARGET_CONFIG}'. Expected: 'debug', 'release', or 'asan'${NC}"
                show_usage "clean"
                return $ERR_BAD_USAGE
                ;;
        esac
    fi           

    echo -e "${YELLOW}Removing Directory '$TARGET_DIR'...${NC}"
    if [[ -d "$TARGET_DIR" && "$TARGET_DIR" != "/" ]]; then
        rm -rf "$TARGET_DIR"
    fi
    echo -e "${GREEN}Cleaned${NC}"
    return 0
}

function generate {
    local TARGET_CONFIG="$1"
    local EXPORT_COMMANDS="$2"
    local CMAKE_FLAGS=""

    case "$TARGET_CONFIG" in
        debug)
            CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Debug"
            ;;
        release)
            CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Release"
            ;;
        asan)
            CMAKE_FLAGS+=" -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON"
            ;;
        *)
            echo -e "${RED}Error: Expected Configuration as first argument.${NC}"
            return $ERR_BAD_USAGE
            ;;
    esac

    if [[ "$EXPORT_COMMANDS" == true ]]; then
        CMAKE_FLAGS+=" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    fi

    local START_TIME=$SECONDS
    echo -e "${YELLOW}Generating [${TARGET_CONFIG^^}] configuration...${NC}"
    cmake -S "$PROJECT_ROOT" -B "$BASE_BUILD_DIR/$TARGET_CONFIG" $CMAKE_FLAGS
    RC=$?

    if [[ $RC == 0 ]]; then
        echo -e "${GREEN}Generation Complete (Took $((SECONDS - START_TIME))s)${NC}"

        if [[ "$EXPORT_COMMANDS" == true ]]; then
            update_vsc_link "$TARGET_CONFIG"
        fi
    else
        echo -e "${RED}Generation Failed${NC}"
    fi

    return $RC
}

function test {
    local TARGET_CONFIG="debug"
    local TEST_DIR=""

    while [[ $# -gt 0 ]]; do
        case "$1" in
            debug|release|asan)
                TARGET_CONFIG="$1"
                shift
                ;;
            *)
                echo -e "${RED}Error: Unknown options '$1'${NC}"
                show_usage "test"
                return $ERR_BAD_USAGE
                ;;
        esac
    done

    TEST_DIR="$BASE_BUILD_DIR/$TARGET_CONFIG"

    if [[ ! -f "$TEST_DIR/CTestTestfile.cmake" ]]; then
        echo -e "${RED}Error: No tests found for [${TARGET_CONFIG^^}]. Ensure the project was built for this configuration.${NC}"
        return $ERR_NO_BUILD 
    fi

    echo -e "${YELLOW}Running Tests...${NC}"
    ctest --test-dir "$TEST_DIR" --output-on-failure
    RC=$?

    if [[ $RC != 0 ]]; then
        echo -e "${RED}Tests Failed${NC}"
        return $RC
    fi

    echo -e "${GREEN}Tests Completed Successfully${NC}"
    return 0
}

function build {
    local CLEAN_BUILD=false
    local PARALLEL_BUILD=false
    local COMPILE=true
    local TARGET_CONFIG="debug"
    local EXPORT_COMMANDS=false
    local TEST_BUILD=false

    while [[ $# -gt 0 ]]; do
        case "$1" in
            -c|--clean)
                CLEAN_BUILD=true
                shift
                ;;
            --parallel)
                PARALLEL_BUILD=true
                shift
                ;;
            -g|--generate) 
                COMPILE=false
                shift
                ;;
            debug|release|asan)
                TARGET_CONFIG="$1"
                shift
                ;;
            --vsc)
                EXPORT_COMMANDS=true
                shift
                ;;
            -t|--test)
                TEST_BUILD=true
                shift
                ;;
            *)
                echo -e "${RED}Error: Unknown options '$1'${NC}"
                show_usage "build"
                return $ERR_BAD_USAGE
                ;;
        esac
    done

    if [[ "$CLEAN_BUILD" == true ]]; then
        clean "$TARGET_CONFIG" || return $?
    fi

    if [[ ! -d "$BASE_BUILD_DIR/$TARGET_CONFIG" ]]; then
        generate "$TARGET_CONFIG" "$EXPORT_COMMANDS" || return $?
    fi

    if [[ "$COMPILE" == true ]]; then
        local CMAKE_BUILD_FLAGS="--build $BASE_BUILD_DIR/$TARGET_CONFIG"

        if [[ "$PARALLEL_BUILD" == true ]]; then
            CMAKE_BUILD_FLAGS+=" --parallel"
        fi

        local START_TIME=$SECONDS
        echo -e "${YELLOW}Compiling [${TARGET_CONFIG^^}] build...${NC}"
        cmake $CMAKE_BUILD_FLAGS || return $?
        echo -e "${GREEN}Build Successful (Took $((SECONDS - START_TIME))s)${NC}"
        update_vsc_link "$TARGET_CONFIG"
    fi

    if [[ "$COMPILE" == true && "$TEST_BUILD" == true ]]; then
        test "$TARGET_CONFIG" || return $?
    elif [[ "$COMPILE" == false && "$TEST_BUILD" == true ]]; then
        echo -e "${YELLOW}Warn: Cannot Test build [${TARGET_CONFIG^^}] since only build files were generated. Compile build to run tests.${NC}"
    fi

    return 0
}

function run {
    local TARGET_CONFIG="debug"
    local APP_ARGS=()

    while [[ $# -gt 0 ]]; do
        case "$1" in
            debug|release|asan)
                TARGET_CONFIG="$1"
                shift
                ;;
            --)
                shift
                APP_ARGS=("$@")
                break
                ;;
            *)
                echo -e "${RED}Error: Unknown options '$1'${NC}"
                show_usage "run"
                return $ERR_BAD_USAGE
                ;;
        esac
    done

    local BINARY_PATH="$BASE_BUILD_DIR/$TARGET_CONFIG/bin/GBoy"
    if [[ ! -f "$BINARY_PATH" ]]; then
        echo -e "${RED}Error: [${TARGET_CONFIG^^}] binary not found at $BINARY_PATH.${NC}"
        echo -e "${YELLOW}Hint: Run './buildtool.sh build $TARGET_CONFIG' first.${NC}"
        return $ERR_NO_BUILD
    fi

    echo -e "${YELLOW}Running GBoy in [${TARGET_CONFIG^^}] configuration...${NC}"
    if [[ ${#APP_ARGS[@]} -gt 0 ]]; then
        echo -e "${YELLOW}With arguments: ${APP_ARGS[*]}${NC}"
    fi

    "$BINARY_PATH" "${APP_ARGS[@]}"
    return $?
}

function main {
    # Safety Check
    if [[ "$PROJECT_DIR_NAME" != "GBoy" ]]; then
        echo -e "${RED}Error: Attempted to run script outside project folder. Ensure script is inside project folder in the '/tools' directory.${NC}"
        exit $ERR_SAFETY_FAILED
    fi

    COMMAND=$1
    shift
    case $COMMAND in
        clean)
            clean "$@"
            exit $?
            ;;

        build)
            build "$@"
            exit $?
            ;;

        test)
            test "$@"
            exit $?
            ;;

        run)
            run "$@"
            exit $?
            ;;

        help)
            show_usage "$@"
            exit 0
            ;;

        *)
            echo -e "${RED}Error: Unknown command '$COMMAND'.${NC}"
            show_usage
            exit 0
    esac
}

main "$@"
exit $?
