# Arcsky Control version.
#
# This is the single source of truth for the app version. Upstream's git-describe
# scheme (Git.cmake) picks up the inherited QGC/Taisync tags (v5.0.x), so the
# values it computes are replaced here before project() is called. Everything
# downstream -- applicationVersion(), the About/General version label, the Android
# versionName/versionCode, installers and the Windows .rc -- derives from this.
#
# Bump these by hand. Keep MAJOR and MINOR to one digit and PATCH to two,
# otherwise the Android version code runs out of room.
#
# Release checklist:
#   1. bump the version below and commit
#   2. tag that commit:  git tag -a arcsky-v<X.Y.Z> -m "Arcsky Control v<X.Y.Z>"
#   3. re-run CMake configure (the version is captured at configure time) and build

set(ARCSKY_VER_MAJOR 1)
set(ARCSKY_VER_MINOR 0)
set(ARCSKY_VER_PATCH 0)

# The upstream QGroundControl release this fork is based on (Taisync's QGC 5.0.6
# branch). Displayed next to our version for support purposes. Update this when
# rebasing onto new QGC.
set(ARCSKY_QGC_BASE_VERSION "5.0.6")

set(ARCSKY_VERSION "${ARCSKY_VER_MAJOR}.${ARCSKY_VER_MINOR}.${ARCSKY_VER_PATCH}")

# A build counts as a release build only when HEAD sits exactly on its own
# arcsky-v<version> tag (prefixed because the inherited upstream QGC history
# already has plain v1.0.0 .. v1.3.15 tags). Everything else is a dev build and
# carries branch + short hash so any field APK can be traced back to a commit.
set(ARCSKY_GIT_TAGS_AT_HEAD "")
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} tag --points-at HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE ARCSKY_GIT_TAGS_AT_HEAD
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    string(REPLACE "\n" ";" ARCSKY_GIT_TAGS_AT_HEAD "${ARCSKY_GIT_TAGS_AT_HEAD}")
endif()

set(ARCSKY_RELEASE_TAG "arcsky-v${ARCSKY_VERSION}")

if(ARCSKY_RELEASE_TAG IN_LIST ARCSKY_GIT_TAGS_AT_HEAD)
    set(ARCSKY_RELEASE_BUILD ON)
    set(ARCSKY_VERSION_STR "${ARCSKY_VERSION}")
    message(STATUS "Arcsky release build: v${ARCSKY_VERSION_STR} (QGC ${ARCSKY_QGC_BASE_VERSION})")
else()
    set(ARCSKY_RELEASE_BUILD OFF)
    set(ARCSKY_VERSION_STR "${ARCSKY_VERSION}-dev")
    message(STATUS "Arcsky dev build: v${ARCSKY_VERSION_STR} (QGC ${ARCSKY_QGC_BASE_VERSION}) ${QGC_GIT_BRANCH} ${QGC_GIT_HASH}")
endif()

# Replace the values Git.cmake derived from the upstream tags
set(QGC_APP_VERSION "${ARCSKY_VERSION}")
set(QGC_APP_VERSION_MAJOR ${ARCSKY_VER_MAJOR})
set(QGC_APP_VERSION_MINOR ${ARCSKY_VER_MINOR})
set(QGC_APP_VERSION_PATCH ${ARCSKY_VER_PATCH})
set(QGC_APP_VERSION_STR "${ARCSKY_VERSION_STR}")
