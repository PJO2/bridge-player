# Bridge Player — Agent instructions

Language: C11
Build system: CMake
Reference DD solver: DDS

Rules:
- Do not commit directly to main.
- One task per branch.
- Every implementation must include tests.
- Never modify an acceptance test merely to make it pass.
- Preserve public interfaces unless explicitly authorized.
- Run all relevant tests before creating a PR.
- Report any unresolved failure.
- No new external dependency without approval.

Definition of done:
- Code compiles without warnings.
- Unit tests pass.
- Relevant integration tests pass.
- CI passes.
- Requirements are traceable.
- Changes have been reviewed.
