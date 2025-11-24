
---

## 🧪 Test Script for All

**`test_all.sh` (root)**

```bash
#!/usr/bin/env bash
set -e

echo "=== [1/3] Building bootloader ==="
(cd bootloader && make)

echo "=== [2/3] Building kernel module ==="
(cd kernel-module && make)

echo "=== [3/3] Building mini shell ==="
(cd minishell && make)

cat <<EOF

All builds completed.

Next steps:

[Bootloader]
  cd bootloader
  make run

[Kernel module]
  cd kernel-module
  sudo insmod param_osinfo.ko
  cat /proc/param_osinfo
  sudo rmmod param_osinfo

[Mini shell]
  cd minishell
  ./minish

EOF
