# !/user/bin/bash
#
# Author: Edward Palmer
# Date: 2025-08-17
# Copyright (c) 2025
#

EXCHANGE_PORT=1234
DATABASE_PORT=2345
ENGINE_PORT=3456

bazel run //src/apps:netadmin_app $EXCHANGE_PORT shutdown &
bazel run //src/apps:netadmin_app $DATABASE_PORT shutdown &
bazel run //src/apps:netadmin_app $ENGINE_PORT shutdown &

echo "Completed application shutdown."