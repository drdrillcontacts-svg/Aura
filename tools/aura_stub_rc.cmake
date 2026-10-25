# Writes a minimal Windows resources .rc for the AURA cross build
# (replaces the juceaide 'rcfile' step, which is disabled).
file(WRITE "${AURA_RC_OUT}"
"1 24 \"AURA\" // stub resource for AURA cross builds (no version metadata)\n"
)
