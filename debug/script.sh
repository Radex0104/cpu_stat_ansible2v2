#!/bin/bash

echo "══════════════════════════════════════════════════════════════"
echo "                 ИНФОРМАЦИЯ О ПРОЦЕССОРЕ"
echo "══════════════════════════════════════════════════════════════"
echo ""

echo "Хост: $(hostname)"
echo ""

echo "Модель CPU:"
if command -v lscpu &> /dev/null; then
    echo "   $(lscpu | grep 'Model name' | head -1 | cut -d':' -f2 | xargs)"
else
    echo "   $(cat /proc/cpuinfo | grep 'model name' | head -1 | cut -d':' -f2 | xargs)"
fi
echo ""

echo "Ядра процессора:"
echo "   Всего ядер: $(nproc)"
if command -v lscpu &> /dev/null; then
    echo "   CPU(s): $(lscpu | grep '^CPU(s):' | awk '{print $2}')"
    echo "   Ядер на сокет: $(lscpu | grep 'Core(s) per socket' | cut -d':' -f2 | xargs)"
    echo "   Сокетов: $(lscpu | grep 'Socket(s)' | cut -d':' -f2 | xargs)"
fi
echo ""

echo "Архитектура: $(uname -m)"
echo ""

if command -v lscpu &> /dev/null; then
    echo "Частота:"
    echo "   $(lscpu | grep 'CPU MHz' | head -1 | cut -d':' -f2 | xargs) MHz"
    echo "   $(lscpu | grep 'CPU max MHz' | cut -d':' -f2 | xargs) MHz (max)"
fi
echo ""

echo "══════════════════════════════════════════════════════════════"
echo "                 ИНФОРМАЦИЯ О ДИСКАХ"
echo "══════════════════════════════════════════════════════════════"
echo ""

echo "Смонтированные разделы:"
echo "----------------------------------------------------------------"
df -h --exclude-type=tmpfs --exclude-type=devtmpfs | grep '^/' | while read line; do
    filesystem=$(echo $line | awk '{print $1}')
    size=$(echo $line | awk '{print $2}')
    used=$(echo $line | awk '{print $3}')
    avail=$(echo $line | awk '{print $4}')
    use_percent=$(echo $line | awk '{print $5}')
    mount=$(echo $line | awk '{print $6}')
    printf "%-20s %-8s %-8s %-8s %-5s %s\n" "$filesystem" "$size" "$used" "$avail" "$use_percent" "$mount"
done
echo "----------------------------------------------------------------"
echo ""

echo "Физические диски:"
if command -v lsblk &> /dev/null; then
    lsblk -d -o NAME,SIZE,MODEL | grep -v 'loop' | tail -n +2 | while read line; do
        echo "   $line"
    done
else
    echo "   lsblk не найден"
fi
echo ""

echo "Использование inode:"
df -i --exclude-type=tmpfs --exclude-type=devtmpfs 2>/dev/null | grep '^/' | head -5 | while read line; do
    filesystem=$(echo $line | awk '{print $1}')
    iused=$(echo $line | awk '{print $3}')
    ifree=$(echo $line | awk '{print $4}')
    iuse_percent=$(echo $line | awk '{print $5}')
    mount=$(echo $line | awk '{print $6}')
    echo "   $mount: inode: $iused/$ifree ($iuse_percent)"
done
echo ""

echo "══════════════════════════════════════════════════════════════"
echo "              КОНЕЦ ОТЧЕТА - $(date '+%Y-%m-%d %H:%M:%S')"
echo "══════════════════════════════════════════════════════════════"