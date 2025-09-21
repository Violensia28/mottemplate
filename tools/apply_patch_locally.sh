
#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
cd "$ROOT_DIR"

# Copy workflow if not present or overwrite
mkdir -p .github/workflows
cp -f "$(dirname "$0")/../.github/workflows/ci_build.yml" .github/workflows/ci_build.yml

# Ensure platformio.ini contains ${sysenv.EXTRA_FLAGS}
if ! grep -q "\${sysenv.EXTRA_FLAGS}" ci_build/platformio.ini 2>/dev/null; then
  echo "Appending \${sysenv.EXTRA_FLAGS} to ci_build/platformio.ini build_flags ..."
  printf "
  \${sysenv.EXTRA_FLAGS}
" >> ci_build/platformio.ini
fi

echo "
>> Please paste SAFE_OFF_SNIPPET into your ci_build/src/main.cpp after Serial.begin(115200);"

# Commit & push & tag
BRANCH=$(git branch --show-current)
[ -z "$BRANCH" ] && BRANCH=main

git add .github/workflows/ci_build.yml ci_build/platformio.ini || true
git commit -m "ci: add workflow + inject EXTRA_FLAGS placeholder" || true
git push origin "$BRANCH"

TAG="v0.4.2-safeoff"
(git tag "$TAG" && git push origin "$TAG") || true

echo "
Done. Check GitHub Actions for tag $TAG."
