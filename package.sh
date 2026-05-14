#!/bin/bash
set -euo pipefail

# Package the myapp 2 folder into a downloadable ZIP archive
# The executable bit on ./myapp is preserved for users who unzip the archive.

cd "$(dirname "$0")"
chmod +x myapp

PACKAGE_NAME="myapp2.zip"
ROOT_DIR="$(pwd)"
PARENT_DIR="$(dirname "$ROOT_DIR")"
FOLDER_NAME="$(basename "$ROOT_DIR")"
ARCHIVE_PATH="$PARENT_DIR/$PACKAGE_NAME"

echo "Packaging folder '$FOLDER_NAME' into '$ARCHIVE_PATH'..."
rm -f "$ARCHIVE_PATH"
cd "$PARENT_DIR"
ditto -c -k --sequesterRsrc --keepParent "$FOLDER_NAME" "$ARCHIVE_PATH"

echo "✓ Package created: $ARCHIVE_PATH"

echo "To share this project, send the ZIP file and tell users to unzip it and run ./myapp"
