#!/bin/bash
#./scripts/diagnostics.sh

echo "--- CPU & CACHES ---"
lscpu | grep -E 'Model name|CPU\(s\):|Thread\(s\) per core|L1|L2|L3|Flags' | grep -iE 'ht|Model|CPU|Thread|L1|L2|L3'

echo -e "\n--- OPERATING SYSTEM ---"
hostnamectl | grep -E 'Operating System|Kernel'

echo -e "\n--- MEMORY (RAM) ---"
sudo dmidecode -t memory | grep -E 'Size|Type|Manufacturer|Speed' | grep -v "No Module Installed"

echo -e "\n--- STORAGE & FILE SYSTEM ---"
lsblk -d -o NAME,MODEL,SIZE
echo "Partition format:" && findmnt -no SOURCE,FSTYPE /
