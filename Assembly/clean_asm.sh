#!/bin/bash

# 检查是否提供了文件名
if [ -z "$1" ]; then
  echo "Usage: $0 <assembly_file>"
  exit 1
fi

# 输入汇编文件
input_file="$1"

# 输出汇编文件
output_file="${input_file%.s}_cleaned.s"

# 使用 sed 处理文件
sed -e '/^\s*\.cfi_/d' \
    -e '/^\s*\.LFB/d' \
    -e '/^\s*\.LFE/d' \
    -e '/^\s*\.size/d' \
    -e '/^\s*\.ident/d' \
    -e '/^\s*\.section/d' \
    -e '/^\s*[0-9]:/d' \
    -e '/^\s*\.long/d' \
    -e '/^\s*\.string/d' \
    -e '/^\s*\.align/d' "$input_file" > "$output_file"

echo "Cleaned assembly file saved to $output_file"