# !/user/bin/bash
#
# Author: Edward Palmer
# Date: 2025-08-16
# Copyright (c) 2025
#

EXCHANGE_PORT=1234
DATABASE_PORT=2345
ENGINE_PORT=3456

LOGDIR="/var/log/talos"
mkdir -p $LOGDIR

# Start the dummy exchange:
bazel run //src/apps:exchange_app $EXCHANGE_PORT > "$LOGDIR/exchange.log" &
sleep 0.5

# Start the dummy database:
bazel run //src/apps:database_app $DATABASE_PORT > "$LOGDIR/database.log" &
sleep 0.5

# Start the Talos OMEngine:
bazel run //src/apps:engine_app -- --engine $ENGINE_PORT --exchange $EXCHANGE_PORT --database $DATABASE_PORT > "$LOGDIR/engine.log" &
sleep 0.5

echo "Completed application startup."