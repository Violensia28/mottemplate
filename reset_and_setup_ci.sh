#!/usr/bin/env bash
set -euo pipefail
cd ~/mottemplate

echo ">> Abort rebase (if any)"
git rebase --abort 2>/dev/null || true

echo ">> Create backup branch"
git switch -c backup/local-changes-$(date +%Y%m%d-%H%M) || true
git switch main

echo ">> Reset main to origin/main"
git fetch origin
git reset --hard origin/main

echo ">> Write workflow"
mkdir -p .github/workflows
cat > .github/workflows/ci_build.yml <<'YAML'
# (paste YAML from section 4 here exactly)
YAML

echo ">> Commit & push workflow"
git add .github/workflows/ci_build.yml
git commit -m "CI: add workflow build full (SAFE_MODE=0, artifacts, optional release)" || true
git push origin main

echo ">> Tag & push to trigger build"
TAG="v0.3.7-safeoff"
git tag "${TAG}" || true
git push origin "${TAG}"
echo "Done. Check GitHub Actions for run tagged ${TAG}."
