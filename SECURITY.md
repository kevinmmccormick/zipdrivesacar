# Security

This project is a ROM and local build/test scripts, with no hosted service or
network component. There is no formal security support schedule.

Use GitHub private vulnerability reporting if it is enabled on the repository
for sensitive findings; do not put secrets or exploit details in a public issue.
Ordinary game bugs belong in public issues. Include the affected revision,
tool versions, reproduction steps, and impact without personal data.

Install DASM and Stella from their official projects. Emulator and assembler
security reports belong with their respective maintainers. Test scripts launch
Stella and stop the processes they start; generated data belongs in `artifacts/`.
