# PR Conventions

## Title style

Write PR titles as a behavior statement or goal. Do not prefix with "Fix:", "Add:", "Update:", or similar verbs.

```
# correct
Sardaukar campaign data included in release package

# wrong
Fix: sardaukar campaign data missing from release
```

## Labels

Only apply these labels when they fit:

- `technical` — purely technical change with no new gameplay mechanics (bug fixes, build system, tooling)
- `refactoring` — code refactor

Do not combine `technical` and `refactoring`; pick the one that fits best.

## Backticks in PR bodies

Write backticks unescaped in PR body text. Use a heredoc when passing the body via `gh pr create` to prevent shell interpretation.

```sh
# correct
gh pr create --body "$(cat <<'EOF'
Use `getStructureId()` to get the structure ID.
EOF
)"

# wrong — backticks will render literally on GitHub
gh pr create --body "Use \`getStructureId()\` to get the structure ID."
```

## Update PR description after rebasing

After a force-push or rebase on a branch that already has an open PR, update the PR title and body to reflect the current state of the changes.

```sh
gh pr edit <number> --title "..." --body "..."
```

## No AI attribution

Do not add a `claude` label, or mention Claude/AI assistance, in PR titles, bodies, or comments.

## Contributor privacy

Don't name or attribute a specific tester or contributor in a PR body, issue comment, or post unless they've explicitly agreed to be credited.
