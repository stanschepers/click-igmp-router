#!/usr/bin/env bash

echo "write router/igmp_$1/igmp.config STARTUP_QUERY_INTERVAL $2" | telnet localhost 10000