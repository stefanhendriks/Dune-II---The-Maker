# Triage Labels

The skills speak in terms of five canonical triage roles. This file maps those roles to the actual label strings used in this repo's issue tracker, plus one project-specific addition (`needs-design`) for issues that need an architecture decision before an agent can implement them.

| Label in mattpocock/skills | Label in our tracker | Meaning                                                       |
| --------------------------- | --------------------- | -------------------------------------------------------------- |
| `needs-triage`               | `needs-triage`         | Newly reported, not yet evaluated                               |
| `needs-info`                 | `needs-info`           | Waiting on reporter for more information                       |
| (no equivalent)              | `needs-design`         | Real problem, but the solution direction isn't obvious yet     |
| `ready-for-agent`            | `ready-for-agent`      | Fully specified, ready for an AFK coding/data agent to implement |
| `ready-for-human`            | `ready-for-human`      | Requires human implementation                                  |
| `wontfix`                    | `wontfix`              | Will not be actioned                                            |

When a skill mentions a role (e.g. "apply the AFK-ready triage label"), use the corresponding label string from this table.

## Pipeline

New reports (from a player, Discord, or a contributor) start at `needs-triage`. From there:

1. **Triage** evaluates the report and routes it to exactly one of: `wontfix`, `needs-info`, `ready-for-agent` (the fix is obvious, no real design question), `ready-for-human`, or `needs-design` (it's real, but *how* to solve it needs a judgment call).
2. **Architecture** works the `needs-design` queue. It writes the chosen direction as a comment on the issue (or a linked ADR under `docs/adr/` for anything substantial enough to outlive the ticket), then relabels to `ready-for-agent` — or back to `needs-info`/`ready-for-human` if it turns out an agent shouldn't run with this at all.
3. **Coding/data agents** only ever self-serve issues labeled `ready-for-agent`. They never pick up `needs-design` themselves. If an agent starts implementing something and discovers real architectural ambiguity mid-task that the label didn't capture, the rule is: stop, relabel the issue back to `needs-design`, and don't guess.

Edit the right-hand column, and this pipeline description, to match however the vocabulary or process actually evolves.
